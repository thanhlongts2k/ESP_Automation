#ifndef RELAY_CONTROLLER_H
#define RELAY_CONTROLLER_H

#include <Arduino.h>
#include "config.h"

/**
 * @class RelayController
 * @brief Lớp điều khiển công tắc ngắt rơ-le (Relay 1 Đèn & Relay 2 Quạt) và Hysteresis tự động.
 * 
 * Lớp này chịu trách nhiệm đóng ngắt tín hiệu GPIO điều khiển Relay mức THẤP (LOW Active),
 * đồng thời tính toán thuật toán Hysteresis (vùng đệm nhiệt độ 32°C/29°C) để tự động bật/tắt quạt
 * tránh hiện tượng relay bị chập chờn nhấp nháy liên tục khi nhiệt độ dao động quanh ngưỡng.
 */
class RelayController {
public:
    RelayController();

    /** @brief Khởi tạo các chân GPIO điều khiển Relay */
    void begin();

    /** @brief Đóng/Ngắt Relay 1 (Đèn) - true: Bật (LOW), false: Tắt (HIGH) */
    void setRelay1(bool state);

    /** @brief Đóng/Ngắt Relay 2 (Quạt) - true: Bật (LOW), false: Tắt (HIGH) */
    void setRelay2(bool state);
    
    /** @brief Lấy trạng thái hiện tại của Relay 1 dưới dạng bool (true/false) */
    bool getRelay1State() const { return _relay1State; }

    /** @brief Lấy trạng thái hiện tại của Relay 2 dưới dạng bool (true/false) */
    bool getRelay2State() const { return _relay2State; }

    /** @brief Lấy trạng thái Relay 1 dưới dạng chuỗi ("ON"/"OFF") */
    String getRelay1StateStr() const { return _relay1State ? "ON" : "OFF"; }

    /** @brief Lấy trạng thái Relay 2 dưới dạng chuỗi ("ON"/"OFF") */
    String getRelay2StateStr() const { return _relay2State ? "ON" : "OFF"; }

    /** 
     * @brief Thuật toán Hysteresis tự động điều khiển Quạt (Relay 2) theo nhiệt độ
     * @param currentTemp Nhiệt độ hiện tại đo từ cảm biến (°C)
     */
    void processHysteresis(float currentTemp);

private:
    bool _relay1State;  ///< Trạng thái bật/tắt của Đèn (Relay 1)
    bool _relay2State;  ///< Trạng thái bật/tắt của Quạt (Relay 2)
};

#endif // RELAY_CONTROLLER_H
