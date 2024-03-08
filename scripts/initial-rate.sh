#!/bin/bash
tty=/dev/ttyS0
exec 4<$tty 5>$tty
stty -F $tty 1200 cs8 -echo -brkint -imaxbel -ixon
#echo >&5
echo 1 >&5
read reply <&4
read -d '?' reply <&4
#reply=$(echo $reply|tr -d '\n')
echo -n $reply
