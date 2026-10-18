"""Small native regressions extracted from the existing Born example."""

from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import unittest
import io
import math

from apbs_check_forces import (
    ApolarForce, PolarForce, compare_force_dicts, extract_forces, parse_forces,
)
from apbs_logger import Logger


ROOT = Path(__file__).resolve().parents[1]
APBS = None


@unittest.skipIf(APBS is None and __name__ != "__main__",
                 "Use CTest to supply the built APBS executable")
class BornRegressionTests(unittest.TestCase):
    def run_example(self, name):
        source = ROOT / "examples/born"
        with tempfile.TemporaryDirectory(prefix="apbs-born-") as directory:
            work = Path(directory)
            for filename in (name + ".in", "ion.pqr", "parm.dat", "mesh.m"):
                shutil.copyfile(source / filename, work / filename)
            input_path = work / (name + ".in")
            records = []
            calculation = None
            for line in input_path.read_text().splitlines():
                fields = line.split()
                if fields[:2] == ["elec", "name"]:
                    calculation = fields[2]
                if fields[:1] == ["write"]:
                    self.assertIsNotNone(calculation)
                    self.assertEqual(fields[2], "dx")
                    # Preserve both solvent and reference maps.
                    fields[3] = calculation + "_" + fields[3]
                    line = "    " + " ".join(fields)
                records.append(line)
            input_path.write_text("\n".join(records) + "\n")
            process = subprocess.run(
                [APBS, name + ".in"], cwd=work, capture_output=True,
                text=True, timeout=180,
            )
            self.assertEqual(
                process.returncode, 0, process.stdout + process.stderr
            )
            energies = [
                float(value) for value in re.findall(
                    r"Global net (?:ELEC|APOL) energy\s*=\s*"
                    r"([+-]?\d+\.\d+E[+-]\d+)", process.stdout,
                )
            ]
            self.assertTrue(all(math.isfinite(value) for value in energies))
            maps = {}
            for path in work.glob("*.dx"):
                lines = path.read_text().splitlines()
                arrays = {}
                for index, line in enumerate(lines):
                    fields = line.split()
                    if line.startswith("object ") and "items" in fields:
                        identifier = int(fields[1])
                        items = int(fields[fields.index("items") + 1])
                        shape = (
                            int(fields[fields.index("shape") + 1])
                            if "shape" in fields else 1
                        )
                        self.assertGreater(items, 0, path.name)
                        self.assertEqual(lines[index + 1], "data follows")
                        data = []
                        for row in lines[index + 2:]:
                            if len(data) >= items * shape:
                                break
                            data.extend(map(float, row.split()))
                        self.assertEqual(
                            len(data), items * shape, path.name
                        )
                        self.assertTrue(
                            all(math.isfinite(value) for value in data),
                            path.name,
                        )
                        arrays[identifier] = (items, shape, data)
                self.assertEqual(set(arrays), {1, 2, 3}, path.name)
                self.assertEqual(arrays[1][1], 3, path.name)
                self.assertEqual(arrays[2][1], 4, path.name)
                self.assertEqual(arrays[3][:2], (arrays[1][0], 1))
                self.assertTrue(all(
                    value.is_integer() and 0 <= value < arrays[1][0]
                    for value in arrays[2][2]
                ), path.name)
                maps[path.name] = arrays
            return energies, maps

    def test_apolar_sphere_area_and_volume(self):
        energies, maps = self.run_example("apbs-apolar")
        self.assertEqual(len(energies), 2)
        self.assertFalse(maps)
        # parm.dat overrides ion.pqr with radius 2 A; gamma/press are unity.
        radius = 2
        area = 4 * math.pi * radius ** 2
        volume = 4 * math.pi * radius ** 3 / 3
        self.assertAlmostEqual(energies[0], area, delta=area * 1e-6)
        # Volume is integrated on the example's finite 0.1 A grid.
        self.assertAlmostEqual(energies[1], volume, delta=volume * 0.01)

    def test_finite_element_examples_and_external_mesh_equivalence(self):
        outputs = {}
        # Historical APBS 1.4.1 values documented in examples/born/README.md.
        for name, reference in (
            ("apbs-mol-fem", -231.9550),
            ("apbs-smol-fem", -230.9760),
            ("apbs-mol-fem-extmesh", -231.9550),
        ):
            with self.subTest(example=name):
                energies, maps = self.run_example(name)
                self.assertEqual(len(energies), 1)
                self.assertAlmostEqual(
                    energies[0], reference, delta=abs(reference) * 1e-4
                )
                calculations = (
                    ("born",) if name == "apbs-smol-fem"
                    else ("born", "born_ref")
                )
                self.assertEqual(set(maps), {
                    f"{calculation}_{kind}.dx"
                    for calculation in calculations for kind in ("pot", "acc")
                })
                for calculation in calculations:
                    accessibility = maps[f"{calculation}_acc.dx"][3][2]
                    self.assertGreater(max(accessibility), min(accessibility))
                    self.assertTrue(all(
                        -1e-6 <= value <= 1 + 1e-6 for value in accessibility
                    ))
                    potential = maps[f"{calculation}_pot.dx"][3][2]
                    self.assertGreater(max(potential), min(potential))
                outputs[name] = (energies, maps)
        self.assertEqual(len(outputs), 3)
        internal = outputs["apbs-mol-fem"]
        external = outputs["apbs-mol-fem-extmesh"]
        self.assertAlmostEqual(internal[0][0], external[0][0], delta=1e-6)
        for name in internal[1]:
            for identifier in (1, 2, 3):
                first_array = internal[1][name][identifier]
                second_array = external[1][name][identifier]
                self.assertEqual(first_array[:2], second_array[:2])
                for first, second in zip(first_array[2], second_array[2]):
                    self.assertAlmostEqual(first, second, delta=1e-6)

    def calculate_energy(self, shift):
        with tempfile.TemporaryDirectory(prefix="apbs-born-") as directory:
            work = Path(directory)
            shutil.copyfile(
                ROOT / "examples/born/apbs-mol-auto.in", work / "born.in"
            )
            records = []
            source = ROOT / "examples/born/ion.pqr"
            for line in source.read_text().splitlines():
                if line.startswith(("ATOM", "HETATM")):
                    fields = line.split()
                    coordinates = [
                        float(value) + offset
                        for value, offset in zip(fields[-5:-2], shift)
                    ]
                    fields[-5:-2] = [f"{value:.6f}" for value in coordinates]
                    line = " ".join(fields)
                records.append(line)
            (work / "ion.pqr").write_text("\n".join(records) + "\n")
            process = subprocess.run(
                [APBS, "born.in"], cwd=work, capture_output=True,
                text=True, timeout=120,
            )
            self.assertEqual(
                process.returncode, 0, process.stdout + process.stderr
            )
            energies = re.findall(
                r"Global net ELEC energy\s*=\s*([+-]?\d+\.\d+E[+-]\d+)",
                process.stdout,
            )
            self.assertEqual(len(energies), 1, process.stdout)
            return float(energies[0])

    def test_existing_solvation_baseline_and_translation_invariance(self):
        reference = self.calculate_energy((0, 0, 0))
        # Existing born/apbs-mol-auto expectation in test_cases.cfg (kJ/mol).
        self.assertAlmostEqual(reference, -2.297735411962E+02, delta=0.023)
        translated = self.calculate_energy((2.5, -1.25, 3.75))
        self.assertAlmostEqual(translated, reference, delta=1e-6)

    def test_converged_centered_ion_forces(self):
        source = ROOT / "examples/born"
        with tempfile.TemporaryDirectory(prefix="apbs-forces-") as directory:
            work = Path(directory)
            for filename in ("ion.pqr", "parm.dat"):
                shutil.copyfile(source / filename, work / filename)
            text = (source / "apbs-forces.in").read_text()
            self.assertEqual(text.count("    mg-auto"), 2)
            text = text.replace("    mg-auto", "    mg-auto\n    etol 1e-10")
            (work / "forces.in").write_text(text)
            process = subprocess.run(
                [APBS, "forces.in"], cwd=work, capture_output=True,
                text=True, timeout=120,
            )
            self.assertEqual(
                process.returncode, 0, process.stdout + process.stderr
            )
            lines = process.stdout.splitlines()
            polar = extract_forces(PolarForce, lines, "print force")
            expected = parse_forces(PolarForce, [
                f"  {component} 0.000E+00 0.000E+00 0.000E+00"
                for component in ("tot", "qf", "ib", "db")
            ])
            logger = Logger(io.StringIO(), io.StringIO())
            # Centered spherical ion: each polar force component vanishes.
            compare_force_dicts(polar, expected, logger)
            apolar = extract_forces(ApolarForce, lines, "print APOL force")
            reference = parse_forces(
                ApolarForce, (source / "apolarforces").read_text().splitlines()
            )
            compare_force_dicts(apolar, reference, logger)


if __name__ == "__main__":
    APBS = str(Path(sys.argv.pop(1)).resolve())
    unittest.main()
