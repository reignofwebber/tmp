#include "controller.h"

// std
#include <vector>
#include <algorithm>

// local
#include "log.h"
#include "random_engine.h"
#include "task_engine.h"
#include "file_engine.h"


Controller::Controller(std::shared_ptr<Manager> mgr)
    : mgr_(mgr), engine_(this) {
    mgr_->setController(this);
}

Controller::~Controller() {
    if (updateTh_.joinable()) {
        updateTh_.join();
    }
}

void Controller::subscribe(const fs::path &p) {
    engine_.addPath(p);
}

void Controller::unsubscribe(const fs::path &p) {
    engine_.removePath(p);
}

void Controller::start() {
    updateTh_ = std::thread(&Controller::update, this);
    engine_.start();
}

void Controller::addMessage(const fs::path &p, const std::string &msg) {
    std::lock_guard<std::mutex> lk(msg_mtx_);
    messages_.emplace_back(p, msg);
    cv_.notify_one();
}

void Controller::update() {
    std::unique_lock<std::mutex> lk(msg_mtx_);
    while (true) {
        cv_.wait(lk, [&]() {
            return !messages_.empty();
        });
        auto message = messages_.back();
        messages_.pop_back();
        log_info << "modified data from .. " << message.first.string();
        mgr_->add_to_send_queue(message.first, message.second);
    }
}

std::vector<std::string> Controller::getAllMessages(const fs::path &p) const {
    return engine_.getAllMessages(p);
}
