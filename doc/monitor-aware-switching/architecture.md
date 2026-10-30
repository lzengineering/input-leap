# Architecture assessment

## Current behavior

- `MSWindowsScreen::updateScreenShape()` in `src/lib/platform/MSWindowsScreen.cpp` reads only the Windows virtual-screen bounding rectangle via `SM_*VIRTUALSCREEN` metrics.
- `WM_DISPLAYCHANGE` reaches `MSWindowsScreen::onDisplayChange()`, but the current shape-change event depends on the bounding rectangle changing.
- `Server::onMouseMovePrimary()` in `src/lib/server/Server.cpp` switches only at the four outer edges of the machine rectangle.
- `MSWindowsHook::mouseHookHandler()` in `src/lib/platform/MSWindowsHook.cpp` consumes only existing outer jump-zone events. A server-only check can therefore allow a visible cursor leak across an internal local seam.
- `Config::Cell` models one named machine. Partial edge intervals exist, but cannot insert B between two monitor surfaces of the same primary cell.
- `ClientInfo` and the `DINF` protocol message carry one bounding rectangle, not a monitor list.

## Compared approaches

### A. Windows internal monitor bridge — selected

Enumerate active monitor rectangles on the Windows server, detect the two-monitor topology, register inward-facing seam zones in the low-level hook, and route those events through the existing server switch machinery. Returning from B chooses an explicit coordinate just inside the appropriate A monitor.

This keeps the client and protocol unchanged. It is robust only if hook interception and server geometry are changed together.

### B. Per-monitor graph

This is the general solution for issue #2361, but requires monitor enumeration on every platform, protocol negotiation, machine-plus-monitor graph nodes, config/GUI changes, and compatibility behavior for 1.6 peers. It is disproportionate for the initial A-B-A use case.

### C. External Windows helper

A separate low-level hook would compete with Input Leap's own hook, cursor centering, key-state transfer, and suppression logic. There is no stable public API for an atomic screen switch with an explicit return monitor. Reject as production architecture.

## Main risks

- Seam events skipped by high-speed motion.
- DPI-coordinate mismatches between enumeration, hook, `GetCursorPos`, and `SetCursorPos`.
- Topology changes whose bounding rectangle is unchanged.
- Immediate bounce when the return cursor lands on the seam.
- Ambiguous monitor identity after HDMI/EDID changes.
- Existing 16-bit wire-coordinate limits for very wide desktops; phase 1 must not worsen them.
