import os
import sys
import glob
import pprint
from distutils import sysconfig as sc

from tools.setup_helpers.cmake_setuptools import *
from tools.setup_helpers.extra_setuptools_commands import *
from setuptools import setup, find_packages

pp = pprint.PrettyPrinter(indent=2)
proj_name = "apbs"

if not os.path.exists(os.path.join(".", __file__)):
    print("-- Must run setup.py in project root directory!")
    sys.exit()

if os.path.exists("MANIFEST"):
    os.remove("MANIFEST")

proj_root = os.path.dirname(os.path.abspath(__file__))
old_path = os.getcwd()
os.chdir(proj_root)
sys.path.insert(0, proj_root)

# This class must be in setup.py to find package root reliably
class CleanBuild(Command):
    """Clean build directory"""

    user_options = []

    def initialize_options(self):
        ...

    def finalize_options(self):
        ...

    def run(self):
        build_dir = get_build_dir()
        rmtree(build_dir)
        for subdir in ("lib", "bin"):
            for f in os.listdir(os.path.join(proj_root, "apbs", subdir)):
                if not f.endswith(".py"):
                    os.remove(os.path.join(proj_root, "apbs", subdir, f))


python_site_pkgs = sc.get_python_lib(prefix="", plat_specific=True)
print(f"-- Found python site-packages directory {python_site_pkgs}")

extra_cmake_args = dict(
    BUILD_DOC="OFF",
    BUILD_SHARED_LIBS="OFF",
    CMAKE_BUILD_TYPE="Release",
    ENABLE_BEM="ON",
    ENABLE_GEOFLOW="ON",
    ENABLE_FETK="ON",
    ENABLE_OPENMP="ON",
    ENABLE_PBAM="ON",
    ENABLE_PBSAM="ON",
    ENABLE_PYTHON="ON",
    ENABLE_TESTS="ON",
    ENABLE_TINKER="OFF",
)

if os.environ.get("Python_ROOT_DIR", None):
    extra_cmake_args["Python_ROOT_DIR"] = os.environ["Python_ROOT_DIR"]
    PYTHON_SITE_PACKAGES_DIR = python_site_pkgs

# Skip installation step if building a whl
if "bdist" in sys.argv or "bdist_wheel" in sys.argv:
    os.environ["DO_INSTALL"] = "0"

raw_package_data = {
    "apbs.bindings": glob.glob(os.path.join("apbs", "bindings", "*")),
    "apbs.lib": glob.glob(os.path.join("apbs", "lib", "*")),
    "apbs.bin": glob.glob(os.path.join("apbs", "bin", "*")),
}

# Filter out python and pyc files from package data
package_data = {}
for k, v in raw_package_data.items():
    package_data[k] = []
    for f in v:
        if not f.endswith(".py") and "__pycache__" not in f:
            package_data[k].append(os.path.basename(f))

print("Package data:")
pp.pprint(package_data)

setup(
    name=proj_name,
    version="3.1.0",
    author="",
    author_email="asher.mancinelli@pnnl.gov",
    description="Adaptive Poisson-Boltzmann Solver",
    long_description=open("LICENSE.md", "r").read(),
    ext_modules=[
        CMakeExtension(f"{proj_root}.apbs.bindings.pybind", **extra_cmake_args)
    ],
    distclass=BinaryDistribution,
    cmdclass={
        "build_ext": CMakeBuild,
        "install": CMakeInstall,
        "develop": CMakeInstall,
        "clean": CleanBuild,
        "sdist": SDistChecked,
        "test": CMakeTest,
    },
    zip_safe=False,
    packages=find_packages(include=["apbs", "apbs.*"]),
    include_package_data=True,
    package_data=package_data,
    classifiers=[
        "Programming Language :: C++",
        "Programming Language :: Python :: 3",
    ]
    + [f"Programming Language :: Python :: 3.{i}" for i in range(6, 9)],
)
