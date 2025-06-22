#include "ftoa.h"

void ftoa(double value, char *str, int precision) {
    if (value < 0) {
        *str++ = '-';
        value = -value;
    }

    int int_part = (int)value;
    double frac_part = value - int_part;

    if (precision == 0) {
        if (frac_part >= 0.5)
            int_part += 1;

        itoa(int_part, str, 10);

        return;
    }

    char int_str[20];
    itoa(int_part, int_str, 10);
    const char *p = int_str;

    while (*p) *str++ = *p++;

    *str++ = '.';

    double scaled = frac_part;

    for (int i = 0; i < precision; i++)
        scaled *= 10;

    int frac_int = (int)(scaled);
    char frac_str[20];

    itoa(frac_int, frac_str, 10);

    int len = 0;

    for (const char *fp = frac_str; *fp; fp++) len++;

    for (int i = 0; i < precision - len; i++)
        *str++ = '0';

    for (const char *fp = frac_str; *fp; fp++)
        *str++ = *fp;

    *str = '\0';
}