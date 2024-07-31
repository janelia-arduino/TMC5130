// ----------------------------------------------------------------------------
// SPIInterface.cpp
//
// Authors:
// Peter Polidoro peter@polidoro.io
// ----------------------------------------------------------------------------
#include "SPIInterface.hpp"


using namespace tmc51x0;

SPIInterface::SPIInterface() :
spi_settings_(SPISettings(constants::spi_clock, constants::spi_bit_order, constants::spi_data_mode))
{
}

void SPIInterface::setup(tmc51x0::SPIParameters spi_parameters)
{
  spi_parameters_ = spi_parameters;

  pinMode(spi_parameters_.chip_select_pin,OUTPUT);
  disableChipSelect();

  spi_parameters_.spi_ptr->begin();
}

void SPIInterface::writeRegister(uint8_t register_address,
  uint32_t data)
{
  PicoDatagram pico_datagram;
  pico_datagram.register_address = register_address;
  pico_datagram.rw = SPI_RW_WRITE;
  pico_datagram.data = data;
  writeRead(pico_datagram);
}

uint32_t SPIInterface::readRegister(uint8_t register_address)
{
  PicoDatagram pico_datagram;
  pico_datagram.register_address = register_address;
  pico_datagram.rw = SPI_RW_READ;
  pico_datagram.data = 0;
  PociDatagram poci_datagram = writeRead(pico_datagram);
  // poci data is returned on second read
  poci_datagram = writeRead(pico_datagram);
  return poci_datagram.data;
}

// private

SPIInterface::PociDatagram SPIInterface::writeRead(PicoDatagram pico_datagram)
{
  uint8_t byte_write, byte_read;
  PociDatagram poci_datagram;
  poci_datagram.bytes = 0x0;
  beginTransaction();
  for (int i=(SPI_DATAGRAM_SIZE - 1); i>=0; --i)
  {
    byte_write = (pico_datagram.bytes >> (8*i)) & 0xff;
    byte_read = spi_parameters_.spi_ptr->transfer(byte_write);
    poci_datagram.bytes |= ((uint32_t)byte_read) << (8*i);
  }
  endTransaction();
  noInterrupts();
  spi_status_ = poci_datagram.spi_status;
  interrupts();
  return poci_datagram;
}

void SPIInterface::enableChipSelect()
{
  digitalWrite(spi_parameters_.chip_select_pin, LOW);
}

void SPIInterface::disableChipSelect()
{
  digitalWrite(spi_parameters_.chip_select_pin, HIGH);
}

void SPIInterface::beginTransaction()
{
  spi_parameters_.spi_ptr->beginTransaction(spi_settings_);
  enableChipSelect();
}

void SPIInterface::endTransaction()
{
  disableChipSelect();
  spi_parameters_.spi_ptr->endTransaction();
}
