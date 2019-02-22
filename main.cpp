// std
#include <iostream>
#include <memory>
#include <thread>

// boost
#include <boost/asio.hpp>

// local
#include "manager.h"
#include "server.h"
#include "log.h"

#include "controller.h"

int main(int argc, char **argv) {

    // std::shared_ptr<Manager> mgr = std::make_shared<Manager>();
    // mgr->start();
    // Controller controller(mgr);
    // controller.start();

    // boost::asio::io_context io_context;

    // try {
    //     Server server(io_context, 9999);
    //     server.setManager(mgr);
    //     server.start();

    //     io_context.run();
    // } catch (const std::exception &e) {
    //     log_error << e.what();
    // }

    std::vector<Message> msgs;
    Message msg;
    msg.setType("Tp");
    msg.setId("id");
    msg.addString("name", "zf");
    msgs.push_back(msg);
    msgs.push_back(msg);
    Message msgwrap;
    msgwrap.setType("WrapTp");
    msgwrap.setId("WrapId");
    msgwrap.addPacketArray("pId", msgs);
    
    log_info << msgwrap.toXml();

    return 0;
}
