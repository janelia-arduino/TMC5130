- [Library Information](#org0ec17f1)
- [Stepper Motors](#org310839a)
- [Stepper Motor Controllers and Drivers](#org564cc65)
- [Examples](#org3c63dfc)
- [Hardware Documentation](#org35a72d7)
- [Host Computer Setup](#orgb0fa576)

    <!-- This file is generated automatically from metadata -->
    <!-- File edits may be overwritten! -->


<a id="org0ec17f1"></a>

# Library Information

-   **Name:** TMC51X0
-   **Version:** 0.1.0
-   **License:** BSD
-   **URL:** <https://github.com/janelia-arduino/TMC51X0>
-   **Author:** Peter Polidoro
-   **Email:** peter@polidoro.io


## Description

TMC5130 and TMC5160 are high-performance ICs combining both a stepper motor controller and driver.

Both have serial communication interfaces, either SPI or UART, and combine a flexible ramp generator for automatic target positioning with an advanced stepper motor driver.

The TMC5130 and TMC5160 are register compatible allowing the same driver library to work with both.

The TMC5130 includes MOSFETs inside the IC to reduce part count and circuit size for driving motors up to 1.64A RMS per coil.

The TMC5160 uses external MOSFETs to drive higher current motors from 1A to several 10A per coil.

<img src="./images/trinamic_wiring-TMC51X0-description.svg" width="1200px">


<a id="org310839a"></a>

# Stepper Motors

From Wikipedia, the free encyclopedia:

A stepper motor, also known as step motor or stepping motor, is a brushless DC electric motor that divides a full rotation into a number of equal steps. The motor's position can be commanded to move and hold at one of these steps without any position sensor for feedback (an open-loop controller), as long as the motor is correctly sized to the application in respect to torque and speed.

[Wikipedia - Stepper Motor](https://en.wikipedia.org/wiki/Stepper_motor)


<a id="org564cc65"></a>

# Stepper Motor Controllers and Drivers

Stepper motors need both a controller and a driver. These may be combined into a single component or separated into multiple components that communicate with each other, as is the case with the TMC51X0 stepper motor driver. One controller may be connected to more than one driver for coordinated multi-axis motion control.


## Stepper Motor Controller

A stepper motor controller is responsible for the commanding either the motor kinetics, the torque, or the motor kinematics, the position, speed, and acceleration of one or more stepper motors.


## Stepper Motor Driver

A stepper motor driver is responsible for commanding the electrical current through the motor coils as it changes with time to meet the requirements of the stepper motor controller.


<a id="org3c63dfc"></a>

# Examples


## Wiring


### TMC5130-BOB

1.  Teensy 4.0

    <img src="./images/trinamic_wiring-TMC51X0-tmc5130_bob-teensy40.svg" width="1200px">


### Wiring Documentation Source

<https://github.com/janelia-kicad/trinamic_wiring>


<a id="org35a72d7"></a>

# Hardware Documentation


## Datasheets

[Datasheets](./datasheet)


<a id="orgb0fa576"></a>

# Host Computer Setup


## Download this repository

<https://github.com/janelia-arduino/TMC51X0.git>

```sh
git clone https://github.com/janelia-arduino/TMC51X0.git
```


## PlatformIO


### Install PlatformIO Core

<https://docs.platformio.org/en/latest/core/installation/index.html>

```sh
python3 -m venv .venv
source .venv/bin/activate
pip install platformio
pio --version
```


### 99-platformio-udev.rules

Linux users have to install udev rules for PlatformIO supported boards/devices.

1.  Download udev rules file to /etc/udev/rules.d

    ```sh
    curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core/develop/platformio/assets/system/99-platformio-udev.rules | sudo tee /etc/udev/rules.d/99-platformio-udev.rules
    ```

2.  Restart udev management tool

    ```sh
    sudo service udev restart
    ```

3.  Add user to groups

    ```sh
    sudo usermod -a -G dialout $USER && sudo usermod -a -G plugdev $USER
    ```

4.  Remove modemmanager

    ```sh
    sudo apt-get purge --auto-remove modemmanager
    ```

5.  After setting up rules and groups

    You will need to log out and log back in again (or reboot) for the user group changes to take effect.
    
    After this file is installed, physically unplug and reconnect your board.


### Compile the firmware

1.  Gnu/Linux

    ```sh
    make firmware
    ```

2.  Other

    ```sh
    pio run -e teensy40
    ```


### Upload the firmware

1.  Gnu/Linux

    ```sh
    make upload
    ```

2.  Other

    ```sh
    pio run -e teensy40 -t upload
    ```


### Serial Terminal Monitor

1.  Gnu/Linux

    ```sh
    make monitor
    ```

2.  Other

    ```sh
    pio device monitor --echo --eol=LF
    ```
