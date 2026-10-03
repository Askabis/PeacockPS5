# Install PeacockPS5

[Home](../README.md) | [Français](INSTALLATION.fr.md) | **English**

PeacockPS5 is a development homebrew application, not an exploitation payload or a retail PKG. This guide assumes that your own PS5 already has a compatible homebrew environment, ShadowMountPlus (or an equivalent loader that accepts folder applications), and a running FTP server.

The graphical application is validated on firmware 6.02, 12.70, and 13.60 with ShadowMountPlus. Peacock connectivity on 13.60 was verified on real hardware on October 3, 2026.

## 1. Prepare Peacock on the PC

1. Start Peacock on a PC connected to the same LAN as the PS5.
2. Bind it to the PC's LAN interface, not only to `127.0.0.1`. Peacock uses port `80` by default and accepts `HOST=0.0.0.0`.
3. Allow that port through the PC's private-network firewall.
4. Note the PC's IPv4 address, for example `192.168.1.10`.

From another device on the LAN, open `http://PC_ADDRESS/authentication/api/configuration/Init`. Any HTTP response, including an application error, confirms that the service is reachable.

## 2. Download and configure the application

1. Open the [latest release](https://github.com/Askabis/PeacockPS5/releases/latest).
2. Download `PPSA99999.zip`, then extract it. You should get a `PPSA99999` folder containing at least `eboot.bin`, `sce_sys/`, and `assets/`.
3. Open `PPSA99999/assets/peacock.conf` and replace `host` with the PC's IPv4 address:

```ini
host=192.168.1.10
port=80
path=/authentication/api/configuration/Init
timeout_ms=3000
```

## 3. Copy it to the PS5

1. Start the FTP server in your existing homebrew environment and note the PS5's IP address.
2. In an FTP client, connect to that address on port `2121` (a common default), using `anonymous` if your server does not require different credentials.
3. Upload the complete `PPSA99999` folder into `/data/homebrew/`.
4. Confirm that the final path is exactly `/data/homebrew/PPSA99999/eboot.bin`. Do not upload only `eboot.bin`, and do not upload the ZIP itself.
5. Ask ShadowMountPlus or your loader to rescan applications, then wait for its ready/installed confirmation.

## 4. Launch and verify

Launch **Peacock PS5 Probe** from the Games section. The screen should show the target, probe result, and first HTTP line. `HTTP/1.1 200`, `3xx`, or `4xx` confirms Peacock is reachable; a timeout usually means a wrong IP, blocked port, or Peacock bound only to localhost.

The log is written to `/download0/peacock_probe.log`. To change the target later without uploading the app again, place a `peacock.conf` file in `/download0/` with the same four keys, then restart PeacockPS5.

This test confirms PS5-to-Peacock connectivity only. It does not yet configure HITMAN to use Peacock.
