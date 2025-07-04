#include "../../api.h"

void module_init(module_t *mod) {
    uint32_t size;
    void *data = read_file("initrd/hello.txt", &size);

    kprintf("\n");

    for (uint32_t i = 0; i < size; i++)
        kprintf("%c", ((char *)data)[i]);
    
    kprintf("\n");

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

    kprintf("\n%s %s %d %02u:%02u:%02u %s %02u\n\n",
        day[day_num], month[curr_time.month - 1], curr_time.day, n_hours, curr_time.minutes, curr_time.seconds, apm, curr_time.year);
}

void module_exit(module_t *mod) {
    
}