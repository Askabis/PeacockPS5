# Peacock And PeacockPatcher Notes

These notes come from a shallow review of the public `thepeacockproject/Peacock` repository at commit `6689c085295a2b2aa1c2b42e4bd6352eaec87188`.

## Server-Side Peacock

Peacock is a Node.js/TypeScript server. The public package metadata identifies it as `@peacockproject/monorepo` version `8.9.1`, with Express 5 as the HTTP framework. The server entry point in `components/index.ts` binds to `process.env.HOST || "0.0.0.0"` and `process.env.PORT || 80`.

Relevant HTTP route families include:

- `/config/:audience/:serverVersion`
- `/authentication/api/configuration/Init`
- `/authentication/api/userchannel/...`
- `/profiles/page/...`

Those routes are protocol-facing and platform-neutral at the HTTP level. The PS5 prototype can safely start by testing basic TCP and HTTP reachability to one of these routes.

## PeacockPatcher

`PeacockPatcher` is not platform-neutral protocol code. The Windows patcher is C# and uses process discovery plus Windows APIs such as `OpenProcess`, `ReadProcessMemory`, `WriteProcessMemory`, and `VirtualProtectEx` to modify a running HITMAN process. The macOS patcher has a separate native implementation with process scanning and patch definitions.

That patching layer is desktop/game-process specific. It should not be ported into this PS5 homebrew project as-is, and this repository deliberately does not implement memory patching, executable patching, exploit delivery, or unauthorized console modification.

## Current Prototype Boundary

PeacockPS5 currently validates only:

- configuration loading from `/download0/peacock.conf` or `/app0/assets/peacock.conf`;
- IPv4 LAN socket creation;
- TCP connect to the configured Peacock host and port;
- one HTTP GET to the configured path;
- visible status and local logging.

Future work should keep this boundary explicit: protocol observation and user-owned interoperability only.
