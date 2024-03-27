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
#ifndef _MDNS_RESOLVER_ANSWER_H
#define _MDNS_RESOLVER_ANSWER_H

#include "Constants.h"
#include "Response.h"
#include "Cache.h"

#ifdef TEST
#include <string.h>
#include <stdlib.h>
#endif

namespace mDNSResolver
{
    class Answer
    {
    public:
        static MDNS_RESULT process(unsigned char *buffer, unsigned int len, Cache &cache);
#ifndef TEST
    private:
#endif
        static MDNS_RESULT resolve(unsigned char *buffer, unsigned int len, unsigned int *offset, Cache &cache);
        static int assembleName(unsigned char *buffer, unsigned int len, unsigned int *offset, char **name, unsigned int maxlen);
        static int assembleName(unsigned char *buffer, unsigned int len, unsigned int *offset, char **name);
        static MDNS_RESULT skipQuestions(unsigned char *buffer, unsigned int len, unsigned int *offset);
        static MDNS_RESULT parseName(char **name, const char *mapped, unsigned int len);
        static MDNS_RESULT resolveAName(unsigned char *buffer, unsigned int len, unsigned int *offset, Response &response, long ttl, int dataLen);
        static MDNS_RESULT resolveCnames(Cache &cache);
    };
};

#endif // _MDNS_RESOLVER_ANSWER_H
