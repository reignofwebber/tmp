#ifndef FILE_ENGINE_H_
#define FILE_ENGINE_H_

// std
#include <ctime>
#include <vector>
#include <utility>
#include <map>
#include <string>
#include <set>
#include <thread>
#include <mutex>

// boost
#include <boost/filesystem.hpp>

// local
#include "global.h"
#include "manager.h"

namespace fs = boost::filesystem;


class FileEngine {
 public:
    explicit FileEngine(Controller *controller);
    ~FileEngine();

    void start();

    void addPath(const fs::path &p);
    void removePath(const fs::path &p);

    std::vector<std::string> getAllMessages(const fs::path &p) const;
    std::vector<std::string> getImmediateMessage(const fs::path &p) const;

 private:
    void loopRead();

    void retrieveAllMessage(const fs::path &p);
    void retrieveMessage(const fs::path &p, const fs::path &fp);
    std::string makeMessage(const std::string &str) const;

    Controller *controller_{nullptr};

    std::thread read_th_;
    std::mutex mtx_;
    // paths to be monitered
    std::set<fs::path> paths_;

    std::map<fs::path, std::map<std::string, std::string>> msg_buf_;
    std::map<fs::path, std::time_t> last_change_time_;
};


#endif  // FILE_ENGINE_H_
