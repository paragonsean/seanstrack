
//
// File: hello_watchdog.c
//
// Contents: Minimal test program for Nios
//           system watchdog timer
//
// Warning: this program will work properly only if
//          the watchdog timer has been implemented.
//          For the default value (1 msec), no dots
//          will be displayed. For a timeout value of
//          1 second, aproximately 42 dots will be
//          displayed.

#include "nios.h"

int main (void)
{
    unsigned long i = 0;
    unsigned int j = 0;
    np_timer *timer = na_timer1;

        //
        // This will not work without a Watchdog Timer!
        //

    printf("\n Watchdog test program: \n");
    printf("   print - o - while resetting the dog, \n");
    printf("   print - . - while NOT resetting the dog,  \n");
    printf("               expect reset on timeout. \n");

        //
        // First, start the watchdog by writing to the start bit:
        //

    timer->np_timercontrol = (1 << np_timercontrol_start_bit);

        //
        // First loop, reset the dog after printing each o
        //

    for (;;) {

        timer->np_timerperiodl = 0x1234;  /* RESET_WATCHDOG */

        if (i++ > 0x0FFFFLL) {
            printf ("o");
            i = 0;
            if (j++ > 1000) break;
        }
    }

        //
        // Next loop, expect reset while printing .
        //

    j = 0;
    for (;;) {
        if (i++ > 0x0FFFFLL) {
            printf (".");
            i = 0;
            if (j++ > 1500) {
                printf("\n-> FAILED! Watchdog should have reset the system by now !");
                break;
            }
        }
    }
}

// end of file
