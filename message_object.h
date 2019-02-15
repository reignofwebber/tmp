#ifndef MESSAGE_OBJECT_H_
#define MESSAGE_OBJECT_H_

// std
#include <string>
#include <map>
#include <vector>

// local
#include "message.h"


// identifiers
enum Message_Identifier {
    C2S_Authentication,
};


struct MessageObject {
    virtual Message_Identifier getMessageIdentifier() const = 0;
    virtual std::string msg() const = 0;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// client -> server
MessageObject getMessageObject(const std::string &msg);


// control protocol
struct C2SAuthentication : public MessageObject{
    std::string username;
    std::string password;

    Message_Identifier getMessageIdentifier() const override {
        return C2S_Authentication;
    }

    std::string msg() const override {}
};






/**
 * ci
 * index
 * name
 * type
 *
 * UI_CIM_et_config
 */
struct PIOInfo : public MessageObject{
    std::string ci;
    std::string id;
    std::string type;
    bool status;

    std::string msg() const override;
};

struct PIOConfig : public MessageObject{
    std::map<std::string, std::map<std::string, PIOInfo>> pioList;

    std::string msg() const override;
};




#endif  // MESSAGE_OBJECT_H_
