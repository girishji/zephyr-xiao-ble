## Zephyr GPIO and ADC Usage Sample for Seeed Xiao BLE

Zephyr uses device trees. It is not straightforward to transition from sample
LED blinky app that Zephyr provides to manipulating GPIO pins. This repository
contains the plumbing needed for GPIO and ADC, in addition to sample usage.

### Installation

Clone this repository. Follow the [Getting
Started](https://docs.zephyrproject.org/3.2.0/develop/getting_started/index.html)
guide and install the following:

- Dependencies
- west tool
- Zephyr SDK

Do not install Zephyr sources according to the Getting Started guide but follow the
procedure below. 

```
# Create Zephyr workspace
cd zephyr-gpio-xiao-ble
west init -l app/

# Fetch modules
west update

# Export Zephyr so cmake can find it
west zephyr-export

# Install Zephyr python dependencies
pip3 install -r zephyr/scripts/requirements-base.txt
```

### Build

```
cd zephyr-gpio-xiao-ble
mkdir -p build
west build -b xiao_ble app
```

### Flash

Double press the reset button located on the top left side of the board. The
UF2 bootloader presents itself as mountable disk. Copy the firmware file named
`zephyr.uf2` from the `build/zephyr` directory to the newly mounted disk.
