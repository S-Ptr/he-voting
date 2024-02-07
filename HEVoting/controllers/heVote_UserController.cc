#include "heVote_UserController.h"

#include "models/Users.h"

using namespace heVote;

// Add definition of your processing function here

UserController::UserController(): dbClient(app().getDbClient()) {
}

void UserController::login(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
{
	auto mapper = drogon::orm::Mapper<drogon_model::votingregister::Users>(dbClient);
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto& reqJson = *(req->getJsonObject());
	std::string username = reqJson["username"].asString();
	std::string pass = reqJson["pass"].asString();
	LOG_INFO << "Login attempt by " << username;

	mapper.findOne(
		drogon::orm::Criteria(drogon_model::votingregister::Users::Cols::_username, drogon::orm::CompareOperator::EQ, username),
		[=](const drogon_model::votingregister::Users& user) {
			if (crypto_pwhash_argon2id_str_verify(user.getValueOfPass().c_str(), pass.c_str(), pass.length()) != 0) {
				auto resp = HttpResponse::newHttpResponse();
				resp->setStatusCode(HttpStatusCode::k401Unauthorized);
				(*callbackPtr)(resp);
			}
			else {
				req->session()->insert("user", username);
				auto json = Json::Value();
				json["user"] = username;
				json["session_id"] = req->session()->sessionId();
				auto resp = HttpResponse::newHttpJsonResponse(json);
				(*callbackPtr)(resp);
			}
		},
		[callbackPtr](const drogon::orm::DrogonDbException& e) {
			LOG_ERROR << e.base().what();
			auto resp = HttpResponse::newHttpResponse();
			resp->setStatusCode(HttpStatusCode::k500InternalServerError);
			(*callbackPtr)(resp);
		}
	);
}

void UserController::adminLogin(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
{
	auto mapper = drogon::orm::Mapper<drogon_model::votingregister::Admins>(dbClient);
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto& reqJson = *(req->getJsonObject());
	std::string username = reqJson["username"].asString();
	std::string pass = reqJson["pass"].asString();
	LOG_INFO << "Login attempt by " << username;

	mapper.findOne(
		drogon::orm::Criteria(drogon_model::votingregister::Admins::Cols::_username, drogon::orm::CompareOperator::EQ, username),
		[=](const drogon_model::votingregister::Admins& user) {
			if (crypto_pwhash_argon2id_str_verify(user.getValueOfPass().c_str(), pass.c_str(), pass.length()) != 0) {
				auto resp = HttpResponse::newHttpResponse();
				resp->setStatusCode(HttpStatusCode::k401Unauthorized);
				(*callbackPtr)(resp);
			}
			else {
				req->session()->insert("user", username);
				auto json = Json::Value();
				json["user"] = username;
				json["session_id"] = req->session()->sessionId();
				auto resp = HttpResponse::newHttpJsonResponse(json);
				(*callbackPtr)(resp);
			}
		},
		[callbackPtr](const drogon::orm::DrogonDbException& e) {
			LOG_ERROR << e.base().what();
			auto resp = HttpResponse::newHttpResponse();
			resp->setStatusCode(HttpStatusCode::k500InternalServerError);
			(*callbackPtr)(resp);
		}
	);
}

void UserController::registerUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
{
	auto mapper = drogon::orm::Mapper<drogon_model::votingregister::Users>(dbClient);
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));

	auto& reqJson = *(req->getJsonObject());
	std::string username = reqJson["username"].asString();
	std::string pass = reqJson["pass"].asString();



	mapper.findBy( //easier to check if user exists this way than with findOne
		drogon::orm::Criteria(drogon_model::votingregister::Users::Cols::_username, drogon::orm::CompareOperator::EQ, username),
		[&mapper, &callbackPtr, username, pass](const std::vector<drogon_model::votingregister::Users>& users) {
			if(users.empty()){
				char hashpass[crypto_pwhash_argon2id_STRBYTES];
				unsigned char salt[crypto_pwhash_argon2id_SALTBYTES];
				randombytes_buf(salt, sizeof(salt));

				drogon_model::votingregister::Users newUser;
				newUser.setUsername(username);

				if (crypto_pwhash_argon2id_str(hashpass, pass.c_str(), pass.length(),
					crypto_pwhash_argon2id_OPSLIMIT_INTERACTIVE, crypto_pwhash_argon2id_MEMLIMIT_INTERACTIVE) != 0) {
					LOG_ERROR << "Error hashing password!\n";
					HttpResponsePtr resp = HttpResponse::newHttpResponse();
					resp->setStatusCode(k500InternalServerError);
					(*callbackPtr)(resp);
				}
				std::string hashpassStr(hashpass);
				newUser.setPass(hashpassStr);
				mapper.insert(newUser,
					[&callbackPtr](const drogon_model::votingregister::Users& user) {
						auto resp = HttpResponse::newHttpResponse();
						resp->setStatusCode(HttpStatusCode::k201Created);
						(*callbackPtr)(resp);
					},
					[&callbackPtr](const drogon::orm::DrogonDbException& e) {
						LOG_ERROR << e.base().what();
						auto resp = HttpResponse::newHttpResponse();
						resp->setStatusCode(HttpStatusCode::k500InternalServerError);
						(*callbackPtr)(resp);
					}
					);
			}else {
				auto resp = HttpResponse::newHttpResponse();
				resp->setStatusCode(HttpStatusCode::k406NotAcceptable);
				(*callbackPtr)(resp);
			}
		},
		[callbackPtr](const drogon::orm::DrogonDbException& e) {
			LOG_ERROR << e.base().what();
			auto resp = HttpResponse::newHttpResponse();
			resp->setStatusCode(HttpStatusCode::k500InternalServerError);
			(*callbackPtr)(resp);
		}
	);
}

