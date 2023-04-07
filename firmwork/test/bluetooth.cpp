#include <Arduino.h>

#include "BleComboKeyboard.h"
#include "BleComboMouse.h"
#include "resource.h"
BleComboKeyboard myKeyboard;
BleComboMouse myMouse(&myKeyboard);

bool connected = false;
bool testdown = false;

void mouse_test(void);
void keypad_test(void);

void setup() {
    Serial.begin(115200);

    myKeyboard.begin();
    myMouse.begin();
}

void loop() {
    if (!myKeyboard.isConnected()) return;

    if (!testdown) {
        delay(1000);
        mouse_test();
        keypad_test();
        testdown = true;
    }
}

void mouse_test(void) {
    unsigned long startTime;

    Serial.println("Move mouse pointer up");
    startTime = millis();
    while (millis() < startTime + 1000) {
        myMouse.move(0, -1);
        delay(5);
    }

    Serial.println("Right click");
    myMouse.click(MOUSE_RIGHT);
    delay(500);

    Serial.println("Move mouse pointer left");
    startTime = millis();
    while (millis() < startTime + 1000) {
        myMouse.move(-1, 0);
        delay(5);
    }

    Serial.println("Left click");
    myMouse.click(MOUSE_LEFT);
    delay(500);

    Serial.println("Move mouse pointer down");
    startTime = millis();
    while (millis() < startTime + 1000) {
        myMouse.move(0, 1);
        delay(5);
    }

    Serial.println("Move mouse pointer right");
    startTime = millis();
    while (millis() < startTime + 1000) {
        myMouse.move(1, 0);
        delay(5);
    }

    Serial.println("Scroll Down");
    myMouse.move(0, 0, -1);

    Serial.println("Scroll wheel click");
    myMouse.click(MOUSE_MIDDLE);
    delay(500);

    Serial.println("Back button click");
    myMouse.click(MOUSE_BACK);
    delay(500);

    Serial.println("Forward button click");
    myMouse.click(MOUSE_FORWARD);
    delay(500);

    Serial.println("Click left+right mouse button at the same time");
    myMouse.click(MOUSE_LEFT | MOUSE_RIGHT);
    delay(500);

    Serial.println(
        "Click left+right mouse button and scroll wheel at the same time");
    myMouse.click(MOUSE_LEFT | MOUSE_RIGHT | MOUSE_MIDDLE);
    delay(500);
}

void keypad_test(void) {
    Serial.println("Media volume up");
    myKeyboard.write(KEY_MEDIA_VOLUME_UP);
    delay(500);

    Serial.println("Media volume down");
    myKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
    delay(500);

    Serial.println("Media volume mute");
    myKeyboard.write(KEY_MEDIA_MUTE);
    delay(500);

    Serial.println("Key touch 102");
    myKeyboard.press(102);
    delay(1);
    myKeyboard.release(102);
    delay(500);

    Serial.println("Key switch eng/chn");
    myKeyboard.press(KEY_LEFT_SHIFT);
    myKeyboard.press(' ');
    myKeyboard.release(KEY_LEFT_SHIFT);
    myKeyboard.release(' ');
    delay(500);
}