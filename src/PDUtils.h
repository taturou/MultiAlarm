#pragma once
#include "pebble.h"

/*
  This code is derived from PDPCLIB, the public domain C runtime
  library by Paul Edwards. http://pdos.sourceforge.net/

  This code is released to the public domain.
*/

time_t p_mktime(struct tm *tmptr);
char *p_strtok(char *s1, const char *s2);
long int p_strtol(const char *nptr, char **endptr, int base);
void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *d1, const void *d2));
void qsort_r(void *base, size_t nmemb, size_t size, int (*compar)(const void *d1, const void *d2, void *arg), void *arg);