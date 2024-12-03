// ----------------------------------------------------------------------------
// Controller.cpp
//
// Authors:
// Peter Polidoro peter@polidoro.io
// ----------------------------------------------------------------------------
#include "Controller.hpp"


using namespace tmc51x0;

Controller::Controller()
{
  controller_parameters_ = ControllerParameters{};
}

void Controller::setup()
{
  writeRampMode(controller_parameters_.ramp_mode);
  writeStopMode(controller_parameters_.stop_mode);
  writeMaxVelocity(controller_parameters_.max_velocity);
  writeMaxAcceleration(controller_parameters_.max_acceleration);
  writeStartVelocity(controller_parameters_.start_velocity);
  writeStopVelocity(controller_parameters_.stop_velocity);
  writeFirstVelocity(controller_parameters_.first_velocity);
  writeFirstAcceleration(controller_parameters_.first_acceleration);
  writeMaxDeceleration(controller_parameters_.max_deceleration);
  writeFirstDeceleration(controller_parameters_.first_deceleration);
  writeTzerowait(controller_parameters_.zero_wait_duration);
}

void Controller::setup(tmc51x0::ControllerParameters controller_parameters)
{
  controller_parameters_ = controller_parameters;
  setup();
}

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
  int32_t actual_velocity = registers_ptr_->read(Registers::VACTUAL);
  if (actual_velocity > MAX_POSITIVE_VELOCITY)
  {
    actual_velocity -= VELOCITY_SIGN_CONVERSION;
  }
  return actual_velocity;
}

void Controller::zeroActualPosition()
{
  return writeActualPosition(0);
}

bool Controller::velocityReached()
{
  Registers::RampStat ramp_stat;
  ramp_stat.bytes = registers_ptr_->read(Registers::RAMP_STAT);
  return ramp_stat.velocity_reached;
}

bool Controller::positionReached()
{
  Registers::RampStat ramp_stat;
  ramp_stat.bytes = registers_ptr_->read(Registers::RAMP_STAT);
  return ramp_stat.position_reached;
}

void Controller::beginRampToZeroVelocity()
{
  ramp_to_zero_settings_.start_velocity = registers_ptr_->getStored(Registers::VSTART);
  ramp_to_zero_settings_.max_velocity = registers_ptr_->getStored(Registers::VMAX);
  writeStartVelocity(0);
  writeMaxVelocity(0);
}

void Controller::endRampToZeroVelocity()
{
  writeStartVelocity(ramp_to_zero_settings_.start_velocity);
  writeMaxVelocity(ramp_to_zero_settings_.max_velocity);
}

bool Controller::zeroVelocity()
{
  Registers::RampStat ramp_stat;
  ramp_stat.bytes = registers_ptr_->read(Registers::RAMP_STAT);
  return ramp_stat.vzero;
}

void Controller::writeMaxVelocity(uint32_t velocity)
{
  registers_ptr_->write(Registers::VMAX, velocity);
}

void Controller::writeMaxAcceleration(uint32_t acceleration)
{
  registers_ptr_->write(Registers::AMAX, acceleration);
}

void Controller::writeStartVelocity(uint32_t velocity)
{
  registers_ptr_->write(Registers::VSTART, velocity);
}

void Controller::writeStopVelocity(uint32_t velocity)
{
  registers_ptr_->write(Registers::VSTOP, velocity);
}

void Controller::writeFirstAcceleration(uint32_t acceleration)
{
  registers_ptr_->write(Registers::A1, acceleration);
}

void Controller::writeFirstVelocity(uint32_t velocity)
{
  registers_ptr_->write(Registers::V1, velocity);
}

void Controller::writeMaxDeceleration(uint32_t deceleration)
{
  registers_ptr_->write(Registers::DMAX, deceleration);
}

void Controller::writeFirstDeceleration(uint32_t deceleration)
{
  registers_ptr_->write(Registers::D1, deceleration);
}

