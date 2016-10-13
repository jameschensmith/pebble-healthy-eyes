/*
 * Healthy Eyes - The Smart Pomodoro Timer for Pebble
 * Copyright (c) 2016 James Smith (jamessmithsmusic@gmail.com)
 * Licensed under the MIT license: http://opensource.org/licenses/MIT
 */

// Import the Clay package
var Clay = require('pebble-clay');
// Load our Clay configuration file
var clayConfig = require('./config');
// Initialize Clay
var clay = new Clay(clayConfig);