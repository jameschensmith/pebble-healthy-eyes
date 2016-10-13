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
#include "window_main.h"
#include "window_wakeup.h"

Window         *windowWakeup;
TextLayer      *wakeupMessage;
StatusBarLayer *statusBarLayerWakeup;
ActionBarLayer *actionBarLayerWakeup;
GBitmap        *iconDown;
GBitmap        *iconSelect;

void window_wakeup_show(void) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "WINDOW PUSH: Wakeup Window.");
  window_stack_push(windowWakeup, true);
  light_enable_interaction();
  vibes_double_pulse();
}

void snooze_handler(void) {
  timer_set_type(TIMER_TYPE_SNOOZE);
  timer_schedule_wakeup();
  window_stack_pop(true);
}

void break_handler(void) {
  break_toggle();
  timer_schedule_wakeup();
  window_stack_pop(true);
  window_main_update();
}

void handle_snooze(ClickRecognizerRef recognizer, void *context) {
  snooze_handler();
}

void handle_start(ClickRecognizerRef recognizer, void *context) {
  break_handler();
}

void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_BACK, (ClickHandler) handle_snooze);
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) handle_start);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) handle_snooze);
}

void window_wakeup_load(Window *window) {
  // Get the bounds of the Window
  Layer *layerWindow = window_get_root_layer(window);
  GRect windowBounds = layer_get_bounds(layerWindow);

  // Load icon bitmap
  iconDown = bitmaps_get_bitmap(RESOURCE_ID_ICON_SNOOZE);
  iconSelect = bitmaps_get_bitmap(RESOURCE_ID_ICON_START);

  // Create StatusBarLayer element
  statusBarLayerWakeup = status_bar_layer_create();
  status_bar_layer_set_colors(statusBarLayerWakeup, PBL_IF_COLOR_ELSE(GColorCobaltBlue, GColorBlack), GColorWhite);
  status_bar_layer_set_separator_mode(statusBarLayerWakeup, StatusBarLayerSeparatorModeDotted);

  // Create ActionBarLayer element
  actionBarLayerWakeup = action_bar_layer_create();
  action_bar_layer_set_click_config_provider(actionBarLayerWakeup, click_config_provider);
  action_bar_layer_set_background_color(actionBarLayerWakeup, PBL_IF_COLOR_ELSE(GColorCobaltBlue, GColorBlack));

  // Set the icons
  action_bar_layer_set_icon(actionBarLayerWakeup, BUTTON_ID_DOWN, iconDown);
  action_bar_layer_set_icon(actionBarLayerWakeup, BUTTON_ID_SELECT, iconSelect);

  // Set the text layer
  wakeupMessage = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(90, 84) - 20, windowBounds.size.w - ACTION_BAR_WIDTH, 80));
  text_layer_set_background_color(wakeupMessage, GColorClear);
  text_layer_set_text(wakeupMessage, "Message");
  text_layer_set_text_color(wakeupMessage, GColorBlack);
  text_layer_set_text_alignment(wakeupMessage, GTextAlignmentCenter);
  text_layer_set_font(wakeupMessage, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));

  // Add to Window
  action_bar_layer_add_to_window(actionBarLayerWakeup, window);
  layer_add_child(layerWindow, status_bar_layer_get_layer(statusBarLayerWakeup));
  layer_add_child(layerWindow, text_layer_get_layer(wakeupMessage));
}

void window_wakeup_unload(Window *window) {
  status_bar_layer_destroy(statusBarLayerWakeup);
  action_bar_layer_remove_from_window(actionBarLayerWakeup);
  action_bar_layer_destroy(actionBarLayerWakeup);
  text_layer_destroy(wakeupMessage);
}

void accel_tap_handler(AccelAxisType axis, int32_t direction) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "A tap event occured.\nShakeSetting: %s", settings_get().ShakeSetting);

  // A tap event occured
  if (strcmp(settings_get().ShakeSetting, "break") == 0) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Shake to Break.");
    break_handler();
  } else if (strcmp(settings_get().ShakeSetting, "snooze") == 0) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Shake to Snooze.");
    snooze_handler();
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "No action needed to take.");
  }
}

void window_wakeup_appear(Window *window) {
  if (break_isStarted()) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Switching text to \"End of Break!\"\n");
    text_layer_set_text(wakeupMessage, "End of Break!");
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Switching text to \"Break Time!\"\n");
    text_layer_set_text(wakeupMessage, "Break Time!");
  }

  // Subscribe to tap events
  accel_tap_service_subscribe(accel_tap_handler);
}

void window_wakeup_disappear(Window *window) {
  // Unsubscribe from tap events
  accel_tap_service_unsubscribe();
}

void wakeup_handler(WakeupId id, int32_t reason) {
  persist_delete(PERSIST_WAKEUP);
  window_wakeup_show();
}

void window_wakeup_create(void) {
  // Create Window element and assign to pointer
  windowWakeup = window_create();

  // Set the background color
  window_set_background_color(windowWakeup, PBL_IF_COLOR_ELSE(GColorPictonBlue, GColorWhite));

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(windowWakeup, (WindowHandlers) {
    .load   = window_wakeup_load,
    .unload = window_wakeup_unload,
    .appear = window_wakeup_appear,
    .disappear = window_wakeup_disappear,
  });
  wakeup_service_subscribe(wakeup_handler);
}

void window_wakeup_destroy(void) {
  window_destroy(windowWakeup);
}

Window *window_wakeup_get_window(void) {
  return windowWakeup;
}