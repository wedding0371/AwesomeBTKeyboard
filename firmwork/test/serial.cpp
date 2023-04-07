#include <Arduino.h>
#include <Ticker.h>

#include "keypad.h"
#include "resource.h"

Keypad keypad;
Ticker event_ticker;

void _output(String m, String s = "") {
    Serial.print("Time: ");
    Serial.print(millis());
    Serial.print(", ");
    Serial.print(m);
    Serial.println(s);
}

void event_handle() {
    struct KeyStatus ks;
    if (keypad.key_read(&ks)) {
        if (ks.press) _output("Key touch: ", String(ks.code));
        if (ks.release) _output("Key release: ", String(ks.code));
    }

    if (int8_t delta = keypad.encoder_change())
        _output("Encoder count: ", String(delta));
    if (keypad.encoder_clicked()) {
        _output("Encoder clicked, and correct rocker");
        keypad.rocker_correct();
    }

    float x = keypad.rocker_shift(1), y = keypad.rocker_shift(0);
    if (x) _output("Rocker Xaxis: ", String(x));
    if (y) _output("Rocker Yaxis: ", String(y));
}

void setup() {
    Serial.begin(115200);

    keypad.begin(20);
    keypad.encoder.setup([] { keypad.encoder.readEncoder_ISR(); });
    event_ticker.attach_ms(10, event_handle);
}

void HC74165_test() {
    int load = 22;     // PL pin 22， Board: IO
    int dataIn = 23;   // HQ7 pin 23, Board: TX
    int clockIn = 19;  // CP pin 19, Board: LED

    pinMode(load, OUTPUT);
    // pinMode(clockEnablePin, OUTPUT);
    pinMode(clockIn, OUTPUT);
    pinMode(dataIn, INPUT);
    while (1) {
        uint32_t timer = micros();
        digitalWrite(load, LOW);  // Write pulse to load pin
        delayMicroseconds(5);
        digitalWrite(load, HIGH);
        delayMicroseconds(5);

        digitalWrite(clockIn, HIGH);  // Get data from 74HC165
        delayMicroseconds(5);

        // digitalWrite(clockEnablePin, LOW);
        byte incoming = shiftIn(dataIn, clockIn, LSBFIRST);
        // digitalWrite(clockEnablePin, HIGH);

        uint32_t delta_time = micros() - timer;
        Serial.print("Time: ");  // It take about 25us, 40kHz
        Serial.print(delta_time);
        Serial.print(", Pin States:\r\n");  // Print to serial monitor
        Serial.println(incoming, BIN);
        delay(200);
    }
}

void loop() { HC74165_test(); }