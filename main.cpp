// std
#include <iostream>
#include <memory>
#include <thread>

// boost
#include <boost/asio.hpp>

// local
#include "manager.h"
// #include "server.h"
#include "log.h"

int main(int argc, char **argv) {
    std::cout << "Hello World" << std::endl;

    std::shared_ptr<Manager> mgr = std::make_shared<Manager>();
    mgr->start();

    boost::asio::io_context io_context;

    try {
        // Server server(io_context, 9999);

        io_context.run();
    } catch (const std::exception &e) {
        log_error << e.what();
    }


    return 0;
}
