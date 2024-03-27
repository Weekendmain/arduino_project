/* Copyright (C) 2021 Lee C. Bussy (@LBussy)

This file is part of Lee Bussy's mDNSResolver.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#pragma once
#ifndef MDNS_RESOLVER_H
#define MDNS_RESOLVER_H

// Auto-generated during developer testing
#define PIO_SRC_NAM "mDNSResolver"
#define PIO_SRC_TAG "1.0.0"
#define PIO_SRC_BRH "main"
#define PIO_SRC_REV "7fc6297"

#include <IPAddress.h>
#include <WiFiUdp.h>
#include "Cache.h"
#include "Query.h"
#include "Answer.h"
#include "Arduino.h"

#ifdef TEST
#include <stdlib.h>
#endif

#define MDNS_BROADCAST_IP IPAddress(224, 0, 0, 251)
#define MDNS_PORT 5353

#ifndef MDNS_RETRY
#define MDNS_RETRY 1000
#endif

#ifndef MDNS_ATTEMPTS
#define MDNS_ATTEMPTS 5
#endif

#ifndef UDP_TIMEOUT
#define UDP_TIMEOUT 255
#endif

#ifndef MDNS_TLD
#define MDNS_TLD ".local"
#endif

namespace mDNSResolver
{
    class Resolver
    {
    public:
        Resolver(WiFiUDP &udp);
        Resolver(WiFiUDP &udp, IPAddress localIP);
        ~Resolver();
        void setLocalIP(IPAddress localIP);
        IPAddress search(const char *name);
        MDNS_RESULT lastResult;
        void loop();
        const char * project();
        const char * version();
        const char * branch();
        const char * build();

    private:
        WiFiUDP udp;
        IPAddress localIP;
        MDNS_RESULT read();
        bool init;
        long timeout;
        void query(const char *name);
        bool isMDNSName(const char *name);
    };
};

#endif // MDNS_RESOLVER_H
