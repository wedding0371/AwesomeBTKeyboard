#include "hc74165.h"

#include "BleComboKeyboard.h"
#include "BleComboMouse.h"

#define PLACEHOLDER 255

#ifdef BOARD_VERSION_0_2
static uint8_t KeyCode[] = {
    'q',           '3',           't',
    KEY_TAB,       '`',           '1',
    PLACEHOLDER,   PLACEHOLDER,

    'z',           KEY_LEFT_ALT,  'c',
    'x',           'w',           '2',
    '4',           '5',

    'f',           ' ',           'e',
    'r',           'a',           KEY_LEFT_SHIFT,
    KEY_LEFT_CTRL, KEY_CAPS_LOCK,

    KEY_LEFT_GUI,  'g',           PLACEHOLDER,
    MOUSE_LEFT,    'd',           's',  // Win right
    'b',           'v',
};
static uint8_t KeyDevice[] = {
    KEY, KEY, KEY, KEY, KEY, KEY,   KEY, KEY, KEY, KEY, KEY,
    KEY, KEY, KEY, KEY, KEY, KEY,   KEY, KEY, KEY, KEY, KEY,
    KEY, KEY, KEY, KEY, KEY, MOUSE, KEY, KEY, KEY, KEY,
};
#endif

HC74165::HC74165() {}

void HC74165::begin(int pl, int q, int cp, int ce, int cascade) {
    this->pl = pl;
    this->q = q;
    this->cp = cp;
    this->ce = ce;
    this->cascade = cascade;
    if (ce == -1) {
        this->shift = &HC74165::_shift_withoutce;
    } else {
        pinMode(this->ce, OUTPUT);
        digitalWrite(this->ce, HIGH);
        this->shift = &HC74165::_shift_withce;
    }

    for (uint16_t i = 0; i < sizeof(this->key_states); i++) {
        this->key_states[i] = true;
    }

    pinMode(this->pl, OUTPUT);
    pinMode(this->cp, OUTPUT);
    pinMode(this->q, INPUT);
}

void HC74165::_shift_withoutce(void) {
    for (uint8_t i = 0; i < this->cascade; i++)
        this->datas[i] = shiftIn(this->q, this->cp, LSBFIRST);
}

void HC74165::_shift_withce(void) {
    digitalWrite(this->ce, LOW);
    for (uint8_t i = 0; i < this->cascade; i++)
        this->datas[i] = shiftIn(this->q, this->cp, LSBFIRST);
    digitalWrite(this->ce, HIGH);
}

void HC74165::scan(void) {
    digitalWrite(this->pl, LOW);
    delayMicroseconds(5);
    digitalWrite(this->pl, HIGH);
    delayMicroseconds(5);

    digitalWrite(this->cp, HIGH);
    delayMicroseconds(5);

    (this->*shift)();

    // // digitalWrite(this->ce, LOW);
    // for (uint8_t i = 0; i < this->cascade; i++)
    //     this->datas[i] = shiftIn(this->q, this->cp, LSBFIRST);
    // // digitalWrite(this->ce, HIGH);
}

void HC74165::append(KeyState s, KeyDevices d, char code) {
    this->buffers[this->buffer_ptr].state = s;
    this->buffers[this->buffer_ptr].device = d;
    this->buffers[this->buffer_ptr].code = code;
    this->buffer_ptr++;
}

bool HC74165::get(KeyValue *v) {
    if (this->buffer_ptr == 0) return false;

    v->state = this->buffers[0].state;
    v->device = this->buffers[0].device;
    v->code = this->buffers[0].code;

    for (uint8_t i = 0; i < this->buffer_ptr - 1; i++) {
        this->buffers[i] = this->buffers[i + 1];
    }
    this->buffer_ptr--;

    return true;
}

void HC74165::read(void) {
    if (test_mode) return;
    scan();

    for (uint8_t i = 0; i < this->cascade; i++) {
        for (int8_t n = 7; n >= 0; n--) {
            bool offset = (bool)(this->datas[i] >> n & 0x01);
            bool *local = &this->key_states[i * 8 + n];
            if (offset != *local) {
                uint8_t index = i * 8 + (7 - n);
                KeyState s = *local ? PRESS : RELEASE;
                append(s, (KeyDevices)KeyDevice[index], (char)KeyCode[index]);
                *local = offset;
            }
        }
    }
}

void HC74165::_print_origin(void) {
    // This function used to test KeyCode
    static uint32_t timer = 0;
    test_mode = true;

    if (millis() - timer > 100) {
        timer = millis();
        scan();

        Serial.print("Pin States:\r\n");
        for (uint8_t i = 0; i < this->cascade; i++) {
            Serial.print(this->datas[i], BIN);
            Serial.print(",");
        }
        Serial.println();
    }
}