# RP2040 F1 keyboard

Native USB keyboard firmware for an RP2040 board (Pico SDK + TinyUSB).
Presses F1 about 2 seconds after the host enumerates it, then every 5 minutes.

It is a real boot-protocol HID keyboard (one interface, no report ID), so it
works in BIOS/UEFI as well as in the OS. The schedule restarts on every
enumeration, which covers the BIOS to OS handoff.

## Install
1. Download `f1_keyboard.uf2` from the Releases page.
2. Hold BOOT while plugging the board in, then release. An `RPI-RP2` drive appears.
3. Drag `f1_keyboard.uf2` onto it. The board reboots and starts working.

## Notes
- VID/PID are the TinyUSB example values (0xCafe:0x4010). Fine for personal use.
- Built for the generic `pico` board definition (12 MHz crystal, generic flash).
- To change the key, edit `HID_KEY_F1` in `main.c`. To change the interval, edit `PERIOD_US`.
