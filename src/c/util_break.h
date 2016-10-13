/*
 * Healthy Eyes
 * Pebble pomodoro timer and break tracker
 *
 * Copyright © 2016 James Smith
 */

#pragma once

#define MAX_BREAKS 20
#define BREAK_TITLE_LENGTH 13
#define BREAK_SUBTITLE_LENGTH 20

typedef struct Break {
  time_t startTime;
  time_t endTime;
  time_t elapsedTime;
} Break;

typedef struct BreakString {
  char title[BREAK_TITLE_LENGTH];
  char subtitle[BREAK_SUBTITLE_LENGTH];
} BreakString;

void break_clear_stored(void);
void break_clear(void);
void break_start(void);
void break_stop(void);
bool break_isStarted(void);
void break_toggle(void);
void breaks_save(void);
void breaks_load(void);
BreakString break_string_get(int index);
int  num_breaks_get(void);