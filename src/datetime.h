#pragma once

// Layers
static TextLayer *s_time_layer;
static TextLayer *s_time_type_layer;
static TextLayer *s_date_layer;

// Fonts
GFont font_roboto_l_50;
GFont font_roboto_l_38;
GFont font_roboto_l_18;

// Positions for 24h vs 12hr
static GRect s_24_start_pos;
static GRect s_24_end_pos;
static GRect s_12_start_pos;
static GRect s_12_end_pos;
static GRect s_12_type_start_pos;
static GRect s_12_type_end_pos;

// Clock variables
static int seconds_width = 0;
static bool clock_is_24h = true;
static bool datetime_initialized = false;
static bool showing_notifications = false;
static char s_time_buffer[] = "00:00";
static char s_time_type_buffer[] = "am";
static char s_time_format[5];

// Functions
void datetime_init (Window *window);
void datetime_deinit ();
void datetime_update_minutes (struct tm *tick_time);
void datetime_update_days (struct tm *tick_time);
void datetime_clock_handler (struct tm *tick_time, TimeUnits units_changed);
void datetime_animate_down ();
void datetime_animate_up ();
void datetime_set_notifications (bool notifications);