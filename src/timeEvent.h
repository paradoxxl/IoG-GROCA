#ifndef timeEvent_h
#define timeEvent_h


template<typename T> struct timeEvent{
    T *state;
    uint8_t hour;
    uint8_t minute;
};

#endif
