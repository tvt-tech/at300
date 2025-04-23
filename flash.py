# /// script
# dependencies = [
#     "adafruit-ampy",
#     "mpy-cross",  # Optional, if using a Python wrapper for mpy-cross
# ]
# python = ">=3.9"
# ///

import os
import subprocess
import argparse
from pathlib import Path
from typing import Generator

# Argument parsing
def parse_args():
    parser = argparse.ArgumentParser(description="Build and flash mpy files.")
    parser.add_argument('-p', '--port', type=str, required=True, help="The port to use for uploading files (e.g., COM25, /dev/ttyUSB0).")
    parser.add_argument('-b', '--baud', type=int, default=115200, help="The baud rate to use for the connection (default is 115200).")
    return parser.parse_args()

root = Path(__file__).parent

arch = "armv6m"
out = root / 'build' / arch

def collect_src() -> Generator[Path, None, None]:
    src = root / 'src'
    py_files = (f for f in src.iterdir() if f.is_file() and f.suffix.lower() == '.py')
    return (f for f in py_files if not f.name.startswith(("__", "build", "setup")))


def build() -> None:
    os.makedirs(out, exist_ok=True)

    for i in collect_src():
        output_file = out / (i.stem + ".mpy")
        print(f"Compiling {i.name} -> {output_file.name}")
        subprocess.run([
            'mpy-cross',
            f"-march={arch}",
            "-o", output_file.as_posix(),
            i.as_posix()
        ], check=True)

# Flash the .mpy files
def flash(port: str, baud: int) -> None:
    for mpy_file in out.glob("*.mpy"):
        print(f"Uploading {mpy_file.name} to {port} at {baud} baud rate")
        subprocess.run([
            "ampy",
            "--port", port,
            "--baud", str(baud),
            "put", str(mpy_file)
        ], check=True)

if __name__ == "__main__":
    # Parse arguments
    args = parse_args()
    build()
    flash(args.port, args.baud)
