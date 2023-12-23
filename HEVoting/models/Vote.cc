/**
 *
 *  Vote.cc
 *  DO NOT EDIT. This file is generated by drogon_ctl
 *
 */

#include "Vote.h"
#include <drogon/utils/Utilities.h>
#include <string>

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::votingregister;

const std::string Vote::Cols::_user_id = "user_id";
const std::string Vote::Cols::_poll_id = "poll_id";
const std::string Vote::Cols::_vote = "vote";
const std::string Vote::primaryKeyName = "";
const bool Vote::hasPrimaryKey = false;
const std::string Vote::tableName = "vote";

const std::vector<typename Vote::MetaData> Vote::metaData_={
{"user_id","int32_t","integer",4,0,0,1},
{"poll_id","int32_t","integer",4,0,0,1},
{"vote","std::vector<char>","bytea",0,0,0,0}
};
const std::string &Vote::getColumnName(size_t index) noexcept(false)
{
    assert(index < metaData_.size());
    return metaData_[index].colName_;
}
Vote::Vote(const Row &r, const ssize_t indexOffset) noexcept
{
    if(indexOffset < 0)
    {
        if(!r["user_id"].isNull())
        {
            userId_=std::make_shared<int32_t>(r["user_id"].as<int32_t>());
        }
        if(!r["poll_id"].isNull())
        {
            pollId_=std::make_shared<int32_t>(r["poll_id"].as<int32_t>());
        }
        if(!r["vote"].isNull())
        {
            auto str = r["vote"].as<string_view>();
            if(str.length()>=2&&
                str[0]=='\\'&&str[1]=='x')
            {
                vote_=std::make_shared<std::vector<char>>(drogon::utils::hexToBinaryVector(str.data()+2,str.length()-2));
            }
        }
    }
    else
    {
        size_t offset = (size_t)indexOffset;
        if(offset + 3 > r.size())
        {
            LOG_FATAL << "Invalid SQL result for this model";
            return;
        }
        size_t index;
        index = offset + 0;
        if(!r[index].isNull())
        {
            userId_=std::make_shared<int32_t>(r[index].as<int32_t>());
        }
        index = offset + 1;
        if(!r[index].isNull())
        {
            pollId_=std::make_shared<int32_t>(r[index].as<int32_t>());
        }
        index = offset + 2;
        if(!r[index].isNull())
        {
            auto str = r[index].as<string_view>();
            if(str.length()>=2&&
                str[0]=='\\'&&str[1]=='x')
            {
                vote_=std::make_shared<std::vector<char>>(drogon::utils::hexToBinaryVector(str.data()+2,str.length()-2));
            }
        }
    }

}

Vote::Vote(const Json::Value &pJson, const std::vector<std::string> &pMasqueradingVector) noexcept(false)
{
    if(pMasqueradingVector.size() != 3)
    {
        LOG_ERROR << "Bad masquerading vector";
        return;
    }
    if(!pMasqueradingVector[0].empty() && pJson.isMember(pMasqueradingVector[0]))
    {
        dirtyFlag_[0] = true;
        if(!pJson[pMasqueradingVector[0]].isNull())
        {
            userId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[0]].asInt64());
        }
    }
    if(!pMasqueradingVector[1].empty() && pJson.isMember(pMasqueradingVector[1]))
    {
        dirtyFlag_[1] = true;
        if(!pJson[pMasqueradingVector[1]].isNull())
        {
            pollId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[1]].asInt64());
        }
    }
    if(!pMasqueradingVector[2].empty() && pJson.isMember(pMasqueradingVector[2]))
    {
        dirtyFlag_[2] = true;
        if(!pJson[pMasqueradingVector[2]].isNull())
        {
            auto str = pJson[pMasqueradingVector[2]].asString();
            vote_=std::make_shared<std::vector<char>>(drogon::utils::base64DecodeToVector(str));
        }
    }
}

Vote::Vote(const Json::Value &pJson) noexcept(false)
{
    if(pJson.isMember("user_id"))
    {
        dirtyFlag_[0]=true;
        if(!pJson["user_id"].isNull())
        {
            userId_=std::make_shared<int32_t>((int32_t)pJson["user_id"].asInt64());
        }
    }
    if(pJson.isMember("poll_id"))
    {
        dirtyFlag_[1]=true;
        if(!pJson["poll_id"].isNull())
        {
            pollId_=std::make_shared<int32_t>((int32_t)pJson["poll_id"].asInt64());
        }
    }
    if(pJson.isMember("vote"))
    {
        dirtyFlag_[2]=true;
        if(!pJson["vote"].isNull())
        {
            auto str = pJson["vote"].asString();
            vote_=std::make_shared<std::vector<char>>(drogon::utils::base64DecodeToVector(str));
        }
    }
}

