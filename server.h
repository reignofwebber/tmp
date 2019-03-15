#ifndef SERVER_H_
#define SERVER_H_

// std
#include <string>
#include <memory>

// boost
#include <boost/asio.hpp>

// local
#include "log.h"
#include "manager.h"

using boost::asio::ip::tcp;


class Session : public std::enable_shared_from_this<Session> {
 public:
    explicit Session(tcp::socket socket, std::shared_ptr<Manager> mgr)
        : socket_(std::move(socket)), mgr_(mgr) {
        msg_buf_ = new char[65535];
        xml_length_ = new uint32_t;
    }

    ~Session() {
        delete[] msg_buf_;
        delete xml_length_;
        log_trace << "------session released";
    }

    void start() {
        do_read_header();
    }

    bool isOpen() {
        return socket_.is_open();
    }

    void writeMessage(const std::string &msg) {
        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(msg),
            [this, self](const boost::system::error_code &ec, size_t /* length */) {
                if (ec) {
                    ++try_count_;
                    print_error(ec, "write error");
                } else {
                    try_count_ = 0;
                }
            });
    }

    void close() {
        if (socket_.is_open()) {
            boost::system::error_code ec_shutdown;
            socket_.shutdown(tcp::socket::shutdown_both, ec_shutdown);
            if (ec_shutdown) {
                log_warning << "[server] shutdown error, code is " << ec_shutdown.value() << ", message is " << ec_shutdown.message();
            }
            socket_.close();
            if (auto mgr = mgr_.lock()) {
                mgr->unregiste(shared_from_this());
            } else {
                log_error << "mgr has been expired";
            }
        }
    }

 private:
    void do_read_header() {
        auto self(shared_from_this());
        boost::asio::async_read(socket_, boost::asio::buffer(xml_length_, 4),
            [self, this](const boost::system::error_code &ec, size_t length) {
                if (!ec) {
                    if (length == 4) {
                        auto actual_length = boost::endian::big_to_native(*xml_length_);
                        do_read_body(actual_length);
                    } else {
                        log_fatal << "read header length error!";
                        close();
                    }
                } else {
                    if (ec == boost::asio::error::connection_reset) {
                        log_info << "remote client is closed, closing this session...";
                        close();
                    } else if (ec == boost::asio::error::eof) {
                        log_info << "connection is over";
                        close();
                    } else if (ec != boost::asio::error::operation_aborted){
                        print_error(ec, "read header error");
                    }
                }
            });

    }

    void do_read_body(uint32_t length) {
        auto self(shared_from_this());

        boost::asio::async_read(socket_, boost::asio::buffer(msg_buf_, length),
            [self, this, length](const boost::system::error_code &ec, size_t read_length) {
                if (!ec) {
                    if (length == read_length) {
                        if (auto mgr = mgr_.lock()) {
                            std::string msg {msg_buf_, length};
                            mgr->process(self, msg);
                        } else {
                            log_error << "mgr has been expired";
                        }
                        do_read_header();
                    } else {
                        log_fatal << "read body length error! should read " << length << ", but " << read_length;
                        close();
                    }
                } else {
                    print_error(ec, "read body error");
                    close();
                }
            });
    }

    void print_error(const boost::system::error_code &ec, const std::string &prefix) {
        log_warning << "[server] "<< prefix << ", code is " << ec.value() << ", message is " << ec.message() << ", " << ec.category().name();
        if (try_count_ > 5) {
            if (socket_.is_open()) {
                log_warning << "try count > 5, closing socket..";
                boost::system::error_code ec_shutdown;
                socket_.shutdown(tcp::socket::shutdown_both, ec_shutdown);
                if (ec_shutdown) {
                    log_warning << "[server] shutdown error, code is " << ec_shutdown.value() << ", message is " << ec_shutdown.message();
                }
                socket_.close();
                if (auto mgr = mgr_.lock()) {
                    mgr->unregiste(shared_from_this());
                } else {
                    log_error << "mgr has been expired";
                }

                log_trace  << "session is closing cause many errors..";
            }
        }
    }

    tcp::socket socket_;
    int try_count_{0};
    std::weak_ptr<Manager> mgr_;
    char *msg_buf_;
    uint32_t *xml_length_;
};


class Server {
 public:
    Server(boost::asio::io_context &io_context, uint16_t port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
    }

    void start() {
        do_accept();
    }

    void setManager(std::shared_ptr<Manager> mgr) {
        mgr_ = mgr;
    }

 private:
    void do_accept() {
        acceptor_.async_accept([=](const boost::system::error_code &ec, tcp::socket socket) {
            if (!ec) {
                log_info << "receive a new connection...";
                std::make_shared<Session>(std::move(socket), mgr_.lock())->start();

            } else {
                log_error << "accept error " << ec.value() << ", " << ec.message();
            }
            do_accept();
        });
    }

    tcp::acceptor acceptor_;
    std::weak_ptr<Manager> mgr_;
};



#endif  // SERVER_H_
