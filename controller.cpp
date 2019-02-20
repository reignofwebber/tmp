#include "controller.h"

// std
#include <vector>
#include <algorithm>

// local
#include "log.h"
#include "random_engine.h"
#include "task_engine.h"


Controller::Controller(std::shared_ptr<Manager> mgr)
    : mgr_(mgr) {
}

Controller::~Controller() {
    if (updateTh_.joinable()) {
        updateTh_.join();
    }
}

void Controller::subscribe(RuleSet set) {
    std::lock_guard<std::mutex> lk(mtx_);
    ruleSets_.insert(set);
}

void Controller::unsubscribe(RuleSet set) {
    std::lock_guard<std::mutex> lk(mtx_);
    ruleSets_.erase(set);
}

void Controller::start() {
    updateTh_ = std::thread(&Controller::update, this);
}

void Controller::update() {
    // RandomEngine engine;
    // engine.init();
    TaskEngine engine;
    engine.init();

    while (true) {
        std::vector<RuleSet> set_copy;

        std::unique_lock<std::mutex> lk(mtx_);
        std::copy(ruleSets_.begin(), ruleSets_.end(), std::back_inserter(set_copy));
        lk.unlock();

        for (RuleSet set : ruleSets_) {
            log_info << "polling tasks... ruleset " << static_cast<uint16_t>(set) << ", index " << engine.getTaskIndex(set);
            mgr_->add_to_send_queue(engine.getOne(set));
        }



        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}