void Vote::updateByMasqueradedJson(const Json::Value &pJson,
                                            const std::vector<std::string> &pMasqueradingVector) noexcept(false)
{
    if(pMasqueradingVector.size() != 3)
    {
        LOG_ERROR << "Bad masquerading vector";
        return;
    }
    if(!pMasqueradingVector[0].empty() && pJson.isMember(pMasqueradingVector[0]))
    {
        dirtyFlag_[0] = true;
        if(!pJson[pMasqueradingVector[0]].isNull())
        {
            userId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[0]].asInt64());
        }
    }
    if(!pMasqueradingVector[1].empty() && pJson.isMember(pMasqueradingVector[1]))
    {
        dirtyFlag_[1] = true;
        if(!pJson[pMasqueradingVector[1]].isNull())
        {
            pollId_=std::make_shared<int32_t>((int32_t)pJson[pMasqueradingVector[1]].asInt64());
        }
    }
    if(!pMasqueradingVector[2].empty() && pJson.isMember(pMasqueradingVector[2]))
    {
        dirtyFlag_[2] = true;
        if(!pJson[pMasqueradingVector[2]].isNull())
        {
            auto str = pJson[pMasqueradingVector[2]].asString();
            vote_=std::make_shared<std::vector<char>>(drogon::utils::base64DecodeToVector(str));
        }
    }
}

void Vote::updateByJson(const Json::Value &pJson) noexcept(false)
{
    if(pJson.isMember("user_id"))
    {
        dirtyFlag_[0] = true;
        if(!pJson["user_id"].isNull())
        {
            userId_=std::make_shared<int32_t>((int32_t)pJson["user_id"].asInt64());
        }
    }
    if(pJson.isMember("poll_id"))
    {
        dirtyFlag_[1] = true;
        if(!pJson["poll_id"].isNull())
        {
            pollId_=std::make_shared<int32_t>((int32_t)pJson["poll_id"].asInt64());
        }
    }
    if(pJson.isMember("vote"))
    {
        dirtyFlag_[2] = true;
        if(!pJson["vote"].isNull())
        {
            auto str = pJson["vote"].asString();
            vote_=std::make_shared<std::vector<char>>(drogon::utils::base64DecodeToVector(str));
        }
    }
}

const int32_t &Vote::getValueOfUserId() const noexcept
{
    const static int32_t defaultValue = int32_t();
    if(userId_)
        return *userId_;
    return defaultValue;
}
const std::shared_ptr<int32_t> &Vote::getUserId() const noexcept
{
    return userId_;
}
void Vote::setUserId(const int32_t &pUserId) noexcept
{
    userId_ = std::make_shared<int32_t>(pUserId);
    dirtyFlag_[0] = true;
}

const int32_t &Vote::getValueOfPollId() const noexcept
{
    const static int32_t defaultValue = int32_t();
    if(pollId_)
        return *pollId_;
    return defaultValue;
}
const std::shared_ptr<int32_t> &Vote::getPollId() const noexcept
{
    return pollId_;
}
void Vote::setPollId(const int32_t &pPollId) noexcept
{
    pollId_ = std::make_shared<int32_t>(pPollId);
    dirtyFlag_[1] = true;
}

const std::vector<char> &Vote::getValueOfVote() const noexcept
{
    const static std::vector<char> defaultValue = std::vector<char>();
    if(vote_)
        return *vote_;
    return defaultValue;
}
std::string Vote::getValueOfVoteAsString() const noexcept
{
    const static std::string defaultValue = std::string();
    if(vote_)
        return std::string(vote_->data(),vote_->size());
    return defaultValue;
}
const std::shared_ptr<std::vector<char>> &Vote::getVote() const noexcept
{
    return vote_;
}
void Vote::setVote(const std::vector<char> &pVote) noexcept
{
    vote_ = std::make_shared<std::vector<char>>(pVote);
    dirtyFlag_[2] = true;
}
void Vote::setVote(const std::string &pVote) noexcept
{
    vote_ = std::make_shared<std::vector<char>>(pVote.c_str(),pVote.c_str()+pVote.length());
    dirtyFlag_[2] = true;
}
void Vote::setVoteToNull() noexcept
{
    vote_.reset();
    dirtyFlag_[2] = true;
}

