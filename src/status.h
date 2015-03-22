#pragma once

// Layers
static BitmapLayer *s_bluetooth_image_layer;
static BitmapLayer *s_battery_image_layer;
static BitmapLayer *s_battery_charge_image_layer;
static TextLayer *s_battery_layer;
static Layer *s_battery_fill_layer;

// Images
static GBitmap *b_bluetooth;
static GBitmap *b_battery;
static GBitmap *b_battery_charge;
    
// Variables
static bool s_bluetooth_connected = false;
static int s_battery_level = 0;
static int s_battery_charging = false;

void status_init (Window *window);
void status_deinit ();
void status_bluetooth_handler (bool connected);
void status_battery_handler (BatteryChargeState charge);
void status_battery_fill_proc (struct Layer *layer, GContext *ctx);
bool status_is_connected ();