void UserController::registerAdmin(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
{
	auto mapper = drogon::orm::Mapper<drogon_model::votingregister::Admins>(dbClient);
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));

	auto& reqJson = *(req->getJsonObject());
	std::string username = reqJson["username"].asString();
	std::string pass = reqJson["pass"].asString();

	mapper.findBy( //easier to check if user exists this way than with findOne
		drogon::orm::Criteria(drogon_model::votingregister::Admins::Cols::_username, drogon::orm::CompareOperator::EQ, username),
		[&mapper, &callbackPtr, username, pass](const std::vector<drogon_model::votingregister::Admins>& users) {
			if (users.empty()) {
				char hashpass[crypto_pwhash_argon2id_STRBYTES];
				unsigned char salt[crypto_pwhash_argon2id_SALTBYTES];
				randombytes_buf(salt, sizeof(salt));

				drogon_model::votingregister::Admins newUser;
				newUser.setUsername(username);

				if (crypto_pwhash_argon2id_str(hashpass, pass.c_str(), pass.length(),
					crypto_pwhash_argon2id_OPSLIMIT_INTERACTIVE, crypto_pwhash_argon2id_MEMLIMIT_INTERACTIVE) != 0) {
					LOG_ERROR << "Error hashing password!\n";
					HttpResponsePtr resp = HttpResponse::newHttpResponse();
					resp->setStatusCode(k500InternalServerError);
					(*callbackPtr)(resp);
				}
				std::string hashpassStr(hashpass);
				newUser.setPass(hashpassStr);
				mapper.insert(newUser,
					[&callbackPtr](const drogon_model::votingregister::Admins& user) {
						auto resp = HttpResponse::newHttpResponse();
						resp->setStatusCode(HttpStatusCode::k201Created);
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
			else {
				auto resp = HttpResponse::newHttpResponse();
				resp->setStatusCode(HttpStatusCode::k406NotAcceptable);
				(*callbackPtr)(resp);
			}
		},
		[callbackPtr](const drogon::orm::DrogonDbException& e) {
			LOG_ERROR << e.base().what();
			auto resp = HttpResponse::newHttpResponse();
			resp->setStatusCode(HttpStatusCode::k500InternalServerError);
			(*callbackPtr)(resp);
		}
	);
}
