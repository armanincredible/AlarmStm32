#ifndef TIMERS_H
#define TIMERS_H
#include <uart.h>
#include <inc/time.h>
#include <inc/error.h>
#include <ctype.h>
#include <string.h>
#include <inc/alarm.h>

#define MAX_ALARMS	10
#define MAX_BUFF_SIZE	64

enum ALARM_TIMERS_ERRS {
	E_ALARM_BAD_INPUT = 1,
	E_ALARM_NO_INPUT
};

struct List {
	struct List *prev, *next;
};

struct alarm_time {
	// This should be on top!
	struct List head;
	uint64_t time;
};

struct alarm_time *init_alarm_timers(struct Uart *uart);
bool proccess_timeout(alarm_t *alarm);
time_t get_closest_hhmmss(struct alarm_time *head);
void time_to_hhmmss(uint64_t t, time_t *time);
uint64_t time_to_sec(time_t *t);
void dump_timer_list(struct alarm_time *head, struct Uart *uart);
struct List *dummy_test();
#endif // TIMERS_H
