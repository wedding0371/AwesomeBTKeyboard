#include "interface.h"

#include "utility.h"

const unsigned char BT_CONNECTED[] = {
    0x00, 0x00, 0x80, 0x00, 0x80, 0x01, 0x80, 0x03, 0x90, 0x07, 0xa0,
    0x05, 0xc0, 0x03, 0x8c, 0x31, 0x8c, 0x31, 0xc0, 0x03, 0xa0, 0x05,
    0x90, 0x07, 0x80, 0x03, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00,
};
const unsigned char BT_DISABLE[] = {
    0x00, 0x00, 0x80, 0x00, 0x80, 0x01, 0x88, 0x03, 0x98, 0x07, 0x30,
    0x0d, 0x60, 0x06, 0xc0, 0x00, 0x80, 0x01, 0xc0, 0x03, 0xe0, 0x07,
    0xb0, 0x0f, 0x90, 0x1b, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00,
};
const unsigned char BT_UNCONNECT[] = {
    0x00, 0x00, 0x80, 0x00, 0x80, 0x01, 0x80, 0x03, 0x90, 0x07, 0xa0,
    0x05, 0xc0, 0x03, 0x80, 0x01, 0x80, 0x01, 0xc0, 0x03, 0xa0, 0x05,
    0x90, 0x07, 0x80, 0x03, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00,
};
const unsigned char CONTROL_MENU[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe6, 0x7f, 0xe6, 0x7f, 0x00,
    0x00, 0x00, 0x00, 0xe6, 0x7f, 0xe6, 0x7f, 0x00, 0x00, 0x00, 0x00,
    0xe6, 0x7f, 0xe6, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const unsigned char CONTROL_VOLUME[] = {
    0x00, 0x00, 0xe0, 0x07, 0xf8, 0x1f, 0xfc, 0x3f, 0xbc, 0x3f, 0x3e,
    0x7f, 0x3e, 0x7c, 0x3e, 0x78, 0x3e, 0x70, 0x3e, 0x7c, 0x3e, 0x7f,
    0xbc, 0x3f, 0xfc, 0x3f, 0xf8, 0x1f, 0xe0, 0x07, 0x00, 0x00,
};
const unsigned char WHEEL[] U8X8_PROGMEM = {
    0x3c, 0x5a, 0x42, 0x42, 0x7e, 0x42, 0x42, 0x3c,
};
const unsigned char MOUSE[] U8X8_PROGMEM = {
    0x00, 0x04, 0x0c, 0x1c, 0x3c, 0x1c, 0x10, 0x00,
};
const unsigned char SUCCESS[] U8X8_PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x07, 0x00, 0x00, 0x00, 0x80, 0x0f, 0x00, 0x00, 0x00, 0xc0, 0x0f,
    0x00, 0x00, 0x00, 0xe0, 0x0f, 0x00, 0x00, 0x00, 0xf0, 0x07, 0x00, 0x00,
    0x00, 0xf8, 0x03, 0x00, 0x00, 0x00, 0xfc, 0x01, 0x60, 0x00, 0x00, 0xfe,
    0x00, 0xf0, 0x01, 0x00, 0x7f, 0x00, 0xf0, 0x03, 0x80, 0x3f, 0x00, 0xf0,
    0x07, 0xc0, 0x1f, 0x00, 0xe0, 0x0f, 0xe0, 0x0f, 0x00, 0xc0, 0x1f, 0xf0,
    0x07, 0x00, 0x80, 0x3f, 0xf8, 0x03, 0x00, 0x00, 0x7f, 0xfc, 0x01, 0x00,
    0x00, 0xfe, 0xfe, 0x00, 0x00, 0x00, 0xfc, 0x7f, 0x00, 0x00, 0x00, 0xf8,
    0x3f, 0x00, 0x00, 0x00, 0xf0, 0x1f, 0x00, 0x00, 0x00, 0xe0, 0x0f, 0x00,
    0x00, 0x00, 0xc0, 0x07, 0x00, 0x00, 0x00, 0x80, 0x03, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static int val_target(int *a, int a_trg, uint8_t step, uint8_t slow_cnt) {
    uint8_t temp;

    temp = abs(a_trg - *a) > slow_cnt ? step : 1;
    if (*a < a_trg) {
        *a += temp;
    } else if (*a > a_trg) {
        *a -= temp;
    } else {
        return 1;
    }
    return 0;
}

Interface::Interface(uint8_t reset_pin) { this->reset_pin = reset_pin; }

void Interface::begin() {
    reset();

    this->u8g2.setBusClock(2000000);
    this->u8g2.begin();
    // this->u8g2.setFont(u8g2_font_10x20_tf);
    this->u8g2.setFont(OLED_FONT);
    this->u8g2.setFontRefHeightExtendedText();
    this->u8g2.setDrawColor(1);
    this->u8g2.setFontPosTop();
    this->u8g2.setFontDirection(0);

    SPIFFS.begin(true);

    this->face.begin(&this->u8g2, 24, 28);
    this->state = idle;

    this->status.begin(&this->u8g2, &this->state);
    this->menu.begin(&this->u8g2, &this->status, 0, 16);
    this->function.begin(&this->u8g2, &this->status, &this->state);
}

void Interface::reset() {
    pinMode(this->reset_pin, OUTPUT);
    digitalWrite(this->reset_pin, 0);
    delay(20);
    digitalWrite(this->reset_pin, 1);
}

void Interface::flash() {
    /* Flash about 18ms, and 20Hz flash frequency */
    static uint32_t timer = 0;
    if (millis() - timer < 50) return;
    timer = millis();

    switch (this->state) {
        case idle:
            // thinking();
            // break;
        case connected:
            thinking();
            break;
        case config:
            this->menu.config();
            break;
        case choose:
            this->function.choose();
            break;
        case success:
            this->function.success();
            break;
        case play_video:
            if (play()) this->state = connected;
            break;
    }
}

void Interface::thinking() {
    static int status = 0;

    if (status <= 7) this->u8g2.clearBuffer();

    this->status.update();

    switch (status) {
        case 0:
            if (this->face.open_eyes(20)) status++;
            break;
        case 1:
            if (this->face.close_eyes(1)) status++;
            break;
        case 2:
            if (this->face.open_eyes(1)) status++;
            break;
        case 3:
            if (this->face.close_eyes(1)) status++;
            break;
        case 4:
            if (this->face.open_eyes(20)) status++;
            break;
        case 5:
            if (this->face.open_to_think()) status++;
            break;
        case 6:
            if (this->face.thinking(30)) status++;
            break;
        case 7:
            if (this->face.think_to_open()) status++;
            break;
        default:
            status = 0;
            break;
    }

    if (status <= 7) this->u8g2.sendBuffer();
}

int Interface::play() {
    if (!this->player.inited) {
        this->player.begin(10);
        Serial.println("player inited");
    }

    if (this->player.played) {
        this->player.close();
        int duration = this->player.get_duration();
        int remain = this->player.get_remain();
        Serial.println(F("GIF video end"));
        Serial.printf("duration: %d, remain: %d (%0.1f %%)\n", duration, remain,
                      100.0 * remain / duration);

        return 1;
    }

    int t_fstart = millis();
    int t_delay = this->player.get_delay();
    int res = this->player.get_frame(this->graphical_buffer);
    if (res < 0) {
        Serial.println(F("ERROR: gd_get_frame() failed!"));
        return res;
    }

    this->u8g2.clearBuffer();
    for (uint8_t y = 0; y < 64; y++) {
        for (uint8_t x = 0; x < 128; x++) {
            if (this->graphical_buffer[y * 128 + x] > (128))
                this->u8g2.drawPixel(x, y);
        }
    }
    this->u8g2.sendBuffer();

    int t_real_delay = t_delay - (millis() - t_fstart);
    int delay_until = millis() + t_real_delay;
    this->player.update(t_real_delay);

    do {
        delay(1);
    } while (millis() < delay_until);

    return 0;
}

StatusBar::StatusBar(void) {}

void StatusBar::begin(U8G2 *u8g2, State *state) {
    this->u8g2 = u8g2;
    this->state = state;
}

void StatusBar::update() {
    switch (*this->state) {
        case idle:
            static int offset = 0, target = 0;

            this->u8g2->drawXBM(110, 0, 16, 16, BT_UNCONNECT);
            this->u8g2->drawRBox(offset, 6, 4, 4, 1);

            if (val_target(&offset, target, 2, 4)) target = offset + 12;
            if (offset > 100) offset = 0, target = 0;
            break;

        case config:
        case choose:
        case success:
            this->u8g2->drawXBM(10, 0, 16, 16, CONTROL_MENU);
        case connected:
            this->u8g2->drawXBM(110, 0, 16, 16, BT_CONNECTED);
            if (GLOBAL_ROCKER_FUNCTION == MenuConfig::R_MOUSE) {
                this->u8g2->drawXBM(100, 0, 8, 8, MOUSE);
            } else if (GLOBAL_ROCKER_FUNCTION == MenuConfig::R_WHEEL) {
                this->u8g2->drawXBM(100, 0, 8, 8, WHEEL);
            }
            break;
        default:
            break;
    }
}

Menu::Menu(void) {}

void Menu::begin(U8G2 *u8g2, StatusBar *status, uint8_t x_offset,
                 uint8_t y_offset) {
    this->u8g2 = u8g2;
    this->status = status;

    this->x_offset = x_offset;
    this->y_offset = y_offset;
}

void Menu::draw_string(uint8_t x, uint8_t row, char *s) {
    this->u8g2->drawStr(this->x_offset + x,
                        this->y_offset + this->y_offset * row, s);
}

void Menu::select_row(uint8_t row) {
    static int y_box = 0;

    // row = min(3, row);
    this->u8g2->setDrawColor(2);
    this->u8g2->drawRBox(0, y_box, 128, 16, 3);
    val_target(&y_box, this->y_offset + row * 16, 10, 4);
    this->u8g2->setDrawColor(1);
}

void Menu::config() {
    static char buffer[32] = {0};
    this->u8g2->clearBuffer();

    uint8_t page = global_menu_choose / 3, select = global_menu_choose % 3;
    uint8_t show_count = min(3, global_menu_config.value);
    if (global_menu_config.value - global_menu_choose < 3) {
        show_count = min(3, global_menu_config.value % 3);
    }

    for (uint8_t i = 0; i < show_count; i++) {
        sprintf(buffer, " %s: %d", global_menu_config.menu[page * 3 + i]->name,
                global_menu_config.menu[page * 3 + i]->value);
        this->draw_string(0, i, buffer);
    }
    select_row(select);

    this->status->update();
    this->u8g2->sendBuffer();
}

Function::Function(void) {}

void Function::begin(U8G2 *u8g2, StatusBar *status, State *state) {
    this->u8g2 = u8g2;
    this->status = status;
    this->state = state;
}

void Function::choose(void) {
    this->u8g2->clearBuffer();
    this->status->update();

    uint8_t height = OLED_HEIGHT / 2, width = OLED_WIDTH / 2,
            radio = OLED_HEIGHT / 10 * 4;

    for (uint8_t i = 0; i < 5; i++) {
        this->u8g2->drawCircle(width, height, radio + i);
    }
    int thick = (int)(global_function_choose.radio * 10);
    if (thick >= 3) {
        uint8_t draw_dir = 0;
        thick = min(thick, 8);
        switch (quadrant(global_function_choose.delta)) {
            case 1:
                draw_dir = U8G2_DRAW_UPPER_RIGHT;
                break;
            case 2:
                draw_dir = U8G2_DRAW_UPPER_LEFT;
                break;
            case 3:
                draw_dir = U8G2_DRAW_LOWER_LEFT;
                break;
            case 4:
                draw_dir = U8G2_DRAW_LOWER_RIGHT;
                break;
            default:
                break;
        }
        for (uint8_t i = 0; i < thick; i++) {
            this->u8g2->drawCircle(width, height, radio - i, draw_dir);
        }
    }

    this->u8g2->sendBuffer();
}
void Function::success(void) {
    static uint8_t choosed_delay = 0;
    this->u8g2->clearBuffer();
    this->status->update();

    uint8_t x = (OLED_WIDTH - 40) / 2, y = (OLED_HEIGHT - 40) / 2;
    uint8_t w = 40, h = 40;
    uint8_t speed = 6;

    this->u8g2->drawXBM(x, y, w, h, SUCCESS);

    if (choosed_delay < w / speed) {
        this->u8g2->setDrawColor(0);
        this->u8g2->drawBox(x + choosed_delay * speed, y, w, h);
        this->u8g2->setDrawColor(1);
    }
    if (++choosed_delay >= 20) (choosed_delay = 0, *this->state = connected);

    this->u8g2->sendBuffer();
}

FaceAnimation::FaceAnimation(void) {}

void FaceAnimation::begin(U8G2 *u8g2, int x, int y) {
    this->u8g2 = u8g2;

    this->x = x;
    this->y = y;
    this->frame = 0;
}

int FaceAnimation::open_to_think() {
    int max_frame = 6;

    this->u8g2->drawRBox(this->x, this->y, 30, 22, 7);
    this->u8g2->drawRBox(this->x + 50, this->y + this->frame, 30,
                         22 - this->frame, 7);

    if (++this->frame >= max_frame) this->frame = 0;
    if (this->frame == 0) return 1;
    return 0;
}

int FaceAnimation::think_to_open() {
    int max_frame = 6;

    this->u8g2->drawRBox(this->x, this->y, 30, 22, 7);
    this->u8g2->drawRBox(this->x + 50, (this->y + 6) - this->frame, 30,
                         (22 - 6) + this->frame, 7);

    if (++this->frame >= max_frame) this->frame = 0;
    if (this->frame == 0) return 1;
    return 0;
}

int FaceAnimation::open_eyes(int max_frame) {
    this->u8g2->drawRBox(this->x, this->y, 30, 22, 7);
    this->u8g2->drawRBox(this->x + 50, this->y, 30, 22, 7);

    if (++this->frame >= max_frame) this->frame = 0;
    if (this->frame == 0) {
        return 1;
    }
    return 0;
}

int FaceAnimation::close_eyes(int max_frame) {
    this->u8g2->drawRBox(this->x, this->y + 20, 30, 4, 1);
    this->u8g2->drawRBox(this->x + 50, this->y + 20, 30, 4, 1);

    if (++this->frame >= max_frame) this->frame = 0;
    if (this->frame == 0) return 1;
    return 0;
}

int FaceAnimation::thinking(int max_frame) {
    this->u8g2->drawRBox(this->x, this->y, 30, 22, 7);
    this->u8g2->drawRBox(this->x + 50, this->y + 7, 30, 22 - 7, 7);

    if (++this->frame >= max_frame) this->frame = 0;
    if (this->frame == 0) return 1;
    return 0;
}

Player::Player(void) {}

int Player::begin(int slow) {
    this->vFile = SPIFFS.open(GIF_FILENAME);
    if (!this->vFile || this->vFile.isDirectory()) {
        Serial.println(
            F("ERROR: Failed to open " GIF_FILENAME " file for reading"));
        // gfx->println(F("ERROR: Failed to open "GIF_FILENAME" file for
        // reading"));
        return -1;
    }

    this->gif = gd_open_gif(&this->vFile);
    if (!this->gif) {
        Serial.println(F("gd_open_gif() failed!"));
        return -1;
    }

    this->duration = 0;
    this->remain = 0;
    this->slow = slow;

    this->played = false;
    this->inited = true;

    return 1;
}

void Player::update(int delay) {
    this->duration += this->delay;
    this->remain += delay;
}

bool Player::get_frame(uint8_t *buffer) {
    int res = gd_get_frame(this->gif, buffer);
    if (res == 0) {
        this->played = true;
    }
    return res;
}

void Player::close() {
    gd_close_gif(this->gif);
    this->inited = false;
}

int Player::get_delay() {
    this->delay = this->gif->gce.delay * this->slow;
    return this->delay;
}

int Player::get_duration() { return this->duration; }

int Player::get_remain() { return this->remain; }