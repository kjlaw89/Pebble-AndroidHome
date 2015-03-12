#pragma once

// Layers
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static Layer *s_seconds_layer;

// Clock variables
static int seconds_width = 0;
static bool clock_is_24h = true;
static bool datetime_initialized = false;

// Functions
void datetime_init (Window *window);
void datetime_deinit ();
void datetime_update_seconds (struct tm *tick_time);
void datetime_update_seconds_layer (Layer *layer, GContext *ctx);
void datetime_update_minutes (struct tm *tick_time, bool changed_style);
void datetime_update_days (struct tm *tick_time);
void datetime_clock_handler (struct tm *tick_time, TimeUnits units_changed);