#include "VotingController.h"



namespace heVote {

	VotingController::VotingController(): dbClient(app().getDbClient()){
		//init encryption parameters
		eParams = new seal::EncryptionParameters(seal::scheme_type::bgv);
		//********************************************
		//Poly modulus degree gives us the number of maximum candidates we can have in a poll
		//Due to how batch encoding works, the ciphertext stores the maximum amount of candidates regardless of how many candidates the poll truly has
		eParams->set_poly_modulus_degree(VOTINGCONTROLLER_POLYMOD_DEGREE);
		//********************************************
		
		//********************************************
		// The plain modulus gives us the maximum number of votes a candidate can have. 
		// Increasing it decreases the noise budget and increases the rate of it's consumption
		// bit size of 32 enables support for up to 4,294,828,032 votes per candidate, which should be enough for anything that doesn't have 
		// more than 50% of the world's population sharing the same opinion. It's also a nice round computable number.
		// 
		// Unfortunately, the noise budget afforded by that bit size isn't capable of handling the amount of votes. So we shall use 29 bits instead.
		//
		// After doing some calculations and running tests using the votingNoiseTest method, I'm going to use 31 bits.
		// It hits that sweet spot where neither the plaintext values, nor the noise budget bring down one another, leading to a nice 2 billion-ish votes available.
		// 
		eParams->set_plain_modulus(seal::PlainModulus::Batching(VOTINGCONTROLLER_POLYMOD_DEGREE,31)); 
		//********************************************


		//********************************************
		//The coefficient modulus is a product of prime numbers, represented by a 
		//vector of bit-lengths. The bit-lengths must be at most 60, and 
		// 
		// The bit-length of the coefficient modulus directly affects the 
		// security level of the scheme, and, more importantly, the total noise budget
		// 
		// On the Microsoft SEAL GitHub page, among the examples,
		// there is a table of maximum values for the coefficient modulus, relative to the poly modulus degree
		//	+------------------------------------------------------+
		//	| poly_modulus_degree | max coeff_modulus bit - length |
		//	+-------------------- - +------------------------------+
		//	| 1024                |   27                           |
		//	| 2048                |   54                           |
		//	| 4096                |   109                          |
		//	| 8192                |   218                          |
		//	| 16384               |   438                          |
		//	| 32768               |   881                          |
		//	+-------------------- - +------------------------------+

		//Considering the previous choice of 4096 as the poly mod degree, as well as the table above
		// 
		// **************
		// ***ATTEMPTS***
		// **************
		// 
		// let's start with 60+48 = 108 => gives a noise budget of 18, not that great. Runtime ~20ms in preferred method, ~40ms in the test method
		// 60+49 = 109 => gives a noise budget of 20, still not that great.
		// 36+36+36 = 108 => gives a noise budget of 32, which is better. Unfortunately, it increases elapsed time from 40-50ms to 100-110ms in the test method
		// After figuring out that the coeff modulus also affects the noise budget, I managed to squeeze out 35 bits of noise budget by using 36+36+36

		eParams->set_coeff_modulus(seal::CoeffModulus::Create(VOTINGCONTROLLER_POLYMOD_DEGREE, { 36,36,36 }));
		//********************************************
		
		
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
		//TODO: add asynchronicity and transaction. Check out why the fuck it be bugging bro
		//init JSON and extend callback lifetime
		auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
		auto& reqJson = *(req->getJsonObject());
		//init mappers

		//init poll with title
		drogon_model::votingregister::Poll newPoll;
		newPoll.setTitle(reqJson["title"].asString());

		auto transaction =  dbClient->newTransaction();
		//init mappers
		drogon::orm::Mapper<drogon_model::votingregister::Poll> pollMapper = drogon::orm::Mapper<drogon_model::votingregister::Poll>(transaction);
		drogon::orm::Mapper<drogon_model::votingregister::Candidate> candidateMapper = drogon::orm::Mapper<drogon_model::votingregister::Candidate>(transaction);

		//init keygen and secret key
		seal::KeyGenerator keygen(*(this->context));
		seal::SecretKey secretKey = keygen.secret_key();
		std::stringstream secretKeyStr;
		secretKey.save(secretKeyStr);
		newPoll.setSecretkey(secretKeyStr.str());

		//encode votes as vector
		std::vector<uint64_t>* votes = new std::vector(reqJson["contacts"].size(), 0ULL);
		seal::BatchEncoder batchEncoder(*(this->context));
		seal::Plaintext zeroArrayPlain;
		batchEncoder.encode(*votes, zeroArrayPlain);

		//encrypt votes
		seal::PublicKey publicKey;
		keygen.create_public_key(publicKey);
		seal::Encryptor encryptor(*(this->context), publicKey);
		seal::Serializable<seal::Ciphertext> ciphertextZero = encryptor.encrypt(zeroArrayPlain);
		std::stringstream strstream;
		ciphertextZero.save(strstream);
		std::string zeroStr = strstream.str();
		newPoll.setVotes(zeroStr);


		try {
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
		catch (const drogon::orm::DrogonDbException& e) {
			transaction->rollback();
			delete votes;
			LOG_ERROR << e.base().what();
			HttpResponsePtr resp = HttpResponse::newHttpResponse();
			resp->setStatusCode(HttpStatusCode::k500InternalServerError);
			(*callbackPtr)(resp);
			return;
		}
		
		
	}

	void VotingController::submitVote(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
		auto start = std::chrono::steady_clock::now();
		auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
		std::shared_ptr<drogon::orm::Transaction> transaction = dbClient->newTransaction();

		auto& reqJson = *(req->getJsonObject());
		std::string voteString = reqJson["vote"].asString();
		int32_t votingId = reqJson["poll"].as<int32_t>();
		int32_t userId = reqJson["user"].as<int32_t>();
		std::stringstream strstream;
		seal::Ciphertext ciphertext;

		size_t bin_size = voteString.size() / 4 * 3;
		unsigned char* bin = new unsigned char[bin_size];
		//LOG_INFO << "Before base64 decode";
		if (sodium_base642bin(bin, bin_size,
			voteString.c_str(), voteString.size(),
			"", &bin_size,
			NULL, sodium_base64_VARIANT_ORIGINAL) != 0) {
			LOG_ERROR << "Base64 Error: Failed to decode string";
			return;
		}

		std::string stringbin(reinterpret_cast<char const*>(bin), bin_size);
		std::stringstream().swap(strstream); //flush the contents of the stream
		strstream << stringbin;
		ciphertext.load(*(this->context), strstream);
		delete[] bin;
		//Ciphertext is now loaded

		try {
			auto voteInsert =  transaction->execSqlAsyncFuture("INSERT INTO vote (user_id, poll_id, vote) VALUES($1 , $2 , $3 )", userId, votingId, voteString);
			voteInsert.get(); //wait for the insertion to complete
			const drogon::orm::Result& result = transaction->execSqlSync("SELECT * FROM poll WHERE id=$1 FOR UPDATE", votingId);

			if (result.size() != 1) {
				LOG_ERROR << "Poll with id '" << votingId << "' not found";
				transaction->rollback();
				HttpResponsePtr resp = HttpResponse::newHttpResponse();
				resp->setStatusCode(HttpStatusCode::k404NotFound);
				(*callbackPtr)(resp);
				return;
			}
			std::stringstream strstream;
			seal::Evaluator evaluator(*(this->context));
			drogon::orm::Result::Reference poll = result.front();
			std::string key = poll["secretkey"].as<std::string>();
			std::string votes = poll["votes"].as<std::string>();
			seal::SecretKey secretkey;
			seal::Ciphertext ciphertextVotes;
			strstream << votes;
			ciphertextVotes.load(*(this->context), strstream);
			std::stringstream().swap(strstream);
			evaluator.add_inplace(ciphertextVotes, ciphertext);

			std::stringstream().swap(strstream);
			ciphertextVotes.save(strstream);
			std::vector<char> ciphertextVotesVector;
			ciphertextVotesVector.assign(std::istreambuf_iterator<char>(strstream), std::istreambuf_iterator<char>());
			transaction->execSqlAsync("UPDATE poll SET votes=$1 WHERE id=$2",
				[=](const drogon::orm::Result& result) {
					LOG_INFO << "Elapsed(ms)=" << since(start).count();
					HttpResponsePtr resp = HttpResponse::newHttpResponse();
					resp->setStatusCode(k202Accepted);
					(*callbackPtr)(resp);
				},
				[=](const drogon::orm::DrogonDbException& e) {
					transaction->rollback();
					LOG_ERROR << e.base().what();
					HttpResponsePtr resp = HttpResponse::newHttpResponse();
					resp->setStatusCode(HttpStatusCode::k500InternalServerError);
					(*callbackPtr)(resp);
					return;
				},
				ciphertextVotesVector, votingId);

		}catch(const drogon::orm::DrogonDbException& e){
			transaction->rollback();
			LOG_ERROR << e.base().what();
			HttpResponsePtr resp = HttpResponse::newHttpResponse();
			resp->setStatusCode(HttpStatusCode::k500InternalServerError);
			(*callbackPtr)(resp);
			return;
		}
	}

	void VotingController::submitVoteTest(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
		//TODO: add DB insertion for votes;
		auto start = std::chrono::steady_clock::now();
		auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
		auto& reqJson = *(req->getJsonObject());
		std::string voteString = reqJson["vote"].asString();
		std::string votingId = reqJson["poll"].asString();
		std::stringstream strstream;
		seal::Ciphertext ciphertext;
		auto candidatesPromise = dbClient->execSqlSync("SELECT COUNT(name) FROM candidate WHERE poll=$1", std::stoi(votingId));
		int32_t candidateCount = candidatesPromise[0]["count"].as<int32_t>();
		LOG_INFO << "Candidate count: " << candidateCount;


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
				std::stringstream strstream;
				strstream << poll.getValueOfSecretkeyAsString();
				secretkey.load(*(this->context), strstream);
				seal::Decryptor decryptor(*(this->context), secretkey);

				std::stringstream().swap(strstream);
				std::string pollVotes = poll.getValueOfVotesAsString();
				
				seal::Ciphertext ciphertextVotes;
				strstream<<pollVotes;
				ciphertextVotes.load(*(this->context), strstream);
				std::cout << "Noise budget before adding: " << decryptor.invariant_noise_budget(ciphertextVotes) << "\n";
				evaluator.add_inplace(ciphertextVotes, ciphertext);
				std::cout << "After adding: " << decryptor.invariant_noise_budget(ciphertextVotes) << "\n";
				if (decryptor.invariant_noise_budget(ciphertextVotes)==0) {
					LOG_ERROR << "Noise budget reached critical underfunding. Votes will need to be re-tallied for poll '" << poll.getValueOfTitle()<<"'";
					std::abort();
				}

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
						LOG_ERROR << e.base().what();
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

	void VotingController::getPoll(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback, std::string votingId) const {
		
		auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));

		auto& reqJson = *(req->getJsonObject());
		
		auto pollId = std::make_shared<int32_t>(std::stoi(votingId));
		int32_t pollId2 = std::stoi(votingId);
		drogon::orm::Mapper<drogon_model::votingregister::Poll> pollMapper = drogon::orm::Mapper<drogon_model::votingregister::Poll>(dbClient);

		pollMapper.findOne(
			drogon::orm::Criteria(drogon_model::votingregister::Poll::Cols::_id, drogon::orm::CompareOperator::EQ, pollId2),
			[=](const drogon_model::votingregister::Poll& poll) {
					drogon::orm::Mapper<drogon_model::votingregister::Candidate> candidateMapper = drogon::orm::Mapper<drogon_model::votingregister::Candidate>(dbClient);
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
							try {
								
								std::string str(poll2.getValueOfSecretkey().begin(), poll2.getValueOfSecretkey().end());
								strstream << str;
								secretkey.load(*(this->context), strstream);
								seal::KeyGenerator pubkeygen(*(this->context), secretkey);
								std::stringstream().swap(strstream); //flush the contents of the stream
								seal::Serializable<seal::PublicKey> publicKey = pubkeygen.create_public_key();
								seal::PublicKey publicKey2;
								pubkeygen.create_public_key(publicKey2);
								publicKey.save(strstream);
							}
							catch (const std::exception& e) {
								LOG_ERROR << e.what();
								resp->setStatusCode(HttpStatusCode::k500InternalServerError);
								(*callbackPtr)(resp);
								return;
							}

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
								LOG_ERROR << "Base64 Error: Failed to encode string";
								auto resp = HttpResponse::newHttpResponse();
								resp->setStatusCode(HttpStatusCode::k500InternalServerError);
								(*callbackPtr)(resp);
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
				auto resp = HttpResponse::newHttpResponse();
				resp->setStatusCode(HttpStatusCode::k500InternalServerError);
				(*callbackPtr)(resp);
			}
		);

	}

