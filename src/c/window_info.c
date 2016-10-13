/*
 * Healthy Eyes
 * Pebble pomodoro timer and break tracker
 *
 * Copyright © 2016 James Smith
 */

#include <pebble.h>
#include "window_info.h"

#define INFO_LENGTH 223

Window         *windowInfo;
ScrollLayer    *scrollLayerInfo;
TextLayer      *textLayerInfo;
char           *textInfo;
StatusBarLayer *statusBarLayerInfo;

void window_info_show(void) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "WINDOW PUSH: Info Window.");
  window_stack_push(windowInfo, true);
}

void window_info_load(Window *window) {
  // Get the bounds of the Window
  Layer *layerWindow = window_get_root_layer(window);
  GRect windowBounds = layer_get_bounds(layerWindow);

  // Create StatusBarLayer element
  statusBarLayerInfo = status_bar_layer_create();
  status_bar_layer_set_colors(statusBarLayerInfo, PBL_IF_COLOR_ELSE(GColorCobaltBlue, GColorBlack), GColorWhite);
  status_bar_layer_set_separator_mode(statusBarLayerInfo, StatusBarLayerSeparatorModeDotted);

  // Initialize the ScrollLayer and bind it to the window.
  scrollLayerInfo = scroll_layer_create(GRect(0, STATUS_BAR_LAYER_HEIGHT, windowBounds.size.w, windowBounds.size.h - STATUS_BAR_LAYER_HEIGHT));
  scroll_layer_set_click_config_onto_window(scrollLayerInfo, window);
  scroll_layer_set_paging(scrollLayerInfo, true);

  textInfo = malloc(INFO_LENGTH);
  resource_load(resource_get_handle(RESOURCE_ID_TEXT_INFO), (uint8_t*)textInfo, INFO_LENGTH);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "RESOURCE LOADED: %s", textInfo);
  textLayerInfo = text_layer_create(GRect(0, 0, windowBounds.size.w, 500));
  text_layer_set_background_color(textLayerInfo, GColorClear);
  text_layer_set_text(textLayerInfo, textInfo);
  text_layer_set_text_color(textLayerInfo, GColorBlack);
  text_layer_set_font(textLayerInfo, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  
  // Add to ScrollLayer
  scroll_layer_add_child(scrollLayerInfo, text_layer_get_layer(textLayerInfo));

  // Add to parent Window
  layer_add_child(layerWindow, status_bar_layer_get_layer(statusBarLayerInfo));
  layer_add_child(layerWindow, scroll_layer_get_layer(scrollLayerInfo));

  // Enable paging and text flow with an inset of 5 pixels
  text_layer_enable_screen_text_flow_and_paging(textLayerInfo, 5);
  
  GSize textSize = text_layer_get_content_size(textLayerInfo);
  text_layer_set_size(textLayerInfo, GSize(textSize.w, 500));
  scroll_layer_set_content_size(scrollLayerInfo, GSize(windowBounds.size.w, textSize.h));
}

void window_info_unload(Window *window) {
  free(textInfo);
  text_layer_destroy(textLayerInfo);
  scroll_layer_destroy(scrollLayerInfo);
  status_bar_layer_destroy(statusBarLayerInfo);
}

void window_info_create(void) {
  // Create Window element and assign to pointer
  windowInfo = window_create();

  // Set the background color
  window_set_background_color(windowInfo, PBL_IF_COLOR_ELSE(GColorPictonBlue, GColorWhite));

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(windowInfo, (WindowHandlers) {
    .load   = window_info_load,
    .unload = window_info_unload
  });
}

void window_info_destroy(void) {
  window_destroy(windowInfo);
}

Window *window_info_get_window(void) {
  return windowInfo;
}