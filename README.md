# intergasHrE-esphome-reader

This is an intergas HRE gateway for ESPHome, which allows you to see debug data from the internal debug interface on the main PCB. It's based on this project:

https://github.com/So871/intergasHrE-esphome-reader/

This version works on more recent versions of ESPHome Device Builder

Add the following to your esphome config:

```
external_components:
  - source: github://jjwbruijn/intergasHrE-esphome-reader-ng

uart:
  - id: uart_2
    rx_pin: GPIO33
    tx_pin: GPIO32
    baud_rate: 9600

sensor:
  - platform: intergas
    uart_id: uart_2
```

Be sure to change the GPIO's to the relevant pins on your board. 

<img width="759" height="1177" alt="{EFCD398E-C68B-49F8-88B3-C6FFACF39112}" src="https://github.com/user-attachments/assets/32acbcef-8f0c-4ee8-81d7-aa1ff6f64585" />
<img width="491" height="1813" alt="{1B011FB8-F7FE-425A-BAAD-030C955ED96F}" src="https://github.com/user-attachments/assets/9b9cc3f1-6f1d-44fa-9669-fdf435b9953c" />

