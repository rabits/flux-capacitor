Flux Capacitor
==============

Compact version of flux capacitor from Back to the Future trilogy for your Pebble Time.

![Look at that](https://github.com/rabits/flux-capacitor/raw/master/watchface.gif)

About
-----
October 21, 2015 already behind, but portable Flux Capacitor will help you not to forget about that!

This working version of time machine can move you into the future.

* Used great model of [DeLorean by Chris Kuhn](http://www.blendswap.com/blends/view/68371)
* Font: SFDigitalReadout

Features
--------
* Shows present time & destination time
* Looks nice!))
* Flux animation
* Alarm on destination time reached (slow time machine)
* Show the battery current charge / charge state
* Instant traveling to the destination time on shake
* Working PM indicators
* Settings: animation every second/minute/off, destination time, time traveling

TODO
----
* Support int64_t to travel further in time

Support
-------
If you like Back to the Future, you can support my open-source development by a small Bitcoin donation.

Bitcoin wallet: `15phQNwkVs3fXxvxzBkhuhXA2xoKikPfUy`

Build
-----
You need to clone this git repository recursively:

```sh
git clone --recursive https://github.com/rabits/flux-capacitor.git
```

Install gimp 2.8 and blender >= 2.75.

Download & unpack pebble sdk and toolchain into ~/local/pebble (follow instructions on the pebble sdk installation page).

Run ./build.sh
