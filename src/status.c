#include <pebble.h>
#include "status.h"
#include "main.h"
#include "notifications.h"
#include "weather.h"

void status_init (Window *window) {
    // Load our images
    b_bluetooth = gbitmap_create_with_resource (RESOURCE_ID_IMAGE_BLUETOOTH);
    b_battery = gbitmap_create_with_resource (RESOURCE_ID_IMAGE_BATTERY);
    b_battery_charge = gbitmap_create_with_resource (RESOURCE_ID_IMAGE_CHARGE);
    
    // Create Bluetooth Image layer
    s_bluetooth_image_layer = bitmap_layer_create (GRect (83, 6, 14, 14));
    bitmap_layer_set_alignment (s_bluetooth_image_layer, GAlignCenter);
    bitmap_layer_set_bitmap (s_bluetooth_image_layer, b_bluetooth);
    
    // Create Battery Image layer
    s_battery_image_layer = bitmap_layer_create (GRect (100, 7, 8, 11));
    bitmap_layer_set_alignment (s_battery_image_layer, GAlignCenter);
    bitmap_layer_set_bitmap (s_battery_image_layer, b_battery);
    
    // Create Battery Fill layer
    s_battery_fill_layer = layer_create (GRect (101, 9, 6, 8));
    layer_set_update_proc (s_battery_fill_layer, status_battery_fill_proc);
    
    // Create Battery Charge Image layer
    s_battery_charge_image_layer = bitmap_layer_create (GRect (0, 0, 6, 8));
    bitmap_layer_set_alignment (s_battery_charge_image_layer, GAlignCenter);
    bitmap_layer_set_bitmap (s_battery_charge_image_layer, b_battery_charge);
    layer_add_child (s_battery_fill_layer, bitmap_layer_get_layer (s_battery_charge_image_layer));
    
    // Create Battery Text Layer
    s_battery_layer = text_layer_create (GRect(113, 5, 33, 16));
    text_layer_set_background_color (s_battery_layer, GColorClear);
    text_layer_set_text_color (s_battery_layer, GColorWhite);
    text_layer_set_text_alignment (s_battery_layer, GTextAlignmentLeft);
    text_layer_set_text (s_battery_layer, "90%");
    text_layer_set_font (s_battery_layer, font_roboto_r_12);
    
    // Add layer to window
    layer_add_child (window_get_root_layer (window), bitmap_layer_get_layer (s_bluetooth_image_layer));
    layer_add_child (window_get_root_layer (window), bitmap_layer_get_layer (s_battery_image_layer));
    layer_add_child (window_get_root_layer (window), text_layer_get_layer (s_battery_layer));
    layer_add_child (window_get_root_layer (window), s_battery_fill_layer);
    
    // Check initial connection state
    s_bluetooth_connected = bluetooth_connection_service_peek ();
    layer_set_hidden (bitmap_layer_get_layer (s_bluetooth_image_layer), !s_bluetooth_connected);
    
    // Check initian battery state
    status_battery_handler (battery_state_service_peek ());
    
    // Subscribe to events
    bluetooth_connection_service_subscribe (status_bluetooth_handler);
    battery_state_service_subscribe (status_battery_handler);
}

void status_deinit () {
    // Destroy layers
    bitmap_layer_destroy (s_bluetooth_image_layer);
    bitmap_layer_destroy (s_battery_image_layer);
    bitmap_layer_destroy (s_battery_charge_image_layer);
    text_layer_destroy (s_battery_layer);
    layer_destroy (s_battery_fill_layer);
    
    // Unload bitmaps
    gbitmap_destroy (b_bluetooth);
    gbitmap_destroy (b_battery);
    gbitmap_destroy (b_battery_charge);
}

void status_bluetooth_handler (bool connected) {    
    s_bluetooth_connected = connected;
    layer_set_hidden (bitmap_layer_get_layer (s_bluetooth_image_layer), !connected);
    
    weather_connected (s_bluetooth_connected);
    notifications_connected (s_bluetooth_connected);
}

void status_battery_handler (BatteryChargeState charge) {
    static char buffer[5];
    snprintf (buffer, sizeof (buffer), "%d%%", (int) charge.charge_percent);
    text_layer_set_text (s_battery_layer, buffer);
    
    s_battery_level = (int) charge.charge_percent;
    s_battery_charging = charge.is_charging || charge.is_plugged;
    layer_mark_dirty (s_battery_fill_layer);
    layer_set_hidden (bitmap_layer_get_layer (s_battery_charge_image_layer), !charge.is_charging && !charge.is_plugged);
    
    weather_power (s_battery_level, s_battery_charging);
}

void status_battery_fill_proc (struct Layer *layer, GContext *ctx) {
    int y = (int) (8 * (1 - (s_battery_level / 100.0)));
    int height = 8 - y;
    
    graphics_context_set_fill_color (ctx, GColorBlack);
    graphics_fill_rect (ctx, GRect (0, 0, 6, 8), 0, GCornerNone);
    
    graphics_context_set_fill_color (ctx, GColorWhite);
    graphics_fill_rect (ctx, GRect (0, y, 6, height), 0, GCornerNone);
}

bool status_is_connected () {
    return s_bluetooth_connected;
}