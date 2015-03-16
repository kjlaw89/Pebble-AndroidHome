#pragma once

// Layers
static BitmapLayer *s_bluetooth_image_layer;

// Images
static GBitmap *b_bluetooth;
    
// Variables
static bool s_bluetooth_connected = false;

void status_init (Window *window);
void status_deinit ();
void status_bluetooth_handler (bool connected);
bool status_is_connected ();