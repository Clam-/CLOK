Two components: the microcontroller arduino code that runs the clock itself,
and a python script that maintains a zoneinfo timezone database archive for the clock
to update itself.

## CLOK.ino
The arduino code that runs on the microcontroller.

## tzdb-updater.py
This is the python script which maintains a zoneinfo archive for the clock device.
You probably want to make/use a virtualenv for this, and install the requirements:
```~/env/bin/pip install -r requirements.txt```

Then you'll probably want to place this script in a contab or similar with something like:
```22 */6 * * * TOPDIR=$HOME/tzdir ~/env/bin/python tzdb-updater.py >/dev/null 2>&1```
(Runs every 6 hours)

P.S. I use tarfile filter option which needs at least python 3.9.17

Or you can just point the CLOK to my zoneinfo server: https://zoneinfo.nyanya.org and don't worry about the tzdb-updater.
