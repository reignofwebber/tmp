#ifndef MANAGER_H_
#define MANAGER_H_

// std
#include <string>
#include <vector>
#include <set>
#include <map>

// boost
#include <boost/filesystem.hpp>

// local
#include "global.h"
#include "message_object.h"

namespace fs = boost::filesystem;

class Manager {
 public:
    Manager() = default;

    void start();
    void setController(Controller *controller);

    void process(SessionPtr session, const std::string &msg);
    void unregiste(SessionPtr session);

    void welcome(SessionPtr session, const fs::path &p);
    void test();


    void add_to_send_queue(const fs::path &p, const std::string &msg);

 private:
    void initPIO();
    fs::path makePath(const C2SQuery &query);
    fs::path makePath(const std::string &id, const std::string &ruleSet, const std::string &level, const std::string &deviceId);
    std::vector<fs::path> makePaths(const std::string &id, const std::string &ruleSet, const std::string &level, const std::vector<std::string> &deviceIds);


 private:
    Controller *controller_;

    std::set<SessionPtr> verifyed_sessions;
    //
    std::map<SessionPtr, std::map<fs::path, int>> sessionMap;

    S2CCIMEtConfig m_pioConfig;
};




#endif  // MANAGER_H_
