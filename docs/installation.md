# Installation

All necessary dependencies and build steps are automated in the `run.sh` file.
To build the VFRB execute `./run.sh build` in bash.
If you want to specify build paths etc, just follow the description from `run.sh --help`.

## Install

### Manually

Clone or download this repository.  
Before installing the program, you may adjust the values in
[parameters.h](https://github.com/Jarthianur/VirtualFlightRadar-Backend/blob/master/src/parameters.h).
These parameters have to stay fixed, so they can not be changed after compilation.
Please read the comments carefully, as these values change the programs behavior, or may even break it.

#### From inside the projects root directory follow these steps

Either adjust the [vfrb.ini](https://github.com/Jarthianur/VirtualFlightRadar-Backend/blob/master/vfrb.ini)
file right now to deploy it ready-to-use, or later.
Next run `./run.sh build install [options]` and look at its output if you want it as a systemd service.
If all requirements are met, the VFR-B was successfully built, stored where specified and a systemd service was properly configured.

If not already done, now the runtime configuration must be specified in the target `.ini` file.

#### Example

```bash
./run.sh build install --path=/home/pi/vfrb --ini-path=/home/pi/vfrb.ini -y
```

### Prebuilt

There are prebuilt binaries available per release, at least for the RaspberryPi.
Download the right one and put it where ever you want it to stay.
Of course you need to write an `.ini` file.
