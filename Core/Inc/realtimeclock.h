#ifndef INC_REALTIMECLOCK_H_
#define INC_REALTIMECLOCK_H_

#include <stdint.h>

#define DS1307_OK 					(0)
#define DS1307_ERR					(-1)
#define DS1307_IC2_ERR				(-2)
#define DS1307_ADDRESS 				(0xD0)
#define MAX_RETRY					(3)

#define DS1307_SECONDS				(0x00)
#define DS1307_MINUTES				(0x01)
#define DS1307_HOURS				(0x02)
#define DS1307_DATE					(0x04)
#define DS1307_MONTH				(0x05)
#define DS1307_YEAR					(0x06)
#define DS1307_CONTROL				(0x07)

#define ADDRESS_SIZE				(1)
#define DATE_SIZE					(7)

/* data and hour MACRO */
#define BUILD_CURRENT_SECONDS  		(( __TIME__[6] - '0') * 10 + __TIME__[7] - '0')
#define BUILD_CURRENT_MINUTES  		(( __TIME__[3] - '0') * 10 + __TIME__[4] - '0')
#define BUILD_CURRENT_HOURS  		(( __TIME__[0] - '0') * 10 + __TIME__[1] - '0')
#define BUILD_CURRENT_DATE  		(((__DATE__[4] >= '0') ? (__DATE__[4] - '0') * 10 : 0) + (__DATE__[5] - '0'))
#define BUILD_CURRENT_YEAR  		(( __DATE__[9] - '0') * 10 + __DATE__[10] - '0')

typedef struct {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t date;
	uint8_t month;
	uint8_t year;
} date_time_t;

extern date_time_t date_time;
extern uint8_t rtc_data[DATE_SIZE];

uint8_t RTC_Init();
int8_t rtc_get_date_time(date_time_t *datetime);
int8_t rtc_set_date_time(date_time_t *datetime);
uint8_t bcd2Dec(uint8_t);
uint8_t dec2Bcd(uint8_t);

void rtc_set_date_time_default(date_time_t *datetime);
void set_current_date(date_time_t *datetime);


#endif /* INC_REALTIMECLOCK_H_ */
