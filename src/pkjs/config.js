/*
 * Healthy Eyes
 * Pebble pomodoro timer and break tracker
 *
 * Copyright © 2016 James Smith
 */

module.exports = [
  {
    "type": "heading",
    "defaultValue": "App Configuration"
  },
  {
    "type": "text",
    "defaultValue": "Here you can customize some settings to suit your needs."
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Time Settings"
      },
      {
        "type": "slider",
        "messageKey": "TimerTime",
        "defaultValue": 15,
        "label": "Timer Time",
        "description": "Adjust the length of time of the timer",
        "min": 1,
        "max": 30
      },
      {
        "type": "slider",
        "messageKey": "BreakTime",
        "defaultValue": 15,
        "label": "Break Time",
        "description": "Adjust the length of time of the break",
        "min": 1,
        "max": 30
      },
      {
        "type": "slider",
        "messageKey": "SnoozeTime",
        "defaultValue": 15,
        "label": "Snooze Time",
        "description": "Adjust the length of time of the snooze",
        "min": 1,
        "max": 30
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Shake Setting"
      },
      {
        "type": "radiogroup",
        "messageKey": "ShakeSetting",
        "defaultValue": "none",
        "options": [
          {
            "label": "Shake to Break",
            "value": "break"
          },
          {
            "label": "Shake to Snooze",
            "value": "snooze"
          },
          {
            "label": "None",
            "value": "none"
          }
        ]
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];