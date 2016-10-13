/*
 * Healthy Eyes
 * Pebble pomodoro timer and break tracker
 *
 * Copyright © 2016 James Smith
 */

#include <pebble.h>
#include <@smallstoneapps/bitmap-loader/bitmap-loader.h>
#include "util_common.h"
#include "util_timer.h"
#include "window_break.h"
#include "window_info.h"
#include "window_main.h"

#define NUM_MENU_SECTIONS 1
#define NUM_MENU_ITEMS 3

Window         *windowMain;
MenuLayer      *menuLayerMain;
StatusBarLayer *statusBarLayerMain;

uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch(section_index) {
    case 0:
      return NUM_MENU_ITEMS;
    default:
      return 0;
  }
}

int16_t menu_get_cell_height_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  return PBL_IF_ROUND_ELSE(menu_layer_is_index_selected(menu_layer, cell_index) ?
                           MENU_CELL_ROUND_FOCUSED_TALL_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_SHORT_CELL_HEIGHT,
                           MENU_CELL_BASIC_CELL_HEIGHT);
}

void menu_draw_row_timer(Timer* timer, GContext* ctx, const Layer *cell_layer) {
  char *timeLeft = malloc(sizeof(char) * TIME_LENGTH);
  format_time(timer->elapsedTime, timeLeft);

  GBitmap *icon = NULL;

  switch (timer->status) {
    case TIMER_STATUS_START:
      icon = bitmaps_get_bitmap(RESOURCE_ID_ICON_PLAY);
      break;
    case TIMER_STATUS_PAUSE:
      icon = bitmaps_get_bitmap(RESOURCE_ID_ICON_PAUSE);
      break;
    case TIMER_STATUS_STOP:
      icon = bitmaps_get_bitmap(RESOURCE_ID_ICON_STOP);
      break;
  }

  switch (timer->type) {
    case TIMER_TYPE_TIMER:
      menu_cell_basic_draw(ctx, cell_layer, "Timer", timeLeft, icon);
      break;
    case TIMER_TYPE_BREAK:
      menu_cell_basic_draw(ctx, cell_layer, "Break", timeLeft, icon);
      break;
    case TIMER_TYPE_SNOOZE:
      menu_cell_basic_draw(ctx, cell_layer, "Snooze", timeLeft, icon);
      break;
  }

  free(timeLeft);
}

void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  switch(cell_index->row) {
    case 0:
      menu_draw_row_timer(timer_get(), ctx, cell_layer);
      break;
    case 1:
      menu_cell_basic_draw(ctx, cell_layer, "Breaks", NULL, NULL);
      break;
    case 2:
      menu_cell_basic_draw(ctx, cell_layer, "More Info", NULL, NULL);
      break;
  }
}

void timer_select(void) {
  Timer *timer = timer_get();

  if (timer == NULL) {
    return;
  }

  switch (timer->status) {
    case TIMER_STATUS_START:
      timer_pause();
      break;
    case TIMER_STATUS_PAUSE:
    case TIMER_STATUS_STOP:
      timer_start();
      break;
  }
}

void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  switch(cell_index->row) {
    case 0:
      timer_select();
      break;
    case 1:
      window_break_show();
      break;
    case 2:
      window_info_show();
      break;
  }
}

void menu_select_long_callback(struct MenuLayer* menu, MenuIndex* cell_index, void* callback_context) {
  if (cell_index->row == 0) {
    timer_stop();
  }
}

void setup_menu_layer(Window *window) {
  // Get the bounds of the Window
  Layer *layerWindow = window_get_root_layer(window);
  GRect windowBounds = layer_get_bounds(layerWindow);

  // Create MenuLayer element and assign to pointer
  menuLayerMain = menu_layer_create(GRect(0, STATUS_BAR_LAYER_HEIGHT, windowBounds.size.w, windowBounds.size.h - STATUS_BAR_LAYER_HEIGHT));

  // Set the normal colors for the MenuLayer
  menu_layer_set_normal_colors(menuLayerMain, PBL_IF_COLOR_ELSE(GColorPictonBlue, GColorWhite), GColorBlack);

  // Set the highlight colors for the MenuLayer
  menu_layer_set_highlight_colors(menuLayerMain, PBL_IF_COLOR_ELSE(GColorCobaltBlue, GColorBlack), GColorWhite);

  // Set handlers to manage the elements inside the MenuLayer
  menu_layer_set_callbacks(menuLayerMain, NULL, (MenuLayerCallbacks) {
    .get_num_sections  = menu_get_num_sections_callback,
    .get_num_rows      = menu_get_num_rows_callback,
    .get_cell_height   = menu_get_cell_height_callback,
    .draw_row          = menu_draw_row_callback,
    .select_click      = menu_select_callback,
    .select_long_click = menu_select_long_callback,
  });

  // Create StatusBarLayer element and assign to pointer
  statusBarLayerMain = status_bar_layer_create();

  // Set the colors for the StatusBarLayer
  status_bar_layer_set_colors(statusBarLayerMain, PBL_IF_COLOR_ELSE(GColorCobaltBlue, GColorBlack), GColorWhite);
  
  // Sets the mode of the StatusBarLayer separator
  status_bar_layer_set_separator_mode(statusBarLayerMain, StatusBarLayerSeparatorModeDotted);

  menu_layer_set_click_config_onto_window(menuLayerMain, window);

  layer_add_child(layerWindow, menu_layer_get_layer(menuLayerMain));
  layer_add_child(layerWindow, status_bar_layer_get_layer(statusBarLayerMain));
}

void window_main_load(Window *window) {
  setup_menu_layer(window);
}

void window_main_unload(Window *window) {
  menu_layer_destroy(menuLayerMain);
  status_bar_layer_destroy(statusBarLayerMain);
}

void window_main_create(void) {
  // Create Window element and assign to pointer
  windowMain = window_create();

  // Set the background color
  window_set_background_color(windowMain, PBL_IF_COLOR_ELSE(GColorPictonBlue, GColorWhite));

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(windowMain, (WindowHandlers) {
    .load   = window_main_load,
    .unload = window_main_unload
  });
}

void window_main_destroy(void) {
  window_destroy(windowMain);
}

Window *window_main_get_window(void) {
  return windowMain;
}

void window_main_update(void) {
  layer_mark_dirty(menu_layer_get_layer(menuLayerMain));
}