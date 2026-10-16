#include "hud.h"
#include "pictures.h"
#include "play.h"
#include "sound.h"
#include "raylib.h"
#include "surface.h"
#include <stdio.h>
#include <stdlib.h>
#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#endif
#if defined(__SWITCH__)
#include <unistd.h>
#endif

enum { SCREEN_W = 320, SCREEN_H = 200, SCALE = 3 };
enum { MOUSE_X_DEAD = 5, MOUSE_Y_DEAD = 10 };
enum {
    SCAN_Q = 0x10, SCAN_Y = 0x15, SCAN_LSHIFT = 0x2a, SCAN_N = 0x31, SCAN_M = 0x32,
    SCAN_RSHIFT = 0x36, SCAN_SPACE = 0x39, SCAN_F1 = 0x3b, SCAN_F2 = 0x3c,
    SCAN_F3 = 0x3d, SCAN_F4 = 0x3e, SCAN_UP = 0x48, SCAN_LEFT = 0x4b,
    SCAN_RIGHT = 0x4d, SCAN_DOWN = 0x50
};
enum { UI_PLAY, UI_PAUSE, UI_QUIT, UI_REBIND, UI_RELEASE };
enum { BIND_LEFT, BIND_RIGHT, BIND_UP, BIND_DOWN, BIND_FIRE, BIND_N, BIND_M, BIND_QUIT, BIND_COUNT };

/* DOS scan-code to the host key. F1–F4 and both shifts stay out of rebinding. */
static const int SCAN_KEY[128] = {
    [0x01] = KEY_ESCAPE,
    [0x02] = KEY_ONE, [0x03] = KEY_TWO, [0x04] = KEY_THREE, [0x05] = KEY_FOUR,
    [0x06] = KEY_FIVE, [0x07] = KEY_SIX, [0x08] = KEY_SEVEN, [0x09] = KEY_EIGHT,
    [0x0a] = KEY_NINE, [0x0b] = KEY_ZERO, [0x0c] = KEY_MINUS, [0x0d] = KEY_EQUAL,
    [0x0e] = KEY_BACKSPACE, [0x0f] = KEY_TAB,
    [0x10] = KEY_Q, [0x11] = KEY_W, [0x12] = KEY_E, [0x13] = KEY_R, [0x14] = KEY_T,
    [0x15] = KEY_Y, [0x16] = KEY_U, [0x17] = KEY_I, [0x18] = KEY_O, [0x19] = KEY_P,
    [0x1a] = KEY_LEFT_BRACKET, [0x1b] = KEY_RIGHT_BRACKET, [0x1c] = KEY_ENTER,
    [0x1d] = KEY_LEFT_CONTROL,
    [0x1e] = KEY_A, [0x1f] = KEY_S, [0x20] = KEY_D, [0x21] = KEY_F, [0x22] = KEY_G,
    [0x23] = KEY_H, [0x24] = KEY_J, [0x25] = KEY_K, [0x26] = KEY_L,
    [0x27] = KEY_SEMICOLON, [0x28] = KEY_APOSTROPHE, [0x29] = KEY_GRAVE,
    [0x2a] = KEY_LEFT_SHIFT, [0x2b] = KEY_BACKSLASH,
    [0x2c] = KEY_Z, [0x2d] = KEY_X, [0x2e] = KEY_C, [0x2f] = KEY_V, [0x30] = KEY_B,
    [0x31] = KEY_N, [0x32] = KEY_M, [0x33] = KEY_COMMA, [0x34] = KEY_PERIOD, [0x35] = KEY_SLASH,
    [0x36] = KEY_RIGHT_SHIFT, [0x37] = KEY_KP_MULTIPLY, [0x38] = KEY_LEFT_ALT, [0x39] = KEY_SPACE,
    [0x3b] = KEY_F1, [0x3c] = KEY_F2, [0x3d] = KEY_F3, [0x3e] = KEY_F4,
    [0x3f] = KEY_F5, [0x40] = KEY_F6, [0x41] = KEY_F7, [0x42] = KEY_F8, [0x43] = KEY_F9, [0x44] = KEY_F10,
    [0x47] = KEY_KP_7, [0x48] = KEY_UP, [0x49] = KEY_KP_9,
    [0x4a] = KEY_KP_SUBTRACT, [0x4b] = KEY_LEFT, [0x4c] = KEY_KP_5,
    [0x4d] = KEY_RIGHT, [0x4e] = KEY_KP_ADD, [0x4f] = KEY_KP_1,
    [0x50] = KEY_DOWN, [0x51] = KEY_KP_3, [0x52] = KEY_KP_0, [0x53] = KEY_KP_DECIMAL
};

