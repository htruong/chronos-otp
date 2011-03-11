// *************************************************************************************************
//
//	Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
//
//
//	  Redistribution and use in source and binary forms, with or without
//	  modification, are permitted provided that the following conditions
//	  are met:
//
//	    Redistributions of source code must retain the above copyright
//	    notice, this list of conditions and the following disclaimer.
//
//	    Redistributions in binary form must reproduce the above copyright
//	    notice, this list of conditions and the following disclaimer in the
//	    documentation and/or other materials provided with the
//	    distribution.
//
//	    Neither the name of Texas Instruments Incorporated nor the names of
//	    its contributors may be used to endorse or promote products derived
//	    from this software without specific prior written permission.
//
//	  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//	  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//	  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//	  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//	  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//	  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//	  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//	  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//	  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//	  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//	  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// *************************************************************************************************
// Time functions.
// *************************************************************************************************


// *************************************************************************************************
// Include section

// system
#include "project.h"

// driver
#include "ports.h"
#include "display.h"
#include "timer.h"

// logic
#include "menu.h"
#include "clock.h"
#include "user.h"

#include "date.h"

#include "rfsimpliciti.h"


// *************************************************************************************************
// Prototypes section
void reset_clock(void);
void clock_tick(void);
void mx_time(u8 line);
void sx_time(u8 line);

u8 current_hour, current_minute, current_second;


// *************************************************************************************************
// Defines section


// *************************************************************************************************
// Global Variable section
struct time sTime;

// Display values for time format selection
#if (OPTION_TIME_DISPLAY == CLOCK_DISPLAY_SELECT)
const u8 selection_Timeformat[][4] =
{
	"24H", "12H"
};
#endif


// *************************************************************************************************
// Extern section

// *************************************************************************************************
// @fn          reset_clock
// @brief       Resets clock time to 00:00:00, 24H time format.
// @param       none
// @return      none
// *************************************************************************************************
void reset_clock(void)
{
	// Set global system time to 0
	sTime.system_time = 0;
	sTime.force_resync = 1;
	
	// Display style of both lines is default (HH:MM)
	sTime.line1ViewStyle = DISPLAY_DEFAULT_VIEW;
	sTime.line2ViewStyle = DISPLAY_DEFAULT_VIEW;

	// Reset timeout detection
	sTime.last_activity               = 0;
	
	sTime.UTCoffset				  = 0;
}


// *************************************************************************************************
// @fn          clock_tick
// @brief       Add 1 second to system time and to display time
// @param       none
// @return      none
// *************************************************************************************************
void clock_tick(void)
{
	// Use sTime.drawFlag to minimize display updates
	// sTime.drawFlag = 1: second
	// sTime.drawFlag = 2: minute, second
	// sTime.drawFlag = 3: hour, minute
	sTime.drawFlag = 1;

	// Increase global system time
	sTime.system_time++;
	
	if (!sTime.force_resync) {
	  // Add 1 second
	  sTime.second++;

	  // Add 1 minute
	  if (sTime.second == 60)
	  {
		  sTime.second = 0;
		  sTime.minute++;
		  sTime.drawFlag++;

		  // Add 1 hour
		  if (sTime.minute == 60)
		  {
			  sTime.minute = 0;
			  sTime.hour++;
			  sTime.drawFlag++;

			  // Resync
			  if (sTime.hour == 24)
			  {
				  sTime.hour = 0;
				  sTime.force_resync = 1;
				  clock_resync();
			  }
		  }
	  }
	} else {
	  clock_resync();
	  sTime.drawFlag = 3;
	}
}

void clock_resync(void)
{
  u32 calc_time;
  if(sTime.UTCoffset == 0) {
	calc_time = sTime.system_time;
  } else {
	// God please let this be right.
	calc_time = (u32)(sTime.system_time + (s32)(sTime.UTCoffset)*3600);
  }

  sTime.second = calc_time % 60;
  sTime.minute = (calc_time / 60) % 60;
  sTime.hour = (calc_time / 3600) % 24;
  sTime.drawFlag = 3;
  sTime.force_resync = 0;
}


// *************************************************************************************************
// @fn          convert_hour_to_12H_format
// @brief       Convert internal 24H time to 12H time.
// @param       u8 hour		Hour in 24H format
// @return      u8				Hour in 12H format
// *************************************************************************************************
#if (OPTION_TIME_DISPLAY > CLOCK_24HR)
u8 convert_hour_to_12H_format(u8 hour)
{
	// 00:00 .. 11:59 --> AM 12:00 .. 11:59
	if (hour == 0)			return (hour + 12);
	else if (hour <= 12)	return (hour);
	// 13:00 .. 23:59 --> PM 01:00 .. 11:59
	else  					return (hour - 12);
}


// *************************************************************************************************
// @fn          is_hour_am
// @brief       Checks if internal 24H time is AM or PM
// @param       u8 hour		Hour in 24H format
// @return      u8				1 = AM, 0 = PM
// *************************************************************************************************
u8 is_hour_am(u8 hour)
{
	// 00:00 .. 11:59 --> AM 12:00 .. 11:59
	if (hour < 12)	return (1);
	// 12:00 .. 23:59 --> PM 12:00 .. 11:59
	else  			return (0);
}

#if (OPTION_TIME_DISPLAY == CLOCK_DISPLAY_SELECT)
// *************************************************************************************************
// @fn          display_selection_Timeformat
// @brief       Display time format 12H / 24H.
// @param       u8 segments			Target segments where to display information
//				u32 index			0 or 1, index for value string
//				u8 digits			Not used
//				u8 blanks			Not used
// @return      none
// *************************************************************************************************
void display_selection_Timeformat1(u8 segments, u32 index, u8 digits, u8 blanks, u8 dummy)
{
	if (index < 2) display_chars(segments, (u8 *)selection_Timeformat[index], SEG_ON_BLINK_ON);
}
#endif // CLOCK_DISPLAY_SELECT

#endif //OPTION_TIME_DISPLAY


// *************************************************************************************************
// @fn          mx_time
// @brief       Clock set routine.
// @param       u8 line		LINE1, LINE2
// @return      none
// *************************************************************************************************
void mx_time(u8 line)
{
  u8 select;

  // Clear display
  clear_display_all();
  
  // Sync only
  display_chars(LCD_SEG_L2_5_0, (u8 *)"SYNCo", SEG_ON);
}


// *************************************************************************************************
// @fn          sx_time
// @brief       Time user routine. Toggles view style between HH:MM and SS.
// @param       line		LINE1
// @return      none
// *************************************************************************************************
void sx_time(u8 line)
{
	// Toggle display view style
	if (sTime.line1ViewStyle == DISPLAY_DEFAULT_VIEW) 	sTime.line1ViewStyle = DISPLAY_ALTERNATIVE_VIEW;
	else 									 			sTime.line1ViewStyle = DISPLAY_DEFAULT_VIEW;
}

// *************************************************************************************************
// @fn          display_time
// @brief       Clock display routine. Supports 24H and 12H time format,
//              through the helper display_hours_with_12_24.
// @param       u8 line			LINE1
//				u8 update		DISPLAY_LINE_UPDATE_FULL, DISPLAY_LINE_UPDATE_PARTIAL
// @return      none
// *************************************************************************************************
void display_time(u8 line, u8 update)
{
  if (update == DISPLAY_LINE_UPDATE_PARTIAL) {
	if (!sTime.drawFlag) return;
	if (line == LINE2) display_chars(LCD_SEG_L2_1_0, itoa(sTime.second, 2, 0), SEG_ON); 
	if (sTime.drawFlag > 2) display_chars(switch_seg(line, LCD_SEG_L1_3_2, LCD_SEG_L2_3_2), itoa(sTime.hour, 2, 0), SEG_ON);
	if (sTime.drawFlag > 1) display_chars(switch_seg(line, LCD_SEG_L1_1_0, LCD_SEG_L2_1_0), itoa(sTime.minute, 2, 0), SEG_ON);
  } else if (update == DISPLAY_LINE_UPDATE_FULL) {
	if (line == LINE2) display_chars(LCD_SEG_L2_1_0, itoa(sTime.second, 2, 0), SEG_ON);
	display_chars(switch_seg(line, LCD_SEG_L1_3_2, LCD_SEG_L2_3_2), itoa(sTime.hour, 2, 0), SEG_ON);
	display_chars(switch_seg(line, LCD_SEG_L1_1_0, LCD_SEG_L2_1_0), itoa(sTime.minute, 2, 0), SEG_ON);
	display_symbol(switch_seg(line, LCD_SEG_L1_COL, LCD_SEG_L2_COL0), SEG_ON_BLINK_ON);
  } else if (update == DISPLAY_LINE_CLEAR) {
	display_symbol(switch_seg(line, LCD_SEG_L1_COL, LCD_SEG_L2_COL0), SEG_OFF_BLINK_OFF);
	display_symbol(LCD_SYMB_AM, SEG_OFF);
  }
  
  sTime.drawFlag = 0;
}

