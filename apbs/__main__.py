import os
import apbs.bin
import sys
import subprocess

apbspath = os.path.join(apbs.bin.get_path(), "apbs")
if len(sys.argv) < 2:
    raise RuntimeError(
        "You must provide a valid binary. Please see `apbs.bin.get_path()` for valid binaries."
    )
bin = sys.argv[1]
if len(sys.argv) > 2:
    subprocess.run(
        [str(os.path.join(apbs.bin.get_path(), sys.argv[1])), *sys.argv[2:]]
    )
else:
    subprocess.run(str(os.path.join(apbs.bin.get_path(), sys.argv[1])))
