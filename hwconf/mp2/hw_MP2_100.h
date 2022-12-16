/*
	Copyright 2018 Benjamin Vedder	benjamin@vedder.se

	This file is part of the VESC firmware.

	The VESC firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published byNTC_RES
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The VESC firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    */

//Modified from "hw_100_250.h"

#ifndef HW_MP2_100
#define HW_MP2_100

//Preliminary conf file for ***V0.3*** MP2/F405 pill
#define HW_NAME					"MP2"


// HW properties
#define HW_HAS_3_SHUNTS
#define INVERTED_SHUNT_POLARITY
#define HW_HAS_PHASE_FILTERS
#define HW_USE_BRK


// Macros
#define LED_GREEN_GPIO			GPIOC
#define LED_GREEN_PIN			12
#define LED_RED_GPIO			GPIOC
#define LED_RED_PIN				9

#define LED_GREEN_ON()			palSetPad(LED_GREEN_GPIO, LED_GREEN_PIN)
#define LED_GREEN_OFF()			palClearPad(LED_GREEN_GPIO, LED_GREEN_PIN)
#define LED_RED_ON()			palSetPad(LED_RED_GPIO, LED_RED_PIN)
#define LED_RED_OFF()			palClearPad(LED_RED_GPIO, LED_RED_PIN)


// Phase filter
#define PHASE_FILTER_OFF()			palSetPad(GPIOC, 13); palSetPad(GPIOC, 14); palSetPad(GPIOC, 15)
#define PHASE_FILTER_ON()			palClearPad(GPIOC, 13); palClearPad(GPIOC, 14); palClearPad(GPIOC, 15)

#define BRK_GPIO				GPIOB
#define BRK_PIN					12


/*
 * ADC Vector
 *
 * 0  (1):	IN0		SENS1
 * 1  (2):	IN1		SENS2
 * 2  (3):	IN2		SENS3
 * 3  (1):	IN10	CURR1
 * 4  (2):	IN11	CURR2
 * 5  (3):	IN12	CURR3
 * 6  (1):	IN5		ADC_EXT1
 * 7  (2):	IN6		ADC_EXT2
 * 8  (3):	IN3		TEMP_MOS
 * 9  (1):	IN14	TEMP_MOTOR
 * 10 (2):	IN15	ADC_EXT3
 * 11 (3):	IN13	AN_IN
 * 12 (1):	Vrefint
 * 13 (2):	IN0		SENS1
 * 14 (3):	IN1		SENS2
 * 15 (1):  IN8		TEMP_MOS_2
 * 16 (2):  IN9		TEMP_MOS_3
 * 17 (3):  IN3		SENS3
 */

#define HW_ADC_CHANNELS			15 //?
#define HW_ADC_INJ_CHANNELS		3
#define HW_ADC_NBR_CONV			5

// ADC Indexes
#define ADC_IND_SENS1			3
#define ADC_IND_SENS2			4
#define ADC_IND_SENS3			5
#define ADC_IND_CURR1			0
#define ADC_IND_CURR2			1
#define ADC_IND_CURR3			2
#define ADC_IND_VIN_SENS		11
#define ADC_IND_EXT				6 //PA4
#define ADC_IND_EXT2			7 //PA7
#define ADC_IND_TEMP_MOS		10 //? FET temp is on PB1
#define ADC_IND_TEMP_MOTOR		7 //? PA7... also jumpered to A brake
#define ADC_IND_VREFINT			12

// ADC macros and settings

//****************************************************************************************************************************************************
// MP2 has a bug (feature?) where the phase voltage dividers are different from the bus voltage dividers.
// for this reason the VIN_R1 and VIN_R2 are different from the values used in the GET_INPUT_VOLTAGE macro
//****************************************************************************************************************************************************



// Component parameters (can be overridden)
#ifndef V_REG
#define V_REG					3.30
#endif
#ifndef VIN_R1
#define VIN_R1					100000.0
#endif
#ifndef VIN_R2
#define VIN_R2					3300.0 // single value for phase voltage dividers   
#endif
#ifndef CURRENT_AMP_GAIN
#define CURRENT_AMP_GAIN		21.88
#endif
#ifndef CURRENT_SHUNT_RES
#define CURRENT_SHUNT_RES		(0.0005 / 3.0) //3 0.5 mOhm shunts in parallel
#endif

// Input voltage
#ifndef VIN_BUS_R1
#define VIN_BUS_R1					270000.0
#endif
#ifndef VIN_BUS_R2
//#define VIN_BUS_R2					11000.0 // default value, for 16s max batteries, all solder jumpers open.
#define VIN_BUS_R2					8913.8 // for 20s max batteries, 100v solder jumper soldered.
//#define VIN_BUS_R2					5958.3 // for 30s max batteries, 150v solder jumper soldered. 
#endif

#define GET_INPUT_VOLTAGE()		((V_REG / 4095.0) * (float)ADC_Value[ADC_IND_VIN_SENS] * ((VIN_BUS_R1 + VIN_BUS_R2) / VIN_BUS_R2))
// #define GET_INPUT_VOLTAGE()		((V_REG / 4095.0) * (float)ADC_Value[ADC_IND_VIN_SENS] * ((VIN_R1 + VIN_R2) / VIN_R2))  // default macro, not used for MP2 v0.3 because of separate values for phase and bus voltage dividers

// NTC Termistors
#define NTC_RES(adc_val)		((4095.0 * 10000.0) / (4095.0-adc_val) - 10000.0)
#define NTC_TEMP(adc_ind)		hwMP2_get_temp()

#define NTC_RES_MOTOR(adc_val)	(10000.0 / ((4095.0 / (float)adc_val) - 1.0)) // Motor temp sensor on low side
#define NTC_TEMP_MOTOR(beta)	(1.0 / ((logf(NTC_RES_MOTOR(ADC_Value[ADC_IND_TEMP_MOTOR]) / 10000.0) / beta) + (1.0 / 298.15)) - 273.15)

#define NTC_TEMP_MOS1()			(1.0 / ((logf(NTC_RES(ADC_Value[ADC_IND_TEMP_MOS]) / 10000.0) / 3380.0) + (1.0 / 298.15)) - 273.15)
#define NTC_TEMP_MOS2()	(0.0)//		(1.0 / ((logf(NTC_RES(ADC_Value[ADC_IND_TEMP_MOS_2]) / 10000.0) / 3380.0) + (1.0 / 298.15)) - 273.15)
#define NTC_TEMP_MOS3()	(0.0)//		(1.0 / ((logf(NTC_RES(ADC_Value[ADC_IND_TEMP_MOS_3]) / 10000.0) / 3380.0) + (1.0 / 298.15)) - 273.15)

// Voltage on ADC channel
#define ADC_VOLTS(ch)			((float)ADC_Value[ch] / 4096.0 * V_REG)

// COMM-port ADC GPIOs
//#define HW_ADC_EXT_GPIO			GPIOA
//#define HW_ADC_EXT_PIN			5
//#define HW_ADC_EXT2_GPIO		GPIOA
//#define HW_ADC_EXT2_PIN			6

// UART Peripheral
#define HW_UART_DEV				SD3
#define HW_UART_GPIO_AF			GPIO_AF_USART3
#define HW_UART_TX_PORT			GPIOB
#define HW_UART_TX_PIN			10
#define HW_UART_RX_PORT			GPIOB
#define HW_UART_RX_PIN			11

// Permanent UART Peripheral (for NRF51)
#define HW_UART_P_BAUD			115200
#define HW_UART_P_DEV			SD1
#define HW_UART_P_GPIO_AF		GPIO_AF_USART1
#define HW_UART_P_TX_PORT		GPIOB
#define HW_UART_P_TX_PIN		6
#define HW_UART_P_RX_PORT		GPIOB
#define HW_UART_P_RX_PIN		7

// ICU Peripheral for servo decoding
#define HW_USE_SERVO_TIM4
#define HW_ICU_TIMER			TIM4
#define HW_ICU_TIM_CLK_EN()		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE)
#define HW_ICU_DEV				ICUD4
#define HW_ICU_CHANNEL			ICU_CHANNEL_1
#define HW_ICU_GPIO_AF			GPIO_AF_TIM4
#define HW_ICU_GPIO				GPIOB
#define HW_ICU_PIN				6

// I2C Peripheral
#define HW_I2C_DEV				I2CD2
#define HW_I2C_GPIO_AF			GPIO_AF_I2C2
#define HW_I2C_SCL_PORT			GPIOB
#define HW_I2C_SCL_PIN			10
#define HW_I2C_SDA_PORT			GPIOB
#define HW_I2C_SDA_PIN			11

// Hall/encoder pins
#define HW_HALL_ENC_GPIO1		GPIOC
#define HW_HALL_ENC_PIN1		6
#define HW_HALL_ENC_GPIO2		GPIOC
#define HW_HALL_ENC_PIN2		7
#define HW_HALL_ENC_GPIO3		GPIOC
#define HW_HALL_ENC_PIN3		8
#define HW_ENC_TIM				TIM3
#define HW_ENC_TIM_AF			GPIO_AF_TIM3
#define HW_ENC_TIM_CLK_EN()		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE)
#define HW_ENC_EXTI_PORTSRC		EXTI_PortSourceGPIOC
#define HW_ENC_EXTI_PINSRC		EXTI_PinSource8
#define HW_ENC_EXTI_CH			EXTI9_5_IRQn
#define HW_ENC_EXTI_LINE		EXTI_Line8
#define HW_ENC_EXTI_ISR_VEC		EXTI9_5_IRQHandler
#define HW_ENC_TIM_ISR_CH		TIM3_IRQn
#define HW_ENC_TIM_ISR_VEC		TIM3_IRQHandler

// SPI pins
//#define HW_SPI_DEV				SPID1
//#define HW_SPI_GPIO_AF			GPIO_AF_SPI1
//#define HW_SPI_PORT_NSS			GPIOA
//#define HW_SPI_PIN_NSS			4
//#define HW_SPI_PORT_SCK			GPIOA
//#define HW_SPI_PIN_SCK			5 //Missing from F405 pill, TO BE UPDATED IN FUTURE F405 PILLs
//#define HW_SPI_PORT_MOSI		GPIOA
//#define HW_SPI_PIN_MOSI			7
//#define HW_SPI_PORT_MISO		GPIOA
//#define HW_SPI_PIN_MISO			6

// SPI pins
#define HW_SPI_DEV				SPID1
#define HW_SPI_GPIO_AF			GPIO_AF_SPI1
#define HW_SPI_PORT_NSS			GPIOA
#define HW_SPI_PIN_NSS			4
#define HW_SPI_PORT_SCK			GPIOA
#define HW_SPI_PIN_SCK			5
#define HW_SPI_PORT_MOSI		GPIOA
#define HW_SPI_PIN_MOSI			7
#define HW_SPI_PORT_MISO		GPIOA
#define HW_SPI_PIN_MISO			6


// Measurement macros
#define ADC_V_L1				ADC_Value[ADC_IND_SENS1]
#define ADC_V_L2				ADC_Value[ADC_IND_SENS2]
#define ADC_V_L3				ADC_Value[ADC_IND_SENS3]
#define ADC_V_ZERO				(ADC_Value[ADC_IND_VIN_SENS] / 2)

// Macros
#define READ_HALL1()			palReadPad(HW_HALL_ENC_GPIO1, HW_HALL_ENC_PIN1)
#define READ_HALL2()			palReadPad(HW_HALL_ENC_GPIO2, HW_HALL_ENC_PIN2)
#define READ_HALL3()			palReadPad(HW_HALL_ENC_GPIO3, HW_HALL_ENC_PIN3)

// Override dead time. See the stm32f4 reference manual for calculating this value.
#define HW_DEAD_TIME_NSEC		900.0

// Default setting overrides
#ifndef MCCONF_L_MIN_VOLTAGE
#define MCCONF_L_MIN_VOLTAGE			38.0		// Minimum input voltage due to DCDC brick cutout
#endif
#ifndef MCCONF_L_MAX_VOLTAGE
#define MCCONF_L_MAX_VOLTAGE			90.0	// Maximum input voltage for 100v power stage / 20s max battery (100V power stage solder jumper soldered
#endif
#ifndef MCCONF_DEFAULT_MOTOR_TYPE
#define MCCONF_DEFAULT_MOTOR_TYPE		MOTOR_TYPE_FOC
#endif
#ifndef MCCONF_FOC_F_ZV
#define MCCONF_FOC_F_ZV					30000.0
#endif
#ifndef MCCONF_L_MAX_ABS_CURRENT
#define MCCONF_L_MAX_ABS_CURRENT		320.0	// The maximum absolute current above which a fault is generated
#endif
#ifndef MCCONF_FOC_SAMPLE_V0_V7
#define MCCONF_FOC_SAMPLE_V0_V7			false	// Run control loop in both v0 and v7 (requires phase shunts)
#endif
#ifndef MCCONF_L_IN_CURRENT_MAX
#define MCCONF_L_IN_CURRENT_MAX			150.0	// Input current limit in Amperes (Upper)
#endif
#ifndef MCCONF_L_IN_CURRENT_MIN
#define MCCONF_L_IN_CURRENT_MIN			-150.0	// Input current limit in Amperes (Lower)
#endif

// Setting limits
#define HW_LIM_CURRENT			-300.0, 300.0
#define HW_LIM_CURRENT_IN		-300.0, 300.0
#define HW_LIM_CURRENT_ABS		0.0, 350.0


#define HW_LIM_VIN				36.0, 90.0

#define HW_LIM_ERPM				-200e3, 200e3
#define HW_LIM_DUTY_MIN			0.0, 0.1
#define HW_LIM_DUTY_MAX			0.0, 0.99
#define HW_LIM_TEMP_FET			-40.0, 95.0

// HW-specific functions
float hwMP2_get_temp(void);

#endif /* HW_MP2_100 */
