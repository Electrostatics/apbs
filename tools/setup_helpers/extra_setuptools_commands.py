
from distutils.command.sdist import sdist
class SDistChecked(sdist):
    """ check submodules on sdist to prevent incomplete tarballs """
    def run(self):
        check_submodules()
        with concat_license_files():
            sdist.run(self)


