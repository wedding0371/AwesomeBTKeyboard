#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#define BOARD_VERSION_0_2


#ifdef BOARD_VERSION_0_2
  // ESP32 TTGO OLED V2.0
  #define OLED_RESET_PIN 16
  #define OLED_CLOCK_PIN 15
  #define OLED_DATA_PIN 4
  #define OLED_WIDTH 128
  #define OLED_HEIGHT 64

  // KeyPad
  #define ROTARY_ENCODER_A_PIN 32
  #define ROTARY_ENCODER_B_PIN 21
  #define ROTARY_ENCODER_BUTTON_PIN 25

  #define X_AXIS_PIN 35
  #define Y_AXIS_PIN 34
  #define BUT_AXIS_PIN 33

  #define KEY_TX_PIN 14
  #define KEY_RX_PIN 13

  #define LED_PIN 2
  #define LED_NUMPIXELS 16

  // HC74165
  #define HC_PL_PIN 22  //  Board: IO
  #define HC_Q_PIN 23   //  Board: TX
  #define HC_CP_PIN 19  // Board: LED
  #define HC_CASCADE 4
#endif



extern int global_menu_choose;

struct MenuLine {
    char* name;
    int value;
    int min, max;
};

class MenuConfig {
  public:
    enum RockerFunction {
        R_MOUSE,
        R_WHEEL,

        _R_MAX,
    };

    enum EncodeFunction {
        E_VOLUME,

        _E_MAX,
    };
    int value;
    struct MenuLine* menu[16];
};

class FunctionChoose {
  public:
    // enum Function { F_MOUSE, F_WHELL, F_PHOLD1, F_PHOLD2 };
    float radio, delta;
    // Function function;
};

extern MenuConfig global_menu_config;
extern FunctionChoose global_function_choose;

#define GLOBAL_ROCKER_FUNCTION global_menu_config.menu[0]->value
#define GLOBAL_ROCKER_MOUSE_SCALE global_menu_config.menu[1]->value
#define GLOBAL_ROCKER_WHEEL_SCALE global_menu_config.menu[2]->value / 10

#define GLOBAL_ENCODE_FUNCTION global_menu_config.menu[3]->value
#define GLOBAL_ENCODE_VOLUME_SCALE global_menu_config.menu[4]->value

#endif