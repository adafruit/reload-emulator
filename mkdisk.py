from tempfile import TemporaryDirectory
import hashlib
import os
import pathlib
import requests
import subprocess

def get_url_with_checksum(url, hasher, digest):
    response = requests.get(url)
    response.raise_for_status()
    content = response.content
    got_digest = hashlib.new(hasher, content).hexdigest()
    if digest != got_digest:
        raise RuntimeError("Expected {digest} got {get_digest}")
    return content

def bin2c(target, data, per_row=8):
    for i in range(0, len(data), per_row):
        row = ",".join("% 3d" % b for b in data[i:i+per_row])
        print(f"    {row},", file=target)

def rom_array(target, name, data):
    print(f"const uint8_t {name}[] = {{", file=target)
    bin2c(target, data)
    print(f"}};", file=target)
    print


def do_woz(name, url, hasher, digest):
    filename = f"src/images/{name}.h"
    symbol = f"{name}_nib_image"

    if os.path.exists(filename):
        return

    print(f"Downloading and converting {name}")

    woz_content = get_url_with_checksum(url, hasher, digest)
    with TemporaryDirectory() as tmpdir:
        path = pathlib.Path(tmpdir)
        woz = (path / "input.woz")
        nib = (path / "input.nib")
        woz.write_bytes(woz_content)
        subprocess.check_call(["tools/woz2dsk/woz2dsk", woz, nib])
        nib_content = nib.read_bytes()

    with open(filename, "w") as f:
        rom_array(f, symbol, nib_content)

def do_dsk(name, url, hasher, digest, is_prodos):
    filename = f"src/images/{name}.h"
    symbol = f"{name}_nib_image"

    if os.path.exists(filename):
        return

    dashp = ["-pp"] if is_prodos else []

    print(f"Downloading and converting {name}")
    dsk_content = get_url_with_checksum(url, hasher, digest)
    with TemporaryDirectory() as tmpdir:
        path = pathlib.Path(tmpdir)
        dsk = (path / "input.dsk")
        nib = (path / "input.nib")
        dsk.write_bytes(dsk_content)
        subprocess.check_call(["tools/dsk2nib/src/dsk2nib", "-i", dsk, "-o", nib, *dashp])
        nib_content = nib.read_bytes()

    with open(filename, "w") as f:
        rom_array(f, symbol, nib_content)

#do_woz("moon_patrol", "https://archive.org/download/wozaday_Moon_Patrol/00playable.woz", "sha256", "13dd66d1fe653bd61038cf60a452d7c946cf05ccc8cf41bb35cfa79608b79bd4")

do_dsk("moon_patrol", "https://archive.org/download/Moon_Patrol/Moon_Patrol.dsk", "sha1", "edd50462f044fa416d19bdc43f61ab7b881de067", False)
do_dsk("moon_patrol_prodos", "https://archive.org/download/Moon_Patrol/Moon_Patrol.dsk", "sha1", "edd50462f044fa416d19bdc43f61ab7b881de067", True)
