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
#include "controller.h"

void Manager::start() {
    // initPIO();
}

void Manager::test() {
    // controller_->subscribe(makePath("CIRuleSet", "DEVICE", "RBC7"));
}

void Manager::setController(Controller *controller) {
    controller_ = controller;
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
        // has verified
        // if (verifyed_sessions.find(session) != verifyed_sessions.end()) {
            auto obj = std::dynamic_pointer_cast<C2SSubscription>(mobj);
            // sessionMap[session] = makePaths(obj->ruleSet, obj->level, obj->ids);
            auto paths = makePaths(obj->id, obj->ruleSet, obj->level, obj->ids);
            if (obj->subscription) {
                // subcribe
                std::for_each(paths.begin(), paths.end(), [&](const fs::path &p) {
                    // can not find this session, all paths' reference count initial to 1
                    if (sessionMap.find(session) == sessionMap.end()) {
                        sessionMap[session][p] = 1;
                        // subcribe from controller
                        controller_->subscribe(p);
                    } else {
                        // can not find path in this session, initalize this path's reference count to 1
                        if (sessionMap[session].find(p) == sessionMap[session].end()) {
                            sessionMap[session][p] = 1;
                            // subcribe from controller
                            controller_->subscribe(p);
                        } else {
                            // exist session && exist path, increase reference count
                            ++sessionMap[session][p];
                        }
                    }
                    welcome(session, p);
                });
            } else {
                // unsubcribe
                std::for_each(paths.begin(), paths.end(), [&](const fs::path &p) {
                    if (sessionMap.find(session) != sessionMap.end()) {
                        if (sessionMap[session].find(p) != sessionMap[session].end()) {
                            // referece count == 1, just remove this path from this session
                            if (sessionMap[session][p] == 1) {
                                sessionMap[session].erase(p);
                                controller_->unsubscribe(p);
                            } else {
                                --sessionMap[session][p];
                            }
                        }
                        // no path left in this session, just remove it.
                        if (sessionMap[session].empty()) {
                            sessionMap.erase(session);
                        }
                    }

                });
            }
        // } else {
        //     log_info << "session is not verified!";
        // }
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

}

void Manager::welcome(SessionPtr session, const fs::path &p) {
    auto messages = controller_->getAllMessages(p);
    std::for_each(messages.begin(), messages.end(), [&](const std::string &s) {
        session->writeMessage(s);
    });
}

void Manager::add_to_send_queue(const fs::path &p, const std::string &msg) {
    std::for_each(sessionMap.begin(), sessionMap.end(), [=](const std::pair<SessionPtr, std::map<fs::path, int>> &v) {
        auto session = v.first;
        auto pathMap = v.second;
        if (pathMap.find(p) != pathMap.end()) {
            session->writeMessage(msg);
        }
    });
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

fs::path Manager::makePath(const std::string &id, const std::string &ruleSet, const std::string &level, const std::string &deviceId) {
    fs::path p(".");
    return p / "data" / id / ruleSet / level / deviceId;
}

std::vector<fs::path> Manager::makePaths(const std::string &id, const std::string &ruleSet, const std::string &level, const std::vector<std::string> &deviceIds) {
    std::vector<fs::path> paths;
    std::for_each(deviceIds.begin(), deviceIds.end(), [&](const std::string &deviceId) {
        fs::path p(".");
        paths.push_back(p / "data" / id / ruleSet / level / deviceId);
    });
    return paths;
}