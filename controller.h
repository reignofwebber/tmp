#ifndef CONTROLLER_H_
#define CONTROLLER_H_

// std
#include <string>
#include <vector>
#include <set>
#include <utility>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>

// boost
#include <boost/filesystem.hpp>

// local
#include "global.h"
#include "file_engine.h"

namespace fs = boost::filesystem;

class Controller {
 public:
    explicit Controller(std::shared_ptr<Manager> mgr);

    ~Controller();

    void start();
    void subscribe(const fs::path &p);
    void unsubscribe(const fs::path &p);

    void addMessage(const fs::path &p, const std::string &msg);
    std::vector<std::string> getAllMessages(const fs::path &p) const;

 private:
    void update();

    std::thread updateTh_;
    std::shared_ptr<Manager> mgr_;

    FileEngine engine_;
    std::vector<std::pair<fs::path, std::string>> messages_;
    std::mutex msg_mtx_;
    std::condition_variable cv_;

};

#endif  // CONTROLLER_H_
