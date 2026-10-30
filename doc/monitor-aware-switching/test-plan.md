# Test plan

## Fully automatable

- Pure seam derivation for A-A-A and A-B-A layouts.
- Unequal resolutions, negative origins, vertical offsets, portrait rotation, gaps, overlap rejection, and corner handling.
- Direction and high-speed segment crossing.
- Return-coordinate inset and anti-bounce behavior.
- Activation/deactivation after synthetic topology changes.
- Existing Config and server regression tests.
- Windows Debug/Release build, unit tests, packaging, and CI artifact creation.

## Windows automation with an interactive session

- Capture monitor rectangles, virtual metrics, DPI context, cursor position, active client, transition events, process state, network reachability, and Input Leap logs.
- Start/stop isolated test builds and collect logs through narrow wrappers.
- Screenshot/Computer-Use checks only when the desktop is unlocked and reserved for testing.

## Hardware/manual gate

- Physical HDMI-switch changes.
- Subjective verification of no visible cursor leak/flicker.
- Repeated rapid crossings with real mouse input.
- Lock screen, user switch, EDID disappearance/reappearance, and rollback to the production build.

Each hardware run records build SHA, PC A/B display state, resolutions/scaling, configuration hash, timestamps, result, and rollback outcome.
