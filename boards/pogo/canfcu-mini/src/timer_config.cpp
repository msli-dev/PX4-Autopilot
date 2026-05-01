/****************************************************************************
 *
 *   Copyright (C) 2012 PX4 Development Team. All rights reserved.
 *
 ****************************************************************************/

#include <px4_arch/io_timer_hw_description.h>

/* Timer allocation
 *
 * PWM1  TIM2_CH4  PA3
 * PWM2  TIM2_CH3  PA2
 * PWM3  TIM2_CH2  PA1
 * PWM4  TIM2_CH1  PA0
 *
 * PWM5  TIM1_CH1  PE9
 * PWM6  TIM1_CH2  PE11
 * PWM7  TIM1_CH3  PE13
 * PWM8  TIM1_CH4  PE14
 *
 * TIM3_CH3  PB0   BUZZER_1  - driven by tone alarm driver
 * TIM4_CH4  PD15  RCIN      - driven by RC/PWM input driver
 */

constexpr io_timers_t io_timers[MAX_IO_TIMERS] = {
	initIOTimer(Timer::Timer2, DMA{DMA::Index1}),
	initIOTimer(Timer::Timer1, DMA{DMA::Index1}),
};

constexpr timer_io_channels_t timer_io_channels[MAX_TIMER_IO_CHANNELS] = {
	initIOTimerChannel(io_timers, {Timer::Timer2, Timer::Channel4}, {GPIO::PortA, GPIO::Pin3}),
	initIOTimerChannel(io_timers, {Timer::Timer2, Timer::Channel3}, {GPIO::PortA, GPIO::Pin2}),
	initIOTimerChannel(io_timers, {Timer::Timer2, Timer::Channel2}, {GPIO::PortA, GPIO::Pin1}),
	initIOTimerChannel(io_timers, {Timer::Timer2, Timer::Channel1}, {GPIO::PortA, GPIO::Pin0}),
	initIOTimerChannel(io_timers, {Timer::Timer1, Timer::Channel1}, {GPIO::PortE, GPIO::Pin9}),
	initIOTimerChannel(io_timers, {Timer::Timer1, Timer::Channel2}, {GPIO::PortE, GPIO::Pin11}),
	initIOTimerChannel(io_timers, {Timer::Timer1, Timer::Channel3}, {GPIO::PortE, GPIO::Pin13}),
	initIOTimerChannel(io_timers, {Timer::Timer1, Timer::Channel4}, {GPIO::PortE, GPIO::Pin14}),
};

constexpr io_timers_channel_mapping_t io_timers_channel_mapping =
	initIOTimerChannelMapping(io_timers, timer_io_channels);
