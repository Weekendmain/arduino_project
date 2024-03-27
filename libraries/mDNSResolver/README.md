# mDNS Resolver

[![Arduino Library](https://www.ardu-badge.com/badge/mDNSResolver.svg?style=plastic)](https://www.ardu-badge.com/mDNSResolver)
[![GitHub Release](https://img.shields.io/github/v/release/lbussy/mDNSResolver.svg?style=plastic)](https://github.com/lbussy/mDNSResolver/releases)
[![MIT License](https://img.shields.io/badge/license-MIT-brightgreen?style=plastic)](https://github.com/lbussy/mDNSResolver/blob/main/LICENSE.txt)
[![GitHub Issues](https://img.shields.io/github/issues/lbussy/mDNSResolver?style=plastic)](http://github.com/lbussy/mDNSResolver/issues)
[![GitHub Pull Requests](https://img.shields.io/github/issues-pr/lbussy/mDNSResolver?style=plastic)](http://github.com/lbussy/mDNSResolver/pulls)
[![Contributors Welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=plastic)](#Contributing)

A simple library that resolves mDNS (Bonjour) names for the ESP8266.

# Compatibility

* Hardware: ESP8266.  The ESP32 libraries have mDNS support.
* Software: Tested against avahi and avahi-alias which adds CNAMEs. Post a bug report if it doesn't work with a server that you are using.

# Why?

Often, IOT devices need to connect to other IOT devices.  This can be made simple using mDNS but the folks at Espressif decided to remove or break simple name resolution for mDNS in a recent change.  I needed this for one of my other libraries which leverages mDNS lookup functionality.

This library originated from madpilot's [mDNSResolver](https://github.com/madpilot/mDNSResolver/).  That project did not seem to be active (albeit it was complete) so I forked it and updated for current core lib functionality.  All of the credit for the initial successful implementation is due to madpilot, I simply cleaned it up, removed a bunch of (probably new) compiler issues, and packaged it nicely for PlatformIO and Arduino libraries.

# Installation

## Arduino IDE

Clone this code in to your [Arduino directory](https://www.arduino.cc/en/Guide/Libraries#toc5).

``` bash
cd ~/Arduino
git clone git@github.com:lbussy/mDNSResolver.git
```

Restart your IDE. You can now include:

``` cpp
#include <mDNSResolver.h>
```

in your sketch.

## PlatformIO

Include `https://github.com/lbussy/mDNSResolver.git` in your `platformio.ini`'s `lib_deps` section, then include:

``` cpp
#include <mDNSResolver.h>
```

in your sketch.

# Usage

``` cpp
IPAddress ip = resolver.search("test.local");
```

Returns INADDR_NONE (255.255.255.255) if the name can't be found.

See the [examples folder](https://github.com/lbussy/mDNSResolver/tree/master/examples) for the full setup details

# Advanced Customization

You can change a few settings by defining some constants before including the mDNSResolver header file, but only if you really know what you are doing

``` cpp
#define MDNS_RETRY    1000      # Number of milliseconds between rebroadcasting a name query
#define MDNS_ATTEMPTS 5         # Number of times to try a resolving before giving up
#define UDP_TIMEOUT   255       # Amount of time (in milliseconds) before giving up on the UDP packet
#define MDNS_TLD      ".local"  # Top-level domain to search for. This can technically be anything, but pretty much everyone says leave it as .local

#include <mDNSResolver.h>
```

# Testing

The library has a suite of specs, which you can run on any machine that supports g++ (requires c++11 support).

The specs are written using [Catch](https://github.com/philsquared/Catch), written in BDD style.

```bash
make test
```

# Contributing

Issues and bugs can be raised on the [Issue tracker on GitHub](https://github.com/lbussy/mDNSResolver/issues)

For code and documentation fixes, clone the code, make the fix, write and run the tests, and submit a pull request.

Feature branches with lots of small commits (especially titled "oops", "fix typo", "forgot to add file", etc.) should be squashed before opening a pull request. At the same time, please refrain from putting multiple unrelated changes into a single pull request.

# License

See [LICENSE.txt](https://github.com/lbussy/mDNSResolver/tree/master/LICENSE.txt)

# Credits

This library originated from madpilot's [mDNSResolver](https://github.com/madpilot/mDNSResolver/).  
Madpilot in turn shared that much of his inspiration, and some parts of the code have been taken (or modified from) https://github.com/mrdunk/esp8266_mdns.
