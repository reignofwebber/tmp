#ifndef CONTROLLER_H_
#define CONTROLLER_H_

// std
#include <thread>
#include <memory>

// local
#include "global.h"
#include "manager.h"

class Controller {
 public:
    Controller(std::shared_ptr<Manager> mgr);

    ~Controller();

    void start();

 private:
    void update();


    std::thread updateTh_;
    std::shared_ptr<Manager> mgr_;
};

#endif  // CONTROLLER_H_
