#pragma once

// Layers
static Layer *s_calls_layer;
static TextLayer *s_calls_text_layer;
static BitmapLayer *s_calls_image_layer;

static Layer *s_texts_layer;
static TextLayer *s_texts_text_layer;
static BitmapLayer *s_texts_image_layer;

static Layer *s_emails_layer;
static TextLayer *s_emails_text_layer;
static BitmapLayer *s_emails_image_layer;

//static TextLayer *s_generic_layer;
//static BitmapLayer *s_generic_image_layer;

// Variables
static bool calls_active = false;
static bool texts_active = false;
static bool emails_active = false;
//static bool generic_active = false;

// Images
static GBitmap *b_notifications_calls;
static GBitmap *b_notifications_texts;
static GBitmap *b_notifications_emails;
//static GBitmap *b_notifications_generic;

// Positions
static GRect s_starting_pos;
static GRect s_position_1_1;
static GRect s_position_1_2;
static GRect s_position_2_2;
static GRect s_position_1_3;
static GRect s_position_2_3;
static GRect s_position_3_3;

// Functions
void notifications_init (Window *window);
void notifications_deinit ();
void notifications_minutes_callback (struct tm *tick_time);
void notifications_days_callback (struct tm *tick_time);
void notifications_get ();
void notifications_app_callback (DictionaryIterator *iterator);
void notifications_update ();
void notifications_activate (bool phone, bool text, bool email);
void notifications_deactivate (bool phone, bool text, bool email);
void notifications_connected (bool connected);