static const int REBIND_SLOT[8] = {
    BIND_FIRE, BIND_LEFT, BIND_RIGHT, BIND_UP, BIND_DOWN, BIND_N, BIND_M, BIND_QUIT
};
static const int REBIND_PROMPT[8] = {4, 0, 1, 2, 3, 6, 7, 5};

static int binding[BIND_COUNT] = {
    SCAN_LEFT, SCAN_RIGHT, SCAN_UP, SCAN_DOWN, SCAN_SPACE, SCAN_N, SCAN_M, SCAN_Q
};
static int ui_mode;
static int rebind_step;
static int sound_gate;
static int leave_game;

static int key_down(int scan) {
    int key = scan > 0 && scan < 128 ? SCAN_KEY[scan] : 0;
    return key && IsKeyDown(key);
}

static int first_scancode(int skip_fixed) {
    for (int scan = 1; scan < 128; scan++) {
        if (!SCAN_KEY[scan] || !IsKeyDown(SCAN_KEY[scan])) continue;
        if (skip_fixed && (scan == SCAN_LSHIFT || scan == SCAN_RSHIFT ||
                           (scan >= SCAN_F1 && scan <= SCAN_F4))) continue;
        return scan;
    }
    return 0;
}

static void service_keys(void) {
    if (ui_mode == UI_PAUSE) {
        if (key_down(SCAN_F4)) ui_mode = UI_PLAY;
        return;
    }
    if (ui_mode == UI_QUIT) {
        if (key_down(SCAN_Y)) leave_game = 1;
        else if (key_down(SCAN_N)) ui_mode = UI_RELEASE;
        return;
    }
    if (ui_mode == UI_RELEASE) {
        if (!first_scancode(0)) ui_mode = UI_PLAY;
        return;
    }
    if (ui_mode == UI_REBIND) {
        int scan = first_scancode(1);
        if (!scan) return;
        for (int i = 0; i < rebind_step; i++) {
            if (binding[REBIND_SLOT[i]] == scan) return;
        }
        binding[REBIND_SLOT[rebind_step]] = scan;
        rebind_step++;
        if (rebind_step >= 8) ui_mode = UI_RELEASE;
        return;
    }
    if (key_down(SCAN_F3)) {
        ui_mode = UI_PAUSE;
        return;
    }
    if (key_down(SCAN_F1)) {
        ui_mode = UI_REBIND;
        rebind_step = 0;
        return;
    }
    if (key_down(SCAN_F2)) {
        if (sound_gate == 0) {
            sound_trigger(SOUND_MUTE);
            sound_gate = 1;
            sound_set_enabled(0);
        } else if (sound_gate == 2) {
            sound_gate = 3;
            sound_set_enabled(1);
            sound_trigger(SOUND_UNMUTE);
        }
    } else if (sound_gate == 1) {
        sound_gate = 2;
    } else if (sound_gate == 3) {
        sound_gate = 0;
    }
    if (key_down(binding[BIND_QUIT])) ui_mode = UI_QUIT;
#if defined(__SWITCH__)
    if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_LEFT)) ui_mode = UI_QUIT;
#endif
}

static float mouse_x;
static float mouse_y;

static unsigned touch_controls(void) {
#if defined(__EMSCRIPTEN__)
    return (unsigned)EM_ASM_INT({ return Module.touchMask | 0; });
#else
    return 0;
#endif
}

#if defined(__EMSCRIPTEN__)
/* The browser calls this only after a real size change. The frame loop does not. */
EMSCRIPTEN_KEEPALIVE
void goose_fit_canvas(int buf_w, int buf_h, int css_w, int css_h) {
    static int last_buf_w, last_buf_h, last_css_w, last_css_h;
    if (buf_w == last_buf_w && buf_h == last_buf_h && css_w == last_css_w && css_h == last_css_h)
        return;
    last_buf_w = buf_w;
    last_buf_h = buf_h;
    last_css_w = css_w;
    last_css_h = css_h;
    if (GetScreenWidth() != buf_w || GetScreenHeight() != buf_h)
        SetWindowSize(buf_w, buf_h);
    EM_ASM({
        var canvas = Module.canvas || document.getElementById("canvas");
        if (!canvas) return;
        canvas.style.width = $0 + "px";
        canvas.style.height = $1 + "px";
    }, css_w, css_h);
}
#endif

