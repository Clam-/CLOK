# tzdb update/packer

# when run, checks if locally stored tzdb database in TOPDIR is up to date or not.
# If not, will fetch tzdb, make it, and package it up in to a zoneinfo.tar in the TOPDIR
# along with the version file next to it.

# You probably want to call it something like:
# TOPDIR=$HOME/tzdir env/bin/python tzdb-updater.py

from os import environ, listdir, mkdir
from os.path import join
import requests
import tarfile
import subprocess
from shutil import copyfile
from tempfile import TemporaryDirectory
import lzip
import io

TOPDIR = environ["TOPDIR"]
try: mkdir(TOPDIR)
except FileExistsError: pass

ETAG = None
try:
    with open(join(TOPDIR,"etag"), encoding="utf-8") as f:
        ETAG = f.read()
except FileNotFoundError: pass

r = requests.get("https://www.iana.org/time-zones/repository/tzdb-latest.tar.lz",
    allow_redirects=True, headers={"If-None-Match": ETAG}, stream=True)
if r.status_code == 304: raise SystemExit(0)

with TemporaryDirectory() as TMPDIR:
    # save new ETAG
    with open(join(TOPDIR,"etag"), mode="w", encoding="utf-8") as f:
        f.write(r.headers["Etag"])

    # unzip to temp .tarfile
    with open(join(TMPDIR,"tzdata.tar"), mode="wb") as f:
        for chunk in lzip.decompress_file_like_iter(r.raw):
            f.write(chunk)

    with tarfile.open(name=join(TMPDIR,"tzdata.tar")) as tarf:
        tarf.extractall(path=TMPDIR, filter="data")
    r.raw.close()

    # append extracted foldername to TMPDIR
    TMPDIR = join(TMPDIR, listdir(TMPDIR)[0])
    with TemporaryDirectory() as TOPTZDIR:
        print(f"RUNNING: {' '.join(['make', f'TOPDIR={TOPTZDIR}', 'install'])}")
        subprocess.run(f"make TOPDIR={TOPTZDIR} install", shell=True, cwd=TMPDIR,
            stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)
        with tarfile.open(name=join(TOPDIR, "zoneinfo.tar"), mode="w") as ntarf:
            ntarf.add(join(TMPDIR, "version"), arcname="version")
            ntarf.add(join(TOPTZDIR, "usr", "share", "zoneinfo"), arcname="zoneinfo")
        copyfile(join(TMPDIR, "version"), join(TOPDIR, "version"))
