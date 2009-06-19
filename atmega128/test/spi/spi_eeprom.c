#include "spi_eeprom.h"
#include "spi.h"
#include <avr/io.h>


//-----------------------------------------------------------------------------
// for 93C46/93C56/93C66: use 8-bit access

static const uint8_t SPI_START_BIT_93CXX = 0x01 << 3;

void ee93Cxx_init()
{
	//SPCR = 0x00;

	// data order: if 0, MSB to LSB. if 1, LSB to MSB.
	SPCR |= _BV(DORD);

	// SPI mode		CPOL	CPHA	at leading edge		at trailing edge
	//--------------------------------------------------------------------
	// 0			0		0		sample (rising)		setup (falling)
	// 1			0		1		setup (rising)		sample (falling)
	// 2			1		0		sample (falling)	setup (rising)
	// 3			1		1		setup (falling)		sample (rising)
	SPCR |= _BV(CPOL);  // clock polarity: if 0, leading edge = rising edge. if 1, leading edge = falling edge.
	SPCR |= _BV(CPHA);  // clock phase: if 0, sample at leading edge. if 1, sample at trailing edge

	// SCK clock rate
	// SPI2X	SPR1	SPR0	SCK frequency
	//-----------------------------------------
	// 0		0		0		Fosc / 4
	// 0		0		1		Fosc / 16
	// 0		1		0		Fosc / 64
	// 0		1		1		Fosc / 128
	// 1		0		0		Fosc / 2
	// 1		0		1		Fosc / 8
	// 1		1		0		Fosc / 32
	// 1		1		1		Fosc / 64
	SPSR &= ~(_BV(SPI2X));
	SPCR &= ~(_BV(SPR1));
	SPCR |= _BV(SPR0);

	spi_init_as_master();
}

void ee93Cxx_chip_select()
{
	// FIXME [check] >>
	PORTB |= _BV(PB0);
}

void ee93Cxx_chip_deselect()
{
	// FIXME [check] >>
	PORTB &= ~(_BV(PB0));
}

int ee93Cxx_set_write_enable()
{
	const uint8_t OP_CODE = 0x00;  // write enable
	const uint16_t addr = 0x0180;

	spi_disable_interrupt();
	ee93Cxx_chip_select();
	spi_master_transmit_a_byte(SPI_START_BIT_93CXX | ((OP_CODE << 1) & 0x06) | ((addr >> 8) & 0x01));
	spi_master_transmit_a_byte(addr & 0x00FF);
	ee93Cxx_chip_deselect();
	spi_disable_interrupt();

	return 1;
}

int ee93Cxx_set_write_disable()
{
	const uint8_t OP_CODE = 0x00;  // write disable
	const uint16_t addr = 0x0000;

	spi_disable_interrupt();
	ee93Cxx_chip_select();
	spi_master_transmit_a_byte(SPI_START_BIT_93CXX | ((OP_CODE << 1) & 0x06) | ((addr >> 8) & 0x01));
	spi_master_transmit_a_byte(addr & 0x00FF);
	ee93Cxx_chip_deselect();
	spi_disable_interrupt();

	return 1;
}

int ee93Cxx_write_a_byte(const uint16_t eeaddr, const uint8_t byte)
{
	const uint8_t OP_CODE = 0x01;  // write

	spi_disable_interrupt();
	ee93Cxx_chip_select();
	spi_master_transmit_a_byte(SPI_START_BIT_93CXX | ((OP_CODE << 1) & 0x06) | ((eeaddr >> 8) & 0x01));
	spi_master_transmit_a_byte(eeaddr & 0x00FF);
	spi_master_transmit_a_byte(byte);
	ee93Cxx_chip_deselect();
	spi_disable_interrupt();

	return 1;
}

int ee93Cxx_read_a_byte(const uint16_t eeaddr, uint8_t *byte)
{
	const uint8_t OP_CODE = 0x02;  // read

	spi_disable_interrupt();
	ee93Cxx_chip_select();
	spi_master_transmit_a_byte(SPI_START_BIT_93CXX | ((OP_CODE << 1) & 0x06) | ((eeaddr >> 8) & 0x01));
	spi_master_transmit_a_byte(eeaddr & 0x00FF);
	const uint8_t upper = spi_master_transmit_a_byte(0x00);  // dummy
	const uint8_t lower = spi_master_transmit_a_byte(0x00);  // dummy
	ee93Cxx_chip_deselect();
	spi_disable_interrupt();

	*byte = (upper << 1) + (lower >> 7);

	return 1;
}

int ee93Cxx_erase(const uint16_t eeaddr)
{
	const uint8_t OP_CODE = 0x03;  // erase

	spi_disable_interrupt();
	ee93Cxx_chip_select();
	spi_master_transmit_a_byte(SPI_START_BIT_93CXX | ((OP_CODE << 1) & 0x06) | ((eeaddr >> 8) & 0x01));
	spi_master_transmit_a_byte(eeaddr & 0x00FF);
	ee93Cxx_chip_deselect();
	spi_disable_interrupt();

	return 1;
}

int ee93Cxx_erase_all()
{
	const uint8_t OP_CODE = 0x00;  // erase all
	const uint16_t addr = 0x0100;

	spi_disable_interrupt();
	ee93Cxx_chip_select();
	spi_master_transmit_a_byte(SPI_START_BIT_93CXX | ((OP_CODE << 1) & 0x06) | ((addr >> 8) & 0x01));
	spi_master_transmit_a_byte(addr & 0x00FF);
	ee93Cxx_chip_deselect();
	spi_disable_interrupt();

	return 1;
}

int ee93Cxx_write_all(const uint8_t byte)
{
	const uint8_t OP_CODE = 0x00;  // write all
	const uint16_t addr = 0x0080;

	spi_disable_interrupt();
	ee93Cxx_chip_select();
	spi_master_transmit_a_byte(SPI_START_BIT_93CXX | ((OP_CODE << 1) & 0x06) | ((addr >> 8) & 0x01));
	spi_master_transmit_a_byte(addr & 0x00FF);
	spi_master_transmit_a_byte(byte);
	ee93Cxx_chip_deselect();
	spi_disable_interrupt();

	return 1;
}

//-----------------------------------------------------------------------------
// for 25128/25256

void ee25xxx_init()
{
	//SPCR = 0x00;

	// data order: if 0, MSB to LSB. if 1, LSB to MSB.
	SPCR |= _BV(DORD);

	// SPI mode		CPOL	CPHA	at leading edge		at trailing edge
	//--------------------------------------------------------------------
	// 0			0		0		sample (rising)		setup (falling)
	// 1			0		1		setup (rising)		sample (falling)
	// 2			1		0		sample (falling)	setup (rising)
	// 3			1		1		setup (falling)		sample (rising)
	SPCR &= ~(_BV(CPOL));  // clock polarity: if 0, leading edge = rising edge. if 1, leading edge = falling edge.
	SPCR &= ~(_BV(CPHA));  // clock phase: if 0, sample at leading edge. if 1, sample at trailing edge
	//SPCR |= _BV(CPOL);  // clock polarity: if 0, leading edge = rising edge. if 1, leading edge = falling edge.
	//SPCR |= _BV(CPHA);  // clock phase: if 0, sample at leading edge. if 1, sample at trailing edge

	// SCK clock rate
	// SPI2X	SPR1	SPR0	SCK frequency
	//-----------------------------------------
	// 0		0		0		Fosc / 4
	// 0		0		1		Fosc / 16
	// 0		1		0		Fosc / 64
	// 0		1		1		Fosc / 128
	// 1		0		0		Fosc / 2
	// 1		0		1		Fosc / 8
	// 1		1		0		Fosc / 32
	// 1		1		1		Fosc / 64
	SPSR &= ~(_BV(SPI2X));
	SPCR &= ~(_BV(SPR1));
	SPCR &= ~(_BV(SPR0));

	//
	spi_init_as_master();
}

void ee25xxx_chip_select()
{
	// FIXME [check] >>
	PORTB &= ~(_BV(PB0));
}

void ee25xxx_chip_deselect()
{
	// FIXME [check] >>
	PORTB |= _BV(PB0);
}

int ee25xxx_set_write_enable()
{
	const uint8_t OP_CODE = 0x06;  // write enable

	spi_disable_interrupt();
	ee25xxx_chip_select();
	spi_master_transmit_a_byte(OP_CODE & 0x07);
	ee25xxx_chip_deselect();
	spi_enable_interrupt();

	return 1;
}

int ee25xxx_set_write_disable()
{
	const uint8_t OP_CODE = 0x04;  // write disable

	spi_disable_interrupt();
	ee25xxx_chip_select();
	spi_master_transmit_a_byte(OP_CODE & 0x07);
	ee25xxx_chip_deselect();
	spi_enable_interrupt();

	return 1;
}

int ee25xxx_write_status_register(const uint8_t byte)
{
	const uint8_t OP_CODE = 0x01;  // write status register

	spi_disable_interrupt();
	ee25xxx_chip_select();
	spi_master_transmit_a_byte(OP_CODE & 0x07);
	spi_master_transmit_a_byte(byte);
	ee25xxx_chip_deselect();
	spi_enable_interrupt();

	return 1;
}

int ee25xxx_read_status_register(uint8_t *byte)
{
	const uint8_t OP_CODE = 0x05;  // read status register

	spi_disable_interrupt();
	ee25xxx_chip_select();
	spi_master_transmit_a_byte(OP_CODE & 0x07);
	*byte = spi_master_transmit_a_byte(0xFF);  // dummy
	ee25xxx_chip_deselect();
	spi_enable_interrupt();

	return 1;
}

int ee25xxx_is_ready()
{
	uint8_t status = 0xFF;
	ee25xxx_read_status_register(&status);
	return (status & 0x01) == 0x00;
}

int ee25xxx_write_a_byte(const uint16_t eeaddr, const uint8_t byte)
{
	const uint8_t OP_CODE = 0x02;  // write data

	spi_disable_interrupt();
	ee25xxx_chip_select();
	spi_master_transmit_a_byte(OP_CODE & 0x07);
	spi_master_transmit_a_byte((eeaddr >> 8) & 0x00FF);
	spi_master_transmit_a_byte(eeaddr & 0x00FF);
	spi_master_transmit_a_byte(byte);
	ee25xxx_chip_deselect();
	spi_enable_interrupt();

	return 1;
}

int ee25xxx_read_a_byte(const uint16_t eeaddr, uint8_t *byte)
{
	const uint8_t OP_CODE = 0x03;  // read data

	spi_disable_interrupt();
	ee25xxx_chip_select();
	spi_master_transmit_a_byte(OP_CODE & 0x07);
	spi_master_transmit_a_byte((eeaddr >> 8) & 0x00FF);
	spi_master_transmit_a_byte(eeaddr & 0x00FF);
	*byte = spi_master_transmit_a_byte(0xFF);  // dummy
	ee25xxx_chip_deselect();
	spi_enable_interrupt();

	return 1;
}
