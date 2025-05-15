4_13 was the first attempt to fix the problem with the zero position.

These are attempts to fix the problem with the zero position and i/o values in the first time-based point.
4_13t is an experiment as follows:

"I globally disable interrupts at the end of the time based sample when checking to see if a position based interrupt has occurred and updating the data pointer.
This will reduce the max speed we can monitor at slightly, but it will be interesting to see if you get better results."

4_13_low_impact fixes problems with the low impact command.

4_13_monitor_only is a monitor only version but I don't know whether it has the 4_13t experiment or not.

4_14 is the release version. It still zeroes the position of the first time-based points sometimes but it is much better than 4_11. It also contains the low impact fixes.