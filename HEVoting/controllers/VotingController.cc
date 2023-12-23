#include "VotingController.h"



namespace heVote {

	VotingController::VotingController(): dbClient(app().getDbClient()){
		//init encryption parameters
		eParams = new seal::EncryptionParameters(seal::scheme_type::bgv);
		//Poly modulus degree gives us the number of maximum candidates we can have in a poll
		//Due to how batch encoding works, the ciphertext stores the maximum amount of votes regardless of how many candidates the poll truly has
		eParams->set_poly_modulus_degree(VOTINGCONTROLLER_POLYMOD_DEGREE);
		eParams->set_coeff_modulus(seal::CoeffModulus::BFVDefault(VOTINGCONTROLLER_POLYMOD_DEGREE));
		// bit size of 32 enables support for up to 4,294,828,032 votes, which should be enough for anything that doesn't have 
		// more than 50% of the world's population sharing the same opinion.
		// 
		// It's also a nice round computable number.
		eParams->set_plain_modulus(seal::PlainModulus::Batching(VOTINGCONTROLLER_POLYMOD_DEGREE,32)); 
		//init context
		context = new seal::SEALContext(*eParams);
		evaluator = new seal::Evaluator(*context);
	}

	VotingController::~VotingController() { 
		delete eParams;
		delete context;
		delete evaluator;
	}

	void VotingController::createPoll(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
		//TODO: add asynchronicity and transaction
		//init JSON and extend callback lifetime
		auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
		auto& reqJson = *(req->getJsonObject());
		//init mappers
		//auto pollTransaction = dbClient->newTransaction();

		//init poll with title
		drogon_model::votingregister::Poll newPoll;
		newPoll.setTitle(reqJson["title"].asString());
		
		drogon::orm::Mapper<drogon_model::votingregister::Poll> pollMapper = drogon::orm::Mapper<drogon_model::votingregister::Poll>(dbClient);
		drogon::orm::Mapper<drogon_model::votingregister::Candidate> candidateMapper = drogon::orm::Mapper<drogon_model::votingregister::Candidate>(dbClient);

		//init keygen and secret key
		seal::KeyGenerator keygen(*(this->context));
		seal::SecretKey secretKey = keygen.secret_key();
		std::stringstream secretKeyStr;
		secretKey.save(secretKeyStr);
		newPoll.setSecretkey(secretKeyStr.str());

		std::vector<uint64_t>* votes = new std::vector(reqJson["contacts"].size(), 0ULL);
		seal::BatchEncoder batchEncoder(*(this->context));
		seal::Plaintext zeroArrayPlain;
		batchEncoder.encode(*votes, zeroArrayPlain);

		seal::PublicKey publicKey;
		keygen.create_public_key(publicKey);
		seal::Encryptor encryptor(*(this->context), publicKey);
		seal::Serializable<seal::Ciphertext> ciphertextZero = encryptor.encrypt(zeroArrayPlain);
		std::stringstream strstream;
		ciphertextZero.save(strstream);
		std::string zeroStr = strstream.str();
		newPoll.setVotes(zeroStr);

		pollMapper.insert(newPoll);
		
		int32_t order = 1;
		for (auto& i : reqJson["contacts"]) {
			drogon_model::votingregister::Candidate newCandidate;
			std::string name = i["name"].asString();
			newCandidate.setName(name);
			newCandidate.setOrd(order);
			newCandidate.setPoll(newPoll.getValueOfId());
			candidateMapper.insert(newCandidate);
			order++;
		}
		delete votes;
		HttpResponsePtr resp = HttpResponse::newHttpResponse();
		resp->setStatusCode(k201Created);
	}

