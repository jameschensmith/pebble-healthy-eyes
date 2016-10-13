/*
 * Healthy Eyes
 * Pebble pomodoro timer and break tracker
 *
 * Copyright © 2016 James Smith
 */

#include <pebble.h>
#include <@smallstoneapps/bitmap-loader/bitmap-loader.h>
#include "util_break.h"
#include "util_common.h"
#include "util_timer.h"
#include "window_break.h"
#include "window_info.h"
#include "window_main.h"
#include "window_splash.h"
#include "window_wakeup.h"

// Generic function to push a window to the stack.
// Animated is set to true.
void window_launch(void *data) {
  window_stack_push((Window*)data, true);
}

// Initialize the app.
void handle_init(void) {
  bool wakeupScheduled = false;
    
  // Load utilities
  timer_load();
  breaks_load();
  bitmaps_init();

  // Create windows
  window_splash_create();
  window_main_create();
  window_break_create();
  window_info_create();
  window_wakeup_create();
  
  settings_load();
  
  // Open AppMessage connection
  app_message_register_inbox_received(handle_inbox_received);
  app_message_open(128, 128);
    
  // Check if we have already scheduled a wakeup event
  // so we can transition to the countdown window
  if (persist_exists(PERSIST_WAKEUP)) {
    Timer *timer = timer_get();
    timer->wakeupId = persist_read_int(PERSIST_WAKEUP);
    // query if event is still valid, otherwise delete
    if (wakeup_query(timer->wakeupId, NULL)) {
      wakeupScheduled = true;
    } else {
      persist_delete(PERSIST_WAKEUP);
      timer->wakeupId = -1;
    }
  }
    
  if (launch_reason() == APP_LAUNCH_WAKEUP) {
    WakeupId id = 0;
    int32_t reason = 0;

    if (wakeup_get_launch_event(&id, &reason)) {
      window_stack_push(window_main_get_window(), true);
      window_wakeup_show();
    }
  } else if (wakeupScheduled) {
    window_stack_push(window_main_get_window(), true);
  } else {
    window_stack_push(window_splash_get_window(), true);
    app_timer_register(1500, window_launch, window_main_get_window());
  }
}

// Deinitialize the app.
void handle_deinit(void) {
  // Save utilities
  timer_save();
  breaks_save();

  // Destroy the images
  bitmaps_cleanup();

  // Destroy the windows
  window_splash_destroy();
  window_main_destroy();
  window_break_destroy();
  window_info_destroy();
  window_wakeup_destroy();
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
  return 0;
}