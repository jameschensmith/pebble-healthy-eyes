/*
 * Healthy Eyes
 * Pebble pomodoro timer and break tracker
 *
 * Copyright © 2016 James Smith
 */

#include <pebble.h>
#include <@smallstoneapps/bitmap-loader/bitmap-loader.h>
#include "window_splash.h"

Window      *windowSplash;
GBitmap     *imageSplash;
BitmapLayer *imageLayerSplash;
TextLayer   *imageTitleSplash;

void window_splash_load(Window *window) {
  // Get the bounds of the Window
  Layer *layerWindow = window_get_root_layer(window);
  GRect windowBounds = layer_get_bounds(layerWindow);

  // Retrieve the splash screen image
  imageSplash = bitmaps_get_bitmap(RESOURCE_ID_IMAGE_SPLASH);

  // Image layer
  imageLayerSplash = bitmap_layer_create(GRect(0, PBL_IF_ROUND_ELSE(20, 10), windowBounds.size.w, 100));
  bitmap_layer_set_bitmap(imageLayerSplash, imageSplash);
  bitmap_layer_set_compositing_mode(imageLayerSplash, GCompOpSet);
  layer_add_child(layerWindow, bitmap_layer_get_layer(imageLayerSplash));

  // Text Layer
  imageTitleSplash = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(100, 90), windowBounds.size.w, 50));
  text_layer_set_background_color(imageTitleSplash, GColorClear);
  text_layer_set_text(imageTitleSplash, "Healthy Eyes");
  text_layer_set_text_color(imageTitleSplash, GColorBlack);
  text_layer_set_text_alignment(imageTitleSplash, GTextAlignmentCenter);
  text_layer_set_font(imageTitleSplash, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(layerWindow, text_layer_get_layer(imageTitleSplash));
}

void window_splash_unload(Window *window) {
  bitmap_layer_destroy(imageLayerSplash);
  text_layer_destroy(imageTitleSplash);
}

void window_splash_disappear(Window *window) {
  window_stack_remove(window, true);
}

void window_splash_create(void) {
  // Create Window element and assign to pointer
  windowSplash = window_create();

  // Set the background color
  window_set_background_color(windowSplash, PBL_IF_COLOR_ELSE(GColorPictonBlue, GColorWhite));

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(windowSplash, (WindowHandlers) {
    .load      = window_splash_load,
    .unload    = window_splash_unload,
    .disappear = window_splash_disappear
  });
}

void window_splash_destroy(void) {
  // Destroy Window
  window_destroy(windowSplash);
}

Window *window_splash_get_window(void) {
  return windowSplash;
}