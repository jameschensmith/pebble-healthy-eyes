/*
 * Healthy Eyes - The Smart Pomodoro Timer for Pebble
 * Copyright (c) 2016 James Smith (jamessmithsmusic@gmail.com)
 * Licensed under the MIT license: http://opensource.org/licenses/MIT
 */

#pragma once

void    window_wakeup_show(void);
void    window_wakeup_create(void);
void    window_wakeup_destroy(void);
Window *window_wakeup_get_window(void);
void    window_wakeup_update(void);