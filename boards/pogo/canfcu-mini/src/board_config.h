/****************************************************************************
 *
 *   Copyright (c) 2022 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file board_config.h
 *
 * Pogo CANFCU Mini internal definitions
 */

#pragma once

/****************************************************************************************************
 * Included Files
 ****************************************************************************************************/

#include <px4_platform_common/px4_config.h>
#include <nuttx/compiler.h>
#include <stdint.h>


#include <stm32_gpio.h>

/****************************************************************************************************
 * Definitions
 ****************************************************************************************************/

#undef TRACE_PINS

/* PX4IO connection configuration

#define BOARD_USES_PX4IO_VERSION       2
#define PX4IO_SERIAL_DEVICE            "/dev/ttyS4"
#define PX4IO_SERIAL_TX_GPIO           GPIO_USART6_TX
#define PX4IO_SERIAL_RX_GPIO           GPIO_USART6_RX
#define PX4IO_SERIAL_BASE              STM32_USART6_BASE
#define PX4IO_SERIAL_VECTOR            STM32_IRQ_USART6
#define PX4IO_SERIAL_TX_DMAMAP         DMAMAP_USART6_TX
#define PX4IO_SERIAL_RX_DMAMAP         DMAMAP_USART6_RX
#define PX4IO_SERIAL_RCC_REG           STM32_RCC_APB2ENR
#define PX4IO_SERIAL_RCC_EN            RCC_APB2ENR_USART6EN
#define PX4IO_SERIAL_CLOCK             STM32_PCLK2_FREQUENCY
#define PX4IO_SERIAL_BITRATE           1500000               // 1.5Mbps -> max rate for IO
*/

//
/* PX4FMU GPIOs ***********************************************************************************/


/* No onboard status LEDs. */

#define BOARD_HAS_CONTROL_STATUS_LEDS 0
// #define GPIO_nLED_BLUE        /* PE3 */  (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|GPIO_OUTPUT_SET|GPIO_PORTE|GPIO_PIN3)
// #define GPIO_nLED_GREEN       /* PE4 */  (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_50MHz|GPIO_OUTPUT_SET|GPIO_PORTE|GPIO_PIN4)

// #define BOARD_HAS_CONTROL_STATUS_LEDS 1
// #define BOARD_OVERLOAD_LED            LED_BLUE
// #define BOARD_ARMED_STATE_LED         LED_GREEN

/*
 * No onboard ADC power sensing.
 *
 * Battery, power, barometer and magnetometer data are expected to be provided
 * by external CAN/DroneCAN nodes where applicable.
 */
/* ADC defines to be used in sensors.cpp to read from a particular channel */


#define ADC1_CH(n) (0)

#define BOARD_NUMBER_BRICKS 1

#define ADC_BATTERY_VOLTAGE_CHANNEL   (-1)
#define ADC_BATTERY_CURRENT_CHANNEL   (-1)
#define ADC_BATTERY2_VOLTAGE_CHANNEL  (-1)
#define ADC_BATTERY2_CURRENT_CHANNEL  (-1)
#define ADC_AIRSPEED_IN_CHANNEL       (-1)
#define ADC_RSSI_IN_CHANNEL           (-1)

#define ADC_CHANNELS                  (0)

#define BOARD_BATTERY1_V_DIV          (1.0f)
#define BOARD_BATTERY1_A_PER_V        (1.0f)
#define BOARD_BATTERY2_V_DIV          (1.0f)
#define BOARD_BATTERY2_A_PER_V        (1.0f)

/* No onboard power-valid sensing. Keep analog_battery.cpp buildable. */
#define BOARD_ADC_BRICK_VALID         (false)
#define BOARD_ADC_USB_CONNECTED       (false)
#define BOARD_ADC_USB_VALID           (false)
#define BOARD_ADC_SERVO_VALID         (true)



// #define HW_REV_VER_ADC_BASE STM32_ADC3_BASE

// #define SYSTEM_ADC_BASE STM32_ADC1_BASE

// /* HW has to large of R termination on ADC todo:change when HW value is chosen */
// #define BOARD_ADC_OPEN_CIRCUIT_V     (5.6f)

// /* HW Version and Revision drive signals Default to 1 to detect */
// #define BOARD_HAS_HW_VERSIONING

