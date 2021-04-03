"""Set up the APBS package."""
import setuptools


setuptools.setup(
    name="apbs",
    version="broken",
    description="APBS biomolecular solvation software",
    python_requires=">=3.8",
    author="Nathan Baker",
    author_email="nathanandrewbaker@gmail.com",
    url="https://www.poissonboltzmann.org",
    packages=setuptools.find_packages(),
    package_data={"": ["data/input-schema.json"]},
    install_requires=["numpy", "jsonschema"],
    tests_require=["pytest", "requests"],
    keywords="science chemistry biophysics biochemistry",
    classifiers=[
        "Development Status :: 4 - Beta",
        "Environment :: Console",
        "Intended Audience :: Science/Research",
        "License :: OSI Approved :: Common Public License",
        "Natural Language :: English",
        "Operating System :: MacOS",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX",
        "Programming Language :: Python :: 3.8",
        "Programming Language :: Python :: 3.9",
        "Topic :: Scientific/Engineering :: Bio-Informatics",
        "Topic :: Scientific/Engineering :: Chemistry",
        "Topic :: Scientific/Engineering :: Physics",
    ],
)