	void VotingController::submitVote(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
		//TODO: add DB insertion for votes;
		auto start = std::chrono::steady_clock::now();
		
		auto candidatesPromise = dbClient->execSqlSync("SELECT COUNT(name) FROM candidate WHERE poll=$1", 53);
		int32_t candidateCount = candidatesPromise[0]["count"].as<int32_t>();
		LOG_INFO << "Candidate count: " << candidateCount;
		auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
		auto& reqJson = *(req->getJsonObject());
		std::string voteString = reqJson["vote"].asString();
		std::string votingId = reqJson["poll"].asString();
		std::stringstream strstream;
		seal::Ciphertext ciphertext;

		size_t bin_size = voteString.size() / 4 * 3;
		unsigned char* bin = new unsigned char[bin_size];
		//LOG_INFO << "Before base64 decode";
		if (sodium_base642bin(bin, bin_size,
			voteString.c_str(), voteString.size(),
			"", &bin_size,
			NULL, sodium_base64_VARIANT_ORIGINAL) != 0) {
			LOG_ERROR<<"Base64 Error: Failed to decode string";
		}
		
		std::string stringbin(reinterpret_cast<char const*>(bin), bin_size);
		std::stringstream().swap(strstream); //flush the contents of the stream
		strstream<<stringbin;
		ciphertext.load(*(this->context), strstream);
		delete[] bin;
		drogon::orm::Mapper<drogon_model::votingregister::Poll> pollMapper = drogon::orm::Mapper<drogon_model::votingregister::Poll>(dbClient);

		pollMapper.findOne(
			drogon::orm::Criteria(drogon_model::votingregister::Poll::Cols::_id, drogon::orm::CompareOperator::EQ, std::stoi(votingId)),
			[=](const drogon_model::votingregister::Poll& poll) {
				drogon::orm::Mapper<drogon_model::votingregister::Poll> pollMapper = drogon::orm::Mapper<drogon_model::votingregister::Poll>(dbClient);
				drogon::orm::Mapper<drogon_model::votingregister::Vote> voteMapper = drogon::orm::Mapper<drogon_model::votingregister::Vote>(dbClient);

				seal::Evaluator evaluator(*(this->context));
				seal::SecretKey secretkey;
				poll.getValueOfSecretkeyAsString();
				std::stringstream strstream;
				strstream << poll.getValueOfSecretkeyAsString();
				secretkey.load(*(this->context), strstream);
				seal::Decryptor decryptor(*(this->context), secretkey);
				seal::Encryptor encryptor(*(this->context), secretkey);

				std::stringstream().swap(strstream);
				std::string pollVotes = poll.getValueOfVotesAsString();
				
				seal::Ciphertext ciphertextVotes;
				strstream<<pollVotes;
				ciphertextVotes.load(*(this->context), strstream);
				std::cout << "Noise budget before adding: " << decryptor.invariant_noise_budget(ciphertextVotes) << "\n";
				evaluator.add_inplace(ciphertextVotes, ciphertext);
				std::cout << "After adding: " << decryptor.invariant_noise_budget(ciphertextVotes) << "\n";

				seal::Plaintext plaintext;
				decryptor.decrypt(ciphertextVotes, plaintext);
				seal::BatchEncoder batchEncoder(*(this->context));
				std::vector<uint64_t> result(3, 0ULL);
				batchEncoder.decode(plaintext, result);
				std::cout << result.size()<<"\n";
				
				for (int i = 0; i < candidateCount; i++) {
					std::cout << result[i] << "\n";
				}


				//seal::Serializable<seal::Ciphertext> ciphertextVotes2(ciphertextVotes);
				std::stringstream().swap(strstream);
				ciphertextVotes.save(strstream);
				std::string ciphertextVotesStr = strstream.str();

				drogon_model::votingregister::Poll newPoll;
				newPoll.setVotes(ciphertextVotesStr);
				newPoll.setId(poll.getValueOfId());
				newPoll.setTitle(poll.getValueOfTitle());
				newPoll.setSecretkey(poll.getValueOfSecretkeyAsString());
				pollMapper.update(newPoll, 
					[=](const size_t vals ) {
						HttpResponsePtr resp = HttpResponse::newHttpResponse();
						resp->setStatusCode(k202Accepted);
						LOG_INFO << "Elapsed(ms)=" << since(start).count();
						(*callbackPtr)(resp);
					},
					[=](const drogon::orm::DrogonDbException& e) {
						LOG_ERROR << "error:" << e.base().what();
						HttpResponsePtr resp = HttpResponse::newHttpResponse();
						resp->setStatusCode(HttpStatusCode::k500InternalServerError);
						(*callbackPtr)(resp);
					});
			},
			[&callbackPtr](const drogon::orm::DrogonDbException& e) {
				LOG_ERROR << e.base().what();
				auto resp = HttpResponse::newHttpResponse();
				resp->setStatusCode(HttpStatusCode::k500InternalServerError);
				(*callbackPtr)(resp);
			}
		);


	}

