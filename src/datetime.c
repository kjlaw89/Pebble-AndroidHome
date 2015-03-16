#include <pebble.h>
#include "main.h"
#include "datetime.h"
#include "weather.h"
#include "notifications.h"

void datetime_init (Window *window) {
    clock_is_24h = clock_is_24h_style ();
    
    s_24_start_pos = GRect (0, 35, 144, 50);
    s_24_end_pos = GRect (0, 10, 144, 50);
    s_12_start_pos = GRect (0, 50, 144, 50);
    s_12_end_pos = GRect (0, 25, 144, 50);
        
    // Create time TextLayer
    s_time_layer = text_layer_create (clock_is_24h ? s_24_start_pos : s_12_start_pos);
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text(s_time_layer, "");
    text_layer_set_text_color(s_time_layer, GColorWhite);
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    text_layer_set_font(s_time_layer, font_roboto_l_34);
    
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
            text_layer_set_font(s_time_layer, font_roboto_l_50);
        }
        
        static char buffer[] = "00:00";
        strftime(buffer, sizeof(buffer), "%H:%M", tick_time);
        text_layer_set_text(s_time_layer, buffer);
    }
    
    else {
        if (changed_style) {
            text_layer_set_font(s_time_layer, font_roboto_l_34);
        }
        
        static char buffer[] = "00:00am";
        strftime(buffer, sizeof(buffer), "%l:%M%p", tick_time);
        text_layer_set_text(s_time_layer, buffer);
    }
    
    main_minutes_callback (tick_time);
    weather_minutes_callback (tick_time);
    notifications_minutes_callback (tick_time);
}

void datetime_update_days (struct tm *tick_time) {
    static char buffer[] = "Saturday, September 10";
    strftime(buffer, sizeof(buffer), "%A, %B %e", tick_time);
    text_layer_set_text(s_date_layer, buffer);
    
    main_days_callback (tick_time);
    weather_days_callback (tick_time);
    notifications_days_callback (tick_time);
}

void datetime_set_notifications (bool notifications) {
    if (notifications == showing_notifications) {
        return;
    }
    
    if (notifications) {
        datetime_animate_up ();
        showing_notifications = true;
    }
    else {
        datetime_animate_down ();
        showing_notifications = false;
    }
}

void datetime_animate_up () {
    Layer *time_layer = text_layer_get_layer (s_time_layer);
    Layer *date_layer = text_layer_get_layer (s_date_layer);
    static PropertyAnimation *s_property_animation_time;
    static PropertyAnimation *s_property_animation_date;
    
    // Set start and end
    GRect from_frame_time = layer_get_frame(time_layer);
    GRect to_frame_time = clock_is_24h ? s_24_end_pos : s_12_end_pos;
    
    GRect from_frame_date = layer_get_frame(date_layer);
    GRect to_frame_date = GRect (0, 63, 144, 20);

    // Create the animation
    s_property_animation_time = property_animation_create_layer_frame(time_layer, &from_frame_time, &to_frame_time);
    s_property_animation_date = property_animation_create_layer_frame(date_layer, &from_frame_date, &to_frame_date);

    // Schedule to occur ASAP with default settings
    animation_schedule((Animation*) s_property_animation_time);
    animation_schedule((Animation*) s_property_animation_date);
}

void datetime_animate_down () {
    Layer *time_layer = text_layer_get_layer (s_time_layer);
    Layer *date_layer = text_layer_get_layer (s_date_layer);
    static PropertyAnimation *s_property_animation_time;
    static PropertyAnimation *s_property_animation_date;
    
    // Set start and end
    GRect from_frame_time = layer_get_frame(time_layer);
    GRect to_frame_time = clock_is_24h ? s_24_start_pos : s_12_start_pos;
    
    GRect from_frame_date = layer_get_frame(date_layer);
    GRect to_frame_date = GRect(0, 88, 144, 20);

    // Create the animation
    s_property_animation_time = property_animation_create_layer_frame(time_layer, &from_frame_time, &to_frame_time);
    s_property_animation_date = property_animation_create_layer_frame(date_layer, &from_frame_date, &to_frame_date);

    // Schedule to occur ASAP with default settings
    animation_schedule((Animation*) s_property_animation_time);
    animation_schedule((Animation*) s_property_animation_date);
}