/* Raylib reads the viewport once. A later turn of the phone may not update it,
 * and the CSS box can stay at the 960x600 startup size. Watch the viewport and
 * integer-scale into its device pixels only when those numbers change. */
static void watch_web_canvas(void) {
#if defined(__EMSCRIPTEN__)
    EM_ASM({
        function measure() {
            var view = window.visualViewport;
            var cssW = Math.round(view ? view.width : window.innerWidth);
            var cssH = Math.round(view ? view.height : window.innerHeight);
            var dpr = window.devicePixelRatio || 1;
            if (dpr < 1) dpr = 1;
            var bufW = Math.max(1, Math.round(cssW * dpr));
            var bufH = Math.max(1, Math.round(cssH * dpr));
            if (bufW === Module.viewBufW && bufH === Module.viewBufH &&
                cssW === Module.viewCssW && cssH === Module.viewCssH) return;
            Module.viewBufW = bufW;
            Module.viewBufH = bufH;
            Module.viewCssW = cssW;
            Module.viewCssH = cssH;
            Module._goose_fit_canvas(bufW, bufH, cssW, cssH);
        }
        function later() {
            requestAnimationFrame(function () { requestAnimationFrame(measure); });
        }
        window.addEventListener("resize", later);
        window.addEventListener("orientationchange", later);
        document.addEventListener("fullscreenchange", later);
        if (screen.orientation) screen.orientation.addEventListener("change", later);
        if (window.visualViewport) window.visualViewport.addEventListener("resize", later);
        measure();
    });
#endif
}

static void sample_mouse(void) {
    Vector2 delta = GetMouseDelta();
    /* A finger on an on-screen button also moves the pointer. Do not steer from that. */
    if (touch_controls()) {
        mouse_x = 0.0f;
        mouse_y = 0.0f;
        return;
    }
    mouse_x += delta.x;
    mouse_y += delta.y;
}

/* Joy-Con faces are not the emulated keyboard: A is not Space. */
static unsigned switch_pad(void) {
#if defined(__SWITCH__)
    unsigned pad = 0;
    float x;
    float y;
    int left;
    int right;
    int up;
    int down;
    if (!IsGamepadAvailable(0)) return 0;
    x = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
    y = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
    left = IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT) || x < -0.4f;
    right = IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT) || x > 0.4f;
    up = IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP) || y > 0.4f;
    down = IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN) || y < -0.4f;
    if (left) pad |= CONTROL_LEFT;
    else if (right) pad |= CONTROL_RIGHT;
    if (up) pad |= CONTROL_UP;
    else if (down) pad |= CONTROL_DOWN;
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) ||
        IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_2))
        pad |= CONTROL_FIRE;
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1) ||
        IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_TRIGGER_2) ||
        IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT))
        pad |= CONTROL_LEFT_DEVICE;
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1) ||
        IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_UP))
        pad |= CONTROL_RIGHT_DEVICE;
    return pad;
#else
    return 0;
#endif
}

