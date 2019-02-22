#ifndef MANAGER_H_
#define MANAGER_H_

// std
#include <string>
#include <vector>
#include <set>
#include <map>

// local
#include "global.h"
#include "message_object.h"

class Manager {
 public:
    Manager() = default;

    void start();

    void process(SessionPtr session, const std::string &msg);
    void unregiste(SessionPtr session);

    void welcome(SessionPtr session);


    void add_to_send_queue(std::shared_ptr<MessageObject> mobj);
    void add_to_send_queue(const std::string &msg);

 private:
    void initPIO();



 private:
    std::set<SessionPtr> verifyed_sessions;
   //  std::set<SessionPtr> sessions;
    std::map<SessionPtr, std::vector<IndentiSet>> sessions;

    S2CCIMEtConfig m_pioConfig;
};




#endif  // MANAGER_H_
