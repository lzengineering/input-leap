# Implementation plan

## Phase 0 — baseline

- Reproduce upstream Windows CI builds and tests without code changes.
- Record PC A/B Input Leap versions, configurations, screen names, certificates/fingerprints, and rollback files.
- Add a read-only Windows display diagnostic utility/script.

## Phase 1 — geometry proof

1. Add a pure monitor-rectangle model and unit tests.
2. Enumerate active Windows monitors with `EnumDisplayMonitors`/`GetMonitorInfoW`; evaluate `QueryDisplayConfig` only if stable display-path identity is needed.
3. Compare topology on every relevant display-change event, not only the bounding rectangle.
4. Derive inward seam segments and activation state for two- versus three-monitor layouts.

## Phase 2 — switching proof

1. Extend the Windows hook with internal seam zones and movement-direction/segment checks.
2. Route a seam event to the configured client through existing server switching.
3. Map B-left/B-right return events to explicit coordinates just inside A-left/A-right.
4. Add hysteresis/debounce and structured diagnostic logging.

## Phase 3 — hardening

- Mixed resolution/DPI/orientation tests.
- Fast-motion and repeated HDMI-switch tests.
- Disconnect/reconnect, restart, lock-screen, and user-switch behavior.
- Configuration validation, rollback docs, and minimal GUI only if text configuration is insufficient.

## CI

Keep the existing cross-platform workflow. Add focused geometry tests to all applicable jobs and a separate Windows artifact job for the feature branch. Do not publish installers as releases until hardware validation passes.
