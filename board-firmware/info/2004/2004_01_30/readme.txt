I have added the commands to change the network settings without having to run
"hello_plugs."  All of the network commands begin with "N" or "n."  If you type
an NM<CR> command, a menu will be transmitted listing all of the commands.

You will notice a "setup index" parameter.  There are 4 sets of network settings
that can be flashed.  Setup index can be between 0 and 3.  The standard set of
network settings will be index 0.  That's the one we have been using.  The other
sets of settings may be useful in that we can flash a set of test network settings
for our in-house testing or for field service.

If it is desired to use one of the other setups, the index value will have to be
changed, new values entered and everything flashed.  When the board is reset, it
will use the new settings.

To go back to the main network settings, the index will have to be changed back
to zero and flashed.  When the board is reset, it will be back to the main settings.

I have also added a new "serial" mode.  If PB switch SW7 is pushed and held until
the board boots at reset, serial mode will be activated.  In this mode, most of
the commands normally sent to the board over the network can be sent to the
serial port.  You cannot monitor or control in this mode.  It is basically
just a command interpreter.  I did this primarily to allow the network
settings to set over the serial port if needed.  In the case of a new board or
if something is corrupted with the flashed settings, network settings can be reset
serially.
