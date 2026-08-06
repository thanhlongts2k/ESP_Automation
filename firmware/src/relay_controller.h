#ifndef RELAY_CONTROLLER_H
#define RELAY_CONTROLLER_H

#include <Arduino.h>
#include "config.h"

class RelayController {
public:
    RelayController();
    void begin();
    void setRelay1(bool state);
    void setRelay2(bool state);
    
    bool getRelay1State() const { return _relay1State; }
    bool getRelay2State() const { return _relay2State; }
    String getRelay1StateStr() const { return _relay1State ? "ON" : "OFF"; }
    String getRelay2StateStr() const { return _relay2State ? "ON" : "OFF"; }

    // Hysteresis điều khiển tự động quạt (Relay 2) theo nhiệt độ
    void processHysteresis(float currentTemp);

private:
    bool _relay1State;
    bool _relay2State;
};

#endif // RELAY_CONTROLLER_H
