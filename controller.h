#ifndef CONTROLLER_H_
#define CONTROLLER_H_

// std
#include <string>
#include <set>
#include <utility>
#include <thread>
#include <memory>
#include <mutex>

// local
#include "global.h"
#include "manager.h"

class Controller {
 public:
    explicit Controller(std::shared_ptr<Manager> mgr);

    ~Controller();

    void start();
    void subscribe(RuleSet set);
    void unsubscribe(RuleSet set);
    void subscribe(const std::string &id, const std::pair<RuleSet, Subscribe_Level> &ruleSets);
    void unsubscribe(const std::string &id, const std::pair<RuleSet, Subscribe_Level> &ruleSets);

 private:
    void update();


    std::thread updateTh_;
    std::shared_ptr<Manager> mgr_;
    std::set<RuleSet> ruleSets_;

    std::set<IndentiSet> sets_;

    std::mutex mtx_;
};

#endif  // CONTROLLER_H_
