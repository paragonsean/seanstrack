Back when the Dual Opto I/O board was developed, I had a lot of problems with ground bounce with 16 bit wide devices used.  The problem manifested itself when a lot of bits were changed at one time - like going from almost no bits set to most of them set in one write.  I solved it by first setting every other bit (0x5555) and then the desired setting.  Doing that avoided a large number of bits transitioning at once.

Having said that, there's still the possibility that every once in a while, a write to the Opto board or Mux adapter may not correctly set or clear a bit.  I have not seen this happen but I suppose it could under the right circumstances.   Setting alarms doesn't cause a lot of bits to change at once so that doesn't quite seem to fit, but it's possible.

I noticed that when the alarms are written, they are only cleared if there is a bit set.  If a bit is set from a previous alarm, it will clear on the next shot if there is not a new alarm.  Once cleared, it doesn't appear that the alarms are actively cleared again unless a bit sets from another alarm.

If that's indeed the case, perhaps you could clear all of the alarms at CS, every shot.   By doing that, if a bit does not clear, it will affect only one shot and be corrected on the next.

I've changed my firmware to write the Opto outputs twice each write.  If a bit sticks, hitting it a second time may set it correctly.  I'll also write / update the outputs at CS.  If somethings not set correctly, that will also give us another chance to correct it.  emc_4_30a.flash has this change as well as the new version of emc_5_0.flash.  emc_4_30a.flash must be loaded with emc_chip_09010100.hexout.flash and ad_loop10.flash.

Greg
