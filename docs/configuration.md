# Configuration

## Note

The configuration file must be a valid `.ini` file. Thus there are sections, key-value parameters and comments.
Comments start with `;`. Parameters are of kind `key = value`,
where several spaces between the key, value and the equal-sign are allowed.
The first non-whitespace character after the equal-sign determines the beginning of the value,
all following characters, including spaces, are taken as part of the value.
Sections start with `[section]`. Until the next section all following parameters are part of that section.

## What To Configure

The *ini* file looks like this
>[general]
feeds      = sbs1,gps  
serverPort = 4353  
gndMode    =  
[fallback]  
latitude  = 50.000000  
longitude = 10.000000  
altitude  = 100  
geoid     = 48.0  
pressure  = 1013.25  
[filter]  
maxHeight = -1  
maxDist   = 40000  
[sbs1]  
host     = localhost  
port     = 1234  
priority = 0  
...

All the parameters and sections should be self explanatory.
Some information can be found in the comments of [vfrb.ini](/vfrb.ini)

### [general]

The `feeds` parameter is one of the most important, because here are the input feeds defined.
It is a comma-separated list, spaces are allowed. The feed type is determined by keywords in its name.
Therefore the entries must contain one of the following keywords

+ aprs
+ sbs
+ sens
+ gps

To enable the [Ground-Mode](#ground-mode), just assign any value to `gndMode`, to disable leave it empty.
`serverPort` defines the port where to serve the NMEA reports.

### [fallback]

Here the static fallback values for Your GPS position and the local air pressure are defined.
The pressure is necessary in order to correctly evaluate the altitude given from ADSB (transponder) targets.

### [filter]

Here the filters for height and distance of aircrafts are defined.
To disable a filter leave its value empty, or explicitly set it to `-1`.
Aircrafts beeing filtered out will not be reported.

### Per Feed Entry Section (e.g. [sbs1])

Every entry in the `feeds` list needs its own section, with exactly the same name as in the list.
If a feed has not all required parameters defined, or the section can not be found or evaluated, that feed will not be run.
The parameters for a feed section are

+ host
+ port
+ priority
+ login

Where `login` is only required for APRS feeds. `host` and `port` define the hostname /-address and port to connect to.
`priority` defines the priority relative to all other feeds of the same type.
The priority is an integer, where a higher value means a higher priority.

#### Ground-Mode

This is a feature that aims to let You stay independent with Your position, but operating static though.
If You have a GPS feed in use, there is no need to set Your position in the fallbacks.
But also, if You operate at a static position, there is no need to receive (possibly jumping) GPS positions all the time.
By activating the Ground-Mode, You tell VFR-B to stop the GPS feed as soon as it receives a "good" position.