"""Exercise atom-potential output using the built APBS executable."""

import math
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest


APBS = None


@unittest.skipIf(APBS is None and __name__ != "__main__",
                 "Use CTest to supply the built APBS executable")
class AtomPotentialTests(unittest.TestCase):
    def check_grid_values(self, path, positions, values):
        lines = path.read_text().splitlines()
        counts = None
        origin = None
        deltas = []
        for index, line in enumerate(lines):
            fields = line.split()
            if "gridpositions" in fields and "counts" in fields:
                counts = tuple(map(int, fields[-3:]))
            elif fields and fields[0] == "origin":
                origin = tuple(map(float, fields[1:]))
            elif fields and fields[0] == "delta":
                deltas.append(tuple(map(float, fields[1:])))
            elif "data follows" in line:
                items = int(fields[fields.index("items") + 1])
                data = []
                for row in lines[index + 1:]:
                    if len(data) >= items:
                        break
                    data.extend(map(float, row.split()))
                break
        else:
            self.fail("OpenDX output has no data array")
        self.assertIsNotNone(counts)
        self.assertIsNotNone(origin)
        self.assertEqual(len(deltas), 3)
        self.assertEqual(len(data), math.prod(counts))
        self.assertTrue(all(math.isfinite(value) for value in data))
        for axis, delta in enumerate(deltas):
            self.assertGreater(delta[axis], 0)
            self.assertTrue(all(
                value == 0 for other, value in enumerate(delta)
                if other != axis
            ))
        for position, actual in zip(positions, values):
            coordinates = [
                (position[axis] - origin[axis]) / deltas[axis][axis]
                for axis in range(3)
            ]
            self.assertTrue(all(
                0 <= coordinate < count - 1
                for coordinate, count in zip(coordinates, counts)
            ))
            lower = [math.floor(coordinate) for coordinate in coordinates]
            fractions = [
                coordinate - cell
                for coordinate, cell in zip(coordinates, lower)
            ]
            expected = 0
            # OpenDX varies the last (z) index fastest.
            for x in (0, 1):
                for y in (0, 1):
                    for z in (0, 1):
                        offsets = (x, y, z)
                        weight = math.prod(
                            fraction if offset else 1 - fraction
                            for fraction, offset in zip(fractions, offsets)
                        )
                        ix, iy, iz = [
                            cell + offset
                            for cell, offset in zip(lower, offsets)
                        ]
                        expected += weight * data[
                            (ix * counts[1] + iy) * counts[2] + iz
                        ]
            self.assertAlmostEqual(
                actual, expected, delta=max(1e-6, abs(expected) * 2e-6)
            )

    def run_calculation(
        self, molecules, molid=1, writes=1, automatic=False,
        selections=None, check_grid=False,
    ):
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            for index, positions in enumerate(molecules, 1):
                records = [
                    f"ATOM {atom} ION ION 1 {x:.8f} {y:.8f} {z:.8f} 1.0 1.0\n"
                    for atom, (x, y, z) in enumerate(positions, 1)
                ]
                (work / f"mol{index}.pqr").write_text("".join(records))
            reads = "\n".join(
                f"mol pqr mol{index}.pqr"
                for index in range(1, len(molecules) + 1)
            )
            if selections is None:
                selections = [molid]
            grid = (
                "mg-auto\ncglen 16 16 16\nfglen 8 8 8\n"
                "cgcent 0 0 0\nfgcent 0 0 0"
                if automatic else "mg-manual\nglen 8 8 8\ngcent 0 0 0"
            )
            calculations = []
            for calculation, selected in enumerate(selections):
                outputs = "\n".join(
                    f"write atompot flat atoms{calculation}_{index}"
                    for index in range(writes)
                )
                if check_grid:
                    outputs += f"\nwrite pot dx grid{calculation}"
                calculations.append(f"""elec
{grid}
dime 33 33 33
mol {selected}
lpbe
bcfl zero
pdie 1
sdie 80
chgm spl2
srfm mol
srad 1.4
swin 0.3
sdens 10
temp 298.15
calcenergy no
calcforce no
{outputs}
end
""")
            (work / "test.in").write_text(
                f"read\n{reads}\nend\n" + "".join(calculations) + "quit\n"
            )
            process = subprocess.run(
                [APBS, "test.in"], cwd=work, capture_output=True,
                text=True, timeout=60,
            )
            self.assertEqual(
                process.returncode, 0, process.stdout + process.stderr
            )
            results = []
            for calculation, selected in enumerate(selections):
                for index in range(writes):
                    path = work / f"atoms{calculation}_{index}.txt"
                    self.assertTrue(
                        path.exists(), process.stdout + process.stderr
                    )
                    values = [
                        float(line) for line in path.read_text().splitlines()
                        if line.strip() and not line.startswith("#")
                    ]
                    self.assertEqual(len(values), len(molecules[selected - 1]))
                    self.assertTrue(
                        all(math.isfinite(value) for value in values)
                    )
                    if check_grid:
                        self.check_grid_values(
                            work / f"grid{calculation}.dx",
                            molecules[selected - 1], values,
                        )
                    results.append(values)
            return results

    def test_second_molecule_matches_first(self):
        target = [(0, 0, 0), (1, 0, 0)]
        other = [(0, 1, 0)]
        for automatic in (False, True):
            with self.subTest(automatic=automatic):
                first = self.run_calculation(
                    [target, other], automatic=automatic
                )
                second = self.run_calculation(
                    [other, target], molid=2, automatic=automatic
                )
                self.assertEqual(first, second)
                self.assertNotEqual(first[0][0], 0)

    def test_third_molecule_matches_first(self):
        target = [
            (-0.37, 0.19, 0.41), (0.82, -0.28, 0.13), (0.21, 1.12, -0.63)
        ]
        others = [[(-1, 0, 0)], [(0, -1, 0), (0, -2, 0)]]
        for automatic in (False, True):
            with self.subTest(automatic=automatic):
                first = self.run_calculation(
                    [target] + others, automatic=automatic, check_grid=True
                )
                third = self.run_calculation(
                    others + [target], molid=3,
                    automatic=automatic, check_grid=True,
                )
                self.assertEqual(first, third)

    def test_successive_calculations_switch_molecules(self):
        molecules = [
            [(-0.31, 0.17, 0.43)],
            [(0.71, -0.23, 0.11), (-0.49, 0.83, -0.37)],
            [(1.13, 0.29, -0.61), (-0.73, -0.41, 0.89), (0.19, 1.07, 0.53)],
        ]
        selections = [3, 1, 2, 3]
        for automatic in (False, True):
            with self.subTest(automatic=automatic):
                independent = [
                    self.run_calculation(
                        [molecule], automatic=automatic, check_grid=True
                    )[0] for molecule in molecules
                ]
                successive = self.run_calculation(
                    molecules, selections=selections,
                    automatic=automatic, check_grid=True,
                )
                self.assertEqual(
                    successive,
                    [independent[selected - 1] for selected in selections],
                )

    def test_multiple_write_directives(self):
        results = self.run_calculation([[(0, 0, 0)]], writes=3)
        self.assertEqual(results[0], results[1])
        self.assertEqual(results[1], results[2])

    def test_atoms_outside_grid_faces(self):
        for axis in range(3):
            for coordinate in (-4.01, 4.01):
                with self.subTest(axis=axis, coordinate=coordinate):
                    position = [0, 0, 0]
                    position[axis] = coordinate
                    results = self.run_calculation(
                        [[tuple(position), (0, 0, 0)]]
                    )
                    self.assertEqual(results[0][0], 0)
                    self.assertNotEqual(results[0][1], 0)

    def test_atoms_on_grid_faces(self):
        # The input explicitly sets zero Dirichlet boundary potentials.
        for axis in range(3):
            for coordinate in (-4, 4):
                with self.subTest(axis=axis, coordinate=coordinate):
                    position = [0, 0, 0]
                    position[axis] = coordinate
                    results = self.run_calculation([[tuple(position)]])
                    self.assertEqual(results[0][0], 0)


if __name__ == "__main__":
    APBS = str(Path(sys.argv.pop(1)).resolve())
    unittest.main()
