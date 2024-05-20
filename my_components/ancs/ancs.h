#pragma once

#include "esphome/core/component.h"
#include <esp32notifications.h>


namespace esphome {
namespace ancs {

class ANCSComponent : public Component {
    public:
        void setup() override;

    protected:
        void onBLEStateChanged(BLENotifications::State state);
        void onNotificationArrived(const ArduinoNotification * notification, const Notification * rawNotificationData);
        void onNotificationRemoved(const ArduinoNotification * notification, const Notification * rawNotificationData);
        BLENotifications bleNotifications;
        uint32_t incomingCallNotificationUUID;
};
 
}
}