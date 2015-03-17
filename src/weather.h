#pragma once

// Layers
static TextLayer *s_weather_layer;
static BitmapLayer *s_weather_image_layer;
    
// Images
static GBitmap *b_weather_night;
static GBitmap *b_weather_sunny;
static GBitmap *b_weather_cloudy;
static GBitmap *b_weather_raining;
static GBitmap *b_weather_storming;
static GBitmap *b_weather_snowing;

// Weather variables
static bool weather_initialized = false;
static int weather_last_updated = -1;
static int weather_update_freq = 30;

// Functions
void weather_init (Window *window);
void weather_deinit ();
void weather_minutes_callback (struct tm *tick_time);
void weather_days_callback (struct tm *tick_time);
void weather_app_callback (DictionaryIterator *iterator);
void weather_get ();
void weather_get_failure ();
void weather_update (int temperature, const char *condition, bool stored);
void weather_connected (bool connected);