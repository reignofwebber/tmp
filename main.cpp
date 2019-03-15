// std
#include <iostream>
#include <memory>
#include <thread>
#include <stdexcept>

// boost
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>

// local
#include "manager.h"
#include "server.h"
#include "log.h"

#include "controller.h"

void handler() {
    exit(1);
} 

int main(int argc, char **argv) {
    // std::set_terminate(handler);

    std::shared_ptr<Manager> mgr = std::make_shared<Manager>();
    mgr->start();
    Controller controller(mgr);
    controller.start();

    boost::asio::io_context io_context;

    try {
        Server server(io_context, 19997);
        server.setManager(mgr);
        server.start();

        mgr->test();
        io_context.run();
    } catch (const std::exception &e) {
        log_error << e.what();
    }


    return 0;
}
