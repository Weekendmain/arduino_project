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

#include "Cache.h"

namespace mDNSResolver
{
    Cache::Cache()
    {
        this->size = 0;
    }

    Cache::~Cache()
    {
    }

    Response &Cache::operator[](const int index)
    {
        return this->list[index];
    }

    void Cache::insert(Response response)
    {
        int existing = search(response);
        if (existing != -1)
        {
            this->list[existing] = response;
            return;
        }

        if (this->size == MDNS_RESOLVER_MAX_CACHE)
        {
            int i = oldest();
            remove(i);
        }

        this->list[this->size] = response;
        this->size += 1;
    }

    void Cache::remove(Response &response)
    {
        int i = search(response);
        if (i != -1)
        {
            remove(i);
        }
    }

    int Cache::search(Response &response)
    {
        return search(response.name);
    }

    int Cache::search(const char *name)
    {
        for (int i = 0; i < this->size; i++)
        {
            if (strcmp(this->list[i].name, name) == 0)
            {
                return i;
            }
        }
        return -1;
    }

    void Cache::expire()
    {
        unsigned long ttls[MDNS_RESOLVER_MAX_CACHE];

        for (int i = 0; i < this->size; i++)
        {
            this->list[i].decrementTTL();
            ttls[i] = this->list[i].ttl;
        }

        for (int i = 0; i < this->size; i++)
        {
            if (ttls[i] == 0)
            {
                remove(i);
            }
        }
    }

    int Cache::length()
    {
        return this->size;
    }

    int Cache::oldest()
    {
        if (this->size == 0)
        {
            return -1;
        }

        long oldestTTL = this->list[0].ttl;
        unsigned int oldestIndex = 0;

        for (int i = 1; i < this->size; i++)
        {
            if (this->list[i].ttl < oldestTTL)
            {
                oldestTTL = this->list[i].ttl;
                oldestIndex = i;
            }
        }

        return oldestIndex;
    }

    void Cache::removeCname(int index)
    {
        Response *response = &this->list[index];

        for (int i = 0; i < length(); i++)
        {
            if (this->list[i].cname == response)
            {
                this->list[i].cname = NULL;
            }
        }
    }

    void Cache::remove(int index)
    {
        removeCname(index);
        this->list[index] = this->list[this->size - 1];
        this->size -= 1;
    }
}