	void VotingController::getAllPolls(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const {
		auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
		dbClient->execSqlAsync("SELECT id, title, until FROM poll", [=](const drogon::orm::Result& polls) {
			Json::Value jsonBody(Json::objectValue);
			jsonBody["polls"] = Json::Value(Json::arrayValue);
			for (auto& poll : polls) {
				Json::Value singlePoll;
				singlePoll["id"] = poll["id"].as<int32_t>();
				singlePoll["title"] = poll["title"].as<std::string>();
				jsonBody["polls"].append(singlePoll);
			}
			auto resp = HttpResponse::newHttpResponse();
			resp->setBody(jsonBody.toStyledString());
			resp->setStatusCode(HttpStatusCode::k200OK);
			(*callbackPtr)(resp);
			},
			[&callbackPtr](const drogon::orm::DrogonDbException& e) {
				LOG_ERROR << e.base().what();
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


	//Method for testing how many operations can a ciphertext take before it gets ruined
	void VotingController::VotingNoiseTest(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) {
		auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
		uint64_t count = 0;
		int candidates = 20;

		int polyModulusDegree = 4096;
		seal::EncryptionParameters encParams1(seal::scheme_type::bgv);
		encParams1.set_poly_modulus_degree(polyModulusDegree);
		encParams1.set_plain_modulus(seal::PlainModulus::Batching(polyModulusDegree, 31));
		encParams1.set_coeff_modulus(seal::CoeffModulus::Create(polyModulusDegree, { 36,36,36 }));
		seal::SEALContext context1(encParams1);
		seal::KeyGenerator keygen1(context1);
		seal::Evaluator evaluator(context1);

		seal::PublicKey publicKey1;
		keygen1.create_public_key(publicKey1);
		seal::Encryptor encryptor1(context1, publicKey1);
		seal::Decryptor decryptor(context1, keygen1.secret_key());
		seal::BatchEncoder encoder(context1);

		seal::Ciphertext mainCiphertext;

		std::vector<uint64_t>* votes = new std::vector(candidates, 0ULL);
		seal::BatchEncoder batchEncoder(*(this->context));
		seal::Plaintext zeroArrayPlain;
		batchEncoder.encode(*votes, zeroArrayPlain);
		delete votes;

		encryptor1.encrypt(zeroArrayPlain, mainCiphertext);
		LOG_INFO << "Polynomial modulus degree: " << polyModulusDegree;
		LOG_INFO << "Plain modulus: " << encParams1.plain_modulus().value();
		int noiseBudget = decryptor.invariant_noise_budget(mainCiphertext);
		LOG_INFO << "Initial noise budget: " << noiseBudget;
		unsigned long long worstEstimate = 2; 
		for(int i = 2; i < noiseBudget; i++){
			worstEstimate*=2;
			worstEstimate--;
		}
		LOG_INFO << "Worst case estimate for how many votes we can support: " << worstEstimate;

		seal::PublicKey publicKey;
		keygen1.create_public_key(publicKey);
		seal::Encryptor encryptor(context1, publicKey);
		seal::Ciphertext ciphertext;
		std::vector<uint64_t> votes1(candidates, 0ULL);
		votes1[2] = 1;
		seal::Plaintext voteArray;
		batchEncoder.encode(votes1, voteArray);
		encryptor.encrypt(zeroArrayPlain, ciphertext);
		LOG_INFO << "Running simulation of voting: ";

		while (decryptor.invariant_noise_budget(mainCiphertext) != 0) {
			count++;
			int before = decryptor.invariant_noise_budget(mainCiphertext);
			evaluator.add_inplace(mainCiphertext, ciphertext);
			int after = decryptor.invariant_noise_budget(mainCiphertext);
			if(before !=after){
				LOG_INFO << "On operation "<< count << " Noise budget decreased from " << before << " to " << after;
			}
		}
		LOG_INFO << "Ciphertext unusable after " << count << " additions";
	}
}
