# GPIO TEST

基于RainMaker测试C6开发板功能 RGB PIN:8

使用前请打补丁 test.patch

编译使用的idf版本  feature/bringup_esp32c6_chip_wifi_rebase1(5444d3d33b963ccc47e8c59620119af416160b54)

烧录证书
```
esptool.py write_flash  0x340000 xxxx.bin
```
日志记录
```
./logger -b 115200 -t -C log.csv /dev/ttyUSB0
```