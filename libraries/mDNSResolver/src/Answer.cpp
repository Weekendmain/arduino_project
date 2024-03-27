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

#include "Answer.h"

namespace mDNSResolver
{
    MDNS_RESULT Answer::process(unsigned char *buffer, unsigned int len, Cache &cache)
    {
        if ((buffer[2] & 0b10000000) != 0b10000000)
        {
            // Not an answer packet
            return E_MDNS_OK;
        }

        if (buffer[2] & 0b00000010)
        {
            // Truncated - we don't know what to do with these
            return E_MDNS_TRUNCATED;
        }

        if (buffer[3] & 0b00001111)
        {
            return E_MDNS_PACKET_ERROR;
        }

        unsigned int answerCount = (buffer[6] << 8) + buffer[7];

        if (answerCount == 0)
        {
            return E_MDNS_OK;
        }

        unsigned int offset = 0;

        MDNS_RESULT questionResult = skipQuestions(buffer, len, &offset);
        if (questionResult != E_MDNS_OK)
        {
            return questionResult;
        }

        MDNS_RESULT answerResult = 0;
        for (unsigned int i = 0; i < answerCount; i++)
        {
            answerResult = resolve(buffer, len, &offset, cache);
            if (answerResult != E_MDNS_OK)
            {
                return answerResult;
            }
        }

        resolveCnames(cache);

        return answerResult;
    }

    MDNS_RESULT Answer::resolveAName(unsigned char *buffer, unsigned int len, unsigned int *offset, Response &response, long ttl, int dataLen)
    {
        if (dataLen == 4)
        {
            unsigned int a = (unsigned int)*(buffer + (*offset)++);
            unsigned int b = (unsigned int)*(buffer + (*offset)++);
            unsigned int c = (unsigned int)*(buffer + (*offset)++);
            unsigned int d = (unsigned int)*(buffer + (*offset)++);

            response.resolved = true;
            response.ttl = ttl;
            response.ipAddress = IPAddress(a, b, c, d);
        }
        else
        {
            (*offset) += dataLen;
        }

        return E_MDNS_OK;
    }

    MDNS_RESULT Answer::resolve(unsigned char *buffer, unsigned int len, unsigned int *offset, Cache &cache)
    {
        char *assembled = (char *)malloc(sizeof(char) * MDNS_MAX_NAME_LEN);
        if (assembled == NULL)
        {
            return E_MDNS_OUT_OF_MEMORY;
        }
        int nameLen = Answer::assembleName(buffer, len, offset, &assembled);

        if (nameLen == -1 * E_MDNS_POINTER_OVERFLOW)
        {
            free(assembled);
            return -1 * nameLen;
        }

        char *name = (char *)malloc(sizeof(char) * nameLen);
        if (name == NULL)
        {
            free(assembled);
            return E_MDNS_OUT_OF_MEMORY;
        }
        parseName(&name, assembled, strlen(assembled));
        int cacheIndex = cache.search(name);
        free(name);

        // unsigned int type = (buffer[(*offset)++] << 8) + buffer[(*offset)++];
        unsigned int type1 = buffer[(*offset)++] << 8;
        unsigned int type2 = buffer[(*offset)++];
        unsigned int type = type1 + type2;
        // unsigned int __attribute__((unused)) aclass = (buffer[(*offset)++] << 8) + buffer[(*offset)++];
        unsigned int aclass1 = buffer[(*offset)++] << 8;
        unsigned int aclass2 = buffer[(*offset)++];
        unsigned int __attribute__((unused)) aclass = aclass1 + aclass2;
        // unsigned long ttl = (buffer[(*offset)++] << 24) + (buffer[(*offset)++] << 16) + (buffer[(*offset)++] << 8) + buffer[(*offset)++];
        unsigned long ttl1 = buffer[(*offset)++] << 24;
        unsigned long ttl2 = buffer[(*offset)++] << 16;
        unsigned long ttl3 = buffer[(*offset)++] << 8;
        unsigned long ttl4 = buffer[(*offset)++];
        unsigned long ttl = ttl1 + ttl2 + ttl3 + ttl4;
        // unsigned int dataLen = (buffer[(*offset)++] << 8) + buffer[(*offset)++];
        unsigned int dataLen1 = buffer[(*offset)++] << 8;
        unsigned int dataLen2 = buffer[(*offset)++];
        unsigned int dataLen = dataLen1 + dataLen2;

        if (type == MDNS_A_RECORD && cacheIndex != -1)
        {
            resolveAName(buffer, len, offset, cache[cacheIndex], ttl, dataLen);
        }
        else if (type == MDNS_CNAME_RECORD && cacheIndex != -1)
        {
            cache[cacheIndex].resolved = false;
            cache[cacheIndex].ttl = ttl;

            unsigned int dataOffset = (*offset);
            (*offset) += dataLen;
            dataLen = Answer::assembleName(buffer, len, &dataOffset, &assembled, dataLen);

            if (dataLen == (unsigned int)(-1 * E_MDNS_POINTER_OVERFLOW))
            {
                free(assembled);
                return -1 * dataLen;
            }

            Response r;
            char *responseName = &*r.name;
            parseName(&responseName, assembled, dataLen - 1);
            int cnameIndex = cache.search(r);

            if (cnameIndex == -1)
            {
                cache.insert(r);
                cnameIndex = cache.search(r);
            }

            cache[cacheIndex].cname = &cache[cnameIndex];
        }
        else
        {
            // Not an A record or a CNAME. Ignore.
            (*offset) += dataLen;
        }

        free(assembled);
        return E_MDNS_OK;
    }

    MDNS_RESULT Answer::parseName(char **name, const char *mapped, unsigned int len)
    {
        unsigned int namePointer = 0;
        unsigned int mapPointer = 0;

        while (mapPointer < len)
        {
            int labelLength = mapped[mapPointer++];

            if (labelLength > 0x3f)
            {
                return E_MDNS_INVALID_LABEL_LENGTH;
            }

            if (namePointer + labelLength > len - 1)
            {
                return E_MDNS_PACKET_ERROR;
            }

            if (namePointer != 0)
            {
                (*name)[namePointer++] = '.';
            }

            for (int i = 0; i < labelLength; i++)
            {
                (*name)[namePointer++] = mapped[mapPointer++];
            }
        }

        (*name)[len - 1] = '\0';

        return E_MDNS_OK;
    }

    int Answer::assembleName(unsigned char *buffer, unsigned int len, unsigned int *offset, char **name, unsigned int maxlen)
    {
        unsigned int index = 0;
        unsigned int nameLength = 0;

        while (index < maxlen)
        {
            if ((buffer[*offset] & 0xc0) == 0xc0)
            {
                // unsigned int pointerOffset = ((buffer[(*offset)++] & 0x3f) << 8) + buffer[(*offset)++];
                unsigned int pointerOffset1 = (buffer[(*offset)++] & 0x3f) << 8;
                unsigned int pointerOffset2 = buffer[(*offset)++];
                unsigned int pointerOffset = pointerOffset1 + pointerOffset2;
                if (pointerOffset > len)
                {
                    // Points to somewhere beyond the packet
                    return -1 * E_MDNS_POINTER_OVERFLOW;
                }

                char *namePointer = *name + nameLength;
                int pointerLen = assembleName(buffer, len, &pointerOffset, &namePointer, MDNS_MAX_NAME_LEN - nameLength);

                if (pointerLen < 0)
                {
                    return pointerLen;
                }

                nameLength += pointerLen;

                break;
            }
            else if (buffer[*offset] == '\0')
            {
                (*name)[nameLength++] = buffer[(*offset)++];
                break;
            }
            else
            {
                (*name)[nameLength++] = buffer[(*offset)++];
            }
            index++;
        }

        return nameLength;
    }

    int Answer::assembleName(unsigned char *buffer, unsigned int len, unsigned int *offset, char **name)
    {
        return assembleName(buffer, len, offset, name, MDNS_MAX_NAME_LEN);
    }

    // Work out how many bytes are dedicated to questions. Since we aren't answering questions, they can be skipped
    // buffer: The mDNS packet we are parsing
    // len: Length of the packet
    // offset: the byte we are up to in the parsing process
    MDNS_RESULT Answer::skipQuestions(unsigned char *buffer, unsigned int len, unsigned int *offset)
    {
        unsigned int questionCount = (buffer[4] << 8) + buffer[5];

        *offset += 12;
        for (unsigned int i = 0; i < questionCount; i++)
        {

            while (buffer[*offset] != '\0')
            {
                // If it's a pointer, add two to the counter
                if ((buffer[*offset] & 0xc0) == 0xc0)
                {
                    (*offset) += 2;
                    break;
                }
                else
                {
                    unsigned int labelLength = (unsigned int)buffer[*offset];

                    if (labelLength > 0x3f)
                    {
                        return E_MDNS_INVALID_LABEL_LENGTH;
                    }

                    (*offset) += 1; // Increment to move to the next byte
                    (*offset) += labelLength;

                    if (*offset > len)
                    {
                        return E_MDNS_PACKET_ERROR;
                    }
                }
            }

            (*offset) += 5; // 2 bytes for the qtypes and 2 bytes qclass + plus one to land us on the next bit
        }

        if (*offset > len + 1)
        {
            return E_MDNS_PACKET_ERROR;
        }

        return E_MDNS_OK;
    }

    MDNS_RESULT Answer::resolveCnames(Cache &cache)
    {
        for (int i = 0; i < cache.length(); i++)
        {
            if (cache[i].cname != NULL && cache[i].cname->resolved)
            {
                cache[i].ipAddress = cache[i].cname->ipAddress;
                cache[i].resolved = true;
                cache[i].cname = NULL;
            }
        }
        return E_MDNS_OK;
    }
};
