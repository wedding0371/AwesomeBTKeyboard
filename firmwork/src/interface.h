#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <SPIFFS.h>
#include <U8g2lib.h>
#include <U8x8lib.h>

#include "gifdec.h"
#include "resource.h"

#define GIF_FILENAME "/loop.gif"

#define U8G2 U8G2_SSD1306_128X64_NONAME_F_HW_I2C
#define OLED_FONT u8g2_font_unifont_t_symbols

class InterfaceCommon {
  public:
    enum State {
        none,
        idle,
        connected,
        config,
        choose,
        success,
        play_video,
    };
};

class StatusBar : public InterfaceCommon {
  private:
    U8G2 *u8g2;
    State *state;
    State last_state;

  public:
    StatusBar();

    void begin(U8G2 *u8g2, State *state);
    void update();
};

class FaceAnimation {
  private:
    int frame;
    int x, y;
    U8G2 *u8g2;

  public:
    FaceAnimation();

    void begin(U8G2 *u8g2, int x, int y);
    int open_eyes(int max_frame);
    int close_eyes(int max_frame);
    int thinking(int max_frame);
    int open_to_think();
    int think_to_open();
};

class Menu {
  private:
    U8G2 *u8g2;
    class StatusBar *status;
    uint8_t x_offset, y_offset;

    void draw_string(uint8_t x, uint8_t y, char *s);
    void select_row(uint8_t);

  public:
    Menu();

    void begin(U8G2 *u8g2, StatusBar *status, uint8_t x_offset,
               uint8_t y_offset);
    void config();
};

class Function : public InterfaceCommon {
  private:
    U8G2 *u8g2;
    class StatusBar *status;
    State *state;

  public:
    Function();
    void begin(U8G2 *u8g2, StatusBar *status, State *state);
    void choose(void);
    void success(void);
};

class Player {
  private:
    File vFile;
    gd_GIF *gif;
    int duration, remain, delay, slow;

  public:
    bool inited = false;
    bool played = false;

    Player();

    int begin(int slow);
    void close();
    void update(int delay);
    bool get_frame(uint8_t *buffer);
    int get_delay();
    int get_duration();
    int get_remain();
};

class Interface : public InterfaceCommon {
  private:
    StatusBar status;
    Menu menu;
    Function function;
    FaceAnimation face;

    int wait_page_count = -31;
    uint8_t graphical_buffer[128 * 64];

    uint8_t clock_pin;
    uint8_t data_pin;
    uint8_t reset_pin;

    Player player = Player();

  public:
    State state;
    U8G2 u8g2 = U8G2(U8G2_R0, /*U8G2_R1*/
                     /* reset=*/U8X8_PIN_NONE, /* clock*/ OLED_CLOCK_PIN,
                     /* data*/ OLED_DATA_PIN);

    Interface(uint8_t reset_pin = OLED_RESET_PIN);

    void begin();
    void reset();
    void flash();
    void wait_connect();
    void thinking();
    int play();
};

#endif