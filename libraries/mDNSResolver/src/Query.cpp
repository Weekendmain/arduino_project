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

#include "Query.h"

namespace mDNSResolver
{
    Query::Query(const char *name)
    {
        this->name = name;
    }

    Query::~Query() {}

    // If we pass in a UDP proxy, we can dynamically allocate the
    // memory without fee of fragmentation, and don't risk losing
    // the reference if this object disappears
    void Query::sendPacket(WiFiUDP &socket)
    {
        int bufferLength = strlen(this->name) + 18;
        unsigned char buffer[bufferLength];
        assemblePacket(buffer, bufferLength);
        socket.write(buffer, bufferLength);
    }

    void Query::assemblePacket(unsigned char *buffer, int bufferLength)
    {
        for (int i = 0; i < bufferLength; i++)
        {
            buffer[i] = 0;
        }

        buffer[5] = 0x01;

        int bufferIndex = 12;
        bufferIndex = buildDNSName(buffer, bufferIndex);

        int qtype = 0x01;
        int qclass = 0x01;

        buffer[bufferIndex++] = '\0';
        buffer[bufferIndex++] = (qtype & 0xFF00) >> 8;
        buffer[bufferIndex++] = qtype & 0xFF;
        buffer[bufferIndex++] = (qclass & 0xFF00) >> 8;
        buffer[bufferIndex++] = qclass & 0xFF;
    }

    int Query::buildDNSName(unsigned char *buffer, unsigned int bufferIndex)
    {
        int wordstart = 0, wordend = 0;

        do
        {
            if (this->name[wordend] == '.' || this->name[wordend] == '\0')
            {
                const int wordlen = wordend - wordstart;
                buffer[bufferIndex++] = (unsigned char)wordlen;

                for (int i = wordstart; i < wordend; i++)
                {
                    buffer[bufferIndex++] = this->name[i];
                }

                if (this->name[wordend] == '.')
                {
                    wordend++;
                }
                wordstart = wordend;
            }

            wordend++;
        } while (this->name[wordstart] != '\0');

        return bufferIndex;
    }
};
