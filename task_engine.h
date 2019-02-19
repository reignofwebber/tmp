#ifndef TASK_ENGINE_H_
#define TASK_ENGINE_H_


// std
#include <vector>
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
    uint32_t getTaskIndex() const;
    std::shared_ptr<MessageObject> getOne();

 private:
    void readTask(boost::property_tree::ptree &pt, const std::string &protocol);
    std::string parse(const std::string &s);
    uint32_t m_msgIndex{0};
    std::vector<std::shared_ptr<MessageObject>> m_msgList;

};



#endif  // TASK_ENGINE_H_
