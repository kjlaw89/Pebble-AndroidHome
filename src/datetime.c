#include <pebble.h>
#include "main.h"
#include "datetime.h"
#include "weather.h"
#include "notifications.h"

void datetime_init (Window *window) {
    clock_is_24h = clock_is_24h_style ();
    
    s_24_start_pos = GRect (0, 35, 144, 50);
    s_24_end_pos = GRect (0, 10, 144, 50);
    s_12_start_pos = GRect (0, 48, 107, 50);
    s_12_end_pos = GRect (0, 23, 107, 50);
    s_12_type_start_pos = GRect (108, 68, 34, 24);
    s_12_type_end_pos = GRect (108, 43, 34, 24);
    
    strcpy (s_time_format, clock_is_24h ? "%H:%M" : "%l:%M");
        
    // Create time TextLayer
    s_time_layer = text_layer_create (clock_is_24h ? s_24_start_pos : s_12_start_pos);
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text(s_time_layer, "");
    text_layer_set_text_color(s_time_layer, GColorWhite);
    text_layer_set_text_alignment(s_time_layer, clock_is_24h ? GTextAlignmentCenter : GTextAlignmentRight);
    text_layer_set_font(s_time_layer, clock_is_24h ? font_roboto_l_50 : font_roboto_l_38);
    
    s_time_type_layer = text_layer_create (s_12_type_start_pos);
    text_layer_set_background_color(s_time_type_layer, GColorClear);
    text_layer_set_text(s_time_type_layer, "");
    text_layer_set_text_color(s_time_type_layer, GColorWhite);
    text_layer_set_text_alignment(s_time_type_layer, GTextAlignmentLeft);
    text_layer_set_font(s_time_type_layer, font_roboto_l_18);
    layer_set_hidden (text_layer_get_layer (s_time_type_layer), clock_is_24h);
    
    // Create date TextLayer
    s_date_layer = text_layer_create(GRect(0, 90, 144, 20));
    text_layer_set_background_color(s_date_layer, GColorClear);
    text_layer_set_text(s_date_layer, "");
    text_layer_set_text_color(s_date_layer, GColorWhite);
    text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
    text_layer_set_font(s_date_layer, font_roboto_r_12);
    
    // Add layers to parent window
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_type_layer));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
    
    // Register with TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT | DAY_UNIT, datetime_clock_handler);
}

void datetime_deinit () {
    text_layer_destroy (s_time_layer);
    text_layer_destroy (s_date_layer);
}

void datetime_clock_handler (struct tm *tick_time, TimeUnits units_changed) {
    if (!datetime_initialized) {
        datetime_initialized = true;
        datetime_update_minutes (tick_time);
        datetime_update_days (tick_time);
    }
    
    if ((units_changed & MINUTE_UNIT) != 0) {        
        datetime_update_minutes (tick_time);
    }
    
    if ((units_changed & DAY_UNIT) != 0) {
        datetime_update_days (tick_time);
    }
}

void datetime_update_minutes (struct tm *tick_time) {
    strftime(s_time_buffer, sizeof(s_time_buffer), s_time_format, tick_time);
    strftime(s_time_type_buffer, sizeof(s_time_type_buffer), "%p", tick_time);
    text_layer_set_text (s_time_layer, s_time_buffer);
    text_layer_set_text (s_time_type_layer, s_time_type_buffer);
    
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
    Layer *time_type_layer = text_layer_get_layer (s_time_type_layer);
    Layer *date_layer = text_layer_get_layer (s_date_layer);
    static PropertyAnimation *s_property_animation_time;
    static PropertyAnimation *s_property_animation_time_type;
    static PropertyAnimation *s_property_animation_date;
    
    // Set start and end
    GRect from_frame_time = layer_get_frame(time_layer);
    GRect to_frame_time = clock_is_24h ? s_24_end_pos : s_12_end_pos;
    
    GRect from_frame_time_type = layer_get_frame(time_type_layer);
    GRect to_frame_time_type = s_12_type_end_pos;
    
    GRect from_frame_date = layer_get_frame(date_layer);
    GRect to_frame_date = GRect (0, 63, 144, 20);

    // Create the animation
    s_property_animation_time = property_animation_create_layer_frame(time_layer, &from_frame_time, &to_frame_time);
    s_property_animation_time_type = property_animation_create_layer_frame(time_type_layer, &from_frame_time_type, &to_frame_time_type);
    s_property_animation_date = property_animation_create_layer_frame(date_layer, &from_frame_date, &to_frame_date);

    // Schedule to occur ASAP with default settings
    animation_schedule((Animation*) s_property_animation_time);
    animation_schedule((Animation*) s_property_animation_time_type);
    animation_schedule((Animation*) s_property_animation_date);
}

void datetime_animate_down () {
    Layer *time_layer = text_layer_get_layer (s_time_layer);
    Layer *time_type_layer = text_layer_get_layer (s_time_type_layer);
    Layer *date_layer = text_layer_get_layer (s_date_layer);
    static PropertyAnimation *s_property_animation_time;
    static PropertyAnimation *s_property_animation_time_type;
    static PropertyAnimation *s_property_animation_date;
    
    // Set start and end
    GRect from_frame_time = layer_get_frame (time_layer);
    GRect to_frame_time = clock_is_24h ? s_24_start_pos : s_12_start_pos;
    
    GRect from_frame_time_type = layer_get_frame (time_type_layer);
    GRect to_frame_time_type = s_12_type_start_pos;
    
    GRect from_frame_date = layer_get_frame (date_layer);
    GRect to_frame_date = GRect(0, 88, 144, 20);

    // Create the animation
    s_property_animation_time = property_animation_create_layer_frame(time_layer, &from_frame_time, &to_frame_time);
    s_property_animation_time_type = property_animation_create_layer_frame(time_type_layer, &from_frame_time_type, &to_frame_time_type);
    s_property_animation_date = property_animation_create_layer_frame(date_layer, &from_frame_date, &to_frame_date);

    // Schedule to occur ASAP with default settings
    animation_schedule((Animation*) s_property_animation_time);
    animation_schedule((Animation*) s_property_animation_time_type);
    animation_schedule((Animation*) s_property_animation_date);
}