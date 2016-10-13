/*
 * Healthy Eyes - The Smart Pomodoro Timer for Pebble
 * Copyright (c) 2016 James Smith (jamessmithsmusic@gmail.com)
 * Licensed under the MIT license: http://opensource.org/licenses/MIT
 */

#include <pebble.h>
#include "util_break.h"
#include "util_common.h"
#include "util_timer.h"

typedef struct BreakData {
  Break breakList[MAX_BREAKS];
  int numBreaks;
} __attribute__((__packed__)) BreakData;

Break breakList[MAX_BREAKS];
int numBreaks = 0;

char breakTitle[MAX_BREAKS][BREAK_TITLE_LENGTH];
char breakSubtitle[MAX_BREAKS][BREAK_SUBTITLE_LENGTH];

void break_store(Break b) {
  char title[TIME_LENGTH], sub1[TIME_LENGTH], sub2[TIME_LENGTH];

  // Format title
  format_time(b.elapsedTime, title);
  snprintf(breakTitle[numBreaks], BREAK_TITLE_LENGTH, "%2d) %s", numBreaks + 1, title);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "TITLE: %s\n", breakTitle[numBreaks]);

  // Format subtitle
  strftime(sub1, TIME_LENGTH, "%T", localtime(&b.startTime));
  strftime(sub2, TIME_LENGTH, "%T", localtime(&b.endTime));
  snprintf(breakSubtitle[numBreaks], BREAK_SUBTITLE_LENGTH, "%s - %s", sub1, sub2);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "SUBTITLE: %s\n", breakSubtitle[numBreaks]);
  numBreaks = numBreaks + 1;
}

void break_clear_stored(void) {
  int i;
  for (i = 0; i < numBreaks; i++) {
    breakList[i].startTime   = 0;
    breakList[i].endTime     = 0;
    breakList[i].elapsedTime = 0;
  }
  numBreaks = 0;
  persist_delete(PERSIST_BREAKS);
}

void break_clear(void) {
  breakList[numBreaks].startTime = 0;
}

void break_start(void) {
  if (numBreaks < MAX_BREAKS) {
    breakList[numBreaks].startTime = time(NULL);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "BREAK START: Storing %d in break slot %d.\n",
            (int)breakList[numBreaks].startTime, numBreaks);
  }
}

void break_stop(void) {
  if (numBreaks < MAX_BREAKS) {
    breakList[numBreaks].endTime = time(NULL);
    breakList[numBreaks].elapsedTime = breakList[numBreaks].endTime - breakList[numBreaks].startTime;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "BREAK STOP: Storing %d in break slot %d.\n",
            (int)breakList[numBreaks].endTime, numBreaks);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "BREAK LENGTH: %d.\n", (int)breakList[numBreaks].elapsedTime);
    break_store(breakList[numBreaks]);
  }
}

bool break_isStarted(void) {
  return breakList[numBreaks].startTime > 0;
}

void break_toggle(void) {
  if (break_isStarted()) {
    timer_set_type(TIMER_TYPE_TIMER);
    break_stop();
  } else {
    timer_set_type(TIMER_TYPE_BREAK);
    break_start();
  }
}

void breaks_save(void) {
  BreakData data = (BreakData) {
    .numBreaks = numBreaks,
  };
  memcpy(data.breakList, breakList, sizeof(breakList));
  persist_write_data(PERSIST_BREAKS, &data, sizeof(BreakData));
}

void breaks_load(void) {
  int i;
  BreakData data;
  if (persist_read_data(PERSIST_BREAKS, &data, sizeof(BreakData)) == E_DOES_NOT_EXIST) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Breaks did not exist.\n");
  } else {
    for (i = 0; i < data.numBreaks; i++) {
      break_store(data.breakList[i]);
    }
    numBreaks = data.numBreaks;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading the breaks.\n");
  }
}

BreakString break_string_get(int index) {
  BreakString bs;
  strcpy(bs.title, breakTitle[index]);
  strcpy(bs.subtitle, breakSubtitle[index]);
  return bs;
}

int num_breaks_get(void) {
  return numBreaks;
}