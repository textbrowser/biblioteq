/** \file z-date.h
    \brief ASN.1 Module UserInfoFormat-dateTime

    Generated automatically by YAZ ASN.1 Compiler 0.4
*/

#ifndef z_date_H
#define z_date_H

#include <yaz/odr.h>
#include <yaz/z-core.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct Z_DateTime Z_DateTime;
YAZ_EXPORT int z_DateTime(ODR o, Z_DateTime **p, int opt, const char *name);

typedef struct Z_DateMonthAndDay Z_DateMonthAndDay;
YAZ_EXPORT int z_DateMonthAndDay(ODR o, Z_DateMonthAndDay **p, int opt, const char *name);

typedef struct Z_DateQuarter Z_DateQuarter;
YAZ_EXPORT int z_DateQuarter(ODR o, Z_DateQuarter **p, int opt, const char *name);

typedef struct Z_DateSeason Z_DateSeason;
YAZ_EXPORT int z_DateSeason(ODR o, Z_DateSeason **p, int opt, const char *name);

typedef struct Z_Era Z_Era;
YAZ_EXPORT int z_Era(ODR o, Z_Era **p, int opt, const char *name);

typedef struct Z_DateFlags Z_DateFlags;
YAZ_EXPORT int z_DateFlags(ODR o, Z_DateFlags **p, int opt, const char *name);

typedef struct Z_Date Z_Date;
YAZ_EXPORT int z_Date(ODR o, Z_Date **p, int opt, const char *name);

typedef struct Z_Time Z_Time;
YAZ_EXPORT int z_Time(ODR o, Z_Time **p, int opt, const char *name);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus
extern "C" {
#endif
struct Z_DateTime {
	Z_Date *date; /* OPT */
	Z_Time *time; /* OPT */
};

struct Z_DateMonthAndDay {
	Odr_int *month;
	Odr_int *day; /* OPT */
};

struct Z_DateQuarter {
	int which;
	union {
		Odr_null *first;
		Odr_null *second;
		Odr_null *third;
		Odr_null *fourth;
#define Z_DateQuarter_first 1
#define Z_DateQuarter_second 2
#define Z_DateQuarter_third 3
#define Z_DateQuarter_fourth 4
	} u;
};

struct Z_DateSeason {
	int which;
	union {
		Odr_null *winter;
		Odr_null *spring;
		Odr_null *summer;
		Odr_null *autumn;
#define Z_DateSeason_winter 1
#define Z_DateSeason_spring 2
#define Z_DateSeason_summer 3
#define Z_DateSeason_autumn 4
	} u;
};

struct Z_Era {
	int which;
	union {
		Odr_null *decade;
		Odr_null *century;
		Odr_null *millennium;
#define Z_Era_decade 1
#define Z_Era_century 2
#define Z_Era_millennium 3
	} u;
};

struct Z_DateFlags {
	Odr_null *circa; /* OPT */
	Z_Era *era; /* OPT */
};

struct Z_Date {
	Odr_int *year;
	int which;
	union {
		Z_DateMonthAndDay *monthAndDay;
		Odr_int *julianDay;
		Odr_int *weekNumber;
		Z_DateQuarter *quarter;
		Z_DateSeason *season;
#define Z_Date_monthAndDay 1
#define Z_Date_julianDay 2
#define Z_Date_weekNumber 3
#define Z_Date_quarter 4
#define Z_Date_season 5
	} u; /* OPT */
	Z_DateFlags *flags; /* OPT */
};

struct Z_Time {
	Odr_int *hour;
	Odr_int *minute; /* OPT */
	Odr_int *second; /* OPT */
	Z_IntUnit *partOfSecond; /* OPT */
	int which;
	union {
		Odr_null *local;
		Odr_null *utc;
		Odr_int *utcOffset;
#define Z_Time_local 1
#define Z_Time_utc 2
#define Z_Time_utcOffset 3
	} u;
};

#ifdef __cplusplus
}
#endif
#endif