// #define GPIO_HW_VER_REV_DRIVE  /* PE12 */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_SET|GPIO_PORTE|GPIO_PIN12)
// #define GPIO_HW_REV_SENSE      /* PC0 */  GPIO_ADC123_INP10
// #define GPIO_HW_VER_SENSE      /* PC1 */  GPIO_ADC123_INP11
// #define HW_INFO_INIT_PREFIX    "V6C"


//                 Base/FMUM
// #define V6C00   HW_VER_REV(0x0,0x0) // FMUV6C,                 Rev 0  I2C4 External but with Internal devices
// #define V6C01   HW_VER_REV(0x0,0x1) // FMUV6C,                 Rev 1  I2C4 Internal I2C2 External
// #define V6C02   HW_VER_REV(0x0,0x2) // FMUV6C,                 Rev 2  I2C4 Internal I2C2 External,BMI088+ICM-42688P
// #define V6C10   HW_VER_REV(0x1,0x0) // NO PX4IO,               Rev 0  I2C4 External but with Internal devices
// #define V6C11   HW_VER_REV(0x1,0x1) // NO PX4IO,               Rev 1  I2C4 Internal I2C2 External
// #define V6C21   HW_VER_REV(0x2,0x1) // FMUV6CMini,             Rev 1  I2C4 Internal I2C2 External
// #define V6C22   HW_VER_REV(0x2,0x2) // FMUV6CMini,             Rev 2  I2C4 Internal I2C2 External,BMI088+ICM-42688P

/* PWM
 */
#define DIRECT_PWM_OUTPUT_CHANNELS   8
#define DIRECT_INPUT_TIMER_CHANNELS  8
#define BOARD_HAS_PWM                DIRECT_PWM_OUTPUT_CHANNELS


/* Power supply control and monitoring GPIOs */

// #define GPIO_nPOWER_IN_A                /* PA15  */ (GPIO_INPUT|GPIO_PULLUP|GPIO_PORTA|GPIO_PIN15)
// #define GPIO_nPOWER_IN_B                /* PB12  */ (GPIO_INPUT|GPIO_PULLUP|GPIO_PORTB|GPIO_PIN12)
// #define GPIO_nPOWER_IN_C                /* PE15  */ (GPIO_INPUT|GPIO_PULLUP|GPIO_PORTE|GPIO_PIN15)

// #define GPIO_nVDD_BRICK1_VALID          GPIO_nPOWER_IN_A /* Brick 1 Is Chosen */
// #define GPIO_nVDD_BRICK2_VALID          GPIO_nPOWER_IN_B /* Brick 2 Is Chosen  */
// #define BOARD_NUMBER_BRICKS             2
// #define GPIO_nVDD_USB_VALID             GPIO_nPOWER_IN_C /* USB     Is Chosen */

// #define GPIO_VDD_5V_PERIPH_nEN          /* PE2  */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_SET|GPIO_PORTE|GPIO_PIN2)
// #define GPIO_VDD_5V_PERIPH_nOC          /* PE3  */ (GPIO_INPUT |GPIO_FLOAT|GPIO_PORTE|GPIO_PIN3)
// #define GPIO_VDD_5V_HIPOWER_nEN         /* PC10 */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_SET|GPIO_PORTC|GPIO_PIN10)
// #define GPIO_VDD_5V_HIPOWER_nOC         /* PC11 */ (GPIO_INPUT |GPIO_FLOAT|GPIO_PORTC|GPIO_PIN11)
// #define GPIO_VDD_3V3_SENSORS_EN         /* PB2  */ (GPIO_OUTPUT|GPIO_PUSHPULL|GPIO_SPEED_2MHz|GPIO_OUTPUT_CLEAR|GPIO_PORTB|GPIO_PIN2)

// /* Define True logic Power Control in arch agnostic form */

// #define VDD_5V_PERIPH_EN(on_true)          px4_arch_gpiowrite(GPIO_VDD_5V_PERIPH_nEN, !(on_true))
// #define VDD_5V_HIPOWER_EN(on_true)         px4_arch_gpiowrite(GPIO_VDD_5V_HIPOWER_nEN, !(on_true))
// #define VDD_3V3_SENSORS_EN(on_true)       px4_arch_gpiowrite(GPIO_VDD_3V3_SENSORS_EN, (on_true))

