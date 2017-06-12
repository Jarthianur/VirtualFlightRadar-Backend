# Installation

## Software Requirements

+ GNU compiler g++ (at least 4.9)
+ GNU make
+ boost library (>= 1.55.0)
  + *(explicit built:)*
    + thread
    + system
    + regex
    + chrono
    + signals

### Boost can be installed via package manager

```bash
sudo apt-get update
sudo apt-get install libboost-dev \
libboost-system-dev \
libboost-thread-dev \
libboost-regex-dev \
libboost-chrono-dev \
libboost-signals-dev
```

### or later, along with the *install.sh*

```bash
./install.sh deps
```

### or manually

Download the boost_1_63_0.tar.gz from [here](http://www.boost.org/users/history/version_1_63_0.html).
From Downloads directory unpack it into ~/boost and run its install scripts

```bash
# if not already existing
mkdir -p ~/boost/
tar xf boost_1_63_0.tar.gz -C ~/boost/
cd ~/boost/boost_1_63_0
./bootstrap.sh --prefix=/home/$USER/boost/
./b2 --build-dir=/home/$USER/boost/ --with-system --with-chrono --with-regex --with-signals --with-thread
```

Now *~/boost/boost_1_63_0/* is the boost root directory.

Note that the boost version from package manager may be too old. *( >= 1.55.0 verified to work also)*

## Install

### Manually

Clone or download this repository.  
Before installing the program, You may adjust the values in
[Parameters.h](https://github.com/Jarthianur/VirtualFlightRadar-Backend/blob/master/src/util/Parameters.h).
These parameters have to stay fixed, so they can not be changed after compilation.
Please read the comments carefully, as these values change the programs behavior, or may even break it.

#### From inside the projects root directory follow these steps

Either adjust the [vfrb.ini](https://github.com/Jarthianur/VirtualFlightRadar-Backend/blob/master/vfrb.ini)
file right now to deploy it ready-to-use, or later.  
Edit the [bootstrap.sh](https://github.com/Jarthianur/VirtualFlightRadar-Backend/blob/master/bootstrap.sh) according to Your needs.  
There are just a few variables, which need to be set as stated in their comments.  
Next run `./install.sh` and look at its output, or run `./install.sh service` if You want it as a systemd service.  
If all requirements are met, the VFR-B was successfully built, stored where specified and a systemd service was properly configured.

If not already done, now the runtime configuration must be specified in the target `.ini` file.
This `.ini` file was also stored according to the path in `bootstrap.sh`.

### Prebuilt

There are prebuilt binaries available per release, at least for the RaspberryPi.
Download the right one and put it where ever You want it to stay.
Of course You need to write an `.ini` file with the config found [here](/vfrb.ini).
In order to use this as a service You will have to write Your own service file with a template like [this](/service).