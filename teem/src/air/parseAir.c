/*
  Teem: Tools to process and visualize scientific data and images
  Copyright (C) 2006, 2005  Gordon Kindlmann
  Copyright (C) 2004, 2003, 2002, 2001, 2000, 1999, 1998  University of Utah

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  (LGPL) as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  The terms of redistributing and/or modifying this software also
  include exceptions to the LGPL that facilitate static linking.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include "air.h"

char
_airBoolStr[][AIR_STRLEN_SMALL] = {
  "(unknown bool)",
  "false",
  "true"
};

char
_airBoolDesc[][AIR_STRLEN_MED] = {
  "unknown boolean",
  "false",
  "true"
};

int
_airBoolVal[] = {
  -1,
  AIR_FALSE,
  AIR_TRUE
};

char
_airBoolStrEqv[][AIR_STRLEN_SMALL] = {
  "0", "no", "n", "false", "f", "off", "nope",
  "1", "yes", "y", "true", "t", "on", "yea",
  ""
};

int
_airBoolValEqv[] = {
  AIR_FALSE, AIR_FALSE, AIR_FALSE, AIR_FALSE, AIR_FALSE, AIR_FALSE, AIR_FALSE,
  AIR_TRUE, AIR_TRUE, AIR_TRUE, AIR_TRUE, AIR_TRUE, AIR_TRUE, AIR_TRUE
};

const airEnum
_airBool = {
  "boolean",
  2,
  _airBoolStr,
  _airBoolVal,
  _airBoolDesc,
  _airBoolStrEqv,
  _airBoolValEqv,
  AIR_FALSE
};

const airEnum *const
airBool = &_airBool;

double
airAtod(const char *str) {
  double val = 0.0;
  
  airSingleSscanf(str, "%lf", &val);
  return val;
}

int
airSingleSscanf(const char *str, const char *fmt, void *ptr) {
  char *tmp;
  double val;
  int ret;
  
  if (!strcmp(fmt, "%e") || !strcmp(fmt, "%f") || !strcmp(fmt, "%g")
      || !strcmp(fmt, "%le") || !strcmp(fmt, "%lf") || !strcmp(fmt, "%lg")) {
    tmp = airStrdup(str);
    if (!tmp) {
      return 0;
    }
    airToLower(tmp);
    if (strstr(tmp, "nan")) {
      val = AIR_NAN;
    }
    else if (strstr(tmp, "-inf")) {
      val = AIR_NEG_INF;
    }
    else if (strstr(tmp, "inf")) {
      val = AIR_POS_INF;
    }
    else {
      /* nothing special matched; pass it off to sscanf() */
      ret = sscanf(str, fmt, ptr);
      free(tmp);
      return ret;
    }
    /* else we matched "nan", "-inf", or "inf", and set val accordingly */
    if (!strncmp(fmt, "%l", 2)) {
      /* we were given a double pointer */
      *((double *)(ptr)) = val;
    }
    else {
      /* we were given a float pointer */
      *((float *)(ptr)) = AIR_CAST(float, val);
    }
    free(tmp);
    return 1;
  }
  else {
    /* this is neither a float nor double */
    return sscanf(str, fmt, ptr);
  }
}

#define _PARSE_STR_ARGS(type) type *out, const char *_s, \
                              const char *ct, unsigned int n, ...
#define _PARSE_STR_BODY(format) \
  unsigned int i; \
  char *tmp, *s, *last; \
  \
  /* if we got NULL, there's nothing to do */ \
  if (!(out && _s && ct)) \
    return 0; \
  \
  /* copy the input so that we don't change it */ \
  s = airStrdup(_s); \
  \
  /* keep calling airStrtok() until we have everything */ \
  for (i=0; i<n; i++) { \
    tmp = airStrtok(i ? NULL : s, ct, &last); \
    if (!tmp) { \
      free(s); \
      return i; \
    } \
    if (1 != airSingleSscanf(tmp, format, out+i)) { \
      free(s); \
      return i; \
    } \
  } \
  free(s); \
  return n; \

/*
******* airParse*()
**
** parse ints, floats, doubles, or single characters, from some
** given string "s"; try to parse "n" of them, as delimited by
** characters in "ct", and put the results in "out".
**
** Returns the number of things succesfully parsed- should be n; 
** there's been an error if return is < n.
**
** The embarrassing reason for the var-args ("...") is that I want the
** type signature of all these functions to be the same, and I have a function
** for parsing airEnums, in case the airEnum must be supplied as a final
** argument.
**
** This uses air's thread-safe strtok() replacement: airStrtok()
*/
unsigned int
airParseStrI(_PARSE_STR_ARGS(int))           { _PARSE_STR_BODY("%d") }

unsigned int
airParseStrUI(_PARSE_STR_ARGS(unsigned int)) { _PARSE_STR_BODY("%u") }

unsigned int
airParseStrZ(_PARSE_STR_ARGS(size_t))     { _PARSE_STR_BODY(_AIR_SIZE_T_CNV) }

unsigned int
airParseStrF(_PARSE_STR_ARGS(float))         { _PARSE_STR_BODY("%f") }

unsigned int
airParseStrD(_PARSE_STR_ARGS(double))        { _PARSE_STR_BODY("%lf") }

unsigned int
airParseStrB(int *out, const char *_s, const char *ct, unsigned int n, ...) {
  unsigned int i;
  char *tmp, *s, *last;

  /* if we got NULL, there's nothing to do */
  if (!(out && _s && ct))
    return 0;

  /* copy the input so that we don't change it */
  s = airStrdup(_s);

  /* keep calling airStrtok() until we have everything */
  for (i=0; i<n; i++) {
    tmp = airStrtok(i ? NULL : s, ct, &last);
    if (!tmp) {
      free(s);
      return i;
    }
    out[i] = airEnumVal(airBool, tmp);
    if (airEnumUnknown(airBool) == out[i]) {
      free(s);
      return i;
    }
  }
  free(s);
  return n;
}

unsigned int
airParseStrC(char *out, const char *_s, const char *ct, unsigned int n, ...) {
  unsigned int i;
  char *tmp, *s, *last;

  /* if we got NULL, there's nothing to do */
  if (!(out && _s && ct))
    return 0;

  /* copy the input so that we don't change it */
  s = airStrdup(_s);

  /* keep calling airStrtok() until we have everything */
  for (i=0; i<n; i++) {
    tmp = airStrtok(i ? NULL : s, ct, &last);
    if (!tmp) {
      free(s);
      return i;
    }
    out[i] = tmp[0];
  }
  free(s);
  return n;
}

unsigned int
airParseStrS(char **out, const char *_s, const char *ct, unsigned int n, ...) {
  unsigned int i;
  int greedy;
  char *tmp, *s, *last;
  airArray *mop;
  va_list ap;

  /* grab "greedy" every time, prior to error checking */
  va_start(ap, n);
  greedy = va_arg(ap, int);
  va_end(ap);

  /* if we got NULL, there's nothing to do */
  if (!(out && _s && ct))
    return 0;

  mop = airMopNew();
  /* copy the input so that we don't change it */
  s = airStrdup(_s);
  airMopMem(mop, &s, airMopAlways);

  /* keep calling airStrtok() until we have everything */
  for (i=0; i<n; i++) {
    /* if n == 1, then with greediness, the whole string is used,
       and without greediness, we use airStrtok() to get only
       the first part of it */
    if (n > 1 || !greedy) {
      tmp = airStrtok(i ? NULL : s, ct, &last);
    }
    else {
      tmp = s;
    }
    if (!tmp) {
      airMopError(mop);
      return i;
    }
    out[i] = airStrdup(tmp);
    if (!out[i]) {
      airMopError(mop);
      return i;
    }
    airMopMem(mop, out+i, airMopOnError);
  }
  airMopOkay(mop);
  return n;
}

unsigned int
airParseStrE(int *out, const char *_s, const char *ct, unsigned int n, ...) {
  unsigned int i;
  char *tmp, *s, *last;
  airArray *mop;
  va_list ap;
  airEnum *enm;

  /* grab the enum every time, prior to error checking */
  va_start(ap, n);
  enm = va_arg(ap, airEnum *);
  va_end(ap);

  /* if we got NULL, there's nothing to do */
  if (!(out && _s && ct)) {
    return 0;
  }

  mop = airMopNew();
  /* copy the input so that we don't change it */
  s = airStrdup(_s);
  airMopMem(mop, &s, airMopAlways);

  if (1 == n) {
    /* Because it should be permissible to have spaces in what is
       intended to be only a single string from an airEnum, we treat
       1==n differently, and do NOT use airStrtok to tokenize the
       input string s into spaces.  We check the whole s string */
    out[0] = airEnumVal(enm, s);
    if (airEnumUnknown(enm) == out[0]) {
      airMopError(mop);
      return 0;
    }
  } else {
    /* keep calling airStrtok() until we have everything */
    for (i=0; i<n; i++) {
      tmp = airStrtok(i ? NULL : s, ct, &last);
      if (!tmp) {
        airMopError(mop);
        return i;
      }
      out[i] = airEnumVal(enm, tmp);
      if (airEnumUnknown(enm) == out[i]) {
        airMopError(mop);
        return i;
      }
    }
  }
  airMopOkay(mop);
  return n;
}

unsigned int
(*airParseStr[AIR_TYPE_MAX+1])(void *, const char *,
                               const char *, unsigned int, ...) = {
  NULL,
  (unsigned int (*)(void *, const char *, const char *,
                    unsigned int, ...))airParseStrB,
  (unsigned int (*)(void *, const char *, const char *,
                    unsigned int, ...))airParseStrI,
  (unsigned int (*)(void *, const char *, const char *,
                    unsigned int, ...))airParseStrUI,
  (unsigned int (*)(void *, const char *, const char *,
                    unsigned int, ...))airParseStrZ,
  (unsigned int (*)(void *, const char *, const char *,
                    unsigned int, ...))airParseStrF,
  (unsigned int (*)(void *, const char *, const char *,
                    unsigned int, ...))airParseStrD,
  (unsigned int (*)(void *, const char *, const char *,
                    unsigned int, ...))airParseStrC,
  (unsigned int (*)(void *, const char *, const char *,
                    unsigned int, ...))airParseStrS,
  (unsigned int (*)(void *, const char *, const char *,
                    unsigned int, ...))airParseStrE,
  NULL   /* no standard way of parsing type "other" */
};

