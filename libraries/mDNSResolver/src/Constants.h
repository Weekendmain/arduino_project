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
#ifndef _MDNS_RESOLVER_CONSTANTS_H
#define _MDNS_RESOLVER_CONSTANTS_H

#ifndef MDNS_RESOLVER_MAX_CACHE
#define MDNS_RESOLVER_MAX_CACHE 4
#endif

#define MDNS_MAX_NAME_LEN 255

#define E_MDNS_OK 0x20
#define E_MDNS_TOO_BIG 0x21
#define E_MDNS_POINTER_OVERFLOW 0x22
#define E_MDNS_PACKET_ERROR 0x23
#define E_MDNS_PARSING_ERROR 0x24
#define E_MDNS_TRUNCATED 0x25
#define E_MDNS_INVALID_LABEL_LENGTH 0x26
#define E_MDNS_OUT_OF_MEMORY 0x27

#define MDNS_A_RECORD 0x01
#define MDNS_CNAME_RECORD 0x05

#define MDNS_RESULT unsigned int

#endif // _MDNS_RESOLVER_CONSTANTS_H
