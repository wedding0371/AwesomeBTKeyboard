#ifndef _KEYPAD_H_
#define _KEYPAD_H_

#include <Adafruit_NeoPixel.h>
#include <HardwareSerial.h>

#include "AiEsp32RotaryEncoder.h"
#include "hc74165.h"
#include "resource.h"

struct KeyStatus {
    bool press = false;
    bool release = false;

    char code = 0;
};

class Keypad {
  private:
    int32_t middle_number[2] = {0, 0};
    int32_t stderr_number[2] = {0, 0};

    uint8_t simple_count = 20;
    uint8_t encoder_limit = 20;
    uint8_t rocker_scale = 50;

    bool led_changed = false;
    uint32_t led_buffer[108] = {0};

    uint8_t encoder_a_pin;
    uint8_t encoder_b_pin;
    uint8_t encoder_but_pin;

    uint8_t axis_x_pin;
    uint8_t axis_y_pin;
    uint8_t axis_but_pin;

    uint8_t key_tx_pin;
    uint8_t key_rx_pin;
    // uint8_t keyboard_serial;

    bool rocker_isclicked = false;

    uint8_t led_pin;
    uint8_t led_number;

    Adafruit_NeoPixel pixels;
    HardwareSerial serial = HardwareSerial(Serial1);

  public:
    AiEsp32RotaryEncoder encoder;
    Keypad(uint8_t encoder_a_pin = ROTARY_ENCODER_A_PIN,
           uint8_t encoder_b_pin = ROTARY_ENCODER_B_PIN,
           uint8_t encoder_but_pin = ROTARY_ENCODER_BUTTON_PIN,
           uint8_t axis_x_pin = X_AXIS_PIN, uint8_t axis_y_pin = Y_AXIS_PIN,
           uint8_t axis_but_pin = BUT_AXIS_PIN, uint8_t key_tx_pin = KEY_TX_PIN,
           uint8_t key_rx_pin = KEY_RX_PIN, uint8_t led_pin = LED_PIN,
           uint8_t led_number = LED_NUMPIXELS);

    void begin(uint8_t encoder_limit = 20);

    int encoder_change();
    bool encoder_clicked();

    int key_read(uint8_t *code, bool *state);
    int key_read(struct KeyStatus *ks);

    void rocker_init();
    void rocker_correct();
    float rocker_shift(int axis);
    float rocker_position(int axis);
    bool rocker_clicked();
    bool rocker_released();

    void led_update();
    void led_clear(uint8_t r, uint8_t g, uint8_t b);
    void led_write(uint8_t num, uint8_t r, uint8_t g, uint8_t b);
};

#endif