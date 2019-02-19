#include "controller.h"

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

void Controller::start() {
    updateTh_ = std::thread(&Controller::update, this);
}

void Controller::update() {
    // RandomEngine engine;
    // engine.init();
    TaskEngine engine;
    engine.init();

    while (true) {
        log_info << "polling tasks... " << engine.getTaskIndex();

        mgr_->add_to_send_queue(engine.getOne());
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
