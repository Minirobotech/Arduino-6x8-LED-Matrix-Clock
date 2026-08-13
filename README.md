# Arduino-6x8-LED-Matrix-Clock

This project is an Arduino-based digital clock featuring six 8×8 LED matrix displays, a DS3231 RTC and a PCA9548/TCA9548A I²C multiplexer. The project includes the main clock program and a utility for synchronizing the RTC with the PC time.

## Hardware

- Arduino
- 6 × 8x8 LED matrix displays
- DS3231 Real-Time Clock (RTC)
- PCA9548 / TCA9548A I²C multiplexer

## Software

The project includes two Arduino sketches.

### Main Clock Program

`Arduino_Clock.ino`

This is the main program for the digital clock. Upload this sketch to Arduino to run the clock.

### RTC Synchronization Program

`RTC_Sync_PC.ino`

This program is used to synchronize the DS3231 RTC with the computer's system time.

After synchronizing the RTC, upload the main clock program to Arduino.

## How to Use

1. Connect the hardware according to the wiring diagram.
2. Upload `Orologio_matrix_8_8_rtc_program.ino` to Arduino.
3. Synchronize the DS3231 with the PC time.
4. Upload `Orologio_matrix_8_8_main_program.ino`.
5. The clock is ready to use.

## YouTube Video

Coming soon.
