/*
 * rewrite from: https://github.com/BasementCat/arduino-tft-gif
 */
#include "gifdec.h"

#include <FS.h>
#include <sys/types.h>

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

int gif_buf_last_idx, gif_buf_idx, file_pos;
uint8_t gif_buf[GIF_BUF_SIZE];

static bool gif_buf_seek(File *fd, int len) {
    if (len > (gif_buf_last_idx - gif_buf_idx)) {
        fd->seek(len - (gif_buf_last_idx - gif_buf_idx), SeekCur);
        gif_buf_idx = gif_buf_last_idx;
    } else {
        gif_buf_idx += len;
    }
    return true;
}

static int gif_buf_read(File *fd, uint8_t *dest, int len) {
    while (len--) {
        if (gif_buf_idx == gif_buf_last_idx) {
            gif_buf_last_idx = fd->read(gif_buf, GIF_BUF_SIZE);
            gif_buf_idx = 0;
        }

        file_pos++;
        *(dest++) = gif_buf[gif_buf_idx++];
    }
    return len;
}

static uint8_t gif_buf_read(File *fd) {
    if (gif_buf_idx == gif_buf_last_idx) {
        gif_buf_last_idx = fd->read(gif_buf, GIF_BUF_SIZE);
        gif_buf_idx = 0;
    }

    file_pos++;
    return gif_buf[gif_buf_idx++];
}

static uint16_t gif_buf_read16(File *fd) {
    return gif_buf_read(fd) + (((uint16_t)gif_buf_read(fd)) << 8);
}

static void read_palette(File *fd, gd_Palette *dest, int32_t num_colors) {
    uint8_t r, g, b;
    dest->size = num_colors;
    for (int32_t i = 0; i < num_colors; i++) {
        r = gif_buf_read(fd);
        g = gif_buf_read(fd);
        b = gif_buf_read(fd);
        dest->colors[i] =
            ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
    }
}

gd_GIF *gd_open_gif(File *fd) {
    uint8_t sigver[3];
    uint16_t width, height, depth;
    uint8_t fdsz, bgidx, aspect;
    int32_t gct_sz;
    gd_GIF *gif;

    // init global variables
    gif_buf_last_idx = GIF_BUF_SIZE;
    gif_buf_idx = gif_buf_last_idx;  // no buffer yet
    file_pos = 0;

    /* Header */
    gif_buf_read(fd, sigver, 3);
    if (memcmp(sigver, "GIF", 3) != 0) {
        Serial.println(F("invalid signature"));
        return NULL;
    }
    /* Version */
    gif_buf_read(fd, sigver, 3);
    if (memcmp(sigver, "89a", 3) != 0) {
        Serial.println(F("invalid version"));
        return NULL;
    }
    /* Width x Height */
    width = gif_buf_read16(fd);
    height = gif_buf_read16(fd);
    /* FDSZ */
    gif_buf_read(fd, &fdsz, 1);
    /* Presence of GCT */
    if (!(fdsz & 0x80)) {
        Serial.println(F("no global color table"));
        return NULL;
    }
    /* Color Space's Depth */
    depth = ((fdsz >> 4) & 7) + 1;
    /* Ignore Sort Flag. */
    /* GCT Size */
    gct_sz = 1 << ((fdsz & 0x07) + 1);
    /* Background Color Index */
    gif_buf_read(fd, &bgidx, 1);
    /* Aspect Ratio */
    gif_buf_read(fd, &aspect, 1);
    /* Create gd_GIF Structure. */
    gif = (gd_GIF *)calloc(1, sizeof(*gif));
    gif->fd = fd;
    gif->width = width;
    gif->height = height;
    gif->depth = depth;
    /* Read GCT */
    read_palette(fd, &gif->gct, gct_sz);
    gif->palette = &gif->gct;
    gif->bgindex = bgidx;
    gif->anim_start = file_pos;  // fd->position();
    gif->table = new_table();
    return gif;
}

static void discard_sub_blocks(gd_GIF *gif) {
    uint8_t size;

    do {
        gif_buf_read(gif->fd, &size, 1);
        gif_buf_seek(gif->fd, size);
    } while (size);
}

