#include <pebble.h>
#include "main.h"
#include "datetime.h"
#include "weather.h"
#include "status.h"
#include "notifications.h"
    
#define KEY_WEATHER 0
#define KEY_BATTERY 1
#define KEY_NOTIFICATIONS 2
#define KEY_TIMEOUT 3
#define KEY_CONFIGURATION 4
    
void main_minutes_callback (struct tm *tick_time) { }
void main_days_callback (struct tm *tick_time) { }

static void main_window_load(Window *window) {
    datetime_init (window);
    weather_init (window);
    notifications_init (window);
    status_init (window);
}

static void main_window_unload(Window *window) {
    datetime_deinit ();
    weather_deinit ();
    notifications_deinit ();
    status_deinit ();
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    Tuple *t = dict_read_first(iterator);

    // For all items
    while(t != NULL) {
        switch(t->key) {
            case KEY_WEATHER:
                weather_app_callback (iterator);
                return;
            case KEY_CONFIGURATION:
                weather_configuration (iterator);
                return;
            case KEY_TIMEOUT:
                APP_LOG (APP_LOG_LEVEL_ERROR, "Timeout occurred");
                return;
            default:
                APP_LOG (APP_LOG_LEVEL_INFO, "Key %d not recognized!", (int)t->key);
                break;
        }

        // Look for next item
        t = dict_read_next(iterator);
    }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped! Reason: %d", reason);
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    Tuple *t = dict_read_first(iterator);

    // For all items
    while(t != NULL) {
        switch(t->key) {
            case KEY_WEATHER:
                weather_get_cache (true);
                return;
        }

        // Look for next item
        t = dict_read_next(iterator);
    }
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void init() {
    // Load our fonts for the application
    font_roboto_r_12 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_REGULAR_12));
    
    // Create main Window element and assign to pointer
    s_main_window = window_create();
    window_set_background_color(s_main_window, GColorBlack);
    window_set_fullscreen(s_main_window, true);

    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });

    // Show the Window on the watch, with animated=true
    window_stack_push(s_main_window, true);
    
    // Register callbacks
    app_message_register_inbox_received (inbox_received_callback);
    app_message_register_inbox_dropped (inbox_dropped_callback);
    app_message_register_outbox_failed (outbox_failed_callback);
    app_message_register_outbox_sent (outbox_sent_callback);
    
    // Open AppMessage
    app_message_open (app_message_inbox_size_maximum (), app_message_outbox_size_maximum ());
}

static void deinit() {
    // Destroy Window
    window_destroy(s_main_window);
    
    // Unload GFont
    fonts_unload_custom_font (font_roboto_r_12);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
