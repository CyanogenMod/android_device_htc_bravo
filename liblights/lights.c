/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "lights"

#include <cutils/log.h>

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>

#include <sys/ioctl.h>
#include <sys/types.h>

#include <hardware/lights.h>

#define LIGHT_ATTENTION	1
#define LIGHT_NOTIFY 	2

/******************************************************************************/
static struct light_state_t *g_notify;
static struct light_state_t *g_attention;
static pthread_once_t g_init = PTHREAD_ONCE_INIT;
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
static int g_backlight = 255;
static int g_buttons = 0;
struct led_prop {
    const char *filename;
    int fd;
};

struct led {
    struct led_prop mode;
    struct led_prop brightness;
    struct led_prop blink;
    struct led_prop color;
    struct led_prop period;
};

enum {
    JOGBALL_LED,
    BUTTONS_LED,
    AMBER_LED,
    GREEN_LED,
    BLUE_LED,
    RED_LED,
    LCD_BACKLIGHT,
    NUM_LEDS,
};

struct led leds[NUM_LEDS] = {
    [JOGBALL_LED] = {
        .brightness = { "/sys/class/leds/jogball-backlight/brightness", 0},
        .color = { "/sys/class/leds/jogball-backlight/color", 0},
        .period = { "/sys/class/leds/jogball-backlight/period", 0},
    },
    [BUTTONS_LED] = {
        .brightness = { "/sys/class/leds/button-backlight/brightness", 0},
    },
    [RED_LED] = {
        .brightness = { "/sys/class/leds/red/brightness", 0},
        .blink = { "/sys/class/leds/red/blink", 0},
    },
    [GREEN_LED] = {
        .brightness = { "/sys/class/leds/green/brightness", 0},
        .blink = { "/sys/class/leds/green/blink", 0},
    },
    [BLUE_LED] = {
        .brightness = { "/sys/class/leds/blue/brightness", 0},
        .blink = { "/sys/class/leds/blue/blink", 0},
    },
    [AMBER_LED] = {
        .brightness = { "/sys/class/leds/amber/brightness", 0},
        .blink = { "/sys/class/leds/amber/blink", 0},
    },
    [LCD_BACKLIGHT] = {
        .brightness = { "/sys/class/leds/lcd-backlight/brightness", 0},
    },
};

enum {
    RGB_BLACK = 0x000000,
    RGB_RED = 0xFF0000,
    RGB_AMBER = 0xFFFF00,  /* note this is actually RGB yellow */
    RGB_GREEN = 0x00FF00,
    RGB_BLUE = 0x0000FF,
    RGB_WHITE = 0xFFFFFF,
    RGB_PINK = 0xFFC0CB,
    RGB_ORANGE = 0xFFA500,
    RGB_YELLOW = 0xFFFF00,
    RGB_PURPLE = 0x800080,
    RGB_LT_BLUE = 0xADD8E6,
};

/**
 * device methods
 */

static int init_prop(struct led_prop *prop)
{
    int fd;

    prop->fd = -1;
    if (!prop->filename)
        return 0;
    fd = open(prop->filename, O_RDWR);
    if (fd < 0) {
        LOGE("init_prop: %s cannot be opened (%s)\n", prop->filename,
             strerror(errno));
        return -errno;
    }

    prop->fd = fd;
    return 0;
}

static void close_prop(struct led_prop *prop)
{
    int fd;

    if (prop->fd > 0)
        close(prop->fd);
    return;
}

void init_globals(void)
{
    int i;
    pthread_mutex_init(&g_lock, NULL);

    for (i = 0; i < NUM_LEDS; ++i) {
        init_prop(&leds[i].brightness);
        init_prop(&leds[i].blink);
        init_prop(&leds[i].mode);
        init_prop(&leds[i].color);
        init_prop(&leds[i].period);
    }
    g_attention = malloc(sizeof(struct light_state_t));
    memset(g_attention, 0, sizeof(*g_attention));
    g_notify = malloc(sizeof(struct light_state_t));
    memset(g_notify, 0, sizeof(*g_notify));
}

static int
write_int(struct led_prop *prop, int value)
{
    char buffer[20];
    int bytes;
    int amt;

    if (prop->fd < 0)
        return 0;

    LOGV("%s %s: 0x%x\n", __func__, prop->filename, value);

    bytes = snprintf(buffer, sizeof(buffer), "%d\n", value);
    while (bytes > 0) {
        amt = write(prop->fd, buffer, bytes);
        if (amt < 0) {
            if (errno == EINTR)
                continue;
            return -errno;
        }
        bytes -= amt;
    }

    return 0;
}

static int
write_rgb(struct led_prop *prop, int red, int green, int blue)
{
    char buffer[20];
    int bytes;
    int amt;

    if (prop->fd < 0)
        return 0;

    LOGV("%s %s: red:%d green:%d blue:%d\n",
          __func__, prop->filename, red, green, blue);

    bytes = snprintf(buffer, sizeof(buffer), "%d %d %d\n", red, green, blue);
    while (bytes > 0) {
        amt = write(prop->fd, buffer, bytes);
        if (amt < 0) {
            if (errno == EINTR)
                continue;
            return -errno;
        }
        bytes -= amt;
    }

    return 0;
}

static unsigned int
set_rgb(int red, int green, int blue)
{
    return(((red << 16) & 0x00ff0000) |
           ((green << 8) & 0x0000ff00) |
           (blue & 0x000000ff));
}

static int
is_lit(struct light_state_t const* state)
{
    return state->color & 0x00ffffff;
}

static int
set_trackball_light(struct light_state_t const* state)
{
    static int trackball_mode = 0;
    int rc = 0;
    int mode = state->flashMode;
    int red, blue, green;
    int period = 0;

    if (state->flashMode == LIGHT_FLASH_HARDWARE) {
        mode = state->flashOnMS;
        period = state->flashOffMS;
    }
    LOGV("%s color=%08x mode=%d period %d\n", __func__,
        state->color, mode, period);


    if (mode != 0) {
        red = (state->color >> 16) & 0xff;
        green = (state->color >> 8) & 0xff;
        blue = state->color & 0xff;

        rc = write_rgb(&leds[JOGBALL_LED].color, red, green, blue);
        if (rc != 0)
            LOGE("set color failed rc = %d\n", rc);
        if (period) {
            rc = write_int(&leds[JOGBALL_LED].period, period);
            if (rc != 0)
               LOGE("set period failed rc = %d\n", rc);
        }
    }
    // If the value isn't changing, don't set it, because this
    // can reset the timer on the breathing mode, which looks bad.
    if (trackball_mode == mode) {
        return 0;
    }
    trackball_mode = mode;

return write_int(&leds[JOGBALL_LED].brightness, mode);
}

static void
handle_trackball_light_locked(int type)
{
    struct light_state_t *new_state = 0;
    int attn_mode = 0;

    if (g_attention->flashMode == LIGHT_FLASH_HARDWARE)
        attn_mode = g_attention->flashOnMS;

    LOGV("%s type %d attention %p notify %p\n",
        __func__, type, g_attention, g_notify);

    switch (type) {
        case LIGHT_ATTENTION: {
            if (attn_mode == 0) {
                /* go back to notify state */
                new_state = g_notify;
            } else {
               new_state = g_attention;
            }
        break;
        }
        case LIGHT_NOTIFY: {
            if (attn_mode != 0) {
                /* attention takes priority over notify state */
                new_state = g_attention;
            } else {
               new_state = g_notify;
            }
        break;
        }
    }
    if (new_state == 0) {
        LOGE("%s: unknown type (%d)\n", __func__, type);
        return;
    }
    LOGV("%s new state %p\n", __func__, new_state);
    set_trackball_light(new_state);
    return;
}

static int
rgb_to_brightness(struct light_state_t const* state)
{
    int color = state->color & 0x00ffffff;
    return ((77*((color>>16)&0x00ff))
            + (150*((color>>8)&0x00ff)) + (29*(color&0x00ff))) >> 8;
}

static int
set_light_backlight(struct light_device_t* dev,
        struct light_state_t const* state)
{
    int err = 0;
    int brightness = rgb_to_brightness(state);
    LOGV("%s brightness=%d color=0x%08x",
            __func__,brightness, state->color);
    pthread_mutex_lock(&g_lock);
    g_backlight = brightness;
    err = write_int(&leds[LCD_BACKLIGHT].brightness, brightness);
    pthread_mutex_unlock(&g_lock);
    return err;
}

static int
set_light_keyboard(struct light_device_t* dev,
        struct light_state_t const* state)
{
    /* nothing to do on bravo*/
    return 0;
}

static int
set_light_buttons(struct light_device_t* dev,
        struct light_state_t const* state)
{
    int err = 0;
    int on = is_lit(state);
    pthread_mutex_lock(&g_lock);
    g_buttons = on;
    err = write_int(&leds[BUTTONS_LED].brightness, on?255:0);
    pthread_mutex_unlock(&g_lock);
    return err;
}

static int
set_speaker_light_locked(struct light_device_t* dev,
        struct light_state_t const* state)
{
    int len;
    unsigned int colorRGB;

    /* Red = amber_led, blue or green = green_led */
    colorRGB = state->color & 0xFFFFFF;

    switch (state->flashMode) {
        case LIGHT_FLASH_TIMED:
            LOGV("set_led_state colorRGB=%08X, flashing\n", colorRGB);
            switch (colorRGB) {
                case RGB_RED:
                    write_int(&leds[RED_LED].blink, 1);
                    break;
                case RGB_AMBER:
                    write_int(&leds[AMBER_LED].blink, 2);
                    break;
                case RGB_GREEN:
                    write_int(&leds[GREEN_LED].blink, 1);
                    break;
                case RGB_BLUE:
                    write_int(&leds[BLUE_LED].blink, 1);
                    break;
                case RGB_BLACK:  /*off*/
                    write_int(&leds[GREEN_LED].blink, 0);
                    write_int(&leds[RED_LED].blink, 0);
                    write_int(&leds[BLUE_LED].blink, 0);
                    write_int(&leds[AMBER_LED].blink, 0);
                    break;
                    break;
                default:
                    LOGE("set_led_state colorRGB=%08X, unknown color\n",
                          colorRGB);
                    break;
            }
            break;
        case LIGHT_FLASH_NONE:
            LOGV("set_led_state colorRGB=%08X, on\n", colorRGB);
            switch (colorRGB) {
               case RGB_RED:
                    /*no support for red solid */
               case RGB_AMBER:
                    write_int(&leds[AMBER_LED].brightness, 1);
                    write_int(&leds[GREEN_LED].brightness, 0);
                    write_int(&leds[BLUE_LED].brightness, 0);
                    break;
                case RGB_GREEN:
                    write_int(&leds[GREEN_LED].brightness, 1);
                    write_int(&leds[AMBER_LED].brightness, 0);
                    write_int(&leds[BLUE_LED].brightness, 0);
                   break;
                case RGB_BLUE:
                    write_int(&leds[BLUE_LED].brightness, 1);
                    write_int(&leds[GREEN_LED].brightness, 0);
                    write_int(&leds[AMBER_LED].brightness, 0);
                   break;
                case RGB_BLACK:  /*off*/
                    write_int(&leds[GREEN_LED].brightness, 0);
                    write_int(&leds[AMBER_LED].brightness, 0);
                    write_int(&leds[BLUE_LED].brightness, 0);
                    write_int(&leds[RED_LED].brightness, 0);
                    break;
                default:
                    LOGE("set_led_state colorRGB=%08X, unknown color\n",
                          colorRGB);
                    break;
            }
            break;
        default:
            LOGE("set_led_state colorRGB=%08X, unknown mode %d\n",
                  colorRGB, state->flashMode);
    }
    return 0;
}

static int
set_light_battery(struct light_device_t* dev,
        struct light_state_t const* state)
{
    pthread_mutex_lock(&g_lock);
    LOGV("%s mode=%d color=0x%08x",
            __func__,state->flashMode, state->color);
    set_speaker_light_locked(dev, state);
    pthread_mutex_unlock(&g_lock);
    return 0;
}

static int
set_light_notifications(struct light_device_t* dev,
        struct light_state_t const* state)
{
    pthread_mutex_lock(&g_lock);

    LOGV("%s mode=%d color=0x%08x On=%d Off=%d\n",
            __func__,state->flashMode, state->color,
            state->flashOnMS, state->flashOffMS);
    /*
    ** TODO Allow for user settings of color and interval
    ** Setting 60% brightness
    */
    switch (state->color & 0x00FFFFFF) {
        case RGB_BLACK:
            g_notify->color = set_rgb(0, 0, 0);
            break;
        case RGB_WHITE:
            g_notify->color = set_rgb(50, 127, 48);
            break;
        case RGB_RED:
            g_notify->color = set_rgb(141, 0, 0);
            break;
        case RGB_GREEN:
            g_notify->color = set_rgb(0, 141, 0);
            break;
        case RGB_BLUE:
            g_notify->color = set_rgb(0, 0, 141);
            break;
        case RGB_PINK:
            g_notify->color = set_rgb(141, 52, 58);
            break;
        case RGB_PURPLE:
            g_notify->color = set_rgb(70, 0, 70);
            break;
        case RGB_ORANGE:
            g_notify->color = set_rgb(141, 99, 0);
            break;
        case RGB_YELLOW:
            g_notify->color = set_rgb(100, 141, 0);
            break;
        case RGB_LT_BLUE:
            g_notify->color = set_rgb(35, 55, 98);
            break;
        default:
            g_notify->color = state->color;
            break;
    }

    if (state->flashMode != LIGHT_FLASH_NONE) {
        g_notify->flashMode = LIGHT_FLASH_HARDWARE;
        g_notify->flashOnMS = 7;
        g_notify->flashOffMS = (state->flashOnMS + state->flashOffMS)/1000;
    } else {
        g_notify->flashOnMS = 0;
        g_notify->flashOffMS = 0;
    }
    handle_trackball_light_locked(LIGHT_NOTIFY);

    pthread_mutex_unlock(&g_lock);
    return 0;
}

static int
set_light_attention(struct light_device_t* dev,
        struct light_state_t const* state)
{
    unsigned int colorRGB;

    LOGV("%s color=0x%08x mode=0x%08x submode=0x%08x",
            __func__, state->color, state->flashMode, state->flashOnMS);

    pthread_mutex_lock(&g_lock);
    /* tune color for hardware*/
    switch (state->color & 0x00FFFFFF) {
        case RGB_WHITE:
            colorRGB = set_rgb(101, 255, 96);
            break;
        case RGB_BLUE:
            colorRGB = set_rgb(0, 0, 235);
            break;
        case RGB_BLACK:
            colorRGB = set_rgb(0, 0, 0);
            break;
        default:
            LOGE("%s colorRGB=%08X, unknown color\n",
                          __func__, state->color);
            colorRGB = set_rgb(101, 255, 96);
            break;
    }
    g_attention->flashMode = state->flashMode;
    g_attention->flashOnMS = state->flashOnMS;
    g_attention->color = colorRGB;
    g_attention->flashOffMS = 0;
    handle_trackball_light_locked(LIGHT_ATTENTION);

    pthread_mutex_unlock(&g_lock);
    return 0;
}


/** Close the lights device */
static int
close_lights(struct light_device_t *dev)
{
    int i;

    for (i = 0; i < NUM_LEDS; ++i) {
        close_prop(&leds[i].brightness);
        close_prop(&leds[i].blink);
        close_prop(&leds[i].mode);
    }

    if (dev) {
        free(dev);
    }
    return 0;
}


/******************************************************************************/

/**
 * module methods
 */

/** Open a new instance of a lights device using name */
static int open_lights(const struct hw_module_t* module, char const* name,
        struct hw_device_t** device)
{
    int (*set_light)(struct light_device_t* dev,
            struct light_state_t const* state);

    if (0 == strcmp(LIGHT_ID_BACKLIGHT, name)) {
        set_light = set_light_backlight;
    }
    else if (0 == strcmp(LIGHT_ID_KEYBOARD, name)) {
        set_light = set_light_keyboard;
    }
    else if (0 == strcmp(LIGHT_ID_BUTTONS, name)) {
        set_light = set_light_buttons;
    }
    else if (0 == strcmp(LIGHT_ID_BATTERY, name)) {
        set_light = set_light_battery;
    }
    else if (0 == strcmp(LIGHT_ID_NOTIFICATIONS, name)) {
        set_light = set_light_notifications;
    }
    else if (0 == strcmp(LIGHT_ID_ATTENTION, name)) {
        set_light = set_light_attention;
    }
    else {
        return -EINVAL;
    }

    pthread_once(&g_init, init_globals);

    struct light_device_t *dev = malloc(sizeof(struct light_device_t));
    memset(dev, 0, sizeof(*dev));

    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (struct hw_module_t*)module;
    dev->common.close = (int (*)(struct hw_device_t*))close_lights;
    dev->set_light = set_light;

    *device = (struct hw_device_t*)dev;
    return 0;
}


static struct hw_module_methods_t lights_module_methods = {
    .open =  open_lights,
};

/*
 * The lights Module
 */
const struct hw_module_t HAL_MODULE_INFO_SYM = {
    .tag = HARDWARE_MODULE_TAG,
    .version_major = 1,
    .version_minor = 0,
    .id = LIGHTS_HARDWARE_MODULE_ID,
    .name = "bravo lights Module",
    .author = "Google, Inc.",
    .methods = &lights_module_methods,
};
