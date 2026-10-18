"""Regression tests for failures previously hidden by the native harness."""

import io
from pathlib import Path
import subprocess
import sys
from unittest.mock import Mock

import pytest


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tests"))

import apbs_tester as harness  # noqa E402
from apbs_check_forces import (  # noqa E402
    ApolarForce, PolarForce, compare_force_dicts, extract_forces, parse_forces,
)
from apbs_logger import Logger  # noqa E402


@pytest.fixture
def logger():
    return Logger(io.StringIO(), io.StringIO())


def test_nonzero_exit_rejected_with_matching_energy(tmp_path, monkeypatch):
    monkeypatch.chdir(tmp_path)
    Path("failed.in").write_text(
        'print("Global net ELEC energy = 1.000000E+00 kJ/mol")\n'
        'raise SystemExit(13)\n'
    )
    with pytest.raises(subprocess.CalledProcessError) as error:
        harness.process_serial(sys.executable, "failed.in")
    assert error.value.returncode == 13
    assert "1.000000E+00" in Path("failed.out").read_text()


def test_timeout_preserves_output(tmp_path, monkeypatch):
    monkeypatch.chdir(tmp_path)
    Path("slow.in").write_text(
        'import time\nprint("started", flush=True)\ntime.sleep(60)\n'
    )
    with pytest.raises(subprocess.TimeoutExpired):
        harness.process_serial(sys.executable, "slow.in", timeout=1)
    assert "started" in Path("slow.out").read_text()


def test_successful_output_and_dotted_filename(tmp_path, monkeypatch):
    monkeypatch.chdir(tmp_path)
    Path("case.v1.in").write_text(
        'print("Global net ELEC energy = -1.250000E+00 kJ/mol")\n'
    )
    assert harness.process_serial(sys.executable, "case.v1.in") == [-1.25]
    assert Path("case.v1.out").exists()


@pytest.mark.parametrize("fails", [False, True])
def test_example_runs_are_isolated(tmp_path, logger, fails):
    source = tmp_path / "example"
    source.mkdir()
    program = (
        'from pathlib import Path\n'
        'Path("input.dat").write_text("modified")\n'
        'Path("generated.dat").write_text("output")\n'
        'print("Global net ELEC energy = 1.000000E+00 kJ/mol")\n'
    )
    if fails:
        program += "raise SystemExit(1)\n"
    (source / "case.in").write_text(program)
    (source / "input.dat").write_text("original")
    original = Path.cwd()
    args = (sys.executable, [("case", "1")], "example", source, None,
            logger, False)
    if fails:
        with pytest.raises(RuntimeError):
            harness.run_test(*args)
    else:
        harness.run_test(*args)
    assert Path.cwd() == original
    assert (source / "input.dat").read_text() == "original"
    assert not (source / "generated.dat").exists()
    assert not (source / "case.out").exists()


def test_failed_setup_is_reported_and_restores_directory(tmp_path, logger):
    (tmp_path / "setup.py").write_text("raise SystemExit(7)\n")
    original = Path.cwd()
    with pytest.raises(subprocess.CalledProcessError):
        harness.run_test(
            sys.executable, [], "setup", tmp_path, "python setup.py",
            logger, False,
        )
    assert Path.cwd() == original


@pytest.mark.parametrize("emitted", [0, 2])
def test_missing_and_extra_energies_fail(tmp_path, logger, emitted):
    (tmp_path / "case.in").write_text(
        'print("Global net ELEC energy = 1.000000E+00 kJ/mol")\n' * emitted
    )
    with pytest.raises(RuntimeError):
        harness.run_test(
            sys.executable, [("case", "1")], "counts", tmp_path, None,
            logger, False,
        )


def test_parallel_result_count_mismatch_fails(monkeypatch, logger):
    monkeypatch.setattr(harness, "split_input", Mock())
    monkeypatch.setattr(
        harness, "process_serial", Mock(side_effect=[[1, 2], [3]])
    )
    with pytest.raises(RuntimeError, match="Inconsistent result counts"):
        harness.process_parallel("apbs", "parallel.in", 2, logger)


def test_force_branch_executes_binary_and_reads_output(
    tmp_path, monkeypatch, logger,
):
    (tmp_path / "forces.in").write_text("input is not force output")
    execute = Mock(return_value=[])
    check = Mock()
    monkeypatch.setattr(harness, "process_serial", execute)
    monkeypatch.setattr(harness, "check_forces", check)
    harness.run_test(
        sys.executable, [("forces", "forces")], "forces", tmp_path, None,
        logger, False,
    )
    execute.assert_called_once_with(str(Path(sys.executable).resolve()),
                                    "forces.in", 600)
    assert check.call_args.args[0] == "forces.out"


def test_existing_born_force_baseline_detects_wrong_value(logger):
    lines = (ROOT / "examples/born/polarforces").read_text().splitlines()
    expected = parse_forces(PolarForce, lines)
    actual = parse_forces(PolarForce, lines)
    compare_force_dicts(actual, expected, logger)
    actual["tot"].x += 0.01
    with pytest.raises(RuntimeError, match="Force mismatches"):
        compare_force_dicts(actual, expected, logger)


@pytest.mark.parametrize("empty", [True, False])
def test_missing_force_components_fail(logger, empty):
    lines = (ROOT / "examples/born/polarforces").read_text().splitlines()
    expected = parse_forces(PolarForce, lines)
    actual = {} if empty else parse_forces(PolarForce, lines[:-1])
    with pytest.raises(ValueError, match="components differ"):
        compare_force_dicts(actual, expected, logger)


def test_apolar_per_atom_and_total_are_not_overwritten():
    lines = (ROOT / "examples/born/apolarforces").read_text().splitlines()
    forces = parse_forces(ApolarForce, lines)
    assert len(forces) == 5
    assert ("tot", "0") in forces
    assert ("tot", "all") in forces


def test_missing_force_output_fails():
    with pytest.raises(ValueError, match="No force results"):
        extract_forces(PolarForce, ["print force\n"], "print force")


def test_force_output_at_end_of_file_keeps_last_component():
    lines = ["print force\n", "  tot 1.000E+00 2.000E+00 3.000E+00\n"]
    assert extract_forces(PolarForce, lines, "print force")["tot"].z == 3


def test_configured_input_paths_match_exact_case():
    from configparser import ConfigParser

    config = ConfigParser()
    config.optionxform = str
    config.read(ROOT / "tests/test_cases.cfg")
    for section in config.sections():
        directory = ROOT / "tests" / config[section]["input_dir"]
        names = {path.name for path in directory.iterdir()}
        for name in config[section]:
            if name not in ("input_dir", "setup"):
                assert name + ".in" in names, (section, name)
