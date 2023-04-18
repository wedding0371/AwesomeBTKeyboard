#include "keypad.h"

#include <math.h>

// const uint8_t KeyCode[] = {
// const uint8_t *KeyCode = {
// 	'1', '4', '7', '*', ' ', ' ', ' ',
// 	'2', '5', '8', '0', ' ', ' ', ' ',
// 	'3', '6', '9', '#', ' ', ' ', ' ',
// 	'A', 'B', 'C', 'D', ' ', ' ', ' '
// };

extern uint8_t KeyCode[];

Keypad::Keypad(uint8_t encoder_a_pin, uint8_t encoder_b_pin,
               uint8_t encoder_but_pin, uint8_t axis_x_pin, uint8_t axis_y_pin,
               uint8_t axis_but_pin, uint8_t key_tx_pin, uint8_t key_rx_pin,
               uint8_t led_pin, uint8_t led_number) {
    this->encoder_a_pin = encoder_a_pin;
    this->encoder_b_pin = encoder_b_pin;
    this->encoder_but_pin = encoder_but_pin;

    this->axis_x_pin = axis_x_pin;
    this->axis_y_pin = axis_y_pin;
    this->axis_but_pin = axis_but_pin;

    this->key_tx_pin = key_tx_pin;
    this->key_rx_pin = key_rx_pin;

    this->led_pin = led_pin;
    this->led_number = led_number;
}

void Keypad::begin(uint8_t encoder_limit) {
    this->pixels = Adafruit_NeoPixel(this->led_number, this->led_pin,
                                     NEO_GRB + NEO_KHZ800);
    this->pixels.begin();
    led_update();

    this->encoder = AiEsp32RotaryEncoder(
        this->encoder_a_pin, this->encoder_b_pin, this->encoder_but_pin, -1, 4);
    this->encoder.begin();
    this->encoder.setBoundaries(0, 100000, true);
    this->encoder.setAcceleration(1);
    this->encoder.setEncoderValue(50000);
    (void)this->encoder.encoderChanged();
    this->encoder_limit = encoder_limit;

    this->serial.begin(9600, SERIAL_8N1, this->key_rx_pin,
                       this->key_tx_pin);  // 14: tx, 13: rx

    rocker_init();
}

// void Keypad::setup(void (*ISR_callback)(void), ) {

// }

int Keypad::encoder_change() {
    // 编码器在实际使用时，会产生两个脉冲波，但是体感旋钮仅转动一次，因此一次返回值，一次取零
    // static uint8_t last_return = false;
    int delta = -this->encoder.encoderChanged();
    if (delta > this->encoder_limit) delta = this->encoder_limit;
    if (delta < -this->encoder_limit) delta = -this->encoder_limit;
    // if (!last_return) {
    //     last_return = true;
    return delta;
    // }
    // last_return = false;
    return 0;
}

bool Keypad::encoder_clicked() {
    return this->encoder.isEncoderButtonClicked();
}

// int Keypad::key_changed() {
// 	return this->serial.available();
// }

int Keypad::key_read(struct KeyStatus *ks) {
    int change = this->serial.available();

    if (change == 0) return change;

    // Serial.println(sizeof(KeyCode));

    // struct KeyStatus s;
    uint8_t key_char = this->serial.read();

    // Serial.println(change);
    // if (change > 1) {
    // 	Serial.println(key_char);
    // 	// Serial.println(this->serial.read());
    // 	Serial.println(this->serial.available());
    // }

    if (key_char < 0x80) {
        ks->press = true;
    } else {
        ks->release = true;
        key_char -= 0x80;
    }
    ks->code = KeyCode[key_char];

    return change;
}

int Keypad::key_read(uint8_t *code, bool *state) {
    int change = this->serial.available();
    if (change == 0) return change;

    *state = false;
    *code = this->serial.read();
    if (*code >= 0x80) {
        *state = true;
        *code -= 0x80;
    }

    return change;
}

void Keypad::rocker_init() {
    pinMode(this->axis_x_pin, INPUT);
    pinMode(this->axis_y_pin, INPUT);
    pinMode(this->axis_but_pin, INPUT);

    delay(500);
    rocker_correct();
}

void Keypad::rocker_correct() {
#define SIMPLE_COUNT 200
    uint16_t buffer[2][SIMPLE_COUNT];
    for (uint8_t i = 0; i < SIMPLE_COUNT; i++) {
        buffer[0][i] = analogRead(this->axis_x_pin);
        buffer[1][i] = analogRead(this->axis_y_pin);

        this->middle_number[0] += buffer[0][i];
        this->middle_number[1] += buffer[1][i];
    }
    this->middle_number[0] /= SIMPLE_COUNT;
    this->middle_number[1] /= SIMPLE_COUNT;
    for (uint8_t i = 0; i < SIMPLE_COUNT; i++) {
        this->stderr_number[0] +=
            pow((buffer[0][i] - this->middle_number[0]), 2);
        this->stderr_number[1] +=
            pow((buffer[1][i] - this->middle_number[1]), 2);
    }
    this->stderr_number[0] = int(this->stderr_number[0] / SIMPLE_COUNT);
    this->stderr_number[1] = int(this->stderr_number[1] / SIMPLE_COUNT);
}

float Keypad::rocker_position(int axis) {
    float value = 0;
    uint8_t pin = (axis == 0) ? this->axis_x_pin : this->axis_y_pin;

    for (uint8_t i = 0; i < this->simple_count; i++) value += analogRead(pin);
    value = value / this->simple_count - this->middle_number[axis];

    // 根据当前的位置进行标准化，为标准化的状态下，左偏最大值-45%，右偏最大值55%
    // if value > this->stderr_number[axis]
    if (abs(value) > this->stderr_number[axis] * 1.02) {
        return value / 4095;
    }
    return 0;
}

float Keypad::rocker_shift(int axis) {
    float value = rocker_position(axis);

    if (value == 0) return 0;

    if (axis == 0) value = -value;

    value *= 2;
    if (value > 1) value = 1;
    if (value < -1) value = -1;

    // value *= abs(tan(value * PI / 4));
    // value *= this->rocker_scale;
    return value;
}

bool Keypad::rocker_clicked() {
    static bool keystate = false;
    if (digitalRead(this->axis_but_pin) == 0) {
        delay(20);
        if (digitalRead(this->axis_but_pin) == 0) {
            if (!keystate) {
                keystate = true;
                this->rocker_isclicked = true;
                return true;
            }
        }
    } else {
        keystate = false;
    }
    return false;
}

bool Keypad::rocker_released() {
    if (this->rocker_isclicked) {
        if (digitalRead(this->axis_but_pin) == 1) {
            delay(20);
            if (digitalRead(this->axis_but_pin) == 1) {
                return true;
            }
        }
    }
    return false;
}

void Keypad::led_clear(uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < this->led_number; i++) {
        this->led_buffer[i] = this->pixels.Color(r, g, b);
    }
    this->led_changed = true;
}

void Keypad::led_write(uint8_t num, uint8_t r, uint8_t g, uint8_t b) {
    this->led_buffer[num] = this->pixels.Color(r, g, b);
    this->led_changed = true;
}

void Keypad::led_update() {
    if (!this->led_changed) return;

    this->pixels.clear();
    for (int i = 0; i < this->led_number; i++) {
        // this->pixels.setPixelColor(i, this->pixels.Color(150, 150, 0));
        this->pixels.setPixelColor(i, this->led_buffer[i]);
    }
    this->pixels.show();
}