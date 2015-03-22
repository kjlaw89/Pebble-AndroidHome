#include <pebble.h>
#include "main.h"
#include "weather.h"
#include "status.h"

#define KEY_WEATHER 0
#define KEY_TEMPERATURE 5
#define KEY_CONDITIONS 6
    
#define KEY_CONFIG_FORECAST_KEY 41
#define KEY_CONFIG_DEGREES 42
#define KEY_CONFIG_REFRESH 43
#define KEY_CONFIG_LOW_POWER 44
#define KEY_CONFIG_LOW_REFRESH 45
    
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
    
    // Load our variables in from persistent storage
    if (persist_exists (KEY_CONFIG_FORECAST_KEY)) {
        persist_read_string (KEY_CONFIG_FORECAST_KEY, weather_key, 60);
    }
    
    weather_in_celsius = persist_exists (KEY_CONFIG_DEGREES) ? persist_read_bool (KEY_CONFIG_DEGREES) : false;
    weather_refresh = persist_exists (KEY_CONFIG_REFRESH) ? persist_read_int (KEY_CONFIG_REFRESH) : 30;
    weather_low_power = persist_exists (KEY_CONFIG_LOW_POWER) ? persist_read_int (KEY_CONFIG_LOW_POWER) : 20;
    weather_low_refresh = persist_exists (KEY_CONFIG_LOW_REFRESH) ? persist_read_int (KEY_CONFIG_LOW_REFRESH) : 60;
    weather_update_freq = weather_refresh;
    
    weather_set_buffer ();
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
        if (status_is_connected () && !weather_get_cache (false)) {
            weather_get ();
            weather_last_updated = 0;    
        }
        else {
            weather_get_cache (true);
        }
        
        return;
    }
    
    weather_last_updated++;
}

void weather_days_callback (struct tm *tick_time) {
    
}

void weather_set_buffer () {
    if (weather_in_celsius) {
        strcpy (weather_text_buffer, "%d °C");
    }
    else {
        strcpy (weather_text_buffer, "%d °F");
    }
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

void weather_configuration (DictionaryIterator *iterator) {
    bool update_weather = false;
    bool old_celsius = weather_in_celsius;
    Tuple *t = dict_read_first(iterator);

    // For all items
    while(t != NULL) {
        switch(t->key) {
             case KEY_CONFIG_FORECAST_KEY:
                if (strcmp (weather_key, t->value->cstring) != 0) {
                    update_weather = true;
                }
            
                strcpy (weather_key, t->value->cstring);
                persist_write_string (KEY_CONFIG_FORECAST_KEY, weather_key);
                break;
            case KEY_CONFIG_DEGREES:
                weather_in_celsius = (strcmp (t->value->cstring, "c") == 0);
                
                if (old_celsius != weather_in_celsius) {
                    weather_set_buffer ();
                    update_weather = true;
                }
                
                persist_write_bool (KEY_CONFIG_DEGREES, weather_in_celsius);
                break;
            case KEY_CONFIG_REFRESH:
                weather_refresh = (int)t->value->int32;
                persist_write_int (KEY_CONFIG_REFRESH, weather_refresh);
                break;
            case KEY_CONFIG_LOW_POWER:
                weather_low_power = (int)t->value->int32;
                persist_write_int (KEY_CONFIG_LOW_POWER, weather_low_power);
                break;
            case KEY_CONFIG_LOW_REFRESH:
                weather_low_refresh = (int)t->value->int32;
                persist_write_int (KEY_CONFIG_LOW_REFRESH, weather_low_refresh);
                break;
            default:
                APP_LOG(APP_LOG_LEVEL_INFO, "Key %d not recognized!", (int)t->key);
                break;
        }

        // Look for next item
        t = dict_read_next(iterator);
    }
    
    // Make sure to change update freq based on new settings and current power level
    if (status_battery_level > 0 && status_battery_level <= weather_low_power && !status_battery_charging) {
        weather_update_freq = weather_low_refresh;
    }
    else {
        weather_update_freq = weather_refresh;
    }
    
    if (update_weather) {
        weather_last_updated = 0;
        weather_get ();
    }
}

void weather_get () {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 1);

    // Send the message!
    app_message_outbox_send();
}

bool weather_get_cache (bool hide) {
    APP_LOG(APP_LOG_LEVEL_WARNING, "Failure to get weather or no connection");
    
    if (persist_exists (KEY_WEATHER) && time (NULL) - persist_read_int (KEY_WEATHER) < 1800) {
        char condition[20];
        persist_read_string (KEY_CONDITIONS, condition, 20);
        weather_update (persist_read_int (KEY_TEMPERATURE), condition, true);
        return true;
    }
    else if (hide) {
        layer_set_hidden (text_layer_get_layer (s_weather_layer), true);
        layer_set_hidden (bitmap_layer_get_layer (s_weather_image_layer), true);
        weather_last_updated = -1;
    }
    
    return false;
}

void weather_update (int temperature, const char *condition, bool stored) {
    static char temperature_buffer[8];
    
    snprintf(temperature_buffer, sizeof (temperature_buffer), weather_text_buffer, temperature);
    text_layer_set_text (s_weather_layer, temperature_buffer);
    
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
    else {
        int last_time = persist_read_int (KEY_WEATHER);
        weather_last_updated = (int) ((time (NULL) - last_time) / 60);
        APP_LOG(APP_LOG_LEVEL_INFO, "Last attempt was %d minutes ago", weather_last_updated);
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

void weather_power (int level, bool charging) {
    status_battery_level = level;
    status_battery_charging = charging;
    
    if (level <= weather_low_power && !charging) {
        weather_update_freq = weather_low_refresh;
        return;
    }
    
    weather_update_freq = weather_refresh;
}