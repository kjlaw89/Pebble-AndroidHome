#include <pebble.h>
#include "main.h"
#include "datetime.h"
#include "weather.h"

void datetime_init (Window *window) {
    clock_is_24h = clock_is_24h_style ();
    
    // Create time TextLayer
    s_time_layer = text_layer_create(GRect(0, 35, 144, 50));
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text(s_time_layer, "");
    text_layer_set_text_color(s_time_layer, GColorWhite);
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    text_layer_set_font(s_time_layer, font_roboto_l_32);
    
    // Create seconds Layer
    s_seconds_layer = layer_create(GRect(0, 88, 144, 2));
    layer_set_update_proc(s_seconds_layer, datetime_update_seconds_layer);
    
    // Create date TextLayer
    s_date_layer = text_layer_create(GRect(0, 90, 144, 20));
    text_layer_set_background_color(s_date_layer, GColorClear);
    text_layer_set_text(s_date_layer, "");
    text_layer_set_text_color(s_date_layer, GColorWhite);
    text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
    text_layer_set_font(s_date_layer, font_roboto_r_12);
    
    // Add layers to parent window
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
    layer_add_child(window_get_root_layer(window), s_seconds_layer);
    
    // Register with TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT | DAY_UNIT, datetime_clock_handler);
}

void datetime_deinit () {
    // Destroy layers
    text_layer_destroy (s_time_layer);
    text_layer_destroy (s_date_layer);
    layer_destroy (s_seconds_layer);
}

void datetime_clock_handler (struct tm *tick_time, TimeUnits units_changed) {
    if (!datetime_initialized) {
        datetime_initialized = true;
        //datetime_update_seconds (tick_time);
        datetime_update_minutes (tick_time, true);
        datetime_update_days (tick_time);
    }
    
    bool changed = (clock_is_24h != clock_is_24h_style ());
    
    if ((units_changed & SECOND_UNIT) != 0) {
        datetime_update_seconds (tick_time);
    }
    
    if ((units_changed & MINUTE_UNIT) != 0 || changed) {
        datetime_update_minutes (tick_time, changed);
        
        if (changed) {
            clock_is_24h = clock_is_24h_style ();
        }
    }
    
    if ((units_changed & DAY_UNIT) != 0) {
        datetime_update_days (tick_time);
    }
}

void datetime_update_seconds (struct tm *tick_time) {
    /*int seconds = tick_time->tm_sec;
    seconds_width = (int) ((seconds / 60.0) * 144.0);
    layer_mark_dirty(s_seconds_layer);*/
}

void datetime_update_seconds_layer (Layer *layer, GContext *ctx) {
    int left = (int) ((144.0 - seconds_width) / 2.0);
    
    graphics_context_set_fill_color (ctx, GColorWhite);
    graphics_fill_rect (ctx, GRect (left, 0, seconds_width, 2), 0, GCornerNone);
}

void datetime_update_minutes (struct tm *tick_time, bool changed_style) {
    if(clock_is_24h_style() == true) {
        if (changed_style) {
            text_layer_set_font(s_time_layer, font_roboto_l_46);
        }
        
        static char buffer[] = "00:00";
        strftime(buffer, sizeof(buffer), "%H:%M", tick_time);
        text_layer_set_text(s_time_layer, buffer);
    }
    
    else {
        if (changed_style) {
            text_layer_set_font(s_time_layer, font_roboto_l_32);
        }
        
        static char buffer[] = "00:00am";
        strftime(buffer, sizeof(buffer), "%l:%M%p", tick_time);
        text_layer_set_text(s_time_layer, buffer);
    }
    
    main_minutes_callback ();
    weather_minutes_callback ();
}

void datetime_update_days (struct tm *tick_time) {
    static char buffer[] = "Saturday, September 10";
    strftime(buffer, sizeof(buffer), "%A, %B %e", tick_time);
    text_layer_set_text(s_date_layer, buffer);
    
    main_days_callback ();
    weather_days_callback ();
}