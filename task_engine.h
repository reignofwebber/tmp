#ifndef TASK_ENGINE_H_
#define TASK_ENGINE_H_


// std
#include <vector>
#include <map>
#include <string>
#include <memory>

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
    std::shared_ptr<MessageObject> getOne(RuleSet set);

 private:
    void readTask(boost::property_tree::ptree &pt, const std::string &protocol);
    std::string parse(const std::string &s);
    std::map<RuleSet, uint32_t> m_msgIndex;
    std::map<RuleSet, std::vector<std::shared_ptr<MessageObject>>> m_msgList;

};



#endif  // TASK_ENGINE_H_
