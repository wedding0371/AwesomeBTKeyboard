// https://www.bilibili.com/video/BV184411q7Wk/?spm_id_from=333.788.recommend_more_video.7
// https://www.bilibili.com/video/BV19Q4y1i76m

#include "interface.h"

Interface::Interface(uint8_t reset_pin) { this->reset_pin = reset_pin; }

void Interface::begin() {
    reset();

    this->u8g2.setBusClock(2000000);
    this->u8g2.begin();
    // this->u8g2.setFont(u8g2_font_10x20_tf);
    this->u8g2.setFont(u8g2_font_unifont_t_symbols);
    this->u8g2.setFontRefHeightExtendedText();
    this->u8g2.setDrawColor(1);
    this->u8g2.setFontPosTop();
    this->u8g2.setFontDirection(0);

    SPIFFS.begin(true);

    this->face.begin(&this->u8g2, 24, 18);
    this->state = idle;
}

void Interface::reset() {
    pinMode(this->reset_pin, OUTPUT);
    digitalWrite(this->reset_pin, 0);
    delay(20);
    digitalWrite(this->reset_pin, 1);
}

void Interface::flash() {
    static uint32_t timer = 0;
    if (millis() - timer < 50)  // 20Hz flash frequency
        return;
    timer = millis();

    switch (this->state) {
        case idle:
            wait_connect();
            break;
        case connected:
            thinking();
            break;
        case play_video:
            if (play()) this->state = connected;
            break;
    }
}

void Interface::wait_connect() {
    this->u8g2.clearBuffer();
    this->u8g2.drawStr(0, 0, "Wait ble");
    this->u8g2.drawStr(30, 18, "connect...");
    this->u8g2.drawBox(0 + this->wait_page_count,
                       54 - this->wait_page_count % 2, 31, 6);
    this->u8g2.drawBox(7 + this->wait_page_count,
                       48 - this->wait_page_count % 2, 17, 6);
    this->u8g2.drawDisc(7 + this->wait_page_count, 59, 4);
    this->u8g2.drawDisc(22 + this->wait_page_count, 59, 4);
    switch (this->wait_page_count % 6) {
        case 0:
            this->u8g2.drawDisc(-6 + this->wait_page_count, 59, 1);
            break;
        case 1:
            this->u8g2.drawDisc(-12 + this->wait_page_count, 58, 2);
            break;
        case 2:
            this->u8g2.drawDisc(-16 + this->wait_page_count, 57, 3);
            break;
        default:
            break;
    }
    this->u8g2.sendBuffer();

    if (this->wait_page_count < 128) {
        this->wait_page_count++;
    } else {
        this->wait_page_count = -31;
    }

    delay(20);
}

void Interface::thinking() {
    static int status = 0;

    if (status <= 7) this->u8g2.clearBuffer();

    // this->u8g2.setCursor(0, 0);
    // this->u8g2.print("connected!");

    // u8g2.drawGlyph(110, 0, 0x2603);

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

FaceAnimation::FaceAnimation(void) {}

void FaceAnimation::begin(U8G2_PROTOCAL *u8g2, int x, int y) {
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