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
#define SCH_UART_MODE (HAL_UART::UART_MODE_FULL)

