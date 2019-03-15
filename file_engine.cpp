#include "file_engine.h"

// std
#include <chrono>
#include <algorithm>
#include <sstream>
#include <streambuf>

// local
#include "controller.h"
#include "log.h"
#include "util.h"


FileEngine::FileEngine(Controller *controller) {
    controller_ = controller;
}

void FileEngine::start() {
    if (controller_ == nullptr) {
        log_error << "file engine start failed";
    }
    read_th_ = std::thread(&FileEngine::loopRead, this);
}

FileEngine::~FileEngine() {
    if (read_th_.joinable()) {
        read_th_.join();
    }
}

void FileEngine::addPath(const fs::path &p) {
    if (!fs::is_directory(p)) {
        log_error << p.string() << " is not a directory.";
        return;
    }
    std::lock_guard<std::mutex> lk(mtx_);
    paths_.insert(p);
    retrieveAllMessage(p);
}

void FileEngine::removePath(const fs::path &p) {
    std::lock_guard<std::mutex> lk(mtx_);
    paths_.erase(p);
}

void FileEngine::retrieveAllMessage(const fs::path &p) {
    if (!fs::exists(p) || !fs::is_directory(p)) {
        log_error << "no such path " << p.string();
        return;
    }

    // std::vector<fs::path> fps;
    // std::copy(fs::directory_iterator(p), fs::directory_iterator(), std::back_inserter(fps));
    // std::sort(fps.begin(), fps.end());

    for (const auto &f : fs::directory_iterator(p)) {
        auto fp = f.path();
        if (!fs::is_regular_file(fp)) continue;
        fs::ifstream in(fp);
        if (in.is_open()) {
            std::ostringstream ss;
            std::copy(std::istreambuf_iterator<char>(in),
                      std::istreambuf_iterator<char>(),
                      std::ostreambuf_iterator<char>(ss));
            auto msg = makeMessage(ss.str());
            msg_buf_[p][fp.filename().string()] = msg;
            last_change_time_[fp] = fs::last_write_time(fp);
        } else {
            log_error << "can not open " << fp.string();
        }
    }
}

void FileEngine::retrieveMessage(const fs::path &p, const fs::path &fp) {
    if (!fs::exists(fp) || !fs::is_regular_file(fp)) {
        log_error << "no such path " << fp.string();
        return;
    }

    fs::ifstream in(fp);
    if (in.is_open()) {
        std::ostringstream ss;
        std::copy(std::istreambuf_iterator<char>(in),
                  std::istreambuf_iterator<char>(),
                  std::ostreambuf_iterator<char>(ss));
        auto msg = makeMessage(ss.str());
        msg_buf_[p][fp.filename().string()] = msg;
        controller_->addMessage(p, msg);
        last_change_time_[fp] = fs::last_write_time(fp);
    } else {
        log_error << "can not open " << fp.string();
    }
}

std::string FileEngine::makeMessage(const std::string &str) const {
    auto len_s = zfutils::dump_hex<uint32_t>(str.size());
    return len_s + str;
}

std::vector<std::string> FileEngine::getAllMessages(const fs::path &p) const {
    if (msg_buf_.find(p) != msg_buf_.end()) {
        std::vector<std::string> msgs;
        std::transform(msg_buf_.at(p).begin(), msg_buf_.at(p).end(), std::back_inserter(msgs),
                       [](const std::pair<std::string, std::string> &v) {
                           return v.second;
                       });
        return msgs;
    } else {
        log_info << "no such path " << p.string();
        return {};
    }
}

void FileEngine::loopRead() {
    try {
        while (true) {
            // make copy from paths
            mtx_.lock();
            auto paths(paths_);
            mtx_.unlock();

            for (const auto &p : paths) {
                for (const auto &fp : fs::directory_iterator(p)) {
                    auto last_time = fs::last_write_time(fp);
                    if (last_change_time_.find(fp) != last_change_time_.end()) {
                        if (last_change_time_.at(fp) != last_time) {
                            retrieveMessage(p, fp);
                        }
                    } else {
                        last_change_time_[fp] = last_time;
                        retrieveMessage(p, fp);
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    } catch (const std::exception &e) {
        log_error << "watch thread exit due to error " << e.what();
    }
}