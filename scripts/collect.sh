#!/bin/bash
tty=/dev/ttyS0
exec 4<$tty 5>$tty
stty -F $tty 1200 -echo
echo 2 >&5
#read reply <&4
#read -d '?' reply <&4
#echo $reply
