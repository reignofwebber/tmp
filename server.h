#ifndef SERVER_H_
#define SERVER_H_

// std
#include <memory>

// boost
#include <boost/asio.hpp>

// local
#include "log.h"

using boost::asio::ip::tcp;


class Session : public std::enable_shared_from_this<Session> {
 public:
    explicit Session(tcp::socket socket) : socket_(std::move(socket)) {
        do_read_header();
    }


 private:

    void do_read_header() {

    }

    tcp::socket socket_;
};





class Server {
 public:
    Server(boost::asio::io_context &io_context, uint16_t port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }
 private:
    void do_accept() {
        acceptor_.async_accept([=](const boost::system::error_code &ec, tcp::socket socket) {
            if (!ec) {
                log_info << "receive a new connection...";

            } else {
                log_error << "accept error " << ec.value() << ", " << ec.message();
            }
            do_accept();
        });
    }

    tcp::acceptor acceptor_;
};



#endif  // SERVER_H_
