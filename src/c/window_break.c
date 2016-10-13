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
#include "window_break.h"

#define NUM_MENU_SECTIONS 2

Window         *windowBreak;
MenuLayer      *menuLayerBreak;
GBitmap        *iconClear;
StatusBarLayer *statusBarLayerBreak;

void window_break_show(void) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "WINDOW PUSH: Break Window.");
  window_stack_push(windowBreak, true);
}

uint16_t break_menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

uint16_t break_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  int numBreaks = num_breaks_get();
  switch(section_index) {
    case 0:
      return ((numBreaks == 0) ? 1:numBreaks);
    case 1:
      return 1;
    default:
      return 0;
  }
}

int16_t break_menu_get_cell_height_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  return PBL_IF_ROUND_ELSE(menu_layer_is_index_selected(menu_layer, cell_index) ?
                           MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
                           MENU_CELL_BASIC_CELL_HEIGHT);
}

int16_t break_menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

void break_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  switch(section_index) {
    case 0:
      menu_cell_basic_header_draw(ctx, cell_layer, "Breaks");
      break;
    case 1:
      menu_cell_basic_header_draw(ctx, cell_layer, "Other");
      break;
  }
}

void break_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  iconClear = bitmaps_get_bitmap(RESOURCE_ID_ICON_CLEAR);
  int numBreaks = num_breaks_get();
  switch(cell_index->section) {
    case 0:
      if (numBreaks == 0) {
        menu_cell_basic_draw(ctx, cell_layer, "No Breaks Yet.", NULL, NULL);
      } else {
        BreakString bs = break_string_get(cell_index->row);
        menu_cell_basic_draw(ctx, cell_layer, bs.title, bs.subtitle, NULL);
      }
      break;
    case 1:
      menu_cell_basic_draw(ctx, cell_layer, "Clear Data", NULL, iconClear);
      break;
  }
}

void break_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void* data) {
  switch(cell_index->section) {
    case 1:
      break_clear_stored();
      menu_layer_reload_data(menu_layer);
      break;
  }
}

void setup_break_menu_layer(Window* window) {
  // Get the bounds of the Window
  Layer *layerWindow = window_get_root_layer(window);
  GRect windowBounds = layer_get_bounds(layerWindow);

  // Create MenuLayer element and assign to pointer
  menuLayerBreak = menu_layer_create(GRect(0, STATUS_BAR_LAYER_HEIGHT, windowBounds.size.w, windowBounds.size.h - STATUS_BAR_LAYER_HEIGHT));

  // Set the normal colors for the MenuLayer
  menu_layer_set_normal_colors(menuLayerBreak, PBL_IF_COLOR_ELSE(GColorPictonBlue, GColorWhite), GColorBlack);

  // Set the highlight colors for the MenuLayer
  menu_layer_set_highlight_colors(menuLayerBreak, PBL_IF_COLOR_ELSE(GColorCobaltBlue, GColorBlack), GColorWhite);

  // Set handlers to manage the elements inside the MenuLayer
  menu_layer_set_callbacks(menuLayerBreak, NULL, (MenuLayerCallbacks) {
    .get_num_sections  = break_menu_get_num_sections_callback,
    .get_num_rows      = break_menu_get_num_rows_callback,
    .get_cell_height   = break_menu_get_cell_height_callback,
    #ifdef PBL_RECT
    .get_header_height = break_menu_get_header_height_callback,
    .draw_header       = break_menu_draw_header_callback,
    #endif
    .draw_row          = break_menu_draw_row_callback,
    .select_click      = break_menu_select_callback,
  });

  // Create StatusBarLayer element and assign to pointer
  statusBarLayerBreak = status_bar_layer_create();

  // Set the colors for the StatusBarLayer
  status_bar_layer_set_colors(statusBarLayerBreak, PBL_IF_COLOR_ELSE(GColorCobaltBlue, GColorBlack), GColorWhite);
  
  // Sets the mode of the StatusBarLayer separator
  status_bar_layer_set_separator_mode(statusBarLayerBreak, StatusBarLayerSeparatorModeDotted);

  menu_layer_set_click_config_onto_window(menuLayerBreak, window);

  layer_add_child(layerWindow, menu_layer_get_layer(menuLayerBreak));
  layer_add_child(layerWindow, status_bar_layer_get_layer(statusBarLayerBreak));
}

void window_break_load(Window *window) {
  setup_break_menu_layer(window);
}

void window_break_unload(Window *window) {
  menu_layer_destroy(menuLayerBreak);
  status_bar_layer_destroy(statusBarLayerBreak);
}

void window_break_create(void) {
  // Create Window element and assign to pointer
  windowBreak = window_create();

  // Set the background color
  window_set_background_color(windowBreak, PBL_IF_COLOR_ELSE(GColorPictonBlue, GColorClear));

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(windowBreak, (WindowHandlers){
    .load   = window_break_load,
    .unload = window_break_unload
  });
}

void window_break_destroy(void) {
  window_destroy(windowBreak);
}

Window *window_break_get_window(void) {
  return windowBreak;
}