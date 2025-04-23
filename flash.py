# /// script
# dependencies = [
#     "adafruit-ampy",
#     "mpy-cross",  # Optional, if using a Python wrapper for mpy-cross
# ]
# python = ">=3.9"
# ///

import argparse
import os
import platform
import posixpath
import shutil
from pathlib import Path
from typing import Generator

import ampy.cli
import ampy.files
import ampy.pyboard
import mpy_cross.__init__ as mpy


# Argument parsing
def parse_args():
    parser = argparse.ArgumentParser(description="Build and flash mpy files.")
    parser.add_argument('-p', '--port', type=str, required=True,
                        help="The port to use for uploading files (e.g., COM25, /dev/ttyUSB0).")
    parser.add_argument('-b', '--baud', type=int, default=115200,
                        help="The baud rate to use for the connection (default is 115200).")
    return parser.parse_args()


root = Path(__file__).parent
arch = "armv6m"
out = root / 'build' / arch
src = root / 'src'


def collect_src() -> Generator[Path, None, None]:
    py_files = (f for f in src.iterdir() if f.is_file() and f.suffix.lower() == '.py')
    return (f for f in py_files if not f.name.startswith(("__", "build", "setup", "boot", "main")))


def _mpy_cross_run(*args):
    compat = None
    bytever = None
    pop = []
    argv = args[1:]
    for i, arg in enumerate(argv):
        a = arg.split("=")
        if a[0] in ("-c", "--compat", "-b", "--bytecode"):
            if compat or bytever:
                raise SystemExit("Error: -b and -c are mutually exclusive.")
            pop.append(i)
            if len(a) > 1:
                val = a[1]
            else:
                val = argv[i + 1]
                pop.append(i + 1)
            if "-c" in a[0]:
                compat = val
            else:
                bytever = val
    for i in reversed(pop):
        argv.pop(i)
    if compat or bytever:
        mpy.set_version(compat, bytever)

    if "-h" in argv or "--help" in argv:
        mpy.usage()
    else:
        mpy.run(*argv).wait()


_board = None


def _ampy_run(port, baud, delay=0):
    global _board
    # On Windows fix the COM port path name for ports above 9 (see comment in
    # windows_full_port_name function).
    if platform.system() == "Windows":
        port = ampy.cli.windows_full_port_name(port)
    _board = ampy.pyboard.Pyboard(port, baudrate=baud, rawdelay=delay)


def _ampy_put(local, remote):
    """Put a file or folder and its contents on the board.

    Put will upload a local file or folder  to the board.  If the file already
    exists on the board it will be overwritten with no warning!  You must pass
    at least one argument which is the path to the local file/folder to
    upload.  If the item to upload is a folder then it will be copied to the
    board recursively with its entire child structure.  You can pass a second
    optional argument which is the path and name of the file/folder to put to
    on the connected board.

    For example to upload a main.py from the current directory to the board's
    root run:

      ampy --port /board/serial/port put main.py

    Or to upload a board_boot.py from a ./foo subdirectory and save it as boot.py
    in the board's root run:

      ampy --port /board/serial/port put ./foo/board_boot.py boot.py

    To upload a local folder adafruit_library and all of its child files/folders
    as an item under the board's root run:

      ampy --port /board/serial/port put adafruit_library

    Or to put a local folder adafruit_library on the board under the path
    /lib/adafruit_library on the board run:

      ampy --port /board/serial/port put adafruit_library /lib/adafruit_library
    """
    # Use the local filename if no remote filename is provided.
    if remote is None:
        remote = os.path.basename(os.path.abspath(local))
    # Check if path is a folder and do recursive copy of everything inside it.
    # Otherwise it's a file and should simply be copied over.
    if os.path.isdir(local):
        # Directory copy, create the directory and walk all children to copy
        # over the files.
        board_files = ampy.files.Files(_board)
        for parent, child_dirs, child_files in os.walk(local, followlinks=True):
            # Create board filesystem absolute path to parent directory.
            remote_parent = posixpath.normpath(
                posixpath.join(remote, os.path.relpath(parent, local))
            )
            try:
                # Create remote parent directory.
                board_files.mkdir(remote_parent)
            except ampy.files.DirectoryExistsError:
                # Ignore errors for directories that already exist.
                pass
            # Loop through all the files and put them on the board too.
            for filename in child_files:
                with open(os.path.join(parent, filename), "rb") as infile:
                    remote_filename = posixpath.join(remote_parent, filename)
                    board_files.put(remote_filename, infile.read())


    else:
        # File copy, open the file and copy its contents to the board.
        # Put the file on the board.
        with open(local, "rb") as infile:
            board_files = ampy.files.Files(_board)
            board_files.put(remote, infile.read())


def build() -> None:
    try:
        os.remove(out)
    except Exception:
        pass
    os.makedirs(out, exist_ok=True)

    # Copy main.py and boot.py to the output directory
    for filename in ['main.py', 'boot.py']:
        src_file = src / filename
        if src_file.exists():
            shutil.copy(src_file, out / filename)
            print(f"Copied {filename} to {out / filename}")
        else:
            print(f"Warning: {filename} not found in src directory.")

    for i in collect_src():
        output_file = out / (i.stem + ".mpy")
        print(f"Compiling {i.name} -> {output_file.name}")
        argv = [
            "mpy-cross",
            f"-march={arch}",
            "-o", output_file.as_posix(),
            i.as_posix(),
        ]
        _mpy_cross_run(*argv)


# Flash the .mpy files
def flash(port: str, baud: int) -> None:
    try:
        _ampy_run(port, baud)

        # Include both .mpy and .py files in the upload
        for py_file in out.glob("*.py"):
            print(f"Uploading {py_file.name} to {port} at {baud} baud rate")
            _ampy_put(py_file.as_posix(), py_file.name)

        for mpy_file in out.glob("*.mpy"):
            print(f"Uploading {mpy_file.name} to {port} at {baud} baud rate")
            _ampy_put(mpy_file.as_posix(), mpy_file.name)

    finally:
        # Try to ensure the board serial connection is always gracefully closed.
        if _board is not None:
            try:
                _board.close()
            except:
                # Swallow errors when attempting to close as it's just a best effort
                # and shouldn't cause a new error or problem if the connection can't
                # be closed.
                pass


if __name__ == "__main__":
    # Parse arguments
    args = parse_args()
    build()
    flash(args.port, args.baud)
