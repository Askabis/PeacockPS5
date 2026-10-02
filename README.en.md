# PeacockPS5

[Home](README.md) | [Français](README.fr.md) | **English**

PeacockPS5 is an early native PS5 homebrew prototype. It checks whether a user-owned console can reach a local [Peacock](https://github.com/thepeacockproject/Peacock) instance over the LAN.

The application loads its configuration, opens a bounded TCP connection, sends an HTTP request, displays the first response line, and writes a local log to `/download0/peacock_probe.log`.

## Current status

- Configurable IPv4 TCP/HTTP probe.
- Peacock settings can be changed without rebuilding.
- On-screen result and local log.
- No HITMAN modification or game redirection at this stage.

## Install

Follow the short guide: **[PS5 installation](docs/INSTALLATION.en.md)**.

A prebuilt version is available from [GitHub Releases](https://github.com/Askabis/PeacockPS5/releases/latest). The project produces a development homebrew application, not a retail PKG file.

## Peacock configuration

Default configuration in `assets/peacock.conf`:

```ini
host=192.168.1.10
port=80
path=/authentication/api/configuration/Init
timeout_ms=3000
```

On the console, `/download0/peacock.conf` overrides these values without rebuilding. `host` currently needs to be a LAN IPv4 address.

## Build

On Linux or WSL:

```bash
make doctor
make
```

The complete output is written to `dist/PPSA99999/`. See [GETTING_STARTED.md](docs/GETTING_STARTED.md) for the build environment and [DEPLOYMENT.md](docs/DEPLOYMENT.md) for the development workflow.

## Scope

This repository is for lawful interoperability research with hardware and games controlled by the user. It contains no PS5 exploit chain, privilege escalation, retail package signing, piracy tooling, or HITMAN process-memory patcher.

The [Peacock technical notes](docs/PEACOCK_PROTOCOL_NOTES.md) distinguish Peacock's HTTP protocol from the Windows-specific PeacockPatcher components.

Based on [ps5-native-app-boilerplate](https://github.com/blackbearreloaded/ps5-native-app-boilerplate). GPL-3.0-or-later; see [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
