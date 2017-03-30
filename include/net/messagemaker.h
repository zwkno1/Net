
#pragma once

#include <vector>
#include <cstdint>

#include <util.h>

namespace net
{

enum
{
    MSG_HEAD_SIZE = 8
};

struct MessageMaker
{
    static std::vector<char> make(const char * data, uint32_t size, uint32_t flag)
    {
        (void) flag;
        std::vector<char> msg;
        msg.resize(size + MSG_HEAD_SIZE);
        //*reinterpret_cast<uint32_t *>(&msg[0]) = util::endian::htole(size);
        //*reinterpret_cast<uint32_t *>(&msg[4]) = util::endian::htole(flag);
        *reinterpret_cast<uint32_t *>(&msg[0]) = size;
        *reinterpret_cast<uint32_t *>(&msg[4]) = flag;
        std::copy(data, data+size, &msg[8]);
        return msg;
    }

    static std::vector<char> unmake(const char * data, uint32_t size, uint32_t flag)
    {
        (void) flag;
        return std::vector<char>(data, data + size);
    }
};

}