static void read_plain_text_ext(gd_GIF *gif) {
    if (gif->plain_text) {
        uint16_t tx, ty, tw, th;
        uint8_t cw, ch, fg, bg;
        off_t sub_block;
        gif_buf_seek(gif->fd, 1); /* block size = 12 */
        tx = gif_buf_read16(gif->fd);
        ty = gif_buf_read16(gif->fd);
        tw = gif_buf_read16(gif->fd);
        th = gif_buf_read16(gif->fd);
        cw = gif_buf_read(gif->fd);
        ch = gif_buf_read(gif->fd);
        fg = gif_buf_read(gif->fd);
        bg = gif_buf_read(gif->fd);
        gif->plain_text(gif, tx, ty, tw, th, cw, ch, fg, bg);
    } else {
        /* Discard plain text metadata. */
        gif_buf_seek(gif->fd, 13);
    }
    /* Discard plain text sub-blocks. */
    discard_sub_blocks(gif);
}

static void read_graphic_control_ext(gd_GIF *gif) {
    uint8_t rdit;

    /* Discard block size (always 0x04). */
    gif_buf_seek(gif->fd, 1);
    gif_buf_read(gif->fd, &rdit, 1);
    gif->gce.disposal = (rdit >> 2) & 3;
    gif->gce.input = rdit & 2;
    gif->gce.transparency = rdit & 1;
    gif->gce.delay = gif_buf_read16(gif->fd);
    gif_buf_read(gif->fd, &gif->gce.tindex, 1);
    /* Skip block terminator. */
    gif_buf_seek(gif->fd, 1);
}

static void read_comment_ext(gd_GIF *gif) {
    if (gif->comment) {
        gif->comment(gif);
    }
    /* Discard comment sub-blocks. */
    discard_sub_blocks(gif);
}

static void read_application_ext(gd_GIF *gif) {
    char app_id[8];
    char app_auth_code[3];

    /* Discard block size (always 0x0B). */
    gif_buf_seek(gif->fd, 1);
    /* Application Identifier. */
    gif_buf_read(gif->fd, (uint8_t *)app_id, 8);
    /* Application Authentication Code. */
    gif_buf_read(gif->fd, (uint8_t *)app_auth_code, 3);
    if (!strncmp(app_id, "NETSCAPE", sizeof(app_id))) {
        /* Discard block size (0x03) and constant byte (0x01). */
        gif_buf_seek(gif->fd, 2);
        gif->loop_count = gif_buf_read16(gif->fd);
        /* Skip block terminator. */
        gif_buf_seek(gif->fd, 1);
    } else if (gif->application) {
        gif->application(gif, app_id, app_auth_code);
        discard_sub_blocks(gif);
    } else {
        discard_sub_blocks(gif);
    }
}

static void read_ext(gd_GIF *gif) {
    uint8_t label;

    gif_buf_read(gif->fd, &label, 1);
    switch (label) {
        case 0x01:
            read_plain_text_ext(gif);
            break;
        case 0xF9:
            read_graphic_control_ext(gif);
            break;
        case 0xFE:
            read_comment_ext(gif);
            break;
        case 0xFF:
            read_application_ext(gif);
            break;
        default:
            Serial.print("unknown extension: ");
            Serial.println(label, HEX);
    }
}

static gd_Table *new_table() {
    // int key;
    // int init_bulk = MAX(1 << (key_size + 1), 0x100);
    // Table *table = (Table*) malloc(sizeof(*table) + sizeof(Entry) *
    // init_bulk); if (table) {
    //     table->bulk = init_bulk;
    //     table->nentries = (1 << key_size) + 2;
    //     table->entries = (Entry *) &table[1];
    //     for (key = 0; key < (1 << key_size); key++)
    //         table->entries[key] = (Entry) {1, 0xFFF, key};
    // }
    // return table;
    int s = sizeof(gd_Table) + (sizeof(gd_Entry) * 4096);
    gd_Table *table = (gd_Table *)malloc(s);
    if (table) {
        Serial.printf("new_table() malloc %d.\n", s);
    } else {
        Serial.printf("new_table() malloc %d failed!\n", s);
    }
    table->entries = (gd_Entry *)&table[1];
    return table;
}

static void reset_table(gd_Table *table, int key_size) {
    table->nentries = (1 << key_size) + 2;
    for (int32_t key = 0; key < (1 << key_size); key++) {
        table->entries[key] = (gd_Entry){1, 0xFFF, (uint8_t)key};
    }
}

/* Add table entry. Return value:
 *  0 on success
 *  +1 if key size must be incremented after this addition
 *  -1 if could not realloc table */
static int32_t add_entry(gd_Table *table, int32_t length, uint16_t prefix,
                         uint8_t suffix) {
    // Table *table = *tablep;
    // if (table->nentries == table->bulk) {
    //     table->bulk *= 2;
    //     table = (Table*) realloc(table, sizeof(*table) + sizeof(Entry) *
    //     table->bulk); if (!table) return -1; table->entries = (Entry *)
    //     &table[1]; *tablep = table;
    // }
    table->entries[table->nentries] = (gd_Entry){length, prefix, suffix};
    table->nentries++;
    if ((table->nentries & (table->nentries - 1)) == 0) return 1;
    return 0;
}

static uint16_t get_key(gd_GIF *gif, int key_size, uint8_t *sub_len,
                        uint8_t *shift, uint8_t *byte) {
    int bits_read;
    int rpad;
    int frag_size;
    uint16_t key;

    key = 0;
    for (bits_read = 0; bits_read < key_size; bits_read += frag_size) {
        rpad = (*shift + bits_read) % 8;
        if (rpad == 0) {
            /* Update byte. */
            if (*sub_len == 0)
                gif_buf_read(gif->fd, sub_len, 1); /* Must be nonzero! */
            gif_buf_read(gif->fd, byte, 1);
            (*sub_len)--;
        }
        frag_size = MIN(key_size - bits_read, 8 - rpad);
        key |= ((uint16_t)((*byte) >> rpad)) << bits_read;
    }
    /* Clear extra bits to the left. */
    key &= (1 << key_size) - 1;
    *shift = (*shift + key_size) % 8;
    return key;
}

/* Compute output index of y-th input line, in frame of height h. */
static int interlaced_line_index(int h, int y) {
    int p; /* number of lines in current pass */

    p = (h - 1) / 8 + 1;
    if (y < p) /* pass 1 */
        return y * 8;
    y -= p;
    p = (h - 5) / 8 + 1;
    if (y < p) /* pass 2 */
        return y * 8 + 4;
    y -= p;
    p = (h - 3) / 4 + 1;
    if (y < p) /* pass 3 */
        return y * 4 + 2;
    y -= p;
    /* pass 4 */
    return y * 2 + 1;
}

/* Decompress image pixels.
 * Return 0 on success or -1 on out-of-memory (w.r.t. LZW code table). */
static int32_t read_image_data(gd_GIF *gif, int interlace, uint8_t *frame) {
    uint8_t sub_len, shift, byte;
    int init_key_size, key_size, table_is_full;
    int frm_off, str_len, p, x, y;
    uint16_t key, clear, stop;
    int32_t ret;
    gd_Entry entry;
    off_t start, end;

    // Serial.println("Read key size");
    gif_buf_read(gif->fd, &byte, 1);
    key_size = (int)byte;
    // Serial.println("Set pos, discard sub blocks");
    // start = gif->fd->position();
    // discard_sub_blocks(gif);
    // end = gif->fd->position();
    // gif_buf_seek(gif->fd, start, SeekSet);
    clear = 1 << key_size;
    stop = clear + 1;
    // Serial.println("New LZW table");
    // table = new_table(key_size);
    reset_table(gif->table, key_size);
    key_size++;
    init_key_size = key_size;
    sub_len = shift = 0;
    // Serial.println("Get init key");
    key = get_key(gif, key_size, &sub_len, &shift, &byte); /* clear code */
    frm_off = 0;
    ret = 0;
    while (1) {
        if (key == clear) {
            // Serial.println("Clear key, reset nentries");
            key_size = init_key_size;
            gif->table->nentries = (1 << (key_size - 1)) + 2;
            table_is_full = 0;
        } else if (!table_is_full) {
            // Serial.println("Add entry to table");
            ret = add_entry(gif->table, str_len + 1, key, entry.suffix);
            // if (ret == -1) {
            //     // Serial.println("Table entry add failure");
            //     free(table);
            //     return -1;
            // }
            if (gif->table->nentries == 0x1000) {
                // Serial.println("Table is full");
                ret = 0;
                table_is_full = 1;
            }
        }
        // Serial.println("Get key");
        key = get_key(gif, key_size, &sub_len, &shift, &byte);
        if (key == clear) continue;
        if (key == stop) break;
        if (ret == 1) key_size++;
        entry = gif->table->entries[key];
        str_len = entry.length;
        uint8_t tindex = gif->gce.tindex;
        // Serial.println("Interpret key");
        while (1) {
            p = frm_off + entry.length - 1;
            x = p % gif->fw;
            y = p / gif->fw;
            if (interlace) {
                y = interlaced_line_index((int)gif->fh, y);
            }
            if (tindex != entry.suffix) {
                frame[(gif->fy + y) * gif->width + gif->fx + x] = entry.suffix;
            }
            if (entry.prefix == 0xFFF)
                break;
            else
                entry = gif->table->entries[entry.prefix];
        }
        frm_off += str_len;
        if (key < gif->table->nentries - 1 && !table_is_full)
            gif->table->entries[gif->table->nentries - 1].suffix = entry.suffix;
    }
    // Serial.println("Done w/ img data, free table and seek to end");
    // free(table);
    gif_buf_read(gif->fd, &sub_len, 1); /* Must be zero! */
    // gif_buf_seek(gif->fd, end, SeekSet);
    return 0;
}

/* Read image.
 * Return 0 on success or -1 on out-of-memory (w.r.t. LZW code table). */
static int32_t read_image(gd_GIF *gif, uint8_t *frame) {
    uint8_t fisrz;
    int interlace;

    /* Image Descriptor. */
    // Serial.println("Read image descriptor");
    gif->fx = gif_buf_read16(gif->fd);
    gif->fy = gif_buf_read16(gif->fd);
    gif->fw = gif_buf_read16(gif->fd);
    gif->fh = gif_buf_read16(gif->fd);
    // Serial.println("Read fisrz?");
    gif_buf_read(gif->fd, &fisrz, 1);
    interlace = fisrz & 0x40;
    /* Ignore Sort Flag. */
    /* Local Color Table? */
    if (fisrz & 0x80) {
        /* Read LCT */
        // Serial.println("Read LCT");
        read_palette(gif->fd, &gif->lct, 1 << ((fisrz & 0x07) + 1));
        gif->palette = &gif->lct;
    } else {
        gif->palette = &gif->gct;
    }
    /* Image Data. */
    // Serial.println("Read image data");
    return read_image_data(gif, interlace, frame);
}

static void render_frame_rect(gd_GIF *gif, uint16_t *buffer, uint8_t *frame) {
    int i, j, k;
    uint8_t index, *color;
    i = gif->fy * gif->width + gif->fx;
    for (j = 0; j < gif->fh; j++) {
        for (k = 0; k < gif->fw; k++) {
            index = frame[(gif->fy + j) * gif->width + gif->fx + k];
            // color = &gif->palette->colors[index*2];
            if (!gif->gce.transparency || index != gif->gce.tindex)
                buffer[(i + k)] = gif->palette->colors[index];
            // memcpy(&buffer[(i+k)*2], color, 2);
        }
        i += gif->width;
    }
}

/* Return 1 if got a frame; 0 if got GIF trailer; -1 if error. */
int32_t gd_get_frame(gd_GIF *gif, uint8_t *frame) {
    char sep;

    while (1) {
        gif_buf_read(gif->fd, (uint8_t *)&sep, 1);
        if (sep == 0) {
            gif_buf_read(gif->fd, (uint8_t *)&sep, 1);
        }
        if (sep == ',') {
            break;
        }
        if (sep == ';') {
            return 0;
        }
        if (sep == '!') {
            read_ext(gif);
        } else {
            Serial.printf("Read sep: [%d].\n", sep);
            return -1;
        }
    }
    // Serial.println("Do read image");
    if (read_image(gif, frame) == -1) return -1;
    return 1;
}

void gd_rewind(gd_GIF *gif) {
    gif->fd->seek(gif->anim_start, SeekSet);
    file_pos = gif->anim_start;
    gif_buf_idx = gif_buf_last_idx;  // reset buffer
}

void gd_close_gif(gd_GIF *gif) {
    gif->fd->close();
    free(gif->table);
    free(gif);
}
