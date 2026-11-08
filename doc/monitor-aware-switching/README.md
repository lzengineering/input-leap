# Monitor-aware cross-PC switching

Status: experimental implementation and test candidate exist; not release-ready. See [current status and TODO](STATUS.md) for verified observations, known bugs and remaining gates.

> [!IMPORTANT]
> This is a personal, experimental fork for one specific hardware setup. It is not a maintained distribution and comes with no user-support or compatibility commitment. Test artifacts must not be treated as production releases.

This work targets a Windows server layout where a switchable physical center monitor alternates between the Input Leap server and a client:

- three-monitor mode: `A-left | A-center | A-right`
- bridged mode: `A-left | B-center | A-right`

In bridged mode, moving between A's two local monitors must traverse B. The feature must detect the Windows display topology automatically, preserve normal behavior in three-monitor mode, and avoid visible cursor leaks across A's internal monitor seam.

## Documents

- [Requirements](requirements.md)
- [Architecture assessment](architecture.md)
- [ADR-0001: Windows internal monitor bridge](adr-0001-windows-internal-monitor-bridge.md)
- [Implementation plan](implementation-plan.md)
- [Test plan](test-plan.md)
- [Windows development setup](windows-development.md)
- [Network and security](network.md)

## Baseline

- Upstream commit: `34a34fb20b93113a6b26052cb5a54f9be2327775`
- Upstream release: v3.0.3
- Protocol: 1.6
- Related issue: https://github.com/input-leap/input-leap/issues/2361
- Related unmerged Phase-1 PR: https://github.com/input-leap/input-leap/pull/2358

No production installation, firewall rule, or Input Leap configuration may be changed by development automation without an explicit test window and rollback plan.
