#ifndef GLOBAL_H_
#define GLOBAL_H_

// std
#include <string>
#include <memory>
#include <utility>

class Message;
class MessageObject;

class Session;
using SessionPtr = std::shared_ptr<Session>;

enum Message_Type {
    RealTime_Type,
    History_Type,
};

enum RuleSet {
    All_RuleSet,    
    OverallRuleSet,
    Unimplement_RuleSet,
};

enum Subscribe_Level {
    Subscribe_Area,
    Subscribe_Line,
    Subscribe_Device,
};

enum Playback_Control {
    Playback_Pause,
    Playback_Resume,
    Playback_Stop,
};

enum Subscribe_Type {
    Sub_Device,
    Sub_Station,
};

struct IndentiSet {
    std::string id;
    std::pair<RuleSet, Subscribe_Level> ruleSet;
};

inline bool operator==(const IndentiSet &l, const IndentiSet &r) {
    return l.id == r.id && l.ruleSet == r.ruleSet;
}



#endif  // GLOBAL_H_
