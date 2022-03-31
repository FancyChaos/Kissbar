# Kissbar
**Kissbar** is a dead simple statusbar manager heavily(!) inspired by dwmblocks.
This statusbar will only collect the output of your specified scripts in an chosen interval and print out the complete statusbar on stdout.

I use it together with spectrwm as my statusbar. Thats why *kissbar.h* already contains some statusbar scripts.

## Configuration
The configuration is done solely inside the *kissbar.h* file. Here you can supply the paths to your desired statusbar scripts and specify an unique interval for every one.
The interval determines the wait time in seconds before a script is called again. The lowest possible value is *1* for one second.

The delimiter is a simple string which will be formatted in between the different scripts outputs.

## Installation
> make

> doas make install

## Thanks
This was a minimal project to get used to C again. Thanks to the creator and contributors of dwmblocks for the inspiration.
