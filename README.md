# Kissbar
**Kissbar** is a dead simple statusbar manager heavily inspired by dwmblocks.
This statusbar will only collect the output of your specified scripts in an chosen interval and output the complete statusbar on stdout
or set the X root window name to it.

I use it together with Kisswm as my statusbar.
That is the reason why *kissbar.h* already contains some statusbar scripts.

## Configuration
The configuration is done solely inside the *kissbar.h* file.
Here you can supply the paths to your desired statusbar scripts and specify an interval for each one.
Each entry (script) is called an element.

The interval determines every nth cycle on which the script of the Element should be called to fetch its output. 
One cycle takes approximately one second.
So an interval of for example three would mean that the script gets called every third cycle which will be approximately every three seconds.

The delimiter will be inserted between each element to seperate the different statuses.

### Make configuration for Linux
Use a different make configuration if compiled on linux:
> make linux

## Installation
> make

> doas make install

## Thanks
This was a minimal project to get used to C again. Thanks to the creator and contributors of dwmblocks for the inspiration.
