/*
 * Healthy Eyes
 * Pebble pomodoro timer and break tracker
 *
 * Copyright © 2016 James Smith
 */

#pragma once

void    window_wakeup_show(void);
void    window_wakeup_create(void);
void    window_wakeup_destroy(void);
Window *window_wakeup_get_window(void);
void    window_wakeup_update(void);