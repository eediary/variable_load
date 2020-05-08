#pragma once

#include "HAL_GPIO.h"
#include "HAL_SPI.h"

// ADS8685 default configuration
#define ADS8685_SPI_DATA_MODE (HAL_SPI::SPI_MODE_0)
#define ADS8685_DEFAULT_RANGE (VREF_x3_BIPOLAR)
#define ADS8685_INTERNAL_REF (4.096)
#define ADS8685_SPI_CLK_SEL (HAL_SPI::SPI_DIV2) // Max SPI CLK freq = 66.67 MHz

// Command bit shifts
#define OPCODE_SHIFT(x) ((long)(x) << 27)
#define WRITE_SEL_SHIFT(x) ((long)(x) << 25)
#define ADDRESS_SHIFT(x) ((long)(x) << 16)
#define DATA_SHIFT(x) ((long)(x) << 0)

// Register bit shifts
#define INTREF_DIS_SHIFT(x) ((x) << 6)
#define RANGE_SEL_SHIFT(x) ((x) << 0)

class ADS8685{

public:
	enum ADS8685_INPUT_RANGE{
		VREF_x3_BIPOLAR = 0b0000,
		VREF_x2_5_BIPOLAR = 0b0001,
		VREF_x1_5_BIPOLAR = 0b0010,
		VREF_x1_25_BIPOLAR = 0b0011,
		VREF_x0_625_BIPOLAR = 0b0100,
		VREF_x3_UNIPOLAR = 0b1000,
		VREF_x2_5_UNIPOLAR = 0b1001,
		VREF_x1_5_UNIPOLAR = 0b1010,
		VREF_x1_25_UNIPOLAR = 0b1011
	};
	enum ADS8685_REFERENCE{
		INTERNAL_REF = 0b0,
		EXTERNAL_REF = 0b1
	};

private:
	// Private data members
	HAL_GPIO &cs_pin;
	HAL_SPI &spi;
	const int resolution;
	float reference_voltage;
	float output_offset;

	// Private enum
	enum ADS8685_REGISTER{
		DEVICE_ID_REG = 0x00,
		DEVICE_ID_REG_H = 0x02,
		RST_PWRCTL_REG = 0x04,
		RST_PWRCTL_REG_H = 0x06,
		SDI_CTL_REG = 0x08,
		SDI_CTL_REG_H = 0x0A,
		SDO_CTL_REG = 0x0C,
		SDO_CTL_REG_H = 0x0E,
		DATAOUT_CTL_REG = 0x10,
		DATAOUT_CTL_REG_H = 0x12,
		RANGE_SEL_REG = 0x14,
		RANGE_SEL_REG_H = 0x16,
		ALARM_REG = 0x20,
		ALARM_REG_H = 0x22,
		ALARM_H_TH_REG = 0x24,
		ALARM_H_TH_REG_H = 0x26,
		ALARM_L_TH_REG = 0x28,
		ALARM_L_TH_REG_H = 0x2A
	};
	enum ADS8685_OPCODE{
		OP_NOP = 0b00000,
		OP_CLEAR_HWORD = 0b11000,
		OP_READ_HWORD = 0b11001,
		OP_READ = 0b01001,
		OP_WRITE = 0b11010,
		OP_SET_HWORD = 0b11011
	};
	enum ADS8685_WRITE_SEL{
		WRITE_FULL = 0b00,
		WRITE_MSB = 0b01,
		WRITE_LSB = 0b10
	};

	// Private methods
	uint32_t send_command(ADS8685_OPCODE command, ADS8685_REGISTER addr, uint16_t data = 0, ADS8685_WRITE_SEL write_sel = WRITE_FULL){
		// transmits command to specified register
		// default data is 0, and default write select is full
		cs_pin.clear_pin();
		uint16_t to_return = spi.send_qbyte(OPCODE_SHIFT(command) | WRITE_SEL_SHIFT(write_sel) | ADDRESS_SHIFT(addr) | DATA_SHIFT(data), ADS8685_SPI_DATA_MODE, ADS8685_SPI_CLK_SEL);
		cs_pin.set_pin();
		return to_return;
	}
	void update_reference(ADS8685_INPUT_RANGE range, ADS8685_REFERENCE ref, float ref_volt = 0){
		// Updates private variables reference_voltage and output_offset, based on 
		// range, ref, and ref_volt
		if(ref == INTERNAL_REF)
			reference_voltage = ADS8685_INTERNAL_REF;
		else
			reference_voltage = ref_volt;
	
		// adjusts reference voltage based on range
		switch(range){
			case VREF_x3_BIPOLAR:
				reference_voltage *= 6;
				output_offset = reference_voltage/2;
				break;
			case VREF_x2_5_BIPOLAR:
				reference_voltage *= 5;
				output_offset = reference_voltage/2;
				break;
			case VREF_x1_5_BIPOLAR:
				reference_voltage *= 3;
				output_offset = reference_voltage/2;
				break;
			case VREF_x1_25_BIPOLAR:
				reference_voltage *= 2.5;
				output_offset = reference_voltage/2;
				break;
			case VREF_x0_625_BIPOLAR:
				reference_voltage *= 1.25;
				output_offset = reference_voltage/2;
				break;
			case VREF_x3_UNIPOLAR:
				reference_voltage *= 3;
				output_offset = 0;
				break;
			case VREF_x2_5_UNIPOLAR:
				reference_voltage *= 2.5;
				output_offset = 0;
				break;
			case VREF_x1_5_UNIPOLAR:
				reference_voltage *= 1.5;
				output_offset = 0;
				break;
			case VREF_x1_25_UNIPOLAR:
				reference_voltage *= 1.25;
				output_offset = 0;
				break;
		}
	}

public:
	ADS8685(HAL_GPIO &gpio_r, HAL_SPI &spi_r): cs_pin(gpio_r), spi(spi_r), resolution(16){
		// Constructor
		// Sets up GPIO, SPI, resolution, reference voltage and output offset
		update_reference(ADS8685_DEFAULT_RANGE, INTERNAL_REF);
	}
	void set_input_range(ADS8685_INPUT_RANGE range, ADS8685_REFERENCE ref = INTERNAL_REF, float ref_volt = 0){
		// Configures device to use selected range and internal or external reference
		update_reference(range, ref, ref_volt);
		send_command(OP_WRITE, RANGE_SEL_REG, INTREF_DIS_SHIFT(ref) | RANGE_SEL_SHIFT(range), WRITE_LSB);
	}
	float read(){
		// sends 16 clocks and NOP command
		cs_pin.clear_pin();
		uint16_t val = spi.send_dbyte(0, ADS8685_SPI_DATA_MODE, ADS8685_SPI_CLK_SEL);
		cs_pin.set_pin();
		return (val * reference_voltage / ((1UL << resolution) - 1))-output_offset;
	}
};