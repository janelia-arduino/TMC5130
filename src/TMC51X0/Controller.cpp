// ----------------------------------------------------------------------------
// Controller.cpp
//
// Authors:
// Peter Polidoro peter@polidoro.io
// ----------------------------------------------------------------------------
#include "Controller.hpp"


using namespace tmc51x0;

void Controller::writeRampMode(RampMode ramp_mode)
{
  registers_ptr_->write(Registers::RAMPMODE, ramp_mode);
}

void Controller::writeStopMode(StopMode stop_mode)
{
  Registers::SwMode sw_mode;
  sw_mode.bytes = registers_ptr_->getStored(Registers::SW_MODE);
  sw_mode.en_softstop = stop_mode;
  registers_ptr_->write(Registers::SW_MODE, sw_mode.bytes);
}

uint32_t Controller::readTstep()
{
  return registers_ptr_->read(Registers::TSTEP);
}

int32_t Controller::readActualPosition()
{
  return registers_ptr_->read(Registers::XACTUAL);
}

void Controller::writeActualPosition(int32_t position)
{
  return registers_ptr_->write(Registers::XACTUAL, position);
}

int32_t Controller::readActualVelocity()
{
  return registers_ptr_->read(Registers::VACTUAL);
}

void Controller::writeVelocityMax(uint32_t velocity)
{
  registers_ptr_->write(Registers::VMAX, velocity);
}

void Controller::writeAccelerationMax(uint32_t acceleration)
{
  registers_ptr_->write(Registers::AMAX, acceleration);
}

// private

void Controller::setup(Registers & registers,
  Converter & converter)
{
  registers_ptr_ = &registers;
  converter_ptr_ = &converter;

  writeRampMode(RAMP_MODE_DEFAULT);
  writeStopMode(STOP_MODE_DEFAULT);
  writeActualPosition(ACTUAL_POSITION_DEFAULT);
  writeVelocityMax(VELOCITY_MAX_DEFAULT);
  writeAccelerationMax(ACCELERATION_MAX_DEFAULT);
}

