#include <Arduino.h>

#include "interface.h"
#include "resource.h"

Interface interface;

void setup() {
    Serial.begin(115200);

    interface.begin();
    interface.wait_connect();
}

void loop() {
    static uint64_t timer = 0;

    if (millis() - timer > 5000) {
        interface.state = interface.state == Interface::connected
                              ? Interface::idle
                              : Interface::connected;
        timer = millis();
    }

    uint64_t _timer = millis();
    interface.flash();
    Serial.print("Runing flash take ");
    Serial.print(millis() - _timer);
    Serial.println("ms");
}