# PeacockPS5 background daemon

[Home](../README.md) | [Français](BACKGROUND_DAEMON.fr.md) | **English**

`peacockps5-daemon.elf` is a network payload independent from the graphical application. When started by an already-running `ps5-payload-elfldr`, it runs in its own process while a game is launched.

This first version does not modify HITMAN or read its memory. It only verifies that Peacock remains reachable while the game is running.

## Prepare the configuration

Create `/data/peacockps5/peacock.conf` on the PS5:

```ini
host=192.168.1.102
port=80
path=/authentication/api/configuration/Init
interval_seconds=15
```

The daemon reloads this file before every probe. The minimum `interval_seconds` value is 5 seconds.

## Start it with PS5Upload

1. Confirm that the persistent elfldr is listening on port `9021`.
2. Open **Send payload** in PS5Upload.
3. Select `peacockps5-daemon.elf` and use port `9021`.
4. Wait for **PeacockPS5 daemon started**, followed by **Peacock reachable**.
5. Do not send the daemon a second time during the same session.

The latest state is stored in `/data/peacockps5/status.txt`. History is appended to `/data/peacockps5/daemon.log`.

## Test with HITMAN

1. Keep Peacock running on the PC.
2. Start the daemon and wait for **Peacock reachable**.
3. Launch HITMAN World of Assassination normally.
4. Play for at least one minute.
5. Return to the home screen and confirm that `daemon.log` received new entries while the game was running.

This test validates background process survival and network connectivity only. It does not yet redirect HITMAN to Peacock.
