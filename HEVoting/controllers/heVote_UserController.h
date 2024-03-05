#pragma once

#include <drogon/HttpController.h>
#include <models/Users.h>
#include <models/Admins.h>
#include <sodium/crypto_pwhash_argon2id.h>
#include <sodium/randombytes.h>
#include <typeinfo>

using namespace drogon;

namespace heVote
{
    class UserController : public drogon::HttpController<UserController>
    {
    public:
        METHOD_LIST_BEGIN
            // use METHOD_ADD to add your custom processing function here;
            // METHOD_ADD(UserController::get, "/{2}/{1}", Get); // path is /heVote/UserController/{arg2}/{arg1}
            // METHOD_ADD(UserController::your_method_name, "/{1}/{2}/list", Get); // path is /heVote/UserController/{arg1}/{arg2}/list
            // ADD_METHOD_TO(UserController::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list

            ADD_METHOD_TO(UserController::login, "/login", Post);
            ADD_METHOD_TO(UserController::adminLogin, "/admin-login", Post);
            ADD_METHOD_TO(UserController::registerAdmin, "/admin-register", Post);
            ADD_METHOD_TO(UserController::registerUser, "/register", Post);
            

        METHOD_LIST_END
            // your declaration of processing function maybe like this:
            // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
            // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;

            void login(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
            void adminLogin(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
            void registerUser(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
            void registerAdmin(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

            UserController();
    private:
        drogon::orm::DbClientPtr dbClient;
    };

}
