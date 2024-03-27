#pragma once

#include <drogon/HttpController.h>
#include <models/Poll.h>
#include <models/Vote.h>
#include <models/Candidate.h>
#include <boost/beast/core/detail/base64.hpp>
#include <sodium/utils.h>
#include <chrono>

//seal includes
#include <seal/encryptionparams.h>
#include <seal/context.h>
#include <seal/ciphertext.h>
#include <seal/plaintext.h>
#include <seal/encryptor.h>
#include <seal/decryptor.h>
#include <seal/evaluator.h>
#include <seal/secretkey.h>
#include <seal/publickey.h>
#include <seal/keygenerator.h>
#include <seal/batchencoder.h>

#define VOTINGCONTROLLER_POLYMOD_DEGREE 4096

using namespace drogon;
namespace heVote {
    class VotingController : public drogon::HttpController<VotingController>
    {
    public:
        METHOD_LIST_BEGIN
            // use METHOD_ADD to add your custom processing function here;
            // METHOD_ADD(VotingController::get, "/{2}/{1}", Get); // path is /VotingController/{arg2}/{arg1}
            // METHOD_ADD(VotingController::your_method_name, "/{1}/{2}/list", Get); // path is /VotingController/{arg1}/{arg2}/list
            // ADD_METHOD_TO(VotingController::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list
            // 
            // 
            //-- ADD_METHOD_TO(VotingController::showPoll, "/poll/{1}", Get);
            ADD_METHOD_TO(VotingController::createPoll, "/create-poll", Post);
            ADD_METHOD_TO(VotingController::getPoll, "/poll/{1}", Get);
            ADD_METHOD_TO(VotingController::getActivePolls, "/get-active-polls", Get);
            ADD_METHOD_TO(VotingController::getFinishedPolls, "/get-finished-polls", Get);
            ADD_METHOD_TO(VotingController::submitVoteTest, "/submit-vote-test", Post);
            ADD_METHOD_TO(VotingController::submitVote, "/submit-vote", Post);
            ADD_METHOD_TO(VotingController::VotingNoiseTest, "/testing-some-stuff", Get); //comment this one out if you don't want it.
        METHOD_LIST_END
            // your declaration of processing function maybe like this:
            // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
            // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;
            // 

            /**
            * @brief constructor
            * @param pJson The json object to construct a new instance.
            * @param pMasqueradingVector The aliases of table columns.
            */
            void createPoll(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback);


            void getPoll(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, std::string votingId) const;
            void getActivePolls(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
            void getFinishedPolls(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback) const;
            void submitVoteTest(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
            void submitVote(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);
            void VotingNoiseTest(const HttpRequestPtr& req, std::function<void(const HttpResponsePtr&)>&& callback);

        VotingController();
        ~VotingController();

    private:
        template <
            class result_t = std::chrono::milliseconds,
            class clock_t = std::chrono::steady_clock,
            class duration_t = std::chrono::milliseconds
        >
        auto since(std::chrono::time_point<clock_t, duration_t> const& start)
        {
            return std::chrono::duration_cast<result_t>(clock_t::now() - start);
        }
        seal::EncryptionParameters* eParams;
        seal::SEALContext* context;
        seal::Evaluator* evaluator;
        drogon::orm::DbClientPtr dbClient;
    };
}