void Controller::writeTzerowait(uint32_t tzerowait)
{
  registers_ptr_->write(Registers::TZEROWAIT, tzerowait);
}

int32_t Controller::readTargetPosition()
{
  return registers_ptr_->read(Registers::XTARGET);
}

void Controller::writeTargetPosition(int32_t position)
{
  registers_ptr_->write(Registers::XTARGET, position);
}

void Controller::zeroTargetPosition()
{
  return writeTargetPosition(0);
}

void Controller::writeComparePosition(int32_t position)
{
  registers_ptr_->write(Registers::X_COMPARE, position);
}

void Controller::enableStallStop()
{
  Registers::SwMode sw_mode;
  sw_mode.bytes = registers_ptr_->getStored(Registers::SW_MODE);
  sw_mode.en_softstop = HARD;
  sw_mode.sg_stop = 1;
  registers_ptr_->write(Registers::SW_MODE, sw_mode.bytes);
}

void Controller::disableStallStop()
{
  Registers::SwMode sw_mode;
  sw_mode.bytes = registers_ptr_->getStored(Registers::SW_MODE);
  sw_mode.sg_stop = 0;
  registers_ptr_->write(Registers::SW_MODE, sw_mode.bytes);
}

void Controller::setupSwitches()
{
  Registers::SwMode sw_mode;
  sw_mode.bytes = registers_ptr_->getStored(Registers::SW_MODE);
  sw_mode.stop_l_enable = switch_parameters_.enable_left_stop;
  sw_mode.stop_r_enable = switch_parameters_.enable_right_stop;
  sw_mode.pol_stop_l = switch_parameters_.invert_left_polarity;
  sw_mode.pol_stop_r = switch_parameters_.invert_right_polarity;
  sw_mode.swap_lr = switch_parameters_.swap_left_right;
  sw_mode.latch_l_active = switch_parameters_.latch_left_active;
  sw_mode.latch_l_inactive = switch_parameters_.latch_left_inactive;
  sw_mode.latch_r_active = switch_parameters_.latch_right_active;
  sw_mode.latch_r_inactive = switch_parameters_.latch_right_inactive;
  sw_mode.en_latch_encoder = switch_parameters_.enable_latch_encoder;
  registers_ptr_->write(Registers::SW_MODE, sw_mode.bytes);
}

void Controller::setupSwitches(SwitchParameters switch_parameters)
{
  switch_parameters_ = switch_parameters;
  setupSwitches();
}

bool Controller::leftSwitchActive()
{
  Registers::RampStat ramp_stat;
  ramp_stat.bytes = registers_ptr_->read(Registers::RAMP_STAT);
  return ramp_stat.status_stop_l;
}

bool Controller::rightSwitchActive()
{
  Registers::RampStat ramp_stat;
  ramp_stat.bytes = registers_ptr_->read(Registers::RAMP_STAT);
  return ramp_stat.status_stop_r;
}

bool Controller::leftLatchActive()
{
  Registers::RampStat ramp_stat;
  ramp_stat.bytes = registers_ptr_->read(Registers::RAMP_STAT);
  return ramp_stat.status_latch_l;
}

bool Controller::rightLatchActive()
{
  Registers::RampStat ramp_stat;
  ramp_stat.bytes = registers_ptr_->read(Registers::RAMP_STAT);
  return ramp_stat.status_latch_r;
}

bool Controller::leftStopEvent()
{
  Registers::RampStat ramp_stat;
  ramp_stat.bytes = registers_ptr_->read(Registers::RAMP_STAT);
  return ramp_stat.event_stop_l;
}

bool Controller::rightStopEvent()
{
  Registers::RampStat ramp_stat;
  ramp_stat.bytes = registers_ptr_->read(Registers::RAMP_STAT);
  return ramp_stat.event_stop_r;
}

// private

void Controller::initialize(Registers & registers)
{
  registers_ptr_ = &registers;

  zeroActualPosition();
  zeroTargetPosition();
}