void Vote::updateId(const uint64_t id)
{
}

const std::vector<std::string> &Vote::insertColumns() noexcept
{
    static const std::vector<std::string> inCols={
        "user_id",
        "poll_id",
        "vote"
    };
    return inCols;
}

void Vote::outputArgs(drogon::orm::internal::SqlBinder &binder) const
{
    if(dirtyFlag_[0])
    {
        if(getUserId())
        {
            binder << getValueOfUserId();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[1])
    {
        if(getPollId())
        {
            binder << getValueOfPollId();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[2])
    {
        if(getVote())
        {
            binder << getValueOfVote();
        }
        else
        {
            binder << nullptr;
        }
    }
}

const std::vector<std::string> Vote::updateColumns() const
{
    std::vector<std::string> ret;
    if(dirtyFlag_[0])
    {
        ret.push_back(getColumnName(0));
    }
    if(dirtyFlag_[1])
    {
        ret.push_back(getColumnName(1));
    }
    if(dirtyFlag_[2])
    {
        ret.push_back(getColumnName(2));
    }
    return ret;
}

void Vote::updateArgs(drogon::orm::internal::SqlBinder &binder) const
{
    if(dirtyFlag_[0])
    {
        if(getUserId())
        {
            binder << getValueOfUserId();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[1])
    {
        if(getPollId())
        {
            binder << getValueOfPollId();
        }
        else
        {
            binder << nullptr;
        }
    }
    if(dirtyFlag_[2])
    {
        if(getVote())
        {
            binder << getValueOfVote();
        }
        else
        {
            binder << nullptr;
        }
    }
}
Json::Value Vote::toJson() const
{
    Json::Value ret;
    if(getUserId())
    {
        ret["user_id"]=getValueOfUserId();
    }
    else
    {
        ret["user_id"]=Json::Value();
    }
    if(getPollId())
    {
        ret["poll_id"]=getValueOfPollId();
    }
    else
    {
        ret["poll_id"]=Json::Value();
    }
    if(getVote())
    {
        ret["vote"]=drogon::utils::base64Encode((const unsigned char *)getVote()->data(),getVote()->size());
    }
    else
    {
        ret["vote"]=Json::Value();
    }
    return ret;
}

Json::Value Vote::toMasqueradedJson(
    const std::vector<std::string> &pMasqueradingVector) const
{
    Json::Value ret;
    if(pMasqueradingVector.size() == 3)
    {
        if(!pMasqueradingVector[0].empty())
        {
            if(getUserId())
            {
                ret[pMasqueradingVector[0]]=getValueOfUserId();
            }
            else
            {
                ret[pMasqueradingVector[0]]=Json::Value();
            }
        }
        if(!pMasqueradingVector[1].empty())
        {
            if(getPollId())
            {
                ret[pMasqueradingVector[1]]=getValueOfPollId();
            }
            else
            {
                ret[pMasqueradingVector[1]]=Json::Value();
            }
        }
        if(!pMasqueradingVector[2].empty())
        {
            if(getVote())
            {
                ret[pMasqueradingVector[2]]=drogon::utils::base64Encode((const unsigned char *)getVote()->data(),getVote()->size());
            }
            else
            {
                ret[pMasqueradingVector[2]]=Json::Value();
            }
        }
        return ret;
    }
    LOG_ERROR << "Masquerade failed";
    if(getUserId())
    {
        ret["user_id"]=getValueOfUserId();
    }
    else
    {
        ret["user_id"]=Json::Value();
    }
    if(getPollId())
    {
        ret["poll_id"]=getValueOfPollId();
    }
    else
    {
        ret["poll_id"]=Json::Value();
    }
    if(getVote())
    {
        ret["vote"]=drogon::utils::base64Encode((const unsigned char *)getVote()->data(),getVote()->size());
    }
    else
    {
        ret["vote"]=Json::Value();
    }
    return ret;
}

bool Vote::validateJsonForCreation(const Json::Value &pJson, std::string &err)
{
    if(pJson.isMember("user_id"))
    {
        if(!validJsonOfField(0, "user_id", pJson["user_id"], err, true))
            return false;
    }
    else
    {
        err="The user_id column cannot be null";
        return false;
    }
    if(pJson.isMember("poll_id"))
    {
        if(!validJsonOfField(1, "poll_id", pJson["poll_id"], err, true))
            return false;
    }
    else
    {
        err="The poll_id column cannot be null";
        return false;
    }
    if(pJson.isMember("vote"))
    {
        if(!validJsonOfField(2, "vote", pJson["vote"], err, true))
            return false;
    }
    return true;
}
bool Vote::validateMasqueradedJsonForCreation(const Json::Value &pJson,
                                              const std::vector<std::string> &pMasqueradingVector,
                                              std::string &err)
{
    if(pMasqueradingVector.size() != 3)
    {
        err = "Bad masquerading vector";
        return false;
    }
    try {
      if(!pMasqueradingVector[0].empty())
      {
          if(pJson.isMember(pMasqueradingVector[0]))
          {
              if(!validJsonOfField(0, pMasqueradingVector[0], pJson[pMasqueradingVector[0]], err, true))
                  return false;
          }
        else
        {
            err="The " + pMasqueradingVector[0] + " column cannot be null";
            return false;
        }
      }
      if(!pMasqueradingVector[1].empty())
      {
          if(pJson.isMember(pMasqueradingVector[1]))
          {
              if(!validJsonOfField(1, pMasqueradingVector[1], pJson[pMasqueradingVector[1]], err, true))
                  return false;
          }
        else
        {
            err="The " + pMasqueradingVector[1] + " column cannot be null";
            return false;
        }
      }
      if(!pMasqueradingVector[2].empty())
      {
          if(pJson.isMember(pMasqueradingVector[2]))
          {
              if(!validJsonOfField(2, pMasqueradingVector[2], pJson[pMasqueradingVector[2]], err, true))
                  return false;
          }
      }
    }
    catch(const Json::LogicError &e)
    {
      err = e.what();
      return false;
    }
    return true;
}
bool Vote::validateJsonForUpdate(const Json::Value &pJson, std::string &err)
{
    if(pJson.isMember("user_id"))
    {
        if(!validJsonOfField(0, "user_id", pJson["user_id"], err, false))
            return false;
    }
    if(pJson.isMember("poll_id"))
    {
        if(!validJsonOfField(1, "poll_id", pJson["poll_id"], err, false))
            return false;
    }
    if(pJson.isMember("vote"))
    {
        if(!validJsonOfField(2, "vote", pJson["vote"], err, false))
            return false;
    }
    return true;
}
bool Vote::validateMasqueradedJsonForUpdate(const Json::Value &pJson,
                                            const std::vector<std::string> &pMasqueradingVector,
                                            std::string &err)
{
    if(pMasqueradingVector.size() != 3)
    {
        err = "Bad masquerading vector";
        return false;
    }
    try {
      if(!pMasqueradingVector[0].empty() && pJson.isMember(pMasqueradingVector[0]))
      {
          if(!validJsonOfField(0, pMasqueradingVector[0], pJson[pMasqueradingVector[0]], err, false))
              return false;
      }
      if(!pMasqueradingVector[1].empty() && pJson.isMember(pMasqueradingVector[1]))
      {
          if(!validJsonOfField(1, pMasqueradingVector[1], pJson[pMasqueradingVector[1]], err, false))
              return false;
      }
      if(!pMasqueradingVector[2].empty() && pJson.isMember(pMasqueradingVector[2]))
      {
          if(!validJsonOfField(2, pMasqueradingVector[2], pJson[pMasqueradingVector[2]], err, false))
              return false;
      }
    }
    catch(const Json::LogicError &e)
    {
      err = e.what();
      return false;
    }
    return true;
}
bool Vote::validJsonOfField(size_t index,
                            const std::string &fieldName,
                            const Json::Value &pJson,
                            std::string &err,
                            bool isForCreation)
{
    switch(index)
    {
        case 0:
            if(pJson.isNull())
            {
                err="The " + fieldName + " column cannot be null";
                return false;
            }
            if(!pJson.isInt())
            {
                err="Type error in the "+fieldName+" field";
                return false;
            }
            break;
        case 1:
            if(pJson.isNull())
            {
                err="The " + fieldName + " column cannot be null";
                return false;
            }
            if(!pJson.isInt())
            {
                err="Type error in the "+fieldName+" field";
                return false;
            }
            break;
        case 2:
            if(pJson.isNull())
            {
                return true;
            }
            if(!pJson.isString())
            {
                err="Type error in the "+fieldName+" field";
                return false;
            }
            break;
        default:
            err="Internal error in the server";
            return false;
    }
    return true;
}