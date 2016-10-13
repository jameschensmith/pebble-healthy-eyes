/*
 * Healthy Eyes - The Smart Pomodoro Timer for Pebble
 * Copyright (c) 2016 James Smith (jamessmithsmusic@gmail.com)
 * Licensed under the MIT license: http://opensource.org/licenses/MIT
 */

#include <pebble.h>
#include "util_break.h"
#include "util_common.h"
#include "util_timer.h"
#include "window_main.h"

typedef struct TimerState {
  TimerType   type;
  TimerStatus status;
  uint32_t    startTime;
  uint32_t    pauseTime;
  uint32_t    elapsedTime;
  WakeupId    wakeupId;
} __attribute__((__packed__)) TimerState;

static Timer *timer = NULL;
AppTimer *updateTimer = NULL;

Timer *timer_create(void) {
  timer = malloc(sizeof(Timer));
  timer->type      = TIMER_TYPE_TIMER;
  timer->status    = TIMER_STATUS_STOP;
  timer->startTime = timer->pauseTime = timer->elapsedTime = 0;
  timer->wakeupId  = -1;

  return timer;
}

void timer_destroy(void) {
  free(timer);
}

void timer_cancel_wakeup(void) {
  if (timer == NULL) {
    return;
  }
  if (timer->wakeupId <= 0) {
    return;
  }
  wakeup_cancel(timer->wakeupId);
  timer->wakeupId = -1;
  persist_delete(PERSIST_WAKEUP);
}

void timer_schedule_wakeup(void) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Scheduling wakeup.");
  if (timer == NULL) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Nothing to schedule.");
    return;
  }
  timer_cancel_wakeup();

  time_t wakeupTime;
  switch (timer->type) {
    case TIMER_TYPE_TIMER:
      wakeupTime = time(NULL) + (settings_get().TimerTime * SECONDS_PER_MINUTE);
      break;
    case TIMER_TYPE_BREAK:
      wakeupTime = time(NULL) + (settings_get().BreakTime * SECONDS_PER_MINUTE);
      break;
    case TIMER_TYPE_SNOOZE:
      wakeupTime = time(NULL) + (settings_get().SnoozeTime * SECONDS_PER_MINUTE);
      break;
    default:
      wakeupTime = time(NULL) + (settings_get().TimerTime * SECONDS_PER_MINUTE);
      break;
  }

  const int cookie = 0;
  const bool notifyIfMissed = true;

  timer->wakeupId = wakeup_schedule(wakeupTime, cookie, notifyIfMissed);

  if(timer->wakeupId >= 0) {
    persist_write_int(PERSIST_WAKEUP, timer->wakeupId);
  }
}

void timer_start(void) {
  if (timer == NULL) {
    timer = timer_create();
  }

  timer->status = TIMER_STATUS_START;

  if (timer->startTime == 0) {
    // Start a new timer.
    timer->startTime = time(NULL);
  } else if (timer->pauseTime != 0) {
    // Resume a timer.
    uint32_t interval = time(NULL) - timer->pauseTime;
    timer->startTime += interval;
  }

  updateTimer = app_timer_register(1000, timer_tick_handler, NULL);
  timer_schedule_wakeup();
  timer_mark_updated();
}

void timer_pause(void) {
  if (timer == NULL) {
    timer = timer_create();
  }

  timer->status    = TIMER_STATUS_PAUSE;
  timer->pauseTime = time(NULL);

  if (updateTimer != NULL) {
    app_timer_cancel(updateTimer);
    updateTimer = NULL;
  }

  timer_cancel_wakeup();
  timer_mark_updated();
}

void timer_stop(void) {
  if (timer == NULL) {
    timer = timer_create();
  }
  timer->type        = TIMER_TYPE_TIMER;
  timer->status      = TIMER_STATUS_STOP;
  timer->startTime   = 0;
  timer->pauseTime   = 0;
  timer->elapsedTime = 0;
  if (updateTimer != NULL) {
    app_timer_cancel(updateTimer);
    updateTimer = NULL;
  }
  break_clear();
  timer_cancel_wakeup();
  timer_mark_updated();
}

void timer_save(void) {
  TimerState state = (TimerState) {
    .type        = timer->type,
    .status      = timer->status,
    .startTime   = timer->startTime,
    .pauseTime   = timer->pauseTime,
    .elapsedTime = timer->elapsedTime,
    .wakeupId    = timer->wakeupId,
  };
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Saving the timer. Elapsed time: %d\n", (int)state.elapsedTime);
  // Write the data structure
  persist_write_data(PERSIST_TIMER, &state, sizeof(TimerState));
  // Free the dynamically allocated memory
  timer_destroy();
}

void timer_load(void) {
  if (timer == NULL) {
    timer = timer_create();
  }
  // Read the data structure
  TimerState state;
  if (persist_read_data(PERSIST_TIMER, &state, sizeof(TimerState)) == E_DOES_NOT_EXIST) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Timer did not exist\n");
  } else {
    timer->type        = state.type;
    timer->status      = state.status;
    timer->startTime   = state.startTime;
    timer->pauseTime   = state.pauseTime;
    timer->elapsedTime = state.elapsedTime;
    timer->wakeupId    = state.wakeupId;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Loading the timer. Elapsed time: %d\n", (int)timer->elapsedTime);
    if (timer->status == TIMER_STATUS_START) {
      updateTimer = app_timer_register(1000, timer_tick_handler, NULL);
    }
  }
}

void timer_tick_handler(void *data) {
  if (timer == NULL) {
    return;
  }

  if (timer->status == TIMER_STATUS_START) {
    uint32_t now = time(NULL);
    timer->elapsedTime = now - timer->startTime;
    updateTimer = app_timer_register(1000, timer_tick_handler, NULL);
    APP_LOG(APP_LOG_LEVEL_INFO, "Time is now %d", (int)timer->elapsedTime);
  }

  // This will be set by wakeup_query()
  time_t wakeupTimestamp = 0;

  // Is the wakeup still scheduled?
  if(wakeup_query(timer->wakeupId, &wakeupTimestamp)) {
    // Get the time remaining
    int seconds_remaining = wakeupTimestamp - time(NULL);
    APP_LOG(APP_LOG_LEVEL_INFO, "%d seconds until wakeup", seconds_remaining);
  }

  timer_mark_updated();
}

void timer_mark_updated(void) {
  window_main_update();
}

void timer_set_type(TimerType type) {
  switch (type) {
    case TIMER_TYPE_TIMER:
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Switching timer to timer mode.\n");
      break;
    case TIMER_TYPE_BREAK:
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Switching timer to break mode.\n");
      break;
    case TIMER_TYPE_SNOOZE:
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Switching timer to snooze mode.\n");
      break;
  }
  timer->type = type;
}

Timer *timer_get(void) {
  return timer;
}