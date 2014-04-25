/*
 * timer.h
 *
 * Created: 4/7/2014 9:26:41 PM
 *  Author: Matt
 */ 


#ifndef TIMERCPP_H_
#define TIMERCPP_H_

//Macro names for pre-scaler settings

// Timer/clock prescaler values and timer overflow rates
// tics = rate at which the timer counts up
// 8bitoverflow = rate at which the timer overflows 8bits (or reaches 256)
// 16bit [overflow] = rate at which the timer overflows 16bits (65536)
//
// overflows can be used to generate periodic interrupts
//
// for 8MHz crystal
// 0 = STOP (Timer not counting)
// 1 = CLOCK		tics= 8MHz			8bitoverflow= 31250Hz		16bit= 122.070Hz
// 2 = CLOCK/8		tics= 1MHz			8bitoverflow= 3906.25Hz		16bit=  15.259Hz
// 3 = CLOCK/64		tics= 125kHz		8bitoverflow=  488.28Hz		16bit=   1.907Hz
// 4 = CLOCK/256	tics= 31250Hz		8bitoverflow=  122.07Hz		16bit=	0.477Hz
// 5 = CLOCK/1024	tics= 7812.5Hz		8bitoverflow=   30.52Hz		16bit=   0.119Hz
// 6 = External Clock on T(x) pin (falling edge)
// 7 = External Clock on T(x) pin (rising edge)

// for 4MHz crystal
// 0 = STOP (Timer not counting)
// 1 = CLOCK		tics= 4MHz			8bitoverflow= 15625Hz		16bit=  61.035Hz
// 2 = CLOCK/8		tics= 500kHz		8bitoverflow= 1953.125Hz	16bit=   7.629Hz
// 3 = CLOCK/64		tics= 62500Hz		8bitoverflow=  244.141Hz	16bit=   0.954Hz
// 4 = CLOCK/256	tics= 15625Hz		8bitoverflow=   61.035Hz	16bit=   0.238Hz
// 5 = CLOCK/1024	tics= 3906.25Hz		8bitoverflow=   15.259Hz	16bit=   0.060Hz
// 6 = External Clock on T(x) pin (falling edge)
// 7 = External Clock on T(x) pin (rising edge)

// for 3.69MHz crystal
// 0 = STOP (Timer not counting)
// 1 = CLOCK		tics= 3.69MHz		8bitoverflow= 14414Hz		16bit=  56.304Hz
// 2 = CLOCK/8		tics= 461250Hz		8bitoverflow= 1801.758Hz	16bit=   7.038Hz
// 3 = CLOCK/64		tics= 57625.25Hz	8bitoverflow=  225.220Hz	16bit=   0.880Hz
// 4 = CLOCK/256	tics= 14414.063Hz	8bitoverflow=   56.305Hz	16bit=   0.220Hz
// 5 = CLOCK/1024	tics=  3603.516Hz	8bitoverflow=   14.076Hz	16bit=   0.055Hz
// 6 = External Clock on T(x) pin (falling edge)
// 7 = External Clock on T(x) pin (rising edge)

// for 32.768KHz crystal on timer 2 (use for real-time clock)
// 0 = STOP
// 1 = CLOCK		tics= 32.768kHz		8bitoverflow= 128Hz
// 2 = CLOCK/8		tics= 4096kHz		8bitoverflow=  16Hz
// 3 = CLOCK/32		tics= 1024kHz		8bitoverflow=   4Hz
// 4 = CLOCK/64		tics= 512Hz			8bitoverflow=   2Hz
// 5 = CLOCK/128	tics= 256Hz			8bitoverflow=   1Hz
// 6 = CLOCK/256	tics= 128Hz			8bitoverflow=   0.5Hz
// 7 = CLOCK/1024	tics= 32Hz			8bitoverflow=   0.125Hz

#define TIMER_CLK_STOP			0x00	///< Timer Stopped
#define TIMER_CLK_DIV1			0x01	///< Timer clocked at F_CPU
#define TIMER_CLK_DIV8			0x02	///< Timer clocked at F_CPU/8
#define TIMER_CLK_DIV64			0x03	///< Timer clocked at F_CPU/64
#define TIMER_CLK_DIV256		0x04	///< Timer clocked at F_CPU/256
#define TIMER_CLK_DIV1024		0x05	///< Timer clocked at F_CPU/1024
#define TIMER_CLK_T_FALL		0x06	///< Timer clocked at T falling edge
#define TIMER_CLK_T_RISE		0x07	///< Timer clocked at T rising edge
#define TIMER_PRESCALE_MASK		0x07	///< Timer Prescaler Bit-Mask

#define TIMERRTC_CLK_STOP		0x00	///< RTC Timer Stopped
#define TIMERRTC_CLK_DIV1		0x01	///< RTC Timer clocked at F_CPU
#define TIMERRTC_CLK_DIV8		0x02	///< RTC Timer clocked at F_CPU/8
#define TIMERRTC_CLK_DIV32		0x03	///< RTC Timer clocked at F_CPU/32
#define TIMERRTC_CLK_DIV64		0x04	///< RTC Timer clocked at F_CPU/64
#define TIMERRTC_CLK_DIV128		0x05	///< RTC Timer clocked at F_CPU/128
#define TIMERRTC_CLK_DIV256		0x06	///< RTC Timer clocked at F_CPU/256
#define TIMERRTC_CLK_DIV1024	0x07	///< RTC Timer clocked at F_CPU/1024
#define TIMERRTC_PRESCALE_MASK	0x07	///< RTC Timer Prescaler Bit-Mask

class Timer8 {
	public:
		Timer8();
		explicit Timer8( unsigned char prescale );
		unsigned char GetPrescale();
		void SetPrescale( unsigned char prescale );
	protected:
	private:
	};

class Timer16 {
	public:
		Timer16();
		explicit Timer16( unsigned char prescale );
		unsigned char GetPrescale();
		void SetPrescale( unsigned char prescale );
	protected:
	private:
};



#endif /* TIMERCPP_H_ */