/*
 * Healthy Eyes - The Smart Pomodoro Timer for Pebble
 * Copyright (c) 2016 James Smith (jamessmithsmusic@gmail.com)
 * Licensed under the MIT license: http://opensource.org/licenses/MIT
 */

#pragma once

// Correct missing constant
#ifndef MENU_CELL_BASIC_CELL_HEIGHT
#define MENU_CELL_BASIC_CELL_HEIGHT 44
#endif

// Default time string: 00:00:00
#define TIME_LENGTH 9

enum {
  PERSIST_WAKEUP,   // Persistent storage key for wakeupId
  PERSIST_TIMER,    // Persistent storage key for timer
  PERSIST_BREAKS,   // Persistent storage key for breaks
  PERSIST_SETTINGS, // Persistent storage key for settings
};

// A structure containing our settings
typedef struct ClaySettings {
  int TimerTime;
  int BreakTime;
  int SnoozeTime;
  char ShakeSetting[7];
} __attribute__((__packed__)) ClaySettings;

void settings_load(void);
void handle_inbox_received(DictionaryIterator *iter, void *context);
ClaySettings settings_get(void);
void format_time(uint32_t time, char* buffer);