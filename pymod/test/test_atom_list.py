from unittest import TestCase, main as unittest_main
import os
import sys
import tempfile
sys.path.insert(0, '..')
from atom_list import AtomList

class TestAtomList(TestCase):
    '''
    Test all methods of clas AtomList
    '''

    fn = 'tmp.pdb'

    def setUp(self) -> None:
        with open(self.__class__.fn, 'w') as f:
            f.write('HETATM    1  C    ALK    1       1.000   4.000   7.000 0.000 1.000\n')
            f.write('HETATM    1  C    ALK    1       2.000   5.000   8.000 0.000 1.000\n')
            f.write('HETATM    1  C    ALK    1       3.000   6.000   9.000 0.000 1.000\n')

    def tearDown(self) -> None:
        ...

    def test_read_pdb(self) -> None:

        sut = AtomList(self.__class__.fn)
        self.assertEqual(len(sut._atoms), 3)

        a = sut._atoms[0]
        self.assertEqual(a.name, 'C')
        self.assertEqual(a.res_name, 'ALK')
        self.assertEqual(a.position.x, 1.)
        self.assertEqual(a.position.y, 4.)
        self.assertEqual(a.position.z, 7.)
        self.assertEqual(a.charge, 0.)
        self.assertEqual(a.radius, 1.)

    def test_min(self) -> None:

        sut = AtomList(self.__class__.fn)

        lo: Coordinate = sut.mincrd()
        self.assertEqual(lo.x, 1.)
        self.assertEqual(lo.y, 4.)
        self.assertEqual(lo.z, 7.)

    def test_max(self) -> None:

        sut = AtomList(self.__class__.fn)

        hi: Coordinate = sut.maxcrd()
        self.assertEqual(hi.x, 3.)
        self.assertEqual(hi.y, 6.)
        self.assertEqual(hi.z, 9.)

    def test_center(self) -> None:

        sut = AtomList(self.__class__.fn)

        mi: Coordinate = sut.center()
        self.assertEqual(mi.x, 2.)
        self.assertEqual(mi.y, 5.)
        self.assertEqual(mi.z, 8.)
