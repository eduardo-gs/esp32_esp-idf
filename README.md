# ESP32 with ESP-IDF Programming using Bluetooth, I2C-bus, Wi-Fi, and OTA

This project creates communication between the ESP32 and a Bluetooth device, such as an Android smartphone using RFCOMM sockets.

In order to present several functionalities of the ESP32, after the Bluetooth pairing, the user can send the following commands:
| Command | Description | Response |
| --- | --- | --- |
| dice roll | Generates a random number from 1 to 6 corresponding to the dice face. | String: [1,6]. |
| i2c detect | Scans the I2C-bus. | String: None or Hex Address. |
| wi-fi scan | Searches Wi-Fi networks. | String: ssid list. |
| connect "ssid" "passwd" | Connects to the selected Wi-Fi network. | String: <Connected + IP/GW/SUBMASK> or <Fail = Disconnected>. |
| **TO DO:** update | Updates the firmware using OTA. | String: Success or Fail 1* or Fail 2**.<br/> <sub>*Error trying to update. **The versions are the same.</sub>|

It was used an additional library for the Bluetooth: BTSTACK. (https://github.com/bluekitchen/btstack)

