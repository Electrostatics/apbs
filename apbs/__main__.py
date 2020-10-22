
import apbs.bin
import sys
import subprocess

apbspath = os.path.join(apbs.bin.get_path(), 'apbs')
subprocess.run([str(apbspath), *sys.argv[1:])
