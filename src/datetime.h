#pragma once

// Layers
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static Layer *s_seconds_layer;

// Positions for 24h vs 12hr
static GRect s_24_start_pos;
static GRect s_24_end_pos;
static GRect s_12_start_pos;
static GRect s_12_end_pos;

// Clock variables
static int seconds_width = 0;
static bool clock_is_24h = true;
static bool datetime_initialized = false;
static bool showing_notifications = false;

// Functions
void datetime_init (Window *window);
void datetime_deinit ();
void datetime_update_seconds (struct tm *tick_time);
void datetime_update_seconds_layer (Layer *layer, GContext *ctx);
void datetime_update_minutes (struct tm *tick_time, bool changed_style);
void datetime_update_days (struct tm *tick_time);
void datetime_clock_handler (struct tm *tick_time, TimeUnits units_changed);
void datetime_animate_down ();
void datetime_animate_up ();
void datetime_set_notifications (bool notifications);