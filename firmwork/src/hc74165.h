#ifndef _HC_74161_H_
#define _HC_74161_H_

#include <Arduino.h>

#include "resource.h"

enum KeyState {
    PRESS,
    RELEASE,
};

enum KeyDevices {
    KEY,
    MOUSE,
};

struct KeyValue {
    KeyState state;
    KeyDevices device = KEY;
    char code = 0;
};

class HC74165 {
  private:
    int pl;
    int q;
    int cp;
    int ce;

    bool test_mode = false;

    uint8_t cascade;
    uint8_t datas[16] = {0};

    bool key_states[256] = {1};

    uint8_t buffer_ptr = 0;
    struct KeyValue buffers[32];

    void append(KeyState s, KeyDevices d, char code);
    void _shift_withoutce(void);
    void _shift_withce(void);
    void (HC74165::*shift)(void);

  public:
    HC74165();
    void begin(int pl = HC_PL_PIN, int q = HC_Q_PIN, int cp = HC_CP_PIN,
               int ce = -1, int cascade = HC_CASCADE);
    void scan(void);
    void read(void);
    void _print_origin(void);

    bool get(KeyValue *v);
};

#endif