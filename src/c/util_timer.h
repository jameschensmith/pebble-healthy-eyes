/*
 * Healthy Eyes
 * Pebble pomodoro timer and break tracker
 *
 * Copyright © 2016 James Smith
 */

#pragma once

typedef enum {
  TIMER_TYPE_TIMER  = 0,
  TIMER_TYPE_BREAK  = 1,
  TIMER_TYPE_SNOOZE = 2,
} TimerType;

typedef enum {
  TIMER_STATUS_START = 0,
  TIMER_STATUS_PAUSE = 1,
  TIMER_STATUS_STOP  = 2,
} TimerStatus;

typedef struct Timer {
  TimerType   type;
  TimerStatus status;
  uint32_t    startTime;
  uint32_t    pauseTime;
  uint32_t    elapsedTime;
  WakeupId    wakeupId;
} Timer;

void   timer_schedule_wakeup(void);
void   timer_start(void);
void   timer_pause(void);
void   timer_stop(void);
void   timer_save(void);
void   timer_load(void);
void   timer_tick_handler(void *context);
void   timer_mark_updated(void);
void   timer_set_type(TimerType type);
Timer *timer_get(void);