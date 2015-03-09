// SDP_timer.h

#ifndef _SDP_TIMER_H_
#define _SDP_TIMER_H_

// TIMER Commands
#define ADI_SDP_CMD_GROUP_TIMER             0xCA000900
#define ADI_SDP_CMD_TIMER_ENABLE            0xCA000901
#define ADI_SDP_CMD_TIMER_DISABLE           0xCA000902
#define ADI_SDP_CMD_TIMER_CONFIG			0xCA000903
#define	ADI_SDP_CMD_TIMER_UPDATE_PULSE		0xCA000904
#define ADI_SDP_CMD_TIMER_UPDATE_PERIOD		0xCA000905

void processTimerCmd(SDP_USB_HEADER *pUsbHeader);

void generateTimer(u32 period, u32 pulse_duration, u8 timerPin, bool polarity);
void disableTimer(u8 timerPin);
void updateTimerDutyCycle(u32 pulse_duration, u8 timerPin);
void updateTimerPeriod(u32 period, u8 timerPin);
void enableTimer(u8 timerPin);


#endif
