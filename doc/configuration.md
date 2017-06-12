# Configuration

## What to configure

The INI file has entries like this
>latitude=49.000000  
longitude=8.000000  
altitude=400  
geoid=47.0  
pressure=1013.25  
temp=15.0  
serverPort=1234  
aprscHost=localhost  
aprscPort=14580  
aprscLogin=user x pass x  
sbsHost=  
sbsPort=x  
climateSensorHost=nA  
climateSensorPort=0  
maxHeight=-1  
maxDist=40000

There are base position parameters, connection parameters and climate fallback values, along with filters.
To disable any input feed, just leave corresponding hostnames empty, or set them to *'nA'* .
Same with the filters, to disable leave it empty, or set to *-1* .
Additional information, like units, can be found in the comments.
Comments begin with a *;* , feel free to add comments containing whatever liked, e.g. available ports and hosts.
