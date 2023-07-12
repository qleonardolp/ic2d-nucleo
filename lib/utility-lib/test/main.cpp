#include <utility/Utility.h>
#include <utility/HALTicker.h>
#include <utility/bechmark.h>
#include "mbed.h"

Serial pc(USBTX,USBRX,921600);

using namespace utility;
void HALTickerTest();
void TimerTest();
// This is the main function
int main() {
    // Code to print float values on serial
    asm(".global _printf_float");

    pc.printf("start!!!!\n");
    
    #if  1
        HALTickerTest();
    #else
        TimerTest();
    #endif
}

void HALTickerTest() {
    HALTicker::startTimer(2000);
    HALTicker::resetFlagTime();
    uint32_t actual_time = 0, last_time = 0;
    uint32_t delta_time = 0;

    // Main loop
    while (true) {
        // If timer 15 has triggered it enters in if statement
        if(HALTicker::isReady()) {
            HALTicker::resetFlagTime();

            // take actual time in microseconds
            actual_time = us_ticker_read();

            // how it takes to do one cycle in microseconds
            delta_time = actual_time - last_time;

            pc.printf("real freq: ~ %.1fHz\r\n",HALTicker::getFreq());
            pc.printf("time %lu\n", HALTicker::getCicleTime());

            last_time = actual_time;
            
            if(HALTicker::isReady()) {
                pc.printf("Frequency not kept\n");
            }
        } else {
            //pc.printf("HALTicker::isReady() == false\n");
        }
    }
}

void TimerTest() {
    START_BENCH(0)
        START_BENCH(1)
            wait_ns(1000);
        END_BENCH(1);
        START_BENCH(2)
            wait_us(1000);
        END_BENCH(2);
        START_BENCH(3)
            wait(3);
        END_BENCH(3);
    END_BENCH(0);

    pc.printf(
        "calculated time waiting 1 us:\t\t%li\n"
        "calculated time waiting 1000 us:\t%li\n"
        "calculated time waiting 3000 us:\t%li\n"
        "calculated time waiting overall:\t%li\n",
        BENCH_VALUE(1), BENCH_VALUE(2), BENCH_VALUE(3), BENCH_VALUE(0)
    );
}
// void TimerTest() {
//     Timer t, t1, t2, t3;
//     uint32_t v, v1, v2, v3;

//     t.start();
//     t1.start(); // 1 nanoseconds;
//     wait_ns(1000);
//     t1.stop(); 
//     v1 = t1.read_high_resolution_us(); // v1 should be around 1.
//     t2.start();  // 1 milliseconds;
//     wait_us(1000);
//     t2.stop();
//     v2 = t2.read_high_resolution_us(); // should be arround 1000;
//     t3.start(); // 3 ms
//     wait_ms(3);
//     t3.stop();
//     v3 = t3.read_high_resolution_us(); // should be arround 3000;
//     wait(10);
//     t.stop();
//     v = t.read_high_resolution_us(); // should be arround 4001 plus the time of
//                                      // the functions of the timer;

//     pc.printf(
//         "calculated time waiting 1 us:\t%li\n"
//         "calculated time waiting 1000 us:\t%li\n"
//         "calculated time waiting 3000 us:\t%li\n"
//         "calculated time waiting overall:\t%li\n",
//         v1, v2, v3, v
//     );
// }