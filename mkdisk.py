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
        raise RuntimeError(f"Expected {digest} got {got_digest}")
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


def do_dsk(name, url, hasher, digest, is_prodos):
    filename = f"src/images/{name}.h"
    symbol = f"{name}_nib_image"
    nibs.append(symbol)
    includes.append(f"{name}.h")

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

nibs = []
includes = []

do_dsk("prodos", "https://archive.org/download/ProDOS_2_4_1/ProDOS_2_4_1.dsk", "sha1", "88d0d66867e607d6ee1117f61b83f8fe37d29f69", False) ## !!
do_dsk("moon_patrol", "https://archive.org/download/Moon_Patrol/Moon_Patrol.dsk", "sha1", "edd50462f044fa416d19bdc43f61ab7b881de067", False)

# TODO: neptune, karateka, lode runner?

with open("src/images/apple2_images.h", "w") as f:
    print("#pragma once", file=f)
    print(file=f)

    for i in includes:
        print(f"#include \"{i}\"", file=f)
    print(file=f)

    print("uint8_t* const apple2_nib_images[] = {", file=f)
    for n in nibs:
        print(f"    (uint8_t*){n},", file=f)
    print("};", file=f)

    print("""uint8_t* apple2_po_images[] = {}; uint32_t apple2_po_image_sizes[] = {}; char* apple2_msc_images[] = {};""", file=f)
