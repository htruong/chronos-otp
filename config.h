// !!!! DO NOT EDIT !!!, use: make config

#ifndef _CONFIG_H_
#define _CONFIG_H_

#define CONFIG_FREQUENCY 433
#define OPTION_TIME_DISPLAY 0
#define CONFIG_METRIC_ONLY
#define FIXEDPOINT
#ifndef THIS_DEVICE_ADDRESS
#define THIS_DEVICE_ADDRESS {0xed,0xc0,0xbb,0x25}
#endif // THIS_DEVICE_ADDRESS
// USE_LCD_CHARGE_PUMP is not set
#define USE_WATCHDOG
// DEBUG is not set
// CONFIG_DAY_OF_WEEK is not set
// CONFIG_TEST is not set
#define CONFIG_OTP
// CONFIG_EGGTIMER is not set
#define CONFIG_PHASE_CLOCK
// CONFIG_ALTITUDE is not set
// CONFIG_VARIO is not set
// CONFIG_PROUT is not set
// CONFIG_SIDEREAL is not set
// CONFIG_INFOMEM is not set
// CONFIG_ACCEL is not set
// CONFIG_STRENGTH is not set
// CONFIG_ALARM is not set
// CONFIG_BATTERY is not set
#define CONFIG_CLOCK
// CONFIG_DATE is not set
#define CONFIG_RFBSL
// CONFIG_STOP_WATCH is not set
// CONFIG_TEMP is not set
// CONFIG_USEPPT is not set
// CONFIG_USE_SYNC_TOSET_TIME is not set
// CONFIG_USE_DISCRET_RFBSL is not set
// CONFIG_USE_GPS is not set

#endif // _CONFIG_H_
