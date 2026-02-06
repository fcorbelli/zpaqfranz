<!-- markdownlint-disable MD041 -->

A slim, multi-architecture Docker image that compiles `zpaqfranz`
from source on Ubuntu (glibc) and publishes it to GHCR.

Quick start

```bash
docker pull ghcr.io/fcorbelli/zpaqfranz:latest

docker run --rm ghcr.io/fcorbelli/zpaqfranz:latest version

docker run --rm \
  -v "$PWD:/data" \
  ghcr.io/fcorbelli/zpaqfranz:latest \
  a backup.zpaq .
```

The container entrypoint is `zpaqfranz`, so any additional arguments
are passed straight through to the tool.
The default working directory is `/data`, and the image runs as a
non-root user.

If you bind-mount a host directory and get permission errors, run the
container as your user:

```bash
docker run --rm \
  -u "$(id -u):$(id -g)" \
  -v "$PWD:/data" \
  ghcr.io/fcorbelli/zpaqfranz:latest \
  a backup.zpaq .
```

Local build

The Docker build context is `docker/` (kept small on purpose), so you
need to stage `zpaqfranz.cpp` into it:

```bash
cp zpaqfranz.cpp docker/zpaqfranz.cpp
docker build -t zpaqfranz:local -f docker/Dockerfile docker
```

Tags and architectures

- `ghcr.io/fcorbelli/zpaqfranz:latest` for `linux/amd64` and
  `linux/arm64` (most recent build).
- `ghcr.io/fcorbelli/zpaqfranz:<version>` for `linux/amd64` and
  `linux/arm64` (specific zpaqfranz tag, for example `64.4`).

How it stays up to date

- Publishing a new GitHub Release triggers the container build
  workflow, which builds multi-arch images with Docker Buildx and
  pushes them to GHCR.
