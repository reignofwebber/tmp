#include "manager.h"

// std
#include <string>
#include <utility>
#include <algorithm>
#include <exception>
#include <memory>

// boost
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

// local
#include "log.h"
#include "message.h"
#include "server.h"

void Manager::start() {
    initPIO();
}

void Manager::process(SessionPtr session, const std::string &msg) {
    auto mobj = getMessageObject(msg);
    if (!mobj) return;
    std::string msgToWrite;
    auto identifier = mobj->getMessageIdentifier();
    if (identifier == C2S_Authentication) {
        auto obj = std::dynamic_pointer_cast<C2SAuthentication>(mobj);
        S2CAuthenticationReply reply;
        if (obj->username == "admin" && obj->password == "123456") {
            reply.success = true;
            verifyed_sessions.insert(session);
        } else {
            reply.success = false;
            reply.reason = "username or password error";
        }
        msgToWrite = reply.msg();
    } else if (identifier == C2S_Configuration) {
        auto obj = std::dynamic_pointer_cast<C2SConfiguration>(mobj);
        S2CConfigurationReply reply;
        reply.success = true;
        msgToWrite = reply.msg();
    } else if (identifier == C2S_Subscription) {
        // TODO(zf)
        // has verified
        if (verifyed_sessions.find(session) != verifyed_sessions.end()) {
            C2SSubscription s;
            sessions.insert(std::make_pair(session, Unimplement_RuleSet));
        } else {
            log_info << "session is not verified!";
        }
    } else if (identifier == C2S_Playback_Start) {
        // TODO(zf)
    } else if (identifier == C2S_Playback_Option) {
        // TODO(zf)
    } else if (identifier == C2S_Playback_Control) {
        // TODO(zf)
    } else if (identifier == C2S_HeartBeat) {
        // TODO(zf)
    } else if (identifier == C2S_Close) {
        // TODO(zf)
    }


    session->writeMessage(msgToWrite);
}

void Manager::unregiste(SessionPtr session) {
    sessions.erase(session);
}

void Manager::welcome(SessionPtr session) {
    // pio config

}

void Manager::add_to_send_queue(std::shared_ptr<MessageObject> mobj) {
    std::for_each(sessions.begin(), sessions.end(), [=](const std::pair<SessionPtr, RuleSet> &p) {
        auto rulesets = mobj->getRuleSet();
        if (std::find(rulesets.begin(), rulesets.end(), p.second) != rulesets.end()) {
            p.first->writeMessage(mobj->msg());
        }
    });
}

void Manager::add_to_send_queue(const std::string &msg) {
    
}

void Manager::initPIO() {
    using boost::property_tree::ptree;
    ptree pt;
    boost::property_tree::read_xml("config/pio_config.xml", pt);
    for (auto &v : pt.get_child("CILIST")) {
        auto pt_ci = v.second;
        std::string ci = pt_ci.get<std::string>("<xmlattr>.id", "");
        if (ci.empty()) continue;

        // get pios
        for (auto &p : pt_ci) {
            S2CCIMEt info;
            info.ci = ci;
            info.id = p.second.get<std::string>("<xmlattr>.id", "");
            info.type = p.second.get<std::string>("<xmlattr>.type", "");
            m_pioConfig.pioList[ci][info.id] = info;
        }
    }
}
