#include <Arduino.h>
#include <Ticker.h>

#include "BleComboKeyboard.h"
#include "BleComboMouse.h"
#include "interface.h"
#include "keypad.h"
#include "resource.h"

Interface interface;

Keypad keypad;
Ticker event_ticker;
BleComboKeyboard myKeyboard;
BleComboMouse myMouse(&myKeyboard);

bool connected = false;

#define ROCKER_SCALE 10

void event_handle() {
    // if (!connected) return;

    struct KeyStatus ks;
    while (keypad.key_read(&ks)) {
        if (ks.press) myKeyboard.press(ks.code);
        if (ks.release) myKeyboard.release(ks.code);
    }

    if (int8_t delta = keypad.encoder_change()) {
        if (delta > 0)
            for (uint8_t i = 0; i < abs(delta); i++)
                myKeyboard.write(KEY_MEDIA_VOLUME_UP);
        else
            for (uint8_t i = 0; i < abs(delta); i++)
                myKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
    }

    if (keypad.encoder_clicked()) {
        // myKeyboard.write(KEY_MEDIA_MUTE);
        keypad.rocker_correct();
    }

    float x = keypad.rocker_shift(1) * ROCKER_SCALE,
          y = keypad.rocker_shift(0) * ROCKER_SCALE;
    // if (x != 0) myMouse.move(0, 0, 0, -x);// WHELL
    // if (y != 0) myMouse.move(0, 0, -y, 0);
    if ((x != 0) || (y != 0)) {
        myMouse.move(x, y);
        // Serial.println(x, y);
    }

    if (keypad.rocker_clicked()) myMouse.click(MOUSE_LEFT);
}

void setup() {
    Serial.begin(115200);

    interface.begin();

    myKeyboard.begin();
    myMouse.begin();

    keypad.begin(20);
    keypad.encoder.setup([] { keypad.encoder.readEncoder_ISR(); });

    event_ticker.attach_ms(10, event_handle);
}

void loop() {
    static uint64_t timer = millis();
    if (millis() - timer > 200) {
        // connected =
        if (myKeyboard.isConnected()) {
            interface.state = Interface::connected;
        } else {
            interface.state = Interface::idle;
        }
        timer = millis();
    }

    // keypad.led_update();
    interface.flash();
}