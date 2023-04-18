#include <Arduino.h>
#include <Ticker.h>
#include <math.h>

#include "BleComboKeyboard.h"
#include "BleComboMouse.h"
#include "hc74165.h"
#include "interface.h"
#include "keypad.h"
#include "resource.h"
#include "utility.h"

Keypad keypad;
Ticker event_ticker;
Ticker key_ticker;
Interface interface;

BleComboKeyboard myKeyboard("Awesome KeyBoard", "Ryan");
BleComboMouse myMouse(&myKeyboard);

bool connected = false;

HC74165 hc;

void event_handle() {
    if (!connected) return;

    struct KeyStatus ks;
    while (keypad.key_read(&ks)) {
        if (ks.press) myKeyboard.press(ks.code);
        if (ks.release) myKeyboard.release(ks.code);
    }
    struct KeyValue kv;
    while (hc.get(&kv)) {
        if (kv.device == MOUSE) {
            if (kv.state == PRESS) myMouse.press(kv.code);
            if (kv.state == RELEASE) myMouse.release(kv.code);
        } else {
            if (kv.state == PRESS) myKeyboard.press(kv.code);
            if (kv.state == RELEASE) myKeyboard.release(kv.code);
        }
    }

    float x = keypad.rocker_shift(1), y = keypad.rocker_shift(0);
    if ((x != 0) || (y != 0)) {
        if (interface.state != Interface::choose &&
            interface.state != Interface::success) {
            if (GLOBAL_ROCKER_FUNCTION == MenuConfig::R_MOUSE) {
                x *= GLOBAL_ROCKER_MOUSE_SCALE, y *= GLOBAL_ROCKER_MOUSE_SCALE;
                myMouse.move(x, -y);
            } else if (GLOBAL_ROCKER_FUNCTION == MenuConfig::R_WHEEL) {
                x *= GLOBAL_ROCKER_WHEEL_SCALE, y *= GLOBAL_ROCKER_WHEEL_SCALE;
                myMouse.move(0, 0, y, -x);
            }
        } else {
            static MenuConfig::RockerFunction buffer = MenuConfig::R_MOUSE;
            global_function_choose.radio = sqrt(x * x + y * y);
            global_function_choose.delta = atan2(y, x) * 180 / PI;
            if (global_function_choose.radio > 0.8) {
                switch (quadrant(global_function_choose.delta)) {
                    case 1:
                        buffer = MenuConfig::R_MOUSE;
                        break;
                    case 2:
                        buffer = MenuConfig::R_WHEEL;
                        break;
                    case 3:
                    case 4:
                    default:
                        break;
                }
                interface.state = Interface::success;
            }
            if (global_function_choose.radio < 0.5)
                GLOBAL_ROCKER_FUNCTION = buffer;
        }
    }

    if (keypad.rocker_clicked()) {
        interface.state = Interface::choose;
    }

    if (int8_t delta = keypad.encoder_change()) {
        if (interface.state == Interface::config) {
            int *v = &global_menu_config.menu[global_menu_choose]->value;
            *v += delta;
            *v = max(global_menu_config.menu[global_menu_choose]->min, *v);
            *v = min(global_menu_config.menu[global_menu_choose]->max, *v);
        } else {
            if (delta > 0)
                myKeyboard.write(KEY_MEDIA_VOLUME_UP);
            else
                myKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
        }
    }

    if (keypad.encoder_clicked()) {
        keypad.rocker_correct();

        if (interface.state == Interface::config) {
            global_menu_choose =
                global_menu_choose == (global_menu_config.value - 1)
                    ? 0
                    : ++global_menu_choose;

            if (global_menu_choose == 0) interface.state = Interface::connected;
        } else if (interface.state == Interface::connected) {
            interface.state = Interface::config;
        }
    }
}

void parameter_init(void) {
    while (global_menu_config.menu[++global_menu_config.value] != NULL)
        ;
}

static void key_scan(void) { hc.read(); }

void setup() {
    Serial.begin(115200);
    parameter_init();

    interface.begin();

    myKeyboard.begin();
    myMouse.begin();

    keypad.begin(20);
    keypad.encoder.setup([] { keypad.encoder.readEncoder_ISR(); });

    event_ticker.attach_ms(10, event_handle);

    hc.begin();
    key_ticker.attach_ms(1, key_scan);
}

void loop() {
    // hc._print_origin();

    static uint64_t timer = millis();
    static uint8_t last_connect_state = connected;
    if (millis() - timer > 200) {
        connected = myKeyboard.isConnected();
        if (!connected) interface.state = Interface::idle;

        if (connected && !last_connect_state)
            interface.state = Interface::connected;

        last_connect_state = connected;
        timer = millis();
    }

    // keypad.led_update();
    interface.flash();
}