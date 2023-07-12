#ifndef UTILITY_H
#define UTILITY_H

#include <cstdio>
#include <cstdarg>
#include <cinttypes>

namespace utility {
    
    inline void errorState(const char *fmt, ...) {
        
        char buf[1024];

        va_list vl;
        va_start(vl, fmt);
        
        vsnprintf( buf, sizeof( buf), fmt, vl);
        va_end( vl);

        printf("ERROR: %s\n",buf);

        while(1); // ERROR LOOP
        
    }
}

#endif //UTILITY_H