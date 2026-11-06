# Linux Resource Job Queue

Run a bounded queue of trusted local commands concurrently with per-process resource limits and wall-time cleanup.

[![Build and verify](https://github.com/netcodermike/linux-resource-job-queue/actions/workflows/verify.yml/badge.svg)](https://github.com/netcodermike/linux-resource-job-queue/actions/workflows/verify.yml)

**Language:** C · **Platform:** Linux · **App type:** Linux concurrent job runner with resource caps

First-release learning implementation, with explicit scope below. This is one independently runnable app, not a placeholder in a combined repository.

## Features

- 1–8 workers and 100 jobs
- Literal tab-separated arguments
- CPU/address-space/file-size limits
- Exit reporting and group timeout termination

## Requirements

Linux, a C11 compiler, make, and Python 3 for real integration tests.

## Quick start

```sh
git clone https://github.com/netcodermike/linux-resource-job-queue.git
cd linux-resource-job-queue
make
./app 2 jobs.tsv
```

Commands run from the repository root. Lines marked for a particular operating system apply only there. Language tools are prerequisites, not installed by the app. Close desktop windows normally; terminal/server apps stop with Ctrl+C unless their usage notes say otherwise.

## Example workflow

Create a job file containing /usr/bin/true and /usr/bin/printf followed by a tab and a literal argument. Run ./app 2 jobs.tsv, then include /usr/bin/false and inspect exit status.

## Architecture, configuration, and data

main.c reads a TSV job file, forks up to the worker count, sets a fresh process group per child, applies RLIMIT_CPU=2s, AS=128MiB, FSIZE=1MiB, CORE=0, NOFILE=32, redirects stdin to /dev/null, and execvp executes arguments directly. A monotonic 10-second wall limit kills the process group. SIGINT/SIGTERM stop launching and clean active groups. Integration tests verify two jobs, literal spaces, and failure propagation.

## Verification

Build with the quick-start command where needed, then run:

```sh
make test
```

Manual acceptance: Create a job file containing /usr/bin/true and /usr/bin/printf followed by a tab and a literal argument. Run ./app 2 jobs.tsv, then include /usr/bin/false and inspect exit status. Also test malformed inputs and an unwritable output destination.

See [validation record](docs/VALIDATION.md) for observed checks and their limits. CI results verify only what the workflow runs; they do not establish native GUI correctness or production readiness.

## Limitations and next steps

Not a security sandbox: commands retain filesystem/network/user access and can deliberately escape process groups. Limits are per process; aggregate descendants are not bounded by cgroups. No shell parsing, environment isolation, robust scheduler persistence, job dependencies, or output separation. Argument count is capped at 32, line size at 8191 bytes, and blank tab fields are not preserved.

## Troubleshooting

Use the exact platform/toolchain above. Preserve originals and use synthetic fixtures first. Errors are displayed in the UI or stderr. A headless machine can run the model tests but cannot verify native GUI/audio behavior. Output paths are chosen explicitly; keep backups of important data.

## Source map

- `main.c`
- `Makefile`
- `tests/integration.py`

## Development and license

Read [contributing](CONTRIBUTING.md) before changing the app, and [security/privacy](SECURITY.md) before supplying private data. Use synthetic fixtures for tests. No telemetry, paid API key, or account is required by the app itself. Dependencies, when present, are stated in Requirements.

[MIT license](LICENSE), copyright 2026 netcodermike.
