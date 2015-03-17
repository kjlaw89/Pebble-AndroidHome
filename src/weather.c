#include <pebble.h>
#include "main.h"
#include "weather.h"
#include "status.h"

#define KEY_WEATHER 0
#define KEY_TEMPERATURE 5
#define KEY_CONDITIONS 6
    
void weather_init (Window *window) {
    // Load our images
    b_weather_night = gbitmap_create_with_resource (RESOURCE_ID_IMAGE_WEATHER_NIGHT);
    b_weather_sunny = gbitmap_create_with_resource (RESOURCE_ID_IMAGE_WEATHER_SUNNY);
    b_weather_cloudy = gbitmap_create_with_resource (RESOURCE_ID_IMAGE_WEATHER_CLOUDY);
    b_weather_raining = gbitmap_create_with_resource (RESOURCE_ID_IMAGE_WEATHER_RAINING);
    b_weather_storming = gbitmap_create_with_resource (RESOURCE_ID_IMAGE_WEATHER_STORMING);
    b_weather_snowing = gbitmap_create_with_resource (RESOURCE_ID_IMAGE_WEATHER_SNOWING);
    
    // Create temperature Layer
    s_weather_layer = text_layer_create (GRect(78, 145, 40, 24));
    text_layer_set_background_color (s_weather_layer, GColorClear);
    text_layer_set_text_color (s_weather_layer, GColorWhite);
    text_layer_set_text_alignment (s_weather_layer, GTextAlignmentLeft);
    text_layer_set_text (s_weather_layer, "");
    text_layer_set_font (s_weather_layer, font_roboto_r_12);
    
    // Create Weather Image layer
    s_weather_image_layer = bitmap_layer_create (GRect (40, 145, 35, 16));
    bitmap_layer_set_alignment (s_weather_image_layer, GAlignCenter);
    
    // Add it as a child layer to the Window's root layer
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_weather_image_layer));
}

void weather_deinit () {
    // Destroy TextLayer
    text_layer_destroy (s_weather_layer);
    bitmap_layer_destroy (s_weather_image_layer);
    
    // Unload bitmaps
    gbitmap_destroy (b_weather_night);
    gbitmap_destroy (b_weather_sunny);
    gbitmap_destroy (b_weather_cloudy);
    gbitmap_destroy (b_weather_raining);
    gbitmap_destroy (b_weather_storming);
    gbitmap_destroy (b_weather_snowing);
}

void weather_minutes_callback (struct tm *tick_time) {
    if (weather_last_updated == -1 || weather_last_updated >= weather_update_freq) {
        if (status_is_connected ()) {
            weather_get ();
            weather_last_updated = 0;    
        }
        else {
            weather_get_failure ();
        }
        
        return;
    }
    
    weather_last_updated++;
}

void weather_days_callback (struct tm *tick_time) {
    
}

void weather_app_callback (DictionaryIterator *iterator) {    
    static int temperature = 0;
    static const char *condition;

    // Read first item
    Tuple *t = dict_read_first(iterator);

    // For all items
    while(t != NULL) {
        switch(t->key) {
            case KEY_WEATHER:
                break;
            case KEY_TEMPERATURE:
                temperature = (int)t->value->int32;
                break;
            case KEY_CONDITIONS:
                condition = t->value->cstring;
                break;
            default:
                APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
                break;
        }

        // Look for next item
        t = dict_read_next(iterator);
    }
    
    weather_update (temperature, condition, false);
}

void weather_get () {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 1);

    // Send the message!
    app_message_outbox_send();
}

void weather_get_failure () {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Failure to get weather or no connection");
    
    if (persist_exists (KEY_WEATHER) && time (NULL) - persist_read_int (KEY_WEATHER) < 1800) {
        char condition[20];
        persist_read_string (KEY_CONDITIONS, condition, 20);
        weather_update (persist_read_int (KEY_TEMPERATURE), condition, true);
    }
    else {
        layer_set_hidden (text_layer_get_layer (s_weather_layer), true);
        layer_set_hidden (bitmap_layer_get_layer (s_weather_image_layer), true);
    }
    
    weather_last_updated = -1;
}

void weather_update (int temperature, const char *condition, bool stored) {
    static char temperature_buffer[8];
    
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%d °F", temperature);
    text_layer_set_text(s_weather_layer, temperature_buffer);
    
    if (condition == NULL) {
        return;
    }
    
    layer_set_hidden (text_layer_get_layer (s_weather_layer), false);
    layer_set_hidden (bitmap_layer_get_layer (s_weather_image_layer), false);
    
    if (!stored) {
        persist_write_int (KEY_WEATHER, time (NULL));
        persist_write_int (KEY_TEMPERATURE, temperature);
        persist_write_string (KEY_CONDITIONS, condition);
    }
    
    if (strcmp (condition, "clear-day") == 0) {
        bitmap_layer_set_bitmap (s_weather_image_layer, b_weather_sunny);
        return;
    }

    if (strcmp (condition, "clear-night") == 0) {
        bitmap_layer_set_bitmap (s_weather_image_layer, b_weather_night);
        return;
    }

    if (strcmp (condition, "partly-cloudy-day") == 0
        || strcmp (condition, "cloudy") == 0
        || strcmp (condition, "partly-cloudy-night") == 0
        || strcmp (condition, "fog") == 0) {
        bitmap_layer_set_bitmap (s_weather_image_layer, b_weather_cloudy);
        return;
    }

    if (strcmp (condition, "rain") == 0 
        || strcmp (condition, "sleet") == 0 
        || strcmp (condition, "wind") == 0) {
        bitmap_layer_set_bitmap (s_weather_image_layer, b_weather_raining);
        return;
    }

    if (strcmp (condition, "hail") == 0 
        || strcmp (condition, "thunderstorm") == 0 
        || strcmp (condition, "tornado") == 0) {
        bitmap_layer_set_bitmap (s_weather_image_layer, b_weather_storming);
        return;
    }

    if (strcmp (condition, "snow") == 0) {
        bitmap_layer_set_bitmap (s_weather_image_layer, b_weather_snowing);
        return;
    }
}

void weather_connected (bool connected) {
    if (connected && (weather_last_updated == -1 || weather_last_updated >= weather_update_freq)) {
        weather_get ();
        weather_last_updated = 0;
    }
}