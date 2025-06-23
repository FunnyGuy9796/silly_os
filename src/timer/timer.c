#include "timer.h"

static uint32_t tick = 0;
static uint64_t uptime_ticks = 0;

static uint32_t seconds = 0;
static uint32_t minutes = 0;
static uint32_t hours = 0;

static rtc_time_t curr_time;

static uint32_t since_rtc_sync = 0;

static const int days_in_month[] = {
    31, 28, 31, 30, 31, 30,
    31, 31, 30, 31, 30, 31
};

int leap_year(int year) {
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

uint64_t rtc_to_epoch(rtc_time_t *rtc) {
    uint64_t days = 0;

    for (int y = 1970; y < rtc->year; y++)
        days += leap_year(y) ? 366 : 365;
    
    for (int m = 1; m < rtc->month; m++) {
        days += days_in_month[m - 1];

        if (m == 2 && leap_year(rtc->year))
            days++;
    }

    days += (rtc->day - 1);

    uint64_t seconds = days * 86400;
    seconds += rtc->hours * 3600;
    seconds += rtc->minutes * 60;
    seconds += rtc->seconds;

    return seconds;
}

void epoch_to_rtc(uint64_t epoch, rtc_time_t *rtc) {
    uint64_t seconds = epoch;
    uint64_t days = seconds / 86400;
    
    seconds %= 86400;
    rtc->hours = seconds / 3600;
    seconds %= 3600;
    rtc->minutes = seconds / 60;
    rtc->seconds = seconds % 60;

    int year = 1970;

    while (1) {
        int days_in_year = leap_year(year) ? 366 : 365;

        if (days < days_in_year)
            break;
        
        days -= days_in_year;
        year++;
    }

    rtc->year = year;

    int month = 0;

    while (1) {
        int dim = days_in_month[month];

        if (month == 1 && leap_year(year))
            dim++;
        
        if (days < dim)
            break;
        
        days -= dim;
        month++;
    }

    rtc->month = month + 1;
    rtc->day = days + 1;
}

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
    
    uint8_t century = cmos_read(0x32);

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
        century = bcd_to_bin(century);
    }

    if (century == 0)
        century = 20;

    time->year += century * 100;

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
    since_rtc_sync = 0;

    kstatus("debug", "pit configured to %uHz\n", frequency);
}

void timer_callback() {
    tick++;
    uptime_ticks++;

    if (tick == TICKS_PER_SECOND) {
        tick = 0;

        update_sys_time();
    }
}

uint32_t timer_ticks() {
    return tick;
}

void update_sys_time() {
    since_rtc_sync++;
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

                int dim = days_in_month[curr_time.month - 1];

                if (curr_time.month == 2 && leap_year(curr_time.year))
                    dim++;
                
                if (curr_time.day > dim) {
                    curr_time.day = 1;
                    curr_time.month++;

                    if (curr_time.month > 12) {
                        curr_time.month = 1;
                        curr_time.year++;
                    }
                }
            }
        }
    }

    if (since_rtc_sync >= 3600) {
        read_rtc_time(&curr_time);
        since_rtc_sync = 0;
    }
}

rtc_time_t get_sys_time() {
    return curr_time;
}

uint64_t get_epoch_time() {
    return rtc_to_epoch(&curr_time);
}

void sleep(uint32_t ms) {
    uint64_t start = uptime_ticks;
    uint64_t wait_ticks = ms / (1000 / TICKS_PER_SECOND);

    while ((uptime_ticks - start) < wait_ticks) {
        __asm__ volatile("hlt");
    }
}