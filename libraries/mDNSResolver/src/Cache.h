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
#ifndef MDNS_RESOLVER_CACHE_H
#define MDNS_RESOLVER_CACHE_H

#include "Constants.h"
#include "Response.h"

namespace mDNSResolver
{
    class Cache
    {
    public:
        Cache();
        ~Cache();
        void insert(Response response);
        void remove(Response &response);

        int search(Response &response);
        int search(const char *name);

        void expire();
        int length();
        Response &operator[](const int index);

    private:
        int oldest();
        void remove(int index);
        void removeCname(int index);

        Response list[MDNS_RESOLVER_MAX_CACHE];
        int size;
    };
};

#endif // MDNS_RESOLVER_CACHE_H
