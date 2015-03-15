#include <pebble.h>
#include "status.h"

void status_init (Window *window) {
    // Load our images
    b_bluetooth = gbitmap_create_with_resource (RESOURCE_ID_IMAGE_BLUETOOTH);
    
    // Create Weather Image layer
    s_bluetooth_image_layer = bitmap_layer_create (GRect (5, 5, 14, 14));
    bitmap_layer_set_alignment (s_bluetooth_image_layer, GAlignCenter);
    bitmap_layer_set_bitmap (s_bluetooth_image_layer, b_bluetooth);
    
    // Add layer to window
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bluetooth_image_layer));
    
    // Subscribe to bluetooth events
    bluetooth_connection_service_subscribe(status_bluetooth_handler);
    APP_LOG(APP_LOG_LEVEL_INFO, "Should be subscribed");
}

void status_deinit () {
    // Destroy layers
    bitmap_layer_destroy (s_bluetooth_image_layer);
    
    // Unload bitmaps
    gbitmap_destroy (b_bluetooth);
}

void status_bluetooth_handler (bool connected) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Bluetooth Status: %s", connected ? "true" : "false");
    
    s_bluetooth_connected = connected;
    layer_set_hidden (bitmap_layer_get_layer (s_bluetooth_image_layer), !connected);
}