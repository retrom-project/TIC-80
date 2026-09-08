// Retrom Web bridge. SPDX-License-Identifier: MIT
#include "tic80.h"
#include "tic.h"
#include "script.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <emscripten.h>

static tic80* machine;
static uint64_t ticks;
static int failed;
static char error_text[512];
static uint32_t pixels[TIC80_WIDTH * TIC80_HEIGHT];
static void on_error(const char* message) {
    failed = 1;
    snprintf(error_text, sizeof(error_text), "%s", message);
}
static void on_exit(void) { on_error("TIC80_GAME_EXITED"); }
static uint64_t counter(void* data) { (void)data; return ticks; }
static uint64_t frequency(void* data) { (void)data; return 60; }

EMSCRIPTEN_KEEPALIVE int retrom_abi(void) { return 1; }
EMSCRIPTEN_KEEPALIVE void retrom_stop(void) {
    if (machine) tic80_delete(machine);
    machine = NULL;
}
// The upstream loader trusts chunk lengths; reject truncation before it reads RAM.
static int valid_cart(const uint8_t* bytes, size_t size) {
    size_t cursor = 0;
    while (cursor < size) {
        if (size - cursor < 4) return 0;
        unsigned type = bytes[cursor] & 31;
        size_t length = bytes[cursor + 1] | ((size_t)bytes[cursor + 2] << 8);
        if (!length && (type == 5 || type == 19)) length = 65536;
        cursor += 4;
        if (length > size - cursor) return 0;
        // Empty data chunks trigger the upstream loader's legacy 64K fallback.
        if (!length && type != 0 && type != 7 && type != 8 && type != 11 && type != 17) return 0;
        cursor += length;
    }
    return cursor == size;
}

EMSCRIPTEN_KEEPALIVE int retrom_load(const void* data, int size) {
    retrom_stop();
    if (!data || size < 4 || size > 4 * 1024 * 1024 || !valid_cart(data, (size_t)size)) return 0;
    failed = 0;
    error_text[0] = 0;
    ticks = 0;
    machine = tic80_create(44100, TIC80_PIXEL_COLOR_RGBA8888);
    if (!machine) return 0;
    machine->callback.error = on_error;
    machine->callback.exit = on_exit;
    tic80_load(machine, (void*)data, size);
    if (!tic_get_script((tic_mem*)machine) || (!((tic_mem*)machine)->cart.code.data[0] && !((tic_mem*)machine)->cart.binary.size)) {
        on_error("TIC80_CART_UNSUPPORTED");
    }
    return !failed;
}
EMSCRIPTEN_KEEPALIVE int retrom_step(uint32_t buttons) {
    if (!machine || failed) return 0;
    tic80_input input = {0};
    input.gamepads.data = buttons;
    tic80_tick(machine, input, counter, frequency);
    tic80_sound(machine);
    for (int y = 0; y < TIC80_HEIGHT; y++) {
        memcpy(pixels + y * TIC80_WIDTH,
            machine->screen + (y + TIC80_MARGIN_TOP) * TIC80_FULLWIDTH + TIC80_MARGIN_LEFT,
            TIC80_WIDTH * sizeof(uint32_t));
    }
    ticks++;
    return !failed;
}
EMSCRIPTEN_KEEPALIVE const void* retrom_pixels(void) { return pixels; }
EMSCRIPTEN_KEEPALIVE const void* retrom_audio(void) { return machine ? machine->samples.buffer : NULL; }
EMSCRIPTEN_KEEPALIVE int retrom_audio_count(void) { return machine ? machine->samples.count : 0; }
EMSCRIPTEN_KEEPALIVE const char* retrom_error(void) { return error_text; }
EMSCRIPTEN_KEEPALIVE int retrom_state_size(void) { return machine ? 1024 : 0; }
EMSCRIPTEN_KEEPALIVE const void* retrom_state(void) {
    return machine ? ((tic_mem*)machine)->ram->persistent.data : NULL;
}
EMSCRIPTEN_KEEPALIVE int retrom_restore(const void* data, int size) {
    if (!machine || !data || size != 1024 || ticks != 0) return 0;
    memcpy(((tic_mem*)machine)->ram->persistent.data, data, 1024);
    return 1;
}

EMSCRIPTEN_KEEPALIVE int retrom_ready(void) { return machine && !failed; }