/* Tone alarm output */

/* USB OTG FS
 *
 * PA9  OTG_FS_VBUS VBUS sensing
 */
#define GPIO_OTGFS_VBUS         /* PA9 */ (GPIO_INPUT|GPIO_PULLDOWN|GPIO_SPEED_100MHz|GPIO_PORTA|GPIO_PIN9)

/* High-resolution timer */
#define HRT_TIMER               12  /* use timer12 for the HRT */
#define HRT_TIMER_CHANNEL       1  /* use capture/compare channel 1 */

/* PWM input driver. Use FMU AUX5 pins attached to timer4 channel 3 */


#define SDIO_SLOTNO                    0  /* Only one slot */
#define SDIO_MINOR                     0

/* SD card bringup does not work if performed on the IDLE thread because it
 * will cause waiting.  Use either:
 *
 *  CONFIG_BOARDCTL=y, OR
 *  CONFIG_BOARD_INITIALIZE=y && CONFIG_BOARD_INITTHREAD=y
 */

#if defined(CONFIG_BOARD_INITIALIZE) && !defined(CONFIG_BOARDCTL) && \
   !defined(CONFIG_BOARD_INITTHREAD)
#  warning SDIO initialization cannot be perfomed on the IDLE thread
#endif

/* By Providing BOARD_ADC_USB_CONNECTED (using the px4_arch abstraction)
 * this board support the ADC system_power interface, and therefore
 * provides the true logic GPIO BOARD_ADC_xxxx macros.
 */
// #define BOARD_ADC_USB_CONNECTED (px4_arch_gpioread(GPIO_OTGFS_VBUS))
// #define BOARD_ADC_USB_VALID     (!px4_arch_gpioread(GPIO_nVDD_USB_VALID))

// /* FMUv6C never powers off the Servo rail */

// #define BOARD_ADC_SERVO_VALID     (1)

// #define BOARD_ADC_BRICK1_VALID  (!px4_arch_gpioread(GPIO_nVDD_BRICK1_VALID))
// #define BOARD_ADC_BRICK2_VALID  (!px4_arch_gpioread(GPIO_nVDD_BRICK2_VALID))

// #define BOARD_ADC_PERIPH_5V_OC  (!px4_arch_gpioread(GPIO_VDD_5V_PERIPH_nOC))
// #define BOARD_ADC_HIPOWER_5V_OC (!px4_arch_gpioread(GPIO_VDD_5V_HIPOWER_nOC))


/* This board provides a DMA pool and APIs */
#define BOARD_DMA_ALLOC_POOL_SIZE 5120

/* This board provides the board_on_reset interface */

#define BOARD_HAS_ON_RESET 1
#define BOARD_HAS_NO_INTERNAL_MAG  1
#define BOARD_HAS_NO_INTERNAL_BARO 1

#define PX4_GPIO_INIT_LIST { \
		GPIO_CAN1_TX, \
		GPIO_CAN1_RX, \
		GPIO_CAN2_TX, \
		GPIO_CAN2_RX, \
	}

#define BOARD_ENABLE_CONSOLE_BUFFER

// #define PX4_I2C_BUS_MTD      4,5

//#define BOARD_OVERRIDE_I2C_DEVICE_EXTERNAL


#define BOARD_NUM_IO_TIMERS 2

__BEGIN_DECLS

/****************************************************************************************************
 * Public Types
 ****************************************************************************************************/

/****************************************************************************************************
 * Public data
 ****************************************************************************************************/

#ifndef __ASSEMBLY__

/****************************************************************************************************
 * Public Functions
 ****************************************************************************************************/

/****************************************************************************
 * Name: stm32_sdio_initialize
 *
 * Description:
 *   Initialize SDIO-based MMC/SD card support
 *
 ****************************************************************************/

int stm32_sdio_initialize(void);

/****************************************************************************************************
 * Name: stm32_spiinitialize
 *
 * Description:
 *   Called to configure SPI chip select GPIO pins for the PX4FMU board.
 *
 ****************************************************************************************************/

extern void stm32_spiinitialize(void);

extern void stm32_usbinitialize(void);

extern void board_peripheral_reset(int ms);

#include <px4_platform_common/board_common.h>

#endif /* __ASSEMBLY__ */

__END_DECLS