static unsigned read_controls(void) {
    unsigned controls = 0;
    unsigned touch = touch_controls();
    unsigned pad = switch_pad();
    unsigned direction = CONTROL_LEFT | CONTROL_RIGHT | CONTROL_UP | CONTROL_DOWN;
    if (key_down(binding[BIND_LEFT]) || (touch & CONTROL_LEFT) || (pad & CONTROL_LEFT)) controls = CONTROL_LEFT;
    else if (key_down(binding[BIND_RIGHT]) || (touch & CONTROL_RIGHT) || (pad & CONTROL_RIGHT)) controls = CONTROL_RIGHT;
    if (key_down(binding[BIND_UP]) || (touch & CONTROL_UP) || (pad & CONTROL_UP)) controls |= CONTROL_UP;
    else if (key_down(binding[BIND_DOWN]) || (touch & CONTROL_DOWN) || (pad & CONTROL_DOWN)) controls |= CONTROL_DOWN;
    if (key_down(binding[BIND_FIRE]) || (touch & CONTROL_FIRE) || (pad & CONTROL_FIRE)) controls |= CONTROL_FIRE;
    if (key_down(binding[BIND_N]) || (touch & CONTROL_LEFT_DEVICE) || (pad & CONTROL_LEFT_DEVICE)) controls |= CONTROL_LEFT_DEVICE;
    if (key_down(binding[BIND_M]) || (touch & CONTROL_RIGHT_DEVICE) || (pad & CONTROL_RIGHT_DEVICE)) controls |= CONTROL_RIGHT_DEVICE;
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) controls |= CONTROL_FIRE;
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) controls |= CONTROL_RIGHT_DEVICE;
    if (controls & direction) return controls;
    if (mouse_x < -(float)MOUSE_X_DEAD) controls |= CONTROL_LEFT;
    else if (mouse_x > (float)MOUSE_X_DEAD) controls |= CONTROL_RIGHT;
    if (mouse_y < -(float)MOUSE_Y_DEAD) controls |= CONTROL_UP;
    else if (mouse_y > (float)MOUSE_Y_DEAD) controls |= CONTROL_DOWN;
    mouse_x = 0.0f;
    mouse_y = 0.0f;
    return controls;
}

int main(void) {
    /* The maximize box is what delivers WM_MAXIMIZE. The restored size stays 960x600. */
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_W * SCALE, SCREEN_H * SCALE, "Star Goose");
#if defined(__SWITCH__)
    /* raylib mounts RomFS but leaves the working directory on the SD card. */
    chdir("romfs:/");
#endif
    watch_web_canvas();
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);
    if (!pictures_load()) {
        CloseWindow();
        return 1;
    }
    sound_init();

    Game game;
    game_boot(&game);
    float step = 0.0f;
    const float tick = 1.0f / 20.0f;
    int shot = getenv("GOOSE_SHOT") != 0;
    const char *scripted = getenv("GOOSE_TICKS");
    if (shot || scripted) game_play(&game);
    if (scripted) {
        int count = atoi(scripted);
        unsigned held = 0;
        if (getenv("GOOSE_UP")) held |= CONTROL_UP;
        if (getenv("GOOSE_LEFT")) held |= CONTROL_LEFT;
        if (getenv("GOOSE_RIGHT")) held |= CONTROL_RIGHT;
        if (getenv("GOOSE_FIRE")) held |= CONTROL_FIRE;
        if (getenv("GOOSE_N")) held |= CONTROL_LEFT_DEVICE;
        if (getenv("GOOSE_M")) held |= CONTROL_RIGHT_DEVICE;
        for (int i = 0; i < count; i++) game_tick(&game, held);
        shot = 1;
    }

    while (!WindowShouldClose() && !leave_game) {
        if (!shot) {
            float frame = GetFrameTime();
            sample_mouse();
            service_keys();
            game_hold_input(ui_mode != UI_PLAY);
            if (ui_mode == UI_PLAY) {
                step += frame;
                if (step > 0.25f) step = 0.25f;
                while (step >= tick && ui_mode == UI_PLAY) {
                    step -= tick;
                    if (IsKeyPressed(KEY_R)
#if defined(__SWITCH__)
                        || IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)
#endif
                        ) game_boot(&game);
                    else game_tick(&game, read_controls());
                }
            }
        }
        sound_pump();
        game_draw(&game);
        if (ui_mode == UI_QUIT) hud_quit_dialog();
        if (ui_mode == UI_REBIND && rebind_step < 8) {
            hud_redefine_dialog();
            hud_control_prompt(REBIND_PROMPT[rebind_step]);
        }
        pictures_finish();
        BeginDrawing();
        ClearBackground(BLACK);
        pictures_present();
        if (shot) {
            pictures_save("shot.png");
            printf("mode %d level %d part %d score %d ammo %d lives %d tunnel %d fuel %d shield %d\n",
                   game.mode, game.level, game.part, game.score, game.ammo, game.lives,
                   game.tunnel_distance, game.flight.fuel, game.flight.shield);
        }
        EndDrawing();
        if (shot) break;
    }
    pictures_shutdown();
    sound_shutdown();
    CloseWindow();
    return 0;
}
