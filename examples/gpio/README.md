# GPIO Example

## Build and Flash firmware

Follow the ESP RainMaker Documentation [Get Started](https://rainmaker.espressif.com/docs/get-started.html) section to build and flash this firmware. Just note the path of this example.

## What to expect in this example?

- This example just provides 3 boolean parameters, linked to 3 GPIOS.
- Toggling the buttons on the phone app should toggle the GPIOs on your board (and the LEDs, if any, connected to the GPIOs), and also print messages like these on the ESP32-S2 monitor:

```
I (16073) app_main: Received value = true for GPIO-Device - Red
```

### Reset to Factory

Press and hold the BOOT button for more than 3 seconds to reset the board to factory defaults. You will have to provision the board again to use it.

esptool.py --chip esp32c3 merge_bin -o merged-rainmaker-c3-v4.3.4.bin @flash_args
esptool.py --chip esp32c3 merge_bin -o merged-rainmaker-c3-v4.3.4.bin @flash_args
esptool.py --chip esp32c3 merge_bin -o merged-rainmaker-c3-v5.0.b.bin @flash_args
esptool.py --chip esp32c3 merge_bin -o merged-rainmaker-c3-v4.3.p.bin @flash_args
esptool.py erase_region 0x00010000 0x6000  