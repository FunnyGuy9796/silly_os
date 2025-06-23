#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include "../misc/byte.h"
#include "../display/printf.h"

#define PIT_FREQUENCY 1193182
#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40
#define CMOS_ADDR 0x70
#define CMOS_DATA 0x71
#define TICKS_PER_SECOND 100 // every 10 milliseconds

typedef struct rtc_time {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} rtc_time_t;

void read_rtc_time(rtc_time_t *time);

void pit_init(uint32_t frequency);

void timer_callback();

uint32_t timer_ticks();

void update_sys_time();

rtc_time_t get_sys_time();

uint64_t get_epoch_time();

void sleep(uint32_t ms);

#endif