#include <timers.h>

static uint8_t uart_buff[MAX_BUFF_SIZE];
static struct List head;

static int get_time(struct Uart *uart, uint64_t *time);
static int64_t atoi64(const char* str);
static char *itoa(int num, char *str);
static void dump_time(struct Uart *uart, time_t *time);

inline static bool __attribute__((always_inline))
list_empty(struct List *list) {
    return list->next == list;
}

inline static void __attribute__((always_inline))
list_init(struct List *list) {
    list->next = list->prev = list;
}

/*
 * Appends list element 'new' after list element 'list'
 */
inline static void __attribute__((always_inline))
list_append(struct List *list, struct List *new) {
	// LAB 6: Your code here
	if (!list || !new)
		return;

	new->prev  = list;
	new->next  = list->next;

	list->next->prev = new;
	list->next = new;
}

/*
 * Deletes list element from list.
 */
inline static struct List *__attribute__((always_inline))
list_del(struct List *list) {
	if (!list)
		return NULL;

	list->prev->next = list->next;
	list->next->prev = list->prev;

	list_init(list);

	return list;
}


static void dump_time(struct Uart *uart, time_t *time)
{
	uart_send_string(uart, itoa(time->hours, uart_buff), 0);
	uart_send_string(uart, ":", 0);
	uart_send_string(uart, itoa(time->minutes, uart_buff), 0);
	uart_send_string(uart, ":", 0);
	uart_send_string(uart, itoa(time->seconds, uart_buff), 0);
	uart_send_string(uart, "\r", 0);
}

void time_to_hhmmss(uint64_t t, time_t *time)
{
	if (!time)
		return;

	time->hours   = t / 3600;
	t %= 3600;
	time->minutes = t / 60;
	time->seconds = t % 60;
}

uint64_t time_to_sec(time_t *t)
{
	return t->hours * 3600 + t->minutes * 60 + t->seconds;
}

static int get_time(struct Uart *uart, uint64_t *time)
{
	if (!uart || !time)
		return -E_BAD_ARG;

	int r = uart_recv_string_n(uart, uart_buff, MAX_ALARMS);
	if (!isdigit(uart_buff[0])) {
		uart_send_string(uart, "finished\r", 0);
		return E_ALARM_NO_INPUT;
	}

	if (r < 0)
		return r;
	
	uint64_t t = atoi64(uart_buff);
	if (t == 0)
		return -E_ALARM_BAD_INPUT;
	
	*time = t;
#if 0
#endif

	r = uart_send_string(uart, "got message: ", 0);
	if (r < 0)
		return r;
		
	r = uart_send_string(uart, itoa(*time, uart_buff), 0);

        r = uart_send_string(uart, "\r", 0);
	if (r < 0)
		return r;

	return 0;
}

static int64_t atoi64(const char* str)
{
	int64_t result = 0;
	int sign = 1;
	int i = 0;

	// skip leading white space
	while (isspace(str[i])) {
		i++;
	}

	// handle optional sign
	if (str[i] == '-') {
		sign = -1;
		i++;
	} else if (str[i] == '+') {
		sign = 1;
		i++;
	}

	// accumulate digits
	while (isdigit(str[i])) {
		result = result * 10 + (str[i] - '0');
		i++;
	}

	// apply sign
	return sign * result;
}

static char *itoa(int num, char *str)
{
	int i = 0;
	int temp = num;

	if (num < 0) {
		num = -num;
		str[i++] = '-';
	}

	do {
		str[i++] = num % 10 + '0';
		num /= 10;
	} while (num);

	str[i] = '\0';

	/* Reverse the string */
	int len = strlen(str);
	for (i = 0; i < len / 2; i++) {
		char tmp = str[i];
		str[i] = str[len - i - 1];
		str[len - i - 1] = tmp;
	}

	return str;
}

struct List *dummy_test()
{
	static struct alarm_time at1, at2;
	at1.time = 5;
	at2.time = 10;

	list_init(&head);
	list_append(&head, (struct List *)&at1);
	list_append((struct List *)&at1, (struct List *)&at2);
}

void dump_timer_list(struct alarm_time *head, struct Uart *uart)
{
	struct List *list = ((struct List *)head)->next;
	int it = 0;

	uart_send_string(uart, ".__________________.\r", 0);
	uart_send_string(uart, "|   TIMERS DUMP    |\r", 0);
	uart_send_string(uart, "|__________________|\r", 0);
	while (list != (struct List *)head) {
		uart_send_string(uart, itoa(it++, uart_buff), 0);
		uart_send_string(uart, ") ", 0);
		uart_send_string(uart, itoa(((struct alarm_time *)list)->time, uart_buff), 0);
		uart_send_string(uart, "\r", 0);

		list = list->next;
	}

	uart_send_string(uart, "____________________\r", 0);
}

struct alarm_time *init_alarm_timers(struct Uart *uart)
{
	static struct alarm_time free_desc[MAX_ALARMS];
	int desc = 0;
	uint64_t time = 0;
	struct alarm_time *at = NULL;
	int r = 0;

	if (!uart)
		return NULL;

	list_init(&head);

	do {
		r = get_time(uart, &time);
		if (r == E_ALARM_NO_INPUT)
			break;
		if (r < 0)
			return NULL;
		
		at       = &free_desc[desc++];
		at->time = time;

		struct List *list = head.next;
		while (list != &head) {
			if (((struct alarm_time *)list)->time >= time) {
				list_append(list->prev, (struct List *)at);
				uart_send_string(uart, "appended between\r", 0);
				break;
			}

			list = list->next;
		}

		if (list == &head) {
			list_append(head.prev, (struct List *)at);
			uart_send_string(uart, "appended at the end\r", 0);
		}

	} while (desc < MAX_ALARMS);

	//dump_timer_list((struct alarm_time *)&head, uart);
	return (struct alarm_time *)&head;
}

bool proccess_timeout(alarm_t *alarm)
{
	if (!alarm)
		return 0;

	if (list_empty(alarm->time_head))
		return 0;

	struct List *lentry = alarm->time_head->next;
	struct alarm_time *at = (struct alarm_time *)(lentry);

	if (alarm->time >= at->time) {
		list_del(lentry);
		return 1;
	}

	return 0;
}

time_t get_closest_hhmmss(struct alarm_time *head)
{
	time_t t = {};
	if (!head)
		return t;

	if (list_empty((struct List *)head))
		return t;

	time_to_hhmmss(((struct alarm_time *)((struct List *)head)->next)->time, &t);

	return t;
}
