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
	
	auto users = dbClient->execSqlAsyncFuture("SELECT username, pass FROM users WHERE username=$1",username);
	
	try {
		auto result = users.get();
		auto user = result[0];
		if (crypto_pwhash_argon2id_str_verify(user["pass"].c_str(), pass.c_str(), pass.length()) != 0) {
			auto json = Json::Value();
			json["error"] = "Invalid password";
			auto resp = HttpResponse::newHttpJsonResponse(json);
			resp->setStatusCode(HttpStatusCode::k200OK);
			(*callbackPtr)(resp);
		}
		else {
			req->session()->insert("user", username);
			auto json = Json::Value();
			json["user"] = username;
			json["session_id"] = req->session()->sessionId();
			auto resp = HttpResponse::newHttpJsonResponse(json);
			resp->setStatusCode(HttpStatusCode::k200OK);
			(*callbackPtr)(resp);
		}


	}
	catch(drogon::orm::UnexpectedRows& e){ // no user found
		auto json = Json::Value();
		json["error"] = "No user found";
		auto resp = HttpResponse::newHttpJsonResponse(json);
		resp->setStatusCode(HttpStatusCode::k204NoContent);
		(*callbackPtr)(resp);
		return;
	} catch (drogon::orm::DrogonDbException& e){
		LOG_ERROR << e.base().what();
		LOG_ERROR << typeid(e).name();
		auto resp = HttpResponse::newHttpResponse();
		resp->setStatusCode(HttpStatusCode::k500InternalServerError);
		(*callbackPtr)(resp);
		return;
	}
}

void UserController::adminLogin(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback)
{
	auto mapper = drogon::orm::Mapper<drogon_model::votingregister::Users>(dbClient);
	auto callbackPtr = std::make_shared<std::function<void(const HttpResponsePtr&)>>(std::move(callback));
	auto& reqJson = *(req->getJsonObject());
	std::string username = reqJson["username"].asString();
	std::string pass = reqJson["pass"].asString();

	auto users = dbClient->execSqlAsyncFuture("SELECT username, pass FROM admins WHERE username=$1", username);

	try {
		auto result = users.get();
		auto user = result[0];
		if (crypto_pwhash_argon2id_str_verify(user["pass"].c_str(), pass.c_str(), pass.length()) != 0) {
			auto json = Json::Value();
			json["error"] = "Invalid password";
			auto resp = HttpResponse::newHttpJsonResponse(json);
			resp->setStatusCode(HttpStatusCode::k200OK);
			(*callbackPtr)(resp);
		}
		else {
			req->session()->insert("user", username);
			auto json = Json::Value();
			json["user"] = username;
			json["session_id"] = req->session()->sessionId();
			auto resp = HttpResponse::newHttpJsonResponse(json);
			resp->setStatusCode(HttpStatusCode::k200OK);
			(*callbackPtr)(resp);
		}


	}
	catch (drogon::orm::UnexpectedRows& e) { // no user found
		auto json = Json::Value();
		json["error"] = "No user found";
		auto resp = HttpResponse::newHttpJsonResponse(json);
		resp->setStatusCode(HttpStatusCode::k204NoContent);
		(*callbackPtr)(resp);
		return;
	}
	catch (drogon::orm::DrogonDbException& e) {
		LOG_ERROR << e.base().what();
		LOG_ERROR << typeid(e).name();
		auto resp = HttpResponse::newHttpResponse();
		resp->setStatusCode(HttpStatusCode::k500InternalServerError);
		(*callbackPtr)(resp);
		return;
	}
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
		[=](const std::vector<drogon_model::votingregister::Users>& users) {
			if(users.empty()){
				char hashpass[crypto_pwhash_argon2id_STRBYTES];
				unsigned char salt[crypto_pwhash_argon2id_SALTBYTES];
				randombytes_buf(salt, sizeof(salt));
				auto mapper = drogon::orm::Mapper<drogon_model::votingregister::Users>(dbClient);
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
					[=](const drogon_model::votingregister::Users& user) {
						auto json = reqJson;
						auto resp = HttpResponse::newHttpJsonResponse(json);
						resp->setStatusCode(HttpStatusCode::k201Created);
						(*callbackPtr)(resp);
					},
					[=](const drogon::orm::DrogonDbException& e) {
						LOG_ERROR << e.base().what();
						auto resp = HttpResponse::newHttpResponse();
						resp->setStatusCode(HttpStatusCode::k500InternalServerError);
						(*callbackPtr)(resp);
					}
					);
			}else {
				Json::Value body;
				body["error"] = "User already exists";
				auto resp = HttpResponse::newHttpJsonResponse(body);

				resp->setStatusCode(HttpStatusCode::k200OK);
				(*callbackPtr)(resp);
			}
		},
		[callbackPtr](const drogon::orm::DrogonDbException& e) {
			LOG_ERROR << e.base().what();
			LOG_ERROR<< typeid(e).name();
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
		[=](const std::vector<drogon_model::votingregister::Admins>& users) {
			if (users.empty()) {
				char hashpass[crypto_pwhash_argon2id_STRBYTES];
				unsigned char salt[crypto_pwhash_argon2id_SALTBYTES];
				randombytes_buf(salt, sizeof(salt));
				auto mapper = drogon::orm::Mapper<drogon_model::votingregister::Admins>(dbClient);
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
					[=](const drogon_model::votingregister::Admins& user) {
						auto json = reqJson;
						auto resp = HttpResponse::newHttpJsonResponse(json);
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
