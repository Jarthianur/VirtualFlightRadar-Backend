# Operation - How To Run

## Manually

Don't care about the following, if boost was installed via package manager.
Boost is linked dynamically, which means the *LD_LIBRARY_PATH* variable has to be set to boost libs path.
This is done by the install.sh automatically through appending the export instruction to *~/.bashrc* .
To apply this change reboot the system.

```bash
{path to binary} -c {path to config file} > {path to log file}  2>&1 &
```

### Example

```bash
./vfrb -c vfrb.ini > vfrb.log 2>&1 &
```

The log will be in the specified file.
If You want to force the *Ground-Mode*, just add `-g` to the commandline arguments.

## As Service

By invoking `./install.sh service`, the systemd service for VFR-B was automatically configured to start after boot.  
To disable it call, where *servicename* is like *VFRB_NAME_B* from `bootstrap.sh`.

```bash
sudo systemctl disable {servicename}.service
```

### Run the service once by

```bash
sudo service {servicename} start
```

### Watch the log with

```bash
journalctl -u {servicename}.service
```

### Examples

```bash
sudo service vfrb start
# ------
sudo systemctl disable vfrb.service
# ------
journalctl -u vfrb.service
```