#ifndef TASK_ENGINE_H_
#define TASK_ENGINE_H_


// std
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <utility>

// boost
#include <boost/property_tree/ptree.hpp>

// local
#include "global.h"

class TaskEngine {
 public:
    TaskEngine() = default;

    void init();

    uint32_t getTaskCount() const;

    uint32_t getTaskIndex(RuleSet set) const;
    uint32_t getTaskIndex(const std::string &deviceId, const std::pair<RuleSet, Subscribe_Level> &ruleSet) const;

    std::shared_ptr<MessageObject> getOne(RuleSet set);
    std::shared_ptr<MessageObject> getOne(const std::string &deviceId, const std::pair<RuleSet, Subscribe_Level> &ruleSet);

 private:
    void readTask(boost::property_tree::ptree &pt, const std::string &protocol);
    std::string parse(const std::string &s);
    std::map<RuleSet, uint32_t> m_msgIndex;
    std::map<RuleSet, std::vector<std::shared_ptr<MessageObject>>> m_msgList;

    // map station -> devices
    std::map<std::string, std::vector<std::string>> m_stationDeviceMap;
    // map device -> (map rule protocols)
    std::map<std::string, std::map<std::pair<RuleSet, Subscribe_Level>, std::vector<std::shared_ptr<MessageObject>>>> m_deviceProtocolMap;
    // map device -> (map rule index)
    std::map<std::string, std::map<std::pair<RuleSet, Subscribe_Level>, uint32_t>> m_deviceIndexMap;

};



#endif  // TASK_ENGINE_H_
