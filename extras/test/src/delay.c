#include <time.h>

void delay(unsigned long ms) {
    struct timespec req, rem;
    req.tv_sec = ms / 1000;
    req.tv_nsec = (ms % 1000) * 1000000;

    while (nanosleep(&req, &rem) == -1) {
        req = rem; // If interrupted, continue with remaining time
    }
}

void delayMicroseconds(unsigned int us) {
    struct timespec req, rem;
    req.tv_sec = us / 1000000;
    req.tv_nsec = (us % 1000000) * 1000;

    while (nanosleep(&req, &rem) == -1) {
        req = rem; // If interrupted, continue with remaining time
    }
}
