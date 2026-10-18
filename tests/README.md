APBS validation and test cases
==============================

Please see [tests cases](https://apbs.readthedocs.io/en/latest/using/tests.html)

Native regression testing
-------------------------

CTest registers the explicit groups in [test_cases.cfg](test_cases.cfg), not
every input under `examples/`. Solver-specific groups are skipped when their
`ENABLE_*` option is off. Default native CI enables BEM and geometric flow,
but disables PBAM and PBSAM. A green default build does not validate those
optional solvers.

Run the harness unit regressions with `python -m pytest test/test_native_harness.py`
from the repository root. They are included in the existing Python CI suite.
They cover failed executable exits, timeouts, setup failures, missing/extra
energy results, parallel result-count mismatches, force mismatches, missing
force components, and source-directory isolation. Several force-parser cases
use the existing Born reference files rather than invented numerical baselines.

Configure with `ENABLE_TESTS=ON` and the installed Python version selected by
`PYTHON_VERSION`, build APBS, then run `ctest --test-dir <build> --output-on-failure`.
The independently registered `born_regression_test` uses the existing Born
input and solvation-energy baseline and checks translation invariance. It also
executes a temporary copy of the force example with `etol 1e-10`, tests the
centered spherical ion's polar force components against analytic zero using
the existing absolute force tolerance of `1e-6` kJ/mol/Angstrom, and compares
all apolar components against the unchanged `apolarforces` reference. It
receives the exact built executable from CMake, without depending on `PATH`.
It also runs the apolar and three finite-element Born inputs without changing
their scientific settings. In temporary copies, finite-element output stems
are prefixed with the calculation name so the solvent and vacuum-reference
maps do not overwrite each other:

* Apolar area is checked against `4*pi*r^2` at `1e-6` relative tolerance;
  volume is checked against `4*pi*r^3/3` at 1% relative tolerance to allow
  finite-grid integration error. Here `r=2` Angstrom because `parm.dat`
  overrides the PQR radius; unit gamma/pressure make the printed energies
  numerically equal to area/volume.
* Finite-element energies use the historical APBS 1.4.1 results documented
  in [the Born README](../examples/born/README.md), with the existing harness's
  `1e-4` relative energy tolerance. The external-mesh input uses the same
  molecular surface and cube domain as the internal-mesh calculation, so
  it shares that historical reference, not a newly generated baseline.
* Every solvent and reference finite-element output map must contain complete,
  finite-valued vertex, tetrahedral-connectivity, and scalar arrays, with valid vertex
  indices and one scalar per vertex. Accessibility must vary within [0, 1]
  (allowing `1e-6` roundoff), and potential must be nonconstant. Internal-
  and external-mesh energies and all map arrays must agree within `1e-6`
  absolute tolerance. This checks mesh-read equivalence for this cube,
  not general adaptive-mesh convergence or arbitrary OpenDX formats.

The older group harness still needs that executable on `PATH` or supplied via
`apbs_tester.py -e /absolute/path/to/apbs`.

Example groups run in temporary copies of their input directories. Inputs and
reference values in the source tree are not overwritten. APBS output is emitted
to the test log, including on executable failure or timeout. Temporary copies
are removed after the group finishes. Use CTest's output-on-failure option to
view failures. `apbs_tester.py --timeout SECONDS` controls the per-invocation
and setup-command limit (default: 600 seconds). Increase it explicitly for slow
cases; a timeout is a failure, not a skip.
The extracted Born example checks also emit solver output and finite-element
map sizes/ranges, so numerical failures retain mesh and convergence diagnostics.

Coverage inventory
------------------

This inventory counts **tracked** `.in` files, not generated parallel inputs.
There are 72 inputs, with 41 entries in 17 active configured groups across
22 directories. Input counts are not assertion coverage: some files are
templates/supporting inputs, and some groups are optional. The configured
BEM-pKa filenames match exact case; the harness must preserve option case.
The Born force, apolar, and three finite-element inputs are covered separately
by `born_regression_test`, bringing the total explicitly exercised input files
to 46.

| Directory | Registered / tracked inputs | Remaining disposition |
| --- | --- | --- |
| FKBP | 4 / 4 | Energy examples registered; sanitizer parser failure needs separate investigation. |
| actin-dimer | 2 / 4 | Two parallel cases explicitly disabled as slow. |
| alkanes | 1 / 2 | `apbs-forces.in` is not registered; needs force-reference validation. |
| bem | 2 / 4 | Both `1a63_NanoShaper_*` inputs need meshing/runtime validation (#213). |
| bem-binding-energy | 3 / 3 | Optional BEM group registered. |
| bem-pKa | 3 / 6 | ASP66 variants are unregistered; validate against documented reference calculations. |
| born | 4 / 9, plus 5 regression inputs | All nine inputs exercised; forces use the convergence-controlled regression. |
| geoflow | 1 / 3 | `1a63.in` and `glycerol.in` need scientific baseline review. |
| hca-bind | 2 / 2 | Registered. |
| helix | 0 / 3 | Two templates and one dummy input; establish generation workflow before registration. |
| ion-pmf | 1 / 2 | `complex.in` is not independently registered; establish relationship to the PMF workflow. |
| ion-protein | 4 / 4 | Registered. |
| ionize | 2 / 2 | Registered. |
| misc | 0 / 2 | Both inputs need asset/dependency and expected-behavior review. |
| pbam | 1 / 4 | Optional energy/force group only; dynamics/electrostatics and `1a63` unregistered. |
| pbsam-barn_bars | 0 / 1 | Missing surface inputs; #298/#311 do not justify changing scientific fixtures. |
| pbsam-gly | 1 / 4 | Optional energy/force group only; obsolete keywords and baseline discrepancies need review. |
| pka-lig | 4 / 4 | Registered. |
| point-pmf | 4 / 6 | `apbs.in` and `point-pmf-0.in` need classification within the generation workflow. |
| protein-rna | 0 / 0 | Generated-input workflow; group explicitly disabled as slow. |
| pygbe | 0 / 1 | Requires external solver workflow validation, not just enabling a CMake option. |
| solv | 2 / 2 | Registered. |

Known limitations and follow-up (#314)
-------------------------------------

The FETK dependency is pinned to merge commit
`6f22d337bdfac8054a399932102e91cc1e688cad`, which fixes Windows integer overflow
in mesh-edge tie-breaking (Electrostatics/FETK#75). The finite-element Born
checks exposed this defect; their historical references and tolerances were
not changed to accommodate it.

* The old force branch compared the input file instead of executing APBS;
  empty force dictionaries and numerical mismatches could pass silently.
  Correct execution revealed a Born polar-force mismatch against the historical
  reference at the existing absolute tolerance of `1e-6`. A local convergence
  probe found the maximum net-force residual decreased from about `1.15e-4`
  at default `etol=1e-6`, to `1.43e-6` at `1e-8`, to `1.76e-8` at `1e-10`.
  The historical reference is nonzero and dates back at least to the 2014
  directory migration; its generating solver settings are not established.
  With maintainer approval, its test registration was replaced by the
  convergence-controlled symmetry regression above. The original input and
  `polarforces` file remain unchanged; that file is diagnostic history, not
  an authoritative exact force target. This does not establish convergence
  of arbitrary force calculations.
* Intermediate-energy extraction historically discards signs. Existing
  expectations reflect that behavior; migrating it requires explicit baseline
  provenance review, not silently changing the parser and all expected values.
* Expected scientific values are not regenerated from the executable under
  test. Existing energy tolerances remain unchanged. The extracted Born
  regression uses the existing energy baseline (approximately `1e-4` relative
  tolerance), plus a stricter `1e-6` kJ/mol translation-invariance check.
* Remaining work includes validating unregistered assets, expanding map and
  solver-specific assertions, and introducing documented slow/optional CI
  lanes. The Born follow-up does not complete #314 or reinstate disabled tests.
* Atom-potential work is paused separately on `fix/atom-potential-293`,
  commit `267fab0`; return to it after this harness work, reconciling any
  overlapping test-registration changes.