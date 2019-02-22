#include "task_engine.h"

// boost
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

// local
#include "log.h"
#include "message_object.h"

void TaskEngine::init() {
    log_info << "loading tasks...";
    boost::property_tree::ptree pt;

    std::vector<std::string> files = {
        "config/task.cim.json",
        "config/task.overallstatus.json"
    };

    for (const auto &file : files) {
        boost::property_tree::read_json(file, pt);

        for (auto &protocol : pt) {
            for (auto &task : protocol.second) {
                auto name = protocol.first;
                readTask(task.second, protocol.first);
            }
        }
    }
    log_info << "tasks loaded!, total count is " << m_msgList.size();
}

std::string TaskEngine::parse(const std::string &s) {
    // if has "|"
    uint32_t lastPos{0}, splitPos;
    std::vector<std::string> choices;

    while ((splitPos = s.find("|", lastPos)) != std::string::npos) {
        if (splitPos != 0 && s.at(splitPos - 1) == '\\') continue;
        choices.push_back(s.substr(lastPos, splitPos - lastPos));
        lastPos = splitPos + 1;
    }
    choices.push_back(s.substr(lastPos));

    auto res = choices.at(rand() % choices.size());
    // rand_r

    return res;
}

std::shared_ptr<MessageObject> TaskEngine::getOne(RuleSet set) {
    if (m_msgIndex.at(set) == m_msgList.at(set).size()) {
        m_msgIndex[set] = 0;
    }

    return m_msgList.at(set).at(m_msgIndex[set]++);
}

std::shared_ptr<MessageObject> TaskEngine::getOne(const std::string &deviceId, const std::pair<RuleSet, Subscribe_Level> &ruleSet) {
    if (m_deviceIndexMap.find(deviceId) != m_deviceIndexMap.end()) {
        if (m_deviceIndexMap[deviceId].find(ruleSet) != m_deviceIndexMap[deviceId].end()) {
            if (m_deviceIndexMap.at(deviceId).at(ruleSet) == m_deviceProtocolMap.at(deviceId).at(ruleSet).size()) {
                m_deviceIndexMap[deviceId][ruleSet] = 0;
            }
            return m_deviceProtocolMap.at(deviceId).at(ruleSet).at(m_deviceIndexMap[deviceId][ruleSet]++);
        } else {
            log_error << "no support ruleset (" << static_cast<int>(ruleSet.first) << ", " << static_cast<int>(ruleSet.second) << ") found.";
        }
    } else {
        log_error << "no device Id " << deviceId << " found.";
    }
    return nullptr;
}

uint32_t TaskEngine::getTaskCount() const {
    // TODO(zf)
    return m_msgList.size();
}
uint32_t TaskEngine::getTaskIndex(RuleSet set) const {
    if (m_msgIndex.at(set) == m_msgList.at(set).size()) {
        return 0;
    }
    return m_msgIndex.at(set);
}

uint32_t TaskEngine::getTaskIndex(const std::string &deviceId, const std::pair<RuleSet, Subscribe_Level> &ruleSet) const {
    if (m_deviceIndexMap.at(deviceId).at(ruleSet) == m_deviceProtocolMap.at(deviceId).at(ruleSet).size()) {
        return 0;
    }
    return m_deviceIndexMap.at(deviceId).at(ruleSet);
}

void TaskEngine::readTask(boost::property_tree::ptree &pt, const std::string &protocol) {
    // #define READS(str) pt.get<std::string>(str)
    // #define READ64(str) pt.get<int64_t>(str)
    // #define READU64(str) pt.get<uint64_t>(str)
    // #define PREADS(str) parse(READS(str))
    // #define QUEUE(mobj) m_msgList.push_back(mobj)

    auto mobj = createMessageObject(protocol);
    if (mobj != nullptr) {
        mobj->retrieveData(pt);
        for (const auto &r : mobj->getSupportRules()) {
            m_deviceProtocolMap[mobj->id][r].push_back(mobj);
        }
    }


    // if (protocol == "S2C_CIM_EtIn") {
    //     auto mobj = std::make_shared<S2CCIMEt>();

    //     mobj->ci = READS("id");
    //     mobj->id = "In";
    //     mobj->id += PREADS("pos");
    //     mobj->status = PREADS("status");
    //     mobj->status1 = PREADS("status1");
    //     mobj->status2 = PREADS("status2");
    //     mobj->description = READS("description");

    //     m_msgList[All_RuleSet].push_back(mobj);
    // } else if (protocol == "S2C_CIM_EtOut") {
    //     S2CCIMEt et;
    //     auto mobj = std::make_shared<S2CCIMEt>();

    //     mobj->ci = READS("id");
    //     mobj->id = "Out";
    //     mobj->id += PREADS("pos");
    //     mobj->status = PREADS("status");
    //     mobj->status1 = PREADS("status1");
    //     mobj->status2 = PREADS("status2");
    //     mobj->description = READS("description");
    // } else if (protocol == "S2C_CIM_Status") {
    //     auto mobj = std::make_shared<S2CCIMStatus>();

    //     mobj->ci = READS("ci");
    //     mobj->index = READS("index");
    //     mobj->status = PREADS("status");
    //     mobj->description = READS("description");
    // } else if (protocol == "S2C_CIM_KxStatus") {
    //     auto mobj = std::make_shared<S2CCIMKxStatus>();

    //     mobj->ci = READS("ci");
    //     mobj->id = READS("id");
    //     mobj->status = PREADS("status");
    //     mobj->status1 = PREADS("status1");
    //     mobj->status2 = PREADS("status2");
    //     mobj->description = READS("description");
    // }

    // #undef READS
    // #undef READ64
    // #undef READU64
    // #undef PREADS
    // #undef QUEUE
}


