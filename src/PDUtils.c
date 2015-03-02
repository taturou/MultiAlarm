/*
  This code is derived from PDPCLIB, the public domain C runtime
  library by Paul Edwards. http://pdos.sourceforge.net/

  This code is released to the public domain.
*/
#include <pebble.h>
#include <ctype.h>

/* scalar date routines    --    public domain by Ray Gardner
** These will work over the range 1-01-01 thru 14699-12-31
** The functions written by Ray are isleap, months_to_days,
** years_to_days, ymd_to_scalar, scalar_to_ymd.
** modified slightly by Paul Edwards
*/

    static int isleap (unsigned yr) {
    return yr % 400 == 0 || (yr % 4 == 0 && yr % 100 != 0);
}

static unsigned months_to_days (unsigned month) {
    return (month * 3057 - 3007) / 100;
}

static unsigned years_to_days (unsigned yr) {
    return yr * 365L + yr / 4 - yr / 100 + yr / 400;
}

static long ymd_to_scalar (unsigned yr, unsigned mo, unsigned day) {
    long scalar;

    scalar = day + months_to_days(mo);
    if (mo > 2) /* adjust if past February */
        scalar -= isleap(yr) ? 1 : 2;
    yr--;
    scalar += years_to_days(yr);
    return scalar;
}

time_t p_mktime (struct tm *timeptr) {
    time_t tt;

    if ((timeptr->tm_year < 70) || (timeptr->tm_year > 120)) {
        tt = (time_t)-1;
    } else {
        tt = ymd_to_scalar(timeptr->tm_year + 1900,
                           timeptr->tm_mon + 1,
                           timeptr->tm_mday)
            - ymd_to_scalar(1970, 1, 1);
        tt = tt * 24 + timeptr->tm_hour;
        tt = tt * 60 + timeptr->tm_min;
        tt = tt * 60 + timeptr->tm_sec;
    }
    return tt;
}

char *p_strtok(char *s1, const char *s2) {
    static char *old = NULL;
    char *p;
    size_t len;
    size_t remain;

    if (s1 != NULL) old = s1;
    if (old == NULL) return (NULL);
    p = old;
    len = strspn(p, s2);
    remain = strlen(p);
    if (remain <= len) { old = NULL; return (NULL); }
    p += len;
    len = strcspn(p, s2);
    remain = strlen(p);
    if (remain <= len) { old = NULL; return (p); }
    *(p + len) = '\0';
    old = p + len + 1;
    return(p);
}

unsigned long int strtoul(const char *nptr, char **endptr, int base) {
    unsigned long x = 0;
    int undecided = 0;

    if (base == 0) {
        undecided = 1;
    }
    while (isspace((unsigned char)*nptr)) {
        nptr++;
    }
    while (1) {
        if (isdigit((unsigned char)*nptr)) {
            if (base == 0) {
                if (*nptr == '0') {
                    base = 8;
                } else {
                    base = 10;
                    undecided = 0;
                }
            }
            x = x * base + (*nptr - '0');
            nptr++;
        } else if (isalpha((unsigned char)*nptr)) {
            if ((*nptr == 'X') || (*nptr == 'x')) {
                if ((base == 0) || ((base == 8) && undecided)) {
                    base = 16;
                    undecided = 0;
                    nptr++;
                } else if (base == 16) {
                    /* hex values are allowed to have an optional 0x */
                    nptr++;
                } else {
                    break;
                }
            } else if (base <= 10) {
                break;
            } else {
                x = x * base + (toupper((unsigned char)*nptr) - 'A') + 10;
                nptr++;
            }
        } else {
            break;
        }
    }
    if (endptr != NULL) {
        *endptr = (char *)nptr;
    }
    return (x);
}

long int strtol(const char *nptr, char **endptr, int base) {
    unsigned long y;
    long x;
    int neg = 0;

    while (isspace((unsigned char)*nptr)) {
        nptr++;
    }
    if (*nptr == '-') {
        neg = 1;
        nptr++;
    } else if (*nptr == '+') {
        nptr++;
    }
    y = strtoul(nptr, endptr, base);
    if (neg) {
        x = (long)-y;
    } else {
        x = (long)y;
    }
    return (x);
}

/* This qsort routine was obtained from libnix (also public domain),
 * and then reformatted.
 *
 * This qsort function does a little trick:
 * To reduce stackspace it iterates the larger interval instead of doing
 * the recursion on both intervals.
 * So stackspace is limited to 32*stack_for_1_iteration =
 * 32*4*(4 arguments+1 returnaddress+11 stored registers) = 2048 Bytes,
 * which is small enough for everybodys use.
 * (And this is the worst case if you own 4GB and sort an array of chars.)
 * Sparing the function calling overhead does improve performance, too.
 */
void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *a, const void *b)) {
    char *base2 = (char *)base;
    size_t i, a, b, c;

    while (nmemb > 1) {
        a = 0;
        b = nmemb - 1;
        c = (a + b) / 2; /* Middle element */
        while (1) {
            while ((*compar)(&base2[size*c], &base2[size*a]) > 0) {
                a++; /* Look for one >= middle */
            }
            while ((*compar)(&base2[size*c], &base2[size*b]) < 0) {
                b--; /* Look for one <= middle */
            }
            if (a >= b) {
                break; /* We found no pair */
            }
            for (i=0; i < size; i++) { /* swap them */
                char tmp = base2[size*a+i];

                base2[size*a+i] = base2[size*b+i];
                base2[size*b+i] = tmp;
            }
            if (c == a) { /* Keep track of middle element */
                c = b;
            } else if (c == b){
                c = a;
            }
            a++; /* These two are already sorted */
            b--;
        } /* a points to first element of right interval now (b to last of left) */
        b++;
        if (b < nmemb-b) { /* do recursion on smaller interval and iteration on larger one */
            qsort(base2, b, size, compar);
            base2 = &base2[size*b];
            nmemb = nmemb-b;
        } else {
            qsort(&base2[size*b], nmemb - b, size, compar);
            nmemb = b;
        }
    }
    return;
}