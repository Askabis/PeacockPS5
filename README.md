# PeacockPS5

PeacockPS5 is an early native PS5 homebrew prototype for testing whether a user-owned PS5 on a trusted LAN can reach a local [Peacock](https://github.com/thepeacockproject/Peacock) server instance. It is based on the public structure and tooling style of [blackbearreloaded/ps5-native-app-boilerplate](https://github.com/blackbearreloaded/ps5-native-app-boilerplate).

The current app does one thing: it loads `PEACOCK_HOST`-style configuration, opens a bounded TCP connection to the configured Peacock server, sends a simple HTTP GET, displays the first HTTP response line, and appends a local log under `/download0/peacock_probe.log`.

## Scope

This repository is for lawful interoperability research with hardware and games controlled by the user.

It does not include a PS5 exploit chain, privilege-escalation helper, retail package signer, piracy workflow, HITMAN game patcher, firmware offset database, or code for modifying consoles that the user does not own or control.

## Configuration

The packaged default lives at:

```text
assets/peacock.conf
```

Example:

```ini
host=192.168.1.10
port=80
path=/authentication/api/configuration/Init
timeout_ms=3000
```

On hardware, create `/download0/peacock.conf` with the same keys to override the packaged default without rebuilding the app.

The first prototype intentionally expects an IPv4 LAN address, not a hostname. Start Peacock on the LAN host with a reachable bind address and port, then set `host` and `port` accordingly.

## Build

Use the same build flow as the upstream native app boilerplate:

```bash
make doctor
make
```

The complete title directory is written to `dist/<TITLE_ID>/`, with archive output in `dist/<TITLE_ID>.zip`. Stage the whole title folder with your normal, legal homebrew deployment workflow.

## Peacock Notes

The working notes in [docs/PEACOCK_PROTOCOL_NOTES.md](docs/PEACOCK_PROTOCOL_NOTES.md) separate Peacock's server-side HTTP behavior from PeacockPatcher's desktop-specific process-memory patching.

At this stage PeacockPS5 only probes server reachability. It does not attempt to make HITMAN on PS5 connect to Peacock.

## Credits And License

This project inherits the GPL-3.0-or-later native boilerplate foundation and clean-room runtime tooling from `ps5-native-app-boilerplate`. See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) and [LICENSE](LICENSE).
