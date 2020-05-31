#pragma once

// System Clock Speed
#define F_CPU (16000000UL)

/********************* LOAD REGULATOR *********************/
// SPI
#define SCH_SPI_OP_MODE (HAL_SPI::SPI_MASTER)
#define SCH_SPI_CLK_SEL (HAL_SPI::SPI_DIV128)

// ADS8685
#define SCH_ADS8685_CS_PORT (HAL_GPIO::GPIO_PORTD)
#define SCH_ADS8685_CS_PIN (HAL_GPIO::GPIO_PIN4)
#define SCH_ADS8685_CS_DIR (HAL_GPIO::GPIO_OUTPUT)
#define SCH_ADS8685_CS_VAL (1)
#define SCH_ADS8685_RANGE (ADS8685::VREF_x1_25_UNIPOLAR)
#define SCH_ADS8685_REF (ADS8685::INTERNAL_REF)
#define SCH_ADS8685_GAIN ((4700.0+698)/698) // voltage divider = 1/8

// LTC2451
#define SCH_LTC2451_REF (5.0)
#define SCH_LTC2451_SAMPLE_RATE (LTC2451::SPEED_60HZ)

// MAX5216
#define SCH_MAX5216_CS_PORT (HAL_GPIO::GPIO_PORTD)
#define SCH_MAX5216_CS_PIN (HAL_GPIO::GPIO_PIN5)
#define SCH_MAX5216_CS_DIR (HAL_GPIO::GPIO_OUTPUT)
#define SCH_MAX5216_CS_VAL (1)
#define SCH_MAX5216_REF (5.0)

// ACS71240
#define SCH_VOLT_PER_AMP_COEFF (0.080) // 0.080 V per Amp
#define SCH_VOLT_TO_AMP(V, OFFSET) (((V)-(OFFSET))/(SCH_VOLT_PER_AMP_COEFF))
#define SCH_AMP_TO_VOLT(A, OFFSET) ((A)*(SCH_VOLT_PER_AMP_COEFF)+(OFFSET))
#define SCH_ZERO_AMP_VOLT (0) // any value below 0.5 V will cause load to draw zero current

/********************* Debugger *********************/
// UART
#define SCH_UART_MODE (HAL_UART::UART_MODE_FULL)

// LEDs
// Green connects to PE6 and TP1
// Blue connects to PF0 and TP2
// Yellow connects to PC7 and TP3
// Red connects to PC6 and TP4
#define SCH_LED_GREEN_PORT (HAL_GPIO::GPIO_PORTE)
#define SCH_LED_GREEN_PIN (HAL_GPIO::GPIO_PIN6)
#define SCH_LED_GREEN_ACTIVE (LED::LED_ACTIVE_LOW)
#define SCH_LED_BLUE_PORT (HAL_GPIO::GPIO_PORTF)
#define SCH_LED_BLUE_PIN (HAL_GPIO::GPIO_PIN0)
#define SCH_LED_BLUE_ACTIVE (LED::LED_ACTIVE_LOW)
#define SCH_LED_YELLOW_PORT (HAL_GPIO::GPIO_PORTC)
#define SCH_LED_YELLOW_PIN (HAL_GPIO::GPIO_PIN7)
#define SCH_LED_YELLOW_ACTIVE (LED::LED_ACTIVE_LOW)
#define SCH_LED_RED_PORT (HAL_GPIO::GPIO_PORTC)
#define SCH_LED_RED_PIN (HAL_GPIO::GPIO_PIN6)
#define SCH_LED_RED_ACTIVE (LED::LED_ACTIVE_LOW)

// TPs
#define SCH_TP5_PORT (HAL_GPIO::GPIO_PORTD)
#define SCH_TP5_PIN (HAL_GPIO::GPIO_PIN7)
#define SCH_TP6_PORT (HAL_GPIO::GPIO_PORTD)
#define SCH_TP6_PIN (HAL_GPIO::GPIO_PIN6)

/********************* Temp Regulator *********************/
// ADC
#define SCH_TR_ADC_DIV (HAL_ADC::ADC_DIV128)
#define SCH_TR_ADC_REF (HAL_ADC::ADC_AVCC)
#define SCH_TR_ADC_REF_VOLT (5.0)
#define SCH_TR_ADC_RESOLUTION (10)
#define SCH_TR_ADC_MASK (0b0000000000000010) // disable ADC1 digital input buffer
#define SCH_TR_ADC_TO_VOLT(C) (C * SCH_TR_ADC_REF_VOLT / (1 << SCH_TR_ADC_RESOLUTION))
#define SCH_TR_ADC_TEMP_MUX (HAL_ADC::ADC_ADC1) // temp ADC input on ADC 1
#define SCH_TR_VOLT_TO_TEMP(V) (V * 1.0 + 0) // converts voltage to temperature

// PWM
// Fan is expecting PWM frequency around 25 kHz
// With 16 MHz clock, divided by 1, PWM output frequency is 31.25 kHz
#define SCH_TR_PWM_MODE (HAL_PWM::PWM_PHASE_0XFF)
#define SCH_TR_PWM_CLK_SEL (HAL_PWM::PWM_CLK_IO)
#define SCH_TR_PWM_OUTPUT_MODE (HAL_PWM::PWM_NONINVERTED)
#define SCH_TR_PWM_VOLT_MIN (1.782)
#define SCH_TR_PWM_VOLT_MAX (4.995)

// Thermistor voltage vs temperature
#define SCH_THERM_0C_VOLT (0.018)
#define SCH_THERM_5C_VOLT (0.330)
#define SCH_THERM_10C_VOLT (0.667)
#define SCH_THERM_15C_VOLT (1.023)
#define SCH_THERM_20C_VOLT (1.386)
#define SCH_THERM_25C_VOLT (1.747)
#define SCH_THERM_30C_VOLT (2.097)
#define SCH_THERM_35C_VOLT (2.429)
#define SCH_THERM_40C_VOLT (2.737)
#define SCH_THERM_45C_VOLT (3.019)
#define SCH_THERM_50C_VOLT (3.272)
#define SCH_THERM_55C_VOLT (3.497)
#define SCH_THERM_60C_VOLT (3.695)
#define SCH_THERM_65C_VOLT (3.868)
#define SCH_THERM_70C_VOLT (4.018)
#define SCH_THERM_75C_VOLT (4.148)
#define SCH_THERM_80C_VOLT (4.260)
#define SCH_THERM_85C_VOLT (4.356)
#define SCH_THERM_90C_VOLT (4.439)
#define SCH_THERM_95C_VOLT (4.510)
#define SCH_THERM_100C_VOLT (4.571)

/********************* User Interface *********************/
// Encoder
#define SCH_UI_A_PORT (HAL_GPIO::GPIO_PORTB)
#define SCH_UI_A_PIN (HAL_GPIO::GPIO_PIN5)
#define SCH_UI_B_PORT (HAL_GPIO::GPIO_PORTB)
#define SCH_UI_B_PIN (HAL_GPIO::GPIO_PIN4)
#define SCH_UI_BTN_PORT (HAL_GPIO::GPIO_PORTB)
#define SCH_UI_BTN_PIN (HAL_GPIO::GPIO_PIN0)
#define SCH_UI_ENC_LEAD (Encoder::B_LEADS_A) // EN11-HSM1BF20

// LCD
#define SCH_UI_LCD_ADDR (PCF8574::ADDR_111)
#define SCH_UI_LCD_COLS (20)
#define SCH_UI_LCD_ROWS (4)
