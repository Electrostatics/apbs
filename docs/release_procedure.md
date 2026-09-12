# APBS release procedure

## 1. Prepare the release

- Update `VERSION`. The value uses `MAJOR_MINOR_PATCH` format.
- Add the release notes to `docs/releases.rst` under a heading matching the new
  version.
- Update copyright or license dates only where they are no longer accurate.
- Open and merge a pull request into `main`.

## 2. Run a package dry run

- Open the **Build APBS** workflow in GitHub Actions.
- Select **Run workflow** on `main`.
- Confirm that all compile, CTest, package, and packaged-example jobs pass for
  Ubuntu, macOS, and Windows.
- Confirm that **Validate Release Inputs** succeeds.
- Download the `release-candidate` artifact and inspect its release notes and
  three ZIP packages.

Manual workflow runs never publish a GitHub release.

## 3. Publish the release

After the package dry run passes, create an annotated tag at the tested `main`
commit. The tag must match `VERSION`; for example, `VERSION` value `3_5_0`
requires tag `v3.5.0`.

```shell
git switch main
git pull --ff-only
git tag -a v3.5.0 -m "APBS 3.5.0"
git push origin v3.5.0
```

The tag starts the same three-platform build, test, package, and packaged-example
jobs. After they pass, the workflow creates the GitHub release, converts the
matching section of `docs/releases.rst` to Markdown, and attaches all three ZIP
packages.

## 4. Verify publication

- Confirm that the GitHub release notes and all three ZIP assets are present.
- Download and inspect each published archive.
- Update the APBS release history on `www.poissonboltzmann.org`.
