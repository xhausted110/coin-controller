#!/bin/bash
tty=/dev/ttyUSB0
exec 4<$tty 5>$tty #Gotta be honest, I copied this from somewhere. It seems to redirect stuff sent to &5 to the tty, and stuff received to the tty to &4
stty -F $tty 1200 cs8 -echo -brkint -imaxbel -ixon # set up the serial port. 1200 baud, 8 bit character size, no echo, disable breaks causing an interrupt signal, flush full impact buffer on a character, disable xon/xoff flow control.
echo 1 >&5 #echo sends a 1 followed by a carriage return to the serial port
read reply <&4 #read the response into the reply variable and stop listening when we get a newline. These are the echoed characters.
read -d '?' reply <&4 # read the response into the reply variable, overwriting the above since we don't care about that. stop reading when we get a ?
echo -n $reply #echo back what we received.
# the resulting output of this command is a 1 or 0 with nothing after it, not even non-printing characters.
