# astcc
Asterisk coin controller AMI interface

### Note: This is *not* a coin controller, it is an interface between Asterisk and your coin controller!

# Compiling

You will need to statically compile this program with C-AMI, which you can find at https://github.com/InterLinked1/cami

You will need `cami.c` and the `include` directory from the CAMI repository (but not `simpleami.c`)

Then you can compile using `make`.

# Operation

This is *not* a complete interface program! It is a boilerplate template that you can extend to work with your particular coin controller.

This is designed to interface between the `res_coindetect` Asterisk module and your coin controller, which could be using serial, TCP, or just about anything: you have the complete flexibility to do this however you like.

If you don't have it already, you also probably want the boilerplate `phreaknet-coin.conf` from the boilerplate config repo: https://github.com/InterLinked1/phreaknet-boilerplate

You will need to do this part first in order to make the interface actually operational, as this program does nothing useful by default, out of the box.

# Running

Run `./astcc -h` to see available options.

If you are running this program under a user with read access to `/etc/asterisk/manager.conf`, you don't need to specify the AMI password on the command line.
