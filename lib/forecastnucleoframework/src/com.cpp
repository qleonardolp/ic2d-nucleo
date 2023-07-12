#include <algorithm>
#include <forecast/com.hpp>
#include <debug.hpp>

using namespace forecast::com;

uint32_t forecast::com::escape(const void* src,
                               void* dst,
                               uint32_t srcSz,
                               uint32_t maxSz) {
    auto* srcBytes = reinterpret_cast<const uint8_t*>(src);
    auto* dstBytes = reinterpret_cast<uint8_t*>(dst);

    uint32_t j = 0;
    for (uint32_t i = 0; i < srcSz and (maxSz == 0 or j < maxSz); ++i) {
        if (srcBytes[i] == stx or srcBytes[i] == etx or srcBytes[i] == esc) {
            if (maxSz == 0 or j + 1 < maxSz)
                dstBytes[j++] = esc;
            else
                break;
        }

        dstBytes[j++] = srcBytes[i];
    }

    return j;
}

uint32_t forecast::com::unescape(const void* src,
                                 void* dst,
                                 uint32_t srcSz,
                                 uint32_t maxSz) {
    auto* srcBytes = reinterpret_cast<const uint8_t*>(src);
    auto* dstBytes = reinterpret_cast<uint8_t*>(dst);

    uint32_t j = 0;
    for (uint32_t i = 0; i < srcSz and j < maxSz; ++i) {
        if (srcBytes[i] == esc)
            i++;

        dstBytes[j++] = srcBytes[i];
    }

    return j;
}

uint8_t forecast::com::checksum(const uint8_t* src, uint32_t size) {
    uint32_t check = 0;

    for (uint32_t i = 0; i < size; ++i)
        check += src[i];

    return static_cast<uint8_t>(check % 0xff);
}

uint32_t forecast::com::buildPkg(com::Type t,
                                 const void* payload,
                                 void* dst,
                                 uint32_t payloadSz,
                                 uint32_t maxSz) {
    auto* payloadBytes = reinterpret_cast<const uint8_t*>(payload);
    auto* dstBytes = reinterpret_cast<uint8_t*>(dst);
    auto cks = checksum(payloadBytes, payloadSz);
    uint32_t i = 0;

    dstBytes[i++] = stx;
    dstBytes[i++] = t;
    i += escape(payload, &dstBytes[i], payloadSz,
                maxSz - 5 /* space for stx, type, 
                chk(could be 2 if escaped) and etx */);
    i += escape(&cks, &dstBytes[i], 1);  // no check on sz (done before)
    dstBytes[i++] = etx;
    return i;
}

bool forecast::com::checkEtx(const uint8_t* pkg, uint32_t size) {
    if (size < 2 or pkg[size-1] != etx)
        return false;
    
    bool escaped = false;
    for (uint32_t i = 2; pkg[size - i] == esc; ++i) {
        escaped = not escaped;
        DEBUG_INFO("escaped %d", escaped);
    }
    
    return not escaped;
}