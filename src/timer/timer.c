#include "timer.h"

static uint32_t tick = 0;

static uint32_t seconds = 0;
static uint32_t minutes = 0;
static uint32_t hours = 0;

static rtc_time_t curr_time;

static uint8_t cmos_read(uint8_t reg) {
    outb(CMOS_ADDR, reg);

    return inb(CMOS_DATA);
}

static int is_cmos_update() {
    outb(CMOS_ADDR, 0x0a);

    return (inb(CMOS_DATA) & 0x80);
}

static uint8_t bcd_to_bin(uint8_t val) {
    return ((val & 0x0f) + ((val / 16) * 10));
}

void read_rtc_time(rtc_time_t *time) {
    while (is_cmos_update());

    time->seconds = cmos_read(0x00);
    time->minutes = cmos_read(0x02);
    time->hours = cmos_read(0x04);
    time->day = cmos_read(0x07);
    time->month = cmos_read(0x08);
    time->year = cmos_read(0x09);
    time->century = cmos_read(0x32);

    uint8_t status_b = cmos_read(0x0b);

    int is_bcd = !(status_b & 0x04);
    int is_12h = !(status_b & 0x02);

    if (is_bcd) {
        time->seconds = bcd_to_bin(time->seconds);
        time->minutes = bcd_to_bin(time->minutes);
        time->hours = bcd_to_bin(time->hours & 0x7f);
        time->day = bcd_to_bin(time->day);
        time->month = bcd_to_bin(time->month);
        time->year = bcd_to_bin(time->year);
        time->century = bcd_to_bin(time->century);
    }

    time->year += time->century * 100;

    if (is_12h) {
        int is_pm = cmos_read(0x04) & 0x80;

        if (time->hours == 12)
            time->hours = is_pm ? 12 : 0;
        else if (is_pm)
            time->hours += 12;
    }
}

void pit_init(uint32_t frequency) {
    uint32_t divisor = PIT_FREQUENCY / frequency;

    outb(PIT_COMMAND, 0x36);

    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xff));
    outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xff));

    read_rtc_time(&curr_time);

    kstatus("debug", "pit configured to %u Hz\n", frequency);
}

void timer_callback() {
    tick++;

    if (tick == TICKS_PER_SECOND) {
        tick = 0;

        update_sys_time();
    }
}

uint32_t timer_ticks() {
    return tick;
}

void update_sys_time() {
    curr_time.seconds++;

    if (curr_time.seconds >= 60) {
        curr_time.seconds = 0;
        curr_time.minutes++;

        if (curr_time.minutes >= 60) {
            curr_time.minutes = 0;
            curr_time.hours++;

            if (curr_time.hours >= 24) {
                curr_time.hours = 0;
                curr_time.day++;
            }
        }
    }
}

rtc_time_t get_sys_time() {
    return curr_time;
}