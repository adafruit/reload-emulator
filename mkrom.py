import hashlib
import requests

def get_url_with_checksum(url, hasher, digest):
    response = requests.get(url)
    response.raise_for_status()
    content = response.content
    got_digest = hashlib.new(hasher, content).hexdigest()
    if digest != got_digest:
        raise RuntimeError("Expected {digest} got {get_digest}")
    return content

rom_e10 = get_url_with_checksum("https://mirrors.apple2.org.za/Apple%20II%20Documentation%20Project/Computers/Apple%20II/Apple%20IIe/ROM%20Images/Apple%20IIe%20Enhanced%20ROM%20Pages%20C0-DF%20-%20342-0304-A%20-%201984.bin", "md5", "6b11018a14668a7e3904319d2097dddc")
rom_e8 = get_url_with_checksum("https://mirrors.apple2.org.za/Apple%20II%20Documentation%20Project/Computers/Apple%20II/Apple%20IIe/ROM%20Images/Apple%20IIe%20Enhanced%20ROM%20Pages%20E0-FF%20-%20342-0303-A%20-%201984.bin", "md5", "6ac9338f6972eea01604f90d9a581f18")
rom_chr = get_url_with_checksum("https://mirrors.apple2.org.za/Apple%20II%20Documentation%20Project/Computers/Apple%20II/Apple%20IIe/ROM%20Images/Apple%20IIe%20Enhanced%20Video%20ROM%20-%20342-0265-A%20-%20US%201983.bin", "md5", "9123fff3442c0e688cc6816be88dd4ab")
rom_kbd = get_url_with_checksum("https://mirrors.apple2.org.za/Apple%20II%20Documentation%20Project/Computers/Apple%20II/Apple%20IIe/ROM%20Images/Apple%20IIe%20Enhanced%20Keyboard%20ROM%20-%20341-0132-D%20-%20US-Dvorak%201984.bin", "md5", "b956c537e7b6b85ffa5c3493b1490d8a")

def bin2c(data, per_row=8):
    for i in range(0, len(data), per_row):
        row = ",".join("% 3d" % b for b in data[i:i+per_row])
        print(f"    {row},")

def rom_array(name, data):
    print(f"uint8_t __not_in_flash() {name}[] = {{")
    bin2c(data)
    print(f"}};")
    print

print("#pragma once")
print("// clang-format off")
rom_array("apple2e_rom", rom_e10 + rom_e8)
rom_array("apple2e_character_rom", rom_chr)
rom_array("apple2e_keyboard_rom", rom_kbd)
print("// clang-format on")
