# Installation

All necessary dependencies and build steps are automated in the `run.sh` file.
To build the VFRB execute `./run.sh build` in bash.
If you want to specify build paths etc, just follow the description from `run.sh --help`.

## Install

### Manually

Clone or download this repository.  
Before installing the program, you may adjust the values in
[parameters.h](https://github.com/Jarthianur/VirtualFlightRadar-Backend/blob/master/include/parameters.h).
These parameters have to stay fixed, so they can not be changed after compilation.
Please read the comments carefully, as these values change the programs behavior, or may even break it.

#### From inside the projects root directory follow these steps

Either adjust the [vfrb.conf.in](https://github.com/Jarthianur/VirtualFlightRadar-Backend/blob/master/vfrb.conf.in)
file right now to deploy it ready-to-use, or edit the final _vfrb.conf_ file later.
Next run `./run.sh build install [options]` and look at its output if you want it as a systemd service.
If all requirements are met, the VFR-B was successfully built and a systemd service was properly configured.
All built files and the config can be found in `build`.

If not already done, now the runtime configuration must be specified in the target `.conf` file.

#### Example

```bash
./run.sh build install --conf=custom.conf -y
```

### Prebuilt

There are prebuilt binaries available per release, at least for the RaspberryPi.
Download the right one and put it where ever you want it to stay.
Of course you need to write an `.conf` file.

### Docker

If you want to run this application containerized, using docker, there is a task which creates a minimal docker image.
Simply run `./run.sh docker` to build the image. Whilst the image creation process, the _vfrb.conf_ file is copied into the image.
This allows to create an image with complete configuration done and without the need to touch the file later.
Please pay attention to your network setup, as "localhost" refers to the docker container and not the actual host.
Hence use the actual IP address of your host for all services that run there. Also the internal port must stay at 4353.
