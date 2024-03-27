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

#include "Response.h"

namespace mDNSResolver
{
    Response::Response()
    {
        this->name[0] = '\0';
        this->resolved = false;
        this->ttl = 0;
        this->cname = NULL;
        this->ipAddress = INADDR_NONE;
    }

    Response::Response(const char *name)
    {
        strncpy(this->name, name, MDNS_MAX_NAME_LEN + 1);
        this->name[MDNS_MAX_NAME_LEN + 1] = '\0';
        this->ttl = 0;
        this->resolved = false;
        this->cname = NULL;
        this->ipAddress = INADDR_NONE;
    }

    Response::Response(const char *name, unsigned long ttl)
    {
        strncpy(this->name, name, MDNS_MAX_NAME_LEN + 1);
        this->name[MDNS_MAX_NAME_LEN + 1] = '\0';
        this->ttl = ttl;
        this->resolved = false;
        this->cname = NULL;
        this->ipAddress = INADDR_NONE;
    }

    Response::~Response()
    {
    }

    void Response::decrementTTL()
    {
        if (this->ttl > 0)
        {
            this->ttl--;
        }
    }

    bool Response::operator==(const Response &response)
    {
        return strcmp(this->name, response.name) == 0;
    }

    bool Response::operator!=(const Response &response)
    {
        return strcmp(this->name, response.name) != 0;
    }
}
