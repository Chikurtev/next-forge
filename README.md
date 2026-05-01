# next-forge
m5stick firmware for task board 2023

## Projects

This repository contains three Arduino-based task board firmware projects:

- `iot-task-board-m5stickcplus2/` — original M5StickC Plus 2 task board implementation.
- `iot_task_board_m5stickcplus2_new/` — newer revision for M5StickC Plus 2 with improved structure and features.
- `iot_task_board_m5sticks3/` — M5Stick S3 version of the task board firmware.

Each project includes source files for hardware abstraction, display, input handling, persistence, telemetry, task checks, and trial logic.

## Library requirements

This repository includes a `requirements.txt` file with the Arduino libraries used by the projects:

- `M5StickC-Plus`
- `M5Unified`
- `WiFiManager`
- `PubSubClient`
- `ArduinoJson`

Use `arduino-cli` to install them automatically:

```bash
arduino-cli lib install "M5StickC-Plus" "M5Unified" "WiFiManager" "PubSubClient" "ArduinoJson"
```

Core ESP32 libraries such as `Preferences`, `Wire`, `WiFi`, `HTTPClient`, and `HTTPUpdate` are provided by the board package.
