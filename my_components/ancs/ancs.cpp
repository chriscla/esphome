#include "ancs.h"
#include "esphome/core/log.h"
 
namespace esphome {
namespace ancs {

static const char *const TAG = "ancs.ancs";


void ANCSComponent::setup() {
    ESP_LOGI(TAG, "ANCSComponent Setup");

    // Set up the BLENotification library
    this->bleNotifications.begin("LOCKBOX");

    //auto connectionStateCallback = std::bind(&ANCSComponent::onBLEStateChanged, this, std::placeholders::_1);
    //this->bleNotifications.setConnectionStateChangedCallback(connectionStateCallback);
    this->bleNotifications.setConnectionStateChangedCallback([this](BLENotifications::State state) {
        this->onBLEStateChanged(state);
        });
    this->bleNotifications.setConnectionStateChangedCallback(connectionStateCallback);
  //  this->bleNotifications.setNotificationCallback(this->onNotificationArrived);
   // this->bleNotifications.setRemovedCallback(this->onNotificationRemoved);
}

// This callback will be called when a Bluetooth LE connection is made or broken.
// You can update the ESP 32's UI or take other action here.
void ANCSComponent::onBLEStateChanged(BLENotifications::State state) {
  switch(state) {
      case BLENotifications::StateConnected:
          ESP_LOGI(TAG, "StateConnected - connected to a phone or tablet"); 
          break;

      case BLENotifications::StateDisconnected:
          ESP_LOGI(TAG, "StateDisconnected - disconnected from a phone or tablet"); 
          /* We need to startAdvertising on disconnection, otherwise the ESP 32 will now be invisible.
          IMO it would make sense to put this in the library to happen automatically, but some people in the Espressif forums
          were requesting that they would like manual control over re-advertising.*/
          this->bleNotifications.startAdvertising(); 
          
          break; 
  }
}


// A notification arrived from the mobile device, ie a social media notification or incoming call.
// parameters:
//  - notification: an Arduino-friendly structure containing notification information. Do not keep a
//                  pointer to this data - it will be destroyed after this function.
//  - rawNotificationData: a pointer to the underlying data. It contains the same information, but is
//                         not beginner-friendly. For advanced use-cases.
void ANCSComponent::onNotificationArrived(const ArduinoNotification * notification, const Notification * rawNotificationData) {
    ESP_LOGI(TAG, "Got notification: ");   
    ESP_LOGI(TAG,"Title: %s",notification->title.c_str()); // The title, ie name of who sent the message
    ESP_LOGI(TAG,"Message: %s",notification->message.c_str()); // The detail, ie "be home for dinner at 7".
    ESP_LOGI(TAG,"App: %s",notification->type.c_str());  // Which app sent it
    ESP_LOGI(TAG,"Category: %s",this->bleNotifications.getNotificationCategoryDescription(notification->category));  // ie "social media"
    ESP_LOGI(TAG,"Cat Count: %d",notification->categoryCount); // How may other notifications are there from this app (ie badge number)
    if (notification->category == CategoryIDIncomingCall) {
		// If this is an incoming call, store it so that we can later send a user action.
        incomingCallNotificationUUID = notification->uuid;
        ESP_LOGI(TAG,"--- INCOMING CALL: PRESS A TO ACCEPT, C TO REJECT ---"); 
    }
    else {
        incomingCallNotificationUUID = 0; // Make invalid - no incoming call
    }
}

// A notification was cleared
void ANCSComponent::onNotificationRemoved(const ArduinoNotification * notification, const Notification * rawNotificationData) {
     ESP_LOGI(TAG, "Removed notification: ");   
     //Serial.println(notification->title);
     //Serial.println(notification->message);
     //Serial.println(notification->type);  
}

}
}



