
#include "excalibur.h"

//----------------------------------------
//         Name: nr_pio_showhex
//  Description: Display lowest 2 hex digits on seven segment display.
//        Input: value
//       Output: none
//
// This is a "PIO" routine, but is only
// meaningful on a pio connected to a
// dual seven segment display, using 8 bits
// for each digit.
//
// More? the display must be called
// na_seven_seg_pio.
//
//----------------------------------------

void nr_pio_showhex(int value)
{
#ifdef na_seven_seg_pio
  static unsigned char _hex_digits_data[] = {
    0x01, 0x4f, 0x12, 0x06, 0x4c, 0x24, 0x20, 0x0f, 0x00, 0x04, // 0-9
    0x08, 0x60, 0x72, 0x42, 0x30, 0x38                      // a-f
  };
  int led_value;

  // Left Hand Digit, goes to PIO bits 8-14
  led_value = _hex_digits_data[value & 0xF];
  led_value |= (_hex_digits_data[(value >> 4) & 0xF]) << 8;

  na_seven_seg_pio->np_piodata = led_value;
#endif // na_seven_seg_pio
}
