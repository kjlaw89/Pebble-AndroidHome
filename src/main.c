#include <pebble.h>
#include "main.h"
#include "datetime.h"
#include "weather.h"
#include "status.h"
#include "notifications.h"

#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
    
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {    
    if (!initialized) {
        initialized = true;
        //update_seconds (tick_time);
        update_minutes (tick_time, true);
        update_days (tick_time);
    }
    
    bool changed = (clock_is_24h != clock_is_24h_style ());
    
    if ((units_changed & SECOND_UNIT) != 0) {
        update_seconds (tick_time);
    }
    
    if ((units_changed & MINUTE_UNIT) != 0 || changed) {
        update_minutes (tick_time, changed);
        
        if (changed) {
            clock_is_24h = clock_is_24h_style ();
        }
    }
    
    if ((units_changed & DAY_UNIT) != 0) {
        update_days (tick_time);
    }
    
    //if (tick_time->tm_sec % 5 == 0) {
        // Begin dictionary
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
    
        // Add a key-value pair
        dict_write_uint8(iter, 0, 1);
    
        // Send the message!
        app_message_outbox_send();
    //}
}

static void update_seconds (struct tm *tick_time) {
    /*int seconds = tick_time->tm_sec;
    seconds_width = (int) ((seconds / 60.0) * 144.0);
    layer_mark_dirty(s_seconds_layer);*/
}

// Write the current hours and minutes into the buffer
static void update_minutes (struct tm *tick_time, bool changed_style) {
    if(clock_is_24h_style() == true) {
        if (changed_style) {
            text_layer_set_font(s_time_layer, s_time_24_font);
        }
        
        static char buffer[] = "00:00";
        strftime(buffer, sizeof(buffer), "%H:%M", tick_time);
        text_layer_set_text(s_time_layer, buffer);
    }
    
    else {
        if (changed_style) {
            text_layer_set_font(s_time_layer, s_time_12_font);
        }
        
        static char buffer[] = "00:00am";
        strftime(buffer, sizeof(buffer), "%l:%M%p", tick_time);
        text_layer_set_text(s_time_layer, buffer);
    }
}

static void update_days (struct tm *tick_time) {
    static char buffer[] = "Saturday, September 10";
    strftime(buffer, sizeof(buffer), "%A, %B %e", tick_time);
    text_layer_set_text(s_date_layer, buffer);
}

static void layer_update (Layer *layer, GContext *ctx) {
    int left = (int) ((144.0 - seconds_width) / 2.0);
    
    graphics_context_set_fill_color (ctx, GColorWhite);
    graphics_fill_rect (ctx, GRect (left, 0, seconds_width, 2), 0, GCornerNone);
}

static void main_window_load(Window *window) {
    clock_is_24h = clock_is_24h_style ();
    
    // Create GFont
    s_time_24_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_LIGHT_46));
    s_time_12_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_LIGHT_32));
    s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_REGULAR_12));
    
    // Create time TextLayer
    s_time_layer = text_layer_create(GRect(0, 35, 144, 50));
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text(s_time_layer, "");
    text_layer_set_text_color(s_time_layer, GColorWhite);
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    text_layer_set_font(s_time_layer, s_time_12_font);
    
    // Create seconds Layer
    s_seconds_layer = layer_create(GRect(0, 88, 144, 2));
    layer_set_update_proc(s_seconds_layer, layer_update);
    
    // Create date TextLayer
    s_date_layer = text_layer_create(GRect(0, 90, 144, 20));
    text_layer_set_background_color(s_date_layer, GColorClear);
    text_layer_set_text(s_date_layer, "");
    text_layer_set_text_color(s_date_layer, GColorWhite);
    text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
    text_layer_set_font(s_date_layer, s_date_font);
    
    // Create temperature Layer
    s_weather_layer = text_layer_create (GRect(85, 145, 40, 24));
    text_layer_set_background_color (s_weather_layer, GColorClear);
    text_layer_set_text_color (s_weather_layer, GColorWhite);
    text_layer_set_text_alignment (s_weather_layer, GTextAlignmentLeft);
    text_layer_set_text (s_weather_layer, "Loading...");
    text_layer_set_font (s_time_layer, s_time_12_font);
    
    // Create Weather Image layer
    s_weather_image_layer = bitmap_layer_create (GRect (40, 146, 35, 16));
    bitmap_layer_set_alignment (s_weather_image_layer, GAlignCenter);

    // Add it as a child layer to the Window's root layer
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
    layer_add_child(window_get_root_layer(window), s_seconds_layer);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_weather_image_layer));

    // Register with TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT | DAY_UNIT, tick_handler);
    
    // Load our images
    b_weather_night = gbitmap_create_with_resource (RESOURCE_ID_IMAGE_WEATHER_NIGHT);
    b_weather_sunny = gbitmap_create_with_resource (RESOURCE_ID_IMAGE_WEATHER_SUNNY);
    b_weather_cloudy = gbitmap_create_with_resource (RESOURCE_ID_IMAGE_WEATHER_CLOUDY);
    b_weather_raining = gbitmap_create_with_resource (RESOURCE_ID_IMAGE_WEATHER_RAINING);
    b_weather_storming = gbitmap_create_with_resource (RESOURCE_ID_IMAGE_WEATHER_STORMING);
    b_weather_snowing = gbitmap_create_with_resource (RESOURCE_ID_IMAGE_WEATHER_SNOWING);
}

static void main_window_unload(Window *window) {
    // Destroy TextLayer
    text_layer_destroy (s_time_layer);
    text_layer_destroy (s_date_layer);
    text_layer_destroy (s_weather_layer);
    layer_destroy (s_seconds_layer);
    bitmap_layer_destroy (s_weather_image_layer);
    
    // Unload GFont
    fonts_unload_custom_font (s_time_24_font);
    fonts_unload_custom_font (s_time_12_font);
    fonts_unload_custom_font (s_date_font);
    
    // Unload bitmaps
    gbitmap_destroy (b_weather_night);
    gbitmap_destroy (b_weather_sunny);
    gbitmap_destroy (b_weather_cloudy);
    gbitmap_destroy (b_weather_raining);
    gbitmap_destroy (b_weather_storming);
    gbitmap_destroy (b_weather_snowing);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    // Store incoming information
    static char temperature_buffer[8];
    static char weather_layer_buffer[32];

    // Read first item
    Tuple *t = dict_read_first(iterator);

    // For all items
    while(t != NULL) {
        switch(t->key) {
            case KEY_TEMPERATURE:
                snprintf(temperature_buffer, sizeof(temperature_buffer), "%d °F", (int)t->value->int32);
                break;
            case KEY_CONDITIONS:
                APP_LOG(APP_LOG_LEVEL_ERROR, "Value %d", strcmp (t->value->cstring, "partly-cloudy-night"));
            
                if (strcmp (t->value->cstring, "clear-day") == 0) {
                    bitmap_layer_set_bitmap (s_weather_image_layer, b_weather_sunny);
                    break;
                }
            
                if (strcmp (t->value->cstring, "clear-night") == 0) {
                    bitmap_layer_set_bitmap (s_weather_image_layer, b_weather_night);
                    break;
                }
            
                if (strcmp (t->value->cstring, "partly-cloudy-day") == 0
                    || strcmp (t->value->cstring, "cloudy") == 0
                    || strcmp (t->value->cstring, "partly-cloudy-night") == 0
                    || strcmp (t->value->cstring, "fog") == 0) {
                    bitmap_layer_set_bitmap (s_weather_image_layer, b_weather_cloudy);
                    break;
                }
                 
                if (strcmp (t->value->cstring, "rain") == 0 
                    || strcmp (t->value->cstring, "sleet") == 0 
                    || strcmp (t->value->cstring, "wind") == 0) {
                    bitmap_layer_set_bitmap (s_weather_image_layer, b_weather_raining);
                    break;
                }
            
                if (strcmp (t->value->cstring, "hail") == 0 
                    || strcmp (t->value->cstring, "thunderstorm") == 0 
                    || strcmp (t->value->cstring, "tornado") == 0) {
                    bitmap_layer_set_bitmap (s_weather_image_layer, b_weather_storming);
                    break;
                }
            
                if (strcmp (t->value->cstring, "snow") == 0) {
                    bitmap_layer_set_bitmap (s_weather_image_layer, b_weather_snowing);
                    break;
                }
            
                break;
            default:
                APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
                break;
        }

        // Look for next item
        t = dict_read_next(iterator);
    }

    // Assemble full string and display
    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s", temperature_buffer);
    text_layer_set_text(s_weather_layer, weather_layer_buffer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void init() {    
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
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
