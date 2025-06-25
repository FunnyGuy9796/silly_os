#ifndef DAY_H
#define DAY_H

static inline int get_day(int day, int month, int year) {
    if (month == 1 || month == 2) {
        month += 12;
        year -= 1;
    }

    int k = year % 100;
    int j = year / 100;
    int h = (day + (13 * (month + 1)) / 5 + k + k / 4 + j / 4 + 5 * j) % 7;

    return h;
}

#endif