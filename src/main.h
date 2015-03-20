#pragma once
    
static Window *s_main_window;
static bool initialized = false;

void main_minutes_callback (struct tm *tick_time);
void main_days_callback (struct tm *tick_time);

// Fonts
GFont font_roboto_r_12;