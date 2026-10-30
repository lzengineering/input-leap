# Requirements

## Functional

1. With three displays reported by Windows on PC A, Input Leap behaves exactly like upstream.
2. With only the two outer A displays reported, crossing their inward-facing edges switches to PC B.
3. Leaving B on its left edge returns to A-left; leaving B on its right edge returns to A-right.
4. Switching is automatic after a display topology change and requires no hotkey.
5. Fast pointer motion must not bypass the bridge.
6. The first implementation is Windows-server-specific and configurable; PC B remains protocol-compatible with upstream 1.6.

## Quality and safety

- No visible cursor leak, bounce, or repeated switch loop at the internal seam.
- No regression at ordinary outer screen edges.
- Handle negative virtual coordinates, unequal resolutions, vertical offsets, rotation, and mixed DPI.
- Use separate test builds. Back up the installed version and configuration before hardware tests.
- Real cursor/keyboard tests require an agreed idle test window and a locally available rollback path.
- Do not log credentials, TLS private keys, clipboard contents, or typed input.

## Explicit non-goals for phase 1

- A general cross-platform per-monitor graph model.
- New protocol messages or a protocol version bump.
- Automatic HDMI-switch control.
- A new GUI before the proof of concept is stable.
