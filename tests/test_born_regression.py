"""Small native regressions extracted from the existing Born example."""

from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import unittest
import io

from apbs_check_forces import (
    ApolarForce, PolarForce, compare_force_dicts, extract_forces, parse_forces,
)
from apbs_logger import Logger


ROOT = Path(__file__).resolve().parents[1]
APBS = None


@unittest.skipIf(APBS is None and __name__ != "__main__",
                 "Use CTest to supply the built APBS executable")
class BornRegressionTests(unittest.TestCase):
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
