#ifndef CONTROLLER_H_
#define CONTROLLER_H_

// std
#include <set>
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

 private:
    void update();


    std::thread updateTh_;
    std::shared_ptr<Manager> mgr_;
    std::set<RuleSet> ruleSets_;
    std::mutex mtx_;
};

#endif  // CONTROLLER_H_
