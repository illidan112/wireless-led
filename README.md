Wireless LED
==========================
It's project of party light. This project must to connect all music in your smartphone with your WS2812 led-strip without cables.
Yeap, without cables.
WITHOUT.CABLES.

##  Model
You install the application on your smartphone and connect to the LED strip controller via your home Wi-Fi network.
When the connection is established, you turn on the music on your smartphone and the data for displaying lighting effects on the led strip is transmitted through your home Wi-Fi router to the LED strip controller.
ESP32 - led strip controller
WS2812 - led strip

## What is done

* Firmware the LED strip controller using ESP-IDF framework.
* A Windows application written in c# that connects to the controller via Wi-Fi and streaming data about the music that is playing on the PC.