#include <TMC51X0.hpp>


#if defined(ARDUINO_ARCH_RP2040)
SPIClassRP2040 & spi = SPI;
pin_size_t SCK_PIN = 18;
pin_size_t TX_PIN = 19;
pin_size_t RX_PIN = 20;
#else
SPIClass & spi = SPI;
#endif

const uint8_t CHIP_SELECT_PIN = 10;
const uint8_t HARDWARE_ENABLE_PIN = 4;

const long SERIAL_BAUD_RATE = 115200;
const int LOOP_DELAY = 500;

// converter constants
// internal clock is ~12MHz
const uint8_t CLOCK_FREQUENCY_MHZ = 12;
// 200 fullsteps per revolution for many steppers * 256 microsteps per fullstep
// 10.49 millimeters per revolution leadscrew -> 51200 / 10.49
// one "real unit" in this example is one millimeters of linear travel
constexpr uint32_t MICROSTEPS_PER_REAL_UNIT = 4881;

// driver constants
const uint8_t GLOBAL_CURRENT_SCALAR = 50; // percent
// const uint8_t HOME_GLOBAL_CURRENT_SCALAR = 20; // percent
const uint8_t RUN_CURRENT = 20; // percent
const uint8_t PWM_OFFSET = 20; // percent
const uint8_t PWM_GRADIENT = 5; // percent
const tmc51x0::Driver::MotorDirection MOTOR_DIRECTION = tmc51x0::Driver::REVERSE;

const uint8_t STEALTH_CHOP_THRESHOLD = 100; // millimeters/s
const uint8_t COOL_STEP_THRESHOLD = 100; // millimeters/s
const uint8_t MIN_COOL_STEP = 1;
const uint8_t MAX_COOL_STEP = 0;
const uint8_t HIGH_VELOCITY_THRESHOLD = 90; // millimeters/s
const int8_t STALL_GUARD_THRESHOLD = 1;

const uint32_t START_VELOCITY = 1; // millimeters/s
const uint32_t FIRST_ACCELERATION = 10;  // millimeters/(s^2)
const uint32_t FIRST_VELOCITY = 10; // millimeters/s
const uint32_t MAX_ACCELERATION = 2;  // millimeters/(s^2)
const uint32_t MAX_DECELERATION = 25;  // millimeters/(s^2)
const uint32_t FIRST_DECELERATION = 20;  // millimeters/(s^2)
const uint32_t MAX_VELOCITY = 20; // millimeters/s
const uint32_t STOP_VELOCITY = 5; // millimeters/s

const int32_t MAX_TARGET_POSITION = 20;  // millimeters
const int32_t MIN_TARGET_POSITION = 180;  // millimeters
const int32_t HOME_TARGET_POSITION = -150;  // millimeters
const tmc51x0::Controller::RampMode RAMP_MODE = tmc51x0::Controller::POSITION;
const int32_t INITIAL_POSITION = 0;

// Instantiate TMC51X0
TMC51X0 stepper_interface;
uint32_t target_position;

void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);
#if defined(ARDUINO_ARCH_RP2040)
  spi.setSCK(SCK_PIN);
  spi.setTX(TX_PIN);
  spi.setRX(RX_PIN);
#endif
  stepper_interface.setup(spi, CHIP_SELECT_PIN);

  tmc51x0::Converter::Settings converter_settings =
    {
      CLOCK_FREQUENCY_MHZ,
      MICROSTEPS_PER_REAL_UNIT
    };
  stepper_interface.converter.setup(converter_settings);

  stepper_interface.driver.setHardwareEnablePin(HARDWARE_ENABLE_PIN);
  stepper_interface.driver.writeGlobalCurrentScaler(stepper_interface.converter.percentToGlobalCurrentScaler(GLOBAL_CURRENT_SCALAR));
  stepper_interface.driver.writeRunCurrent(stepper_interface.converter.percentToCurrentSetting(RUN_CURRENT));
  stepper_interface.driver.writePwmOffset(stepper_interface.converter.percentToPwmSetting(PWM_OFFSET));
  stepper_interface.driver.writePwmGradient(stepper_interface.converter.percentToPwmSetting(PWM_GRADIENT));
  stepper_interface.driver.writeMotorDirection(MOTOR_DIRECTION);
  stepper_interface.driver.writeStealthChopThreshold(stepper_interface.converter.velocityRealToTstep(STEALTH_CHOP_THRESHOLD));
  // stepper_interface.driver.writeCoolStepThreshold(stepper_interface.converter.velocityRealToTstep(COOL_STEP_THRESHOLD));
  // stepper_interface.driver.enableCoolStep(MIN_COOL_STEP, MAX_COOL_STEP);
  // stepper_interface.driver.writeHighVelocityThreshold(stepper_interface.converter.velocityRealToTstep(HIGH_VELOCITY_THRESHOLD));
  // stepper_interface.driver.writeStallGuardThreshold(STALL_GUARD_THRESHOLD);

  stepper_interface.controller.writeFirstAcceleration(stepper_interface.converter.accelerationRealToChip(FIRST_ACCELERATION));
  stepper_interface.controller.writeFirstVelocity(stepper_interface.converter.velocityRealToChip(FIRST_VELOCITY));
  stepper_interface.controller.writeMaxAcceleration(stepper_interface.converter.accelerationRealToChip(MAX_ACCELERATION));
  stepper_interface.controller.writeMaxDeceleration(stepper_interface.converter.accelerationRealToChip(MAX_DECELERATION));
  stepper_interface.controller.writeFirstDeceleration(stepper_interface.converter.accelerationRealToChip(FIRST_DECELERATION));
  stepper_interface.controller.writeStopVelocity(stepper_interface.converter.velocityRealToChip(STOP_VELOCITY));
  stepper_interface.controller.writeRampMode(RAMP_MODE);
  stepper_interface.controller.writeActualPosition(stepper_interface.converter.positionRealToChip(INITIAL_POSITION));

  stepper_interface.driver.enable();

  stepper_interface.controller.rampToZeroVelocity();
  while (!stepper_interface.controller.zeroVelocity())
  {
    Serial.println("Waiting for zero velocity.");
    delay(LOOP_DELAY);
  }

  stepper_interface.controller.writeStartVelocity(stepper_interface.converter.velocityRealToChip(START_VELOCITY));
  stepper_interface.controller.writeMaxVelocity(stepper_interface.converter.velocityRealToChip(MAX_VELOCITY));

  // home
  // stepper_interface.driver.writeGlobalCurrentScaler(stepper_interface.converter.percentToGlobalCurrentScaler(HOME_GLOBAL_CURRENT_SCALAR));
  // stepper_interface.controller.writeTargetPosition(stepper_interface.converter.positionRealToChip(HOME_TARGET_POSITION));

  stepper_interface.driver.writeGlobalCurrentScaler(stepper_interface.converter.percentToGlobalCurrentScaler(GLOBAL_CURRENT_SCALAR));
  randomSeed(analogRead(A0));
  long random_delay = random(5000);
  delay(random_delay);

  target_position = MIN_TARGET_POSITION;
  stepper_interface.controller.writeTargetPosition(stepper_interface.converter.positionRealToChip(target_position));

}

void loop()
{
  tmc51x0::Registers::RampStat ramp_stat;
  ramp_stat.bytes = stepper_interface.registers.read(tmc51x0::Registers::RAMP_STAT);
  stepper_interface.printer.printRampStat(ramp_stat);
  tmc51x0::Registers::DrvStatus drv_status;
  drv_status.bytes = stepper_interface.registers.read(tmc51x0::Registers::DRV_STATUS);
  stepper_interface.printer.printDrvStatus(drv_status);

  Serial.print("max_velocity (millimeters per second): ");
  Serial.println(MAX_VELOCITY);

  int32_t actual_velocity_chip = stepper_interface.controller.readActualVelocity();
  int32_t actual_velocity_real = stepper_interface.converter.velocityChipToReal(actual_velocity_chip);
  Serial.print("actual_velocity (millimeters per second): ");
  Serial.println(actual_velocity_real);

  int32_t actual_position_chip = stepper_interface.controller.readActualPosition();
  int32_t actual_position_real = stepper_interface.converter.positionChipToReal(actual_position_chip);
  Serial.print("actual position (millimeters): ");
  Serial.println(actual_position_real);

  int32_t target_position_chip = stepper_interface.controller.readTargetPosition();
  int32_t target_position_real = stepper_interface.converter.positionChipToReal(target_position_chip);
  Serial.print("target position (millimeters): ");
  Serial.println(target_position_real);
  Serial.println("--------------------------");

  if (stepper_interface.controller.positionReached())
  {
    Serial.println("Reached target position!");
    Serial.println("--------------------------");
    long random_delay = random(3000);
    delay(random_delay);
    if (target_position == MIN_TARGET_POSITION)
    {
      target_position = MAX_TARGET_POSITION;
    }
    else
    {
      target_position = MIN_TARGET_POSITION;
    }
stepper_interface.controller.writeTargetPosition(stepper_interface.converter.positionRealToChip(target_position));
  }

  Serial.println("--------------------------");
  delay(LOOP_DELAY);
}