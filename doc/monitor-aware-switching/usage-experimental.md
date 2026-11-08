# Experimental middle-monitor client (Windows)

Status: accepted backend checkpoint per 2026-11-04 handoff: Windows build and 17/17 focused tests passed; user confirmed four-way transitions and overlap/portrait operation. GUI offset support is implemented; broader service/runtime and release acceptance remain pending. See [current status and TODO](STATUS.md).
This personal fork has no support commitment.

## Use
In Configure Server, double-click the remote client's screen and enable
"Insert this client between the server's two monitors (Windows)". Select only
one remote client, not the server screen. Its normal grid placement is retained
for ordinary routing. Known bug: adjacent grid links can also trigger at outer edges while bridging. Workaround confirmed 2026-11-06: place the client diagonally (no shared edge), retain the bridge option, then Apply/Reload.

Text configuration equivalent, in section: screens, under the remote client:

    remote-client:
        monitorBridge = true
        monitorBridgeYOffset = 0

With exactly two horizontally ordered server monitors and a connected selected
client, the inner edges route through that client: left local <-> remote <->
right local. Returning uses an eight-pixel inset. Cursor height maps pixel-for-pixel using the signed Y offset, not proportionally
to each destination's full height. The offset places the client top edge in server-global Y coordinates: positive moves down, negative up; default is zero. Entry maps clientY = serverGlobalY - offset + clientShapeTop; return is the inverse. Non-overlapping rows block transitions and allow sliding along the seam. Physical gaps and negative desktop coordinates
are allowed. The two local monitors must be horizontally ordered and non-overlapping; each is checked independently for vertical overlap with the remote client.
Three local monitors (or any topology other than the supported two) leave the
ordinary computer-grid routing in effect. The remote client is unchanged;
protocol 1.6 is unchanged, and the new setting is not sent on the wire.
Screen-lock and configured switch restrictions still apply.

## Rollback
Uncheck the option (or remove monitorBridge from the config) and re-apply.
Nothing changes the Windows display configuration, firewall, startup or installed
Input Leap. Development code alone does not alter an existing running instance.

## Remaining acceptance gates
- Integrate the historically passing standalone GUI offset/config/drag-drop regression harness into repository tests.
- TLS pairing and profile consistency, without key copying or blind trust.
- Login screen, admin/restricted-user switching and standby recovery.
- Candidate package dependencies, hashes and clean-host acceptance.
- Switch-delay/two-tap and topology-change race review.

The handoff records 147/148 full-suite passes; the Windows-rejected --daemon
argument remains a known failure, not a passing release gate.
The existing Windows installer changes services and firewall rules; building
an artifact does not authorize installing it or replacing a running backend.