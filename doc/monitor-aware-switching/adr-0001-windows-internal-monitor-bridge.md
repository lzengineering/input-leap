# ADR-0001: Windows internal monitor bridge

- Status: accepted for proof of concept
- Date: 2026-10-30

## Decision

Implement a narrowly scoped, opt-in Windows-server feature that treats the inward-facing edges of two selected local monitors as a bridge to one existing Input Leap client.

The Windows platform layer will enumerate and compare monitor rectangles after display changes. The low-level mouse hook will consume configured internal seam crossings. The server will use existing client switching and protocol 1.6, with explicit return coordinates on the selected local monitor.

## Consequences

- PC B can run an unmodified Input Leap client.
- No protocol negotiation or cross-platform monitor model is required.
- The patch is not a complete implementation of issue #2361.
- Windows hardware and mixed-DPI tests are mandatory before production use.
- If reliable seam interception cannot be proven, stop and revisit the per-monitor graph rather than shipping a best-effort hook hack.
