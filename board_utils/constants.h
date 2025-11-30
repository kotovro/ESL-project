#ifndef CONFIG_H
#define CONFIG_H

// --- Button settings ---
#define BUTTON_PIN          38
#define BUTTON_PRESSED       0
#define DEBOUNCE_TIME_MS    50
#define DOUBLE_CLICK_MS    400

// --- Modes ---
#define SLEEP               0
#define PICKING_HUE         1
#define PICKING_SATURATION  2
#define PICKING_VALUE       3

// --- Color ---
#define COLOR_CHANGE_MS    100
#define COLOR_CHANGE_DELAY (DOUBLE_CLICK_MS + 50)

// --- Fade steps ---
// Allow user override
#ifndef FADE_STEPS
#define FADE_STEPS         32
#endif

// --- Directions ---
#define INCREASE true
#define DECREASE false

#endif // CONFIG_H
