#pragma once

// Layers
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static Layer *s_seconds_layer;

// Fonts
static GFont s_time_24_font;
static GFont s_time_12_font;
static GFont s_date_font;

// Clock variables
static int seconds_width = 0;
static bool clock_is_24h = true;

// Functions
static void update_seconds (struct tm *tick_time);
static void update_minutes (struct tm *tick_time, bool changed_style);
static void update_days (struct tm *tick_time);
static void layer_update (struct Layer *layer, GContext *ctx);