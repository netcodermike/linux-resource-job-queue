# Validation

Authoring date: November 6, 2026. Host: Windows. Target: Linux.

## Local result

Target-platform source CI is recorded after publication. This host has Python, Node.js, and .NET; other native toolchains are verified through CI. No unobserved GUI/device behavior is claimed.

## Reproducible checks

The GitHub Actions workflow builds/runs the following on `ubuntu-latest`:

```sh
make
make test
```

## Manual acceptance

Create a job file containing /usr/bin/true and /usr/bin/printf followed by a tab and a literal argument. Run ./app 2 jobs.tsv, then include /usr/bin/false and inspect exit status. Also test malformed inputs and an unwritable output destination.

A workflow file is not evidence of a passing run. Observed CI run links and source commits are recorded after publication. Native GUI, sound, sleep/resume, and platform permissions require direct target-platform acceptance even when model tests pass.
