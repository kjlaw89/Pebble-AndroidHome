#include <pebble.h>
#include "main.h"
#include "datetime.h"
#include "notifications.h"

void notifications_init (Window *window) {
    // Load our images
    b_notifications_calls = gbitmap_create_with_resource (RESOURCE_ID_IMAGE_NOTIFICATIONS_PHONE);
    b_notifications_texts = gbitmap_create_with_resource (RESOURCE_ID_IMAGE_NOTIFICATIONS_TEXT);
    b_notifications_emails = gbitmap_create_with_resource (RESOURCE_ID_IMAGE_NOTIFICATIONS_EMAIL);
    
    // Set our positions
    s_starting_pos = GRect (72, 102, 0, 24);
    s_position_1_1 = GRect (57, 102, 48, 24);
    s_position_1_2 = GRect (30, 102, 48, 24);
    s_position_2_2 = GRect (78, 102, 48, 24);
    s_position_1_3 = GRect (9, 102, 48, 24);
    s_position_2_3 = GRect (57, 102, 48, 24);
    s_position_3_3 = GRect (105, 102, 42, 24);
    
    // Create Calls TextLayer
    s_calls_text_layer = text_layer_create(GRect(20, 0, 20, 28));
    text_layer_set_background_color(s_calls_text_layer, GColorClear);
    text_layer_set_text(s_calls_text_layer, "1");
    text_layer_set_text_color(s_calls_text_layer, GColorWhite);
    text_layer_set_text_alignment(s_calls_text_layer, GTextAlignmentCenter);
    text_layer_set_font(s_calls_text_layer, font_roboto_r_12);
    
    // Create Texts TextLayer
    s_texts_text_layer = text_layer_create(GRect(20, 0, 20, 28));
    text_layer_set_background_color(s_texts_text_layer, GColorClear);
    text_layer_set_text(s_texts_text_layer, "1");
    text_layer_set_text_color(s_texts_text_layer, GColorWhite);
    text_layer_set_text_alignment(s_texts_text_layer, GTextAlignmentCenter);
    text_layer_set_font(s_texts_text_layer, font_roboto_r_12);
    
    // Create Emails TextLayer
    s_emails_text_layer = text_layer_create(GRect(20, 0, 20, 28));
    text_layer_set_background_color(s_emails_text_layer, GColorClear);
    text_layer_set_text(s_emails_text_layer, "1");
    text_layer_set_text_color(s_emails_text_layer, GColorWhite);
    text_layer_set_text_alignment(s_emails_text_layer, GTextAlignmentCenter);
    text_layer_set_font(s_emails_text_layer, font_roboto_r_12);
    
    // Create Calls BitmapLayer
    s_calls_image_layer = bitmap_layer_create (GRect (0, 0, 16, 20));
    bitmap_layer_set_alignment (s_calls_image_layer, GAlignLeft);
    bitmap_layer_set_bitmap (s_calls_image_layer, b_notifications_calls);
    
    // Create Texts BitmapLayer
    s_texts_image_layer = bitmap_layer_create (GRect (0, 0, 16, 20));
    bitmap_layer_set_alignment (s_texts_image_layer, GAlignLeft);
    bitmap_layer_set_bitmap (s_texts_image_layer, b_notifications_texts);
    
    // Create Emails BitmapLayer
    s_emails_image_layer = bitmap_layer_create (GRect (0, 0, 16, 20));
    bitmap_layer_set_alignment (s_emails_image_layer, GAlignLeft);
    bitmap_layer_set_bitmap (s_emails_image_layer, b_notifications_emails);
    
    // Create master layers
    s_calls_layer = layer_create (s_starting_pos);
    layer_add_child (s_calls_layer, text_layer_get_layer (s_calls_text_layer));
    layer_add_child (s_calls_layer, bitmap_layer_get_layer (s_calls_image_layer));
    
    s_texts_layer = layer_create (s_starting_pos);
    layer_add_child (s_texts_layer, text_layer_get_layer (s_texts_text_layer));
    layer_add_child (s_texts_layer, bitmap_layer_get_layer (s_texts_image_layer));
    
    s_emails_layer = layer_create (s_starting_pos);
    layer_add_child (s_emails_layer, text_layer_get_layer (s_emails_text_layer));
    layer_add_child (s_emails_layer, bitmap_layer_get_layer (s_emails_image_layer));
    
    // Add layers to parent window
    layer_add_child(window_get_root_layer(window), s_calls_layer);
    layer_add_child(window_get_root_layer(window), s_texts_layer);
    layer_add_child(window_get_root_layer(window), s_emails_layer);
    
    //datetime_animate_up ();
}

void notifications_deinit () {
    // Destroy images
    gbitmap_destroy (b_notifications_calls);
    gbitmap_destroy (b_notifications_texts);
    gbitmap_destroy (b_notifications_emails);
    
    // Destroy text layers
    text_layer_destroy (s_calls_text_layer);
    text_layer_destroy (s_texts_text_layer);
    text_layer_destroy (s_emails_text_layer);
    
    // Destroy parent layers
    layer_destroy (s_calls_layer);
    layer_destroy (s_texts_layer);
    layer_destroy (s_emails_layer);
}

void notifications_minutes_callback (struct tm *tick_time) {
    
}

void notifications_days_callback (struct tm *tick_time) {
    
}

void notifications_get () {
    
}

void notifications_app_callback (DictionaryIterator *iterator) {
    
}

void notifications_update () {
    
}

void notifications_activate (bool phone, bool text, bool email) {
    
}

void notifications_deactivate (bool phone, bool text, bool email) {
    
}