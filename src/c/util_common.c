/*
 * Healthy Eyes
 * Pebble pomodoro timer and break tracker
 *
 * Copyright © 2016 James Smith
 */

#include <pebble.h>
#include "util_common.h"

static ClaySettings settings;

// Initialize the default settings (WILL BE 15)
void settings_default(void) {
  settings.TimerTime    = 15;
  settings.BreakTime    = 15;
  settings.SnoozeTime   = 15;
  memcpy(settings.ShakeSetting, "none", 5);
}

// Read settings from persistent storage
void settings_load(void) {
  // Load the default settings
  settings_default();
  // Read settings from persistent storage, if they exist
  persist_read_data(PERSIST_SETTINGS, &settings, sizeof(settings));
}

// Save the settings to persistent storage
void settings_save(void) {
  persist_write_data(PERSIST_SETTINGS, &settings, sizeof(settings));
}

// Handle the response from AppMessage
void handle_inbox_received(DictionaryIterator *iter, void *context) {
  Tuple *timerTime = dict_find(iter, MESSAGE_KEY_TimerTime);
  if (timerTime) {
    settings.TimerTime = timerTime->value->int32;
  }
  Tuple *breakTime = dict_find(iter, MESSAGE_KEY_BreakTime);
  if (timerTime) {
    settings.BreakTime = breakTime->value->int32;
  }
  Tuple *snoozeTime = dict_find(iter, MESSAGE_KEY_SnoozeTime);
  if (timerTime) {
    settings.SnoozeTime = snoozeTime->value->int32;
  }
  Tuple *shakeSetting = dict_find(iter, MESSAGE_KEY_ShakeSetting);
  if (timerTime) {
    memcpy(settings.ShakeSetting, shakeSetting->value->cstring, shakeSetting->length);
  }
  // Save the new settings to persistent storage
  settings_save();
}

ClaySettings settings_get(void) {
  return settings;
}

// Display time as a string.
void format_time(uint32_t time, char* buffer) {
  int seconds = (int)time % 60;
  int minutes = (int)(time / 60) % 60;
  int hours   = (int)time / 3600;

  snprintf(buffer, TIME_LENGTH, "%02d:%02d:%02d", hours, minutes, seconds);
}