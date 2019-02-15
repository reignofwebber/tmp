#ifndef MANAGER_H_
#define MANAGER_H_

// std
#include <string>
#include <vector>
#include <map>

// local
#include "global.h"
#include "message_object.h"

class Manager {
 public:
    Manager() = default;

    void start();

    void welcome();

    // set pio status
    /**
     * ci
     * pos
     * status true or false
     * status1
     * status2
     * description
     * timestamp
     *
     * UI_CIM_et_in
     * UI_CIM_et_out
     */
    void updatePIOStatus(const std::string &ci, const std::string pio_id, bool status);

 private:
    void initPIO();

    void add_to_send_queue(const MessageObject &mobj);

 private:
    PIOConfig m_pioConfig;
};




#endif  // MANAGER_H_
