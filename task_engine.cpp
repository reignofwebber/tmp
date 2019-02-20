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

    boost::property_tree::read_json("config/task.json", pt);

    for (auto &protocol : pt) {
        for (auto &task : protocol.second) {
            auto name = protocol.first;
            readTask(task.second, protocol.first);
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

uint32_t TaskEngine::getTaskCount() const {
    return m_msgList.size();
}
uint32_t TaskEngine::getTaskIndex(RuleSet set) const {
    if (m_msgIndex.at(set) == m_msgList.at(set).size()) {
        return 0;
    }
    return m_msgIndex.at(set);
}

void TaskEngine::readTask(boost::property_tree::ptree &pt, const std::string &protocol) {
    #define READS(str) pt.get<std::string>(str)
    #define PREADS(str) parse(READS(str))
    #define QUEUE(mobj) m_msgList.push_back(mobj)

    if (protocol == "S2C_CIM_EtIn") {
        auto mobj = std::make_shared<S2CCIMEt>();

        mobj->ci = READS("id");
        mobj->id = "In";
        mobj->id += PREADS("pos");
        mobj->status = PREADS("status");
        mobj->status1 = PREADS("status1");
        mobj->status2 = PREADS("status2");
        mobj->description = READS("description");
                
        m_msgList[All_RuleSet].push_back(mobj);
    } else if (protocol == "S2C_CIM_EtOut") {
        S2CCIMEt et;
        auto mobj = std::make_shared<S2CCIMEt>();

        mobj->ci = READS("id");
        mobj->id = "Out";
        mobj->id += PREADS("pos");
        mobj->status = PREADS("status");
        mobj->status1 = PREADS("status1");
        mobj->status2 = PREADS("status2");
        mobj->description = READS("description");
    } else if (protocol == "S2C_CIM_Status") {
        auto mobj = std::make_shared<S2CCIMStatus>();

        mobj->ci = READS("ci");
        mobj->index = READS("index");
        mobj->status = PREADS("status");
        mobj->description = READS("description");
    } else if (protocol == "S2C_CIM_KxStatus") {
        auto mobj = std::make_shared<S2CCIMKxStatus>();

        mobj->ci = READS("ci");
        mobj->id = READS("id");
        mobj->status = PREADS("status");
        mobj->status1 = PREADS("status1");
        mobj->status2 = PREADS("status2");
        mobj->description = READS("description");
    }

    #undef READS
    #undef PREADS
    #undef QUEUE
}