	void VotingController::testMethod(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
		//TODO: everything
		auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
		auto& reqJson = *(req->getJsonObject());
		


	}

	void VotingController::getPoll(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, std::string votingId) const {
		
		auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));

		auto body = std::make_shared<Json::Value>(Json::objectValue);
		
		auto pollId = std::make_shared<int32_t>(std::stoi(votingId));
		int32_t pollId2 = std::stoi(votingId);
		drogon::orm::Mapper<drogon_model::votingregister::Poll> pollMapper = drogon::orm::Mapper<drogon_model::votingregister::Poll>(dbClient);

		pollMapper.findOne(
			drogon::orm::Criteria(drogon_model::votingregister::Poll::Cols::_id, drogon::orm::CompareOperator::EQ, pollId2),
			[=](const drogon_model::votingregister::Poll& poll) {
					drogon::orm::Mapper<drogon_model::votingregister::Candidate> candidateMapper = drogon::orm::Mapper<drogon_model::votingregister::Candidate>(app().getDbClient());
					drogon_model::votingregister::Poll poll2 = poll;
					candidateMapper.findBy(
						drogon::orm::Criteria(drogon_model::votingregister::Candidate::Cols::_poll, drogon::orm::CompareOperator::EQ, pollId2), 
						[=](const std::vector<drogon_model::votingregister::Candidate>& candidates) {
							Json::Value jsonBody(Json::objectValue);
							auto resp = HttpResponse::newHttpResponse();
							resp->setStatusCode(HttpStatusCode::k200OK);
							jsonBody["poll"] = poll2.toJson();
							jsonBody["candidates"] = Json::Value(Json::arrayValue);
							for (auto& candidate : candidates) {
								jsonBody["candidates"].append(candidate.toJson());
							}
							seal::SecretKey secretkey;
							std::stringstream strstream;
							std::string str(poll2.getValueOfSecretkey().begin(), poll2.getValueOfSecretkey().end());
							strstream << str;
							secretkey.load(*(this->context), strstream);
							seal::KeyGenerator pubkeygen(*(this->context), secretkey);
							std::stringstream().swap(strstream); //flush the contents of the stream
							seal::Serializable<seal::PublicKey> publicKey = pubkeygen.create_public_key();
							seal::PublicKey publicKey2;
							pubkeygen.create_public_key(publicKey2);
							publicKey.save(strstream);

							//base64

							const size_t keyLen = strstream.str().size();
							const size_t  base64_max_len = sodium_base64_encoded_len(keyLen, sodium_base64_VARIANT_ORIGINAL);
							std::string base64_str(base64_max_len - 1, 0);
							char* encoded_str_char = sodium_bin2base64(
								base64_str.data(),
								base64_max_len,
								(unsigned char*) strstream.str().data(),
								keyLen,
								sodium_base64_VARIANT_ORIGINAL
							);

							if (encoded_str_char == NULL) {
								throw "Base64 Error: Failed to encode string";
							}
							
							jsonBody["poll"]["secretkey"] = base64_str;
							
							resp->setBody(jsonBody.toStyledString());
							(*callbackPtr)(resp);
						},
						[&](const drogon::orm::DrogonDbException& e) {
							LOG_ERROR << e.base().what();
							auto resp = HttpResponse::newHttpResponse();
							resp->setStatusCode(HttpStatusCode::k500InternalServerError);
							(*callbackPtr)(resp);
						});
					
			},
			[&callbackPtr](const drogon::orm::DrogonDbException& e) {
				LOG_ERROR << e.base().what();
				seal::PublicKey publicKey;
				auto resp = HttpResponse::newHttpResponse();
				resp->setStatusCode(HttpStatusCode::k500InternalServerError);
				(*callbackPtr)(resp);
			}
		);


		


	}


	void VotingController::results(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, std::string votingId) const {
		//TODO: everything
		int32_t votingId2 = std::stoi(votingId);

		auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));


	}


}
