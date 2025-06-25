#include "command.h"

void cmd_help(int argc, char **argv) {
    kprintf("Available commands:\n");
    kprintf("\thelp - Show this help message\n");
    kprintf("\techo - Echo a string\n");
    kprintf("\tdate - Get the current date and time based on RTC\n");
    kprintf("\tclear - Clear the screen\n");
    kprintf("\tmem - Get the current memory info and usage\n");
}

void cmd_echo(int argc, char **argv) {
    for (int i = 1; i < argc; i++)
        kprintf("%s ", argv[i]);
    
    kprintf("\n");
}

void cmd_date(int argc, char **argv) {
    char *day[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    char *month[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    rtc_time_t curr_time = get_sys_time();
    char *apm = "AM";
    uint8_t n_hours = curr_time.hours;
    int day_num = get_day(curr_time.day, curr_time.month, curr_time.year);

    if (curr_time.hours > 12) {
        n_hours -= 12;
        apm = "PM";
    } else if (curr_time.hours == 0)
        n_hours = 12;

    kprintf("%s %s %d %02u:%02u:%02u %s %02u\n",
        day[day_num], month[curr_time.month - 1], curr_time.day, n_hours, curr_time.minutes, curr_time.seconds, apm, curr_time.year);
}

void cmd_clear(int argc, char **argv) {
    kclear();
}

void cmd_mem(int argc, char **argv) {
    uint32_t mem_total = get_mem_total();
    uint32_t mem_used = get_used_mem();
    uint32_t mem_free = get_free_mem();

    kprintf("Memory:\n\tTotal: %u KB\n\tUsed: %u KB\n\tFree: %u KB\n", mem_total / 1024, mem_used / 1024, mem_free / 1024);
}