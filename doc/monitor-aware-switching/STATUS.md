# Windows monitor bridge: WIP status and next tasks

Updated 2026-11-08. Experimental personal fork, not a supported release.
Implementation exists; the older architecture and phased plan describe design intent,
not an up-to-date completion checklist. This file is the current portable handoff.

## Implemented
- Windows internal-seam interception for exactly two horizontally ordered local monitors,
  routing through one selected remote client. Three local monitors disable the bridge.
- Signed pixel Y offset, overlap rejection with vertical sliding, portrait/negative
  coordinates, topology generation checks and explicit left/right return mapping.
- GUI checkbox and Y-offset persistence/config output; screen drag/drop modifier fix.
- PlatformScreenLoggingWrapper::leave forwarding and Windows Bonjour query fixes.
- Offline Inno packaging, OpenSSL runtime staging and staged-dependency verification.
  Wire protocol remains 1.6; bridge settings are server-only.

## User observations (historical, not fresh automated tests)
- 2026-11-05: four-way transitions and offset calibration worked; client login/lock
  basically worked, with one cursor jump back to the server (also seen previously).
- 2026-11-06: switching between three local monitors and two plus the middle client
  worked in both directions.
- Server standby currently requires Reload in the GUI. Root cause is unconfirmed.
- The left outer edge can incorrectly enter the bridge client through the ordinary
  GUI neighbor link. Bridge routing does not yet suppress that ordinary link.
- Confirmed workaround (2026-11-06): place the remote client DIAGONALLY relative to
  the server in the GUI grid, with no shared edge; retain bridge checkbox and offset.
  The grid represents computers, not individual physical monitors.

## Next coding-agent tasks, in order
1. Add a regression for the outer-edge ordinary-link conflict, then define/fix routing
   when the bridge is active without breaking non-bridge clients or three-monitor mode.
   Start at Server::handle_motion_primary_event, onMouseMovePrimary and getNeighbor;
   tagged seam events and ordinary edge events currently take different paths.
   Cover both outer edges, both inner directions, portrait, offsets and topology changes.
2. Diagnose standby before changing reconnect logic: collect timestamped server AND
   client logs after server resume, BEFORE Reload. Separate listener, TLS connection,
   watchdog/session and display-topology recovery; test server/client suspend separately.
3. Integrate GUI offset/settings/config/drag-drop regression tests into the repository
   test infrastructure. The earlier standalone Qt harness is not part of this checkout.
4. Expand runtime acceptance: cold boot/pre-login, server lock/unlock, admin/restricted
   sessions, reconnect/duplicate-client recovery, switch delay/two-tap and long runs.
   Verify fingerprint comparison UX with real certificates; never disable TLS or copy keys.
5. Review installer upgrade/uninstall and firewall scope. Current Inno script creates
   an automatic service and an inbound TCP rule, and removes common application data
   on uninstall. Do not run installers against a working setup without an explicit
   test window, protected local identity/config backup and rollback plan.
6. Close licensing/provenance gates before distributing release binaries: exact Qt
   corresponding source/build changes, relink/replace/install information as applicable,
   compiled third-party attribution, Input Leap/Qt/OpenSSL license compatibility,
   software OpenGL provenance and MSVC/UCRT redistribution terms. Collected license
   texts alone are NOT compliance approval. Signing and clean-target acceptance remain open.

## Build and test entry (Windows)
Reference toolchain used for the November candidate: Windows x64, VS 2022 C++ tools,
Windows SDK, CMake, Qt 6.5.3 MSVC x64, OpenSSL 3.5.9, Bonjour SDK and Inno Setup 6.7.4.
These dependencies are NOT vendored. Supply your own paths; browser/Linux coding
sessions cannot validate Windows hooks, desktop/session behavior or installer execution.
See [Windows development](windows-development.md) and upstream CI for alternatives.

From a suitable developer shell, replacing dependency placeholders:

~~~powershell
$env:BONJOUR_SDK_HOME = '<bonjour-sdk>'
cmake -S . -B build-win -G 'Visual Studio 17 2022' -A x64 -DCMAKE_PREFIX_PATH='<qt-prefix>' -DOPENSSL_ROOT_DIR='<openssl-prefix>' -DINPUTLEAP_BUILD_TESTS=ON
cmake --build build-win --config Release --target input-leap input-leapc input-leaps input-leapd unittests
./build-win/bin/Release/unittests.exe --gtest_filter=MonitorBridgeGeometryTests.*:PlatformScreenLoggingWrapperTests.*
ctest --test-dir build-win -C Release --output-on-failure
cmake --install build-win --config Release --prefix '<absolute-stage>'
cmake -DSTAGE_DIR='<absolute-stage>' -DIL_DEEP_SCAN=1 -P cmake/VerifyWindowsPackage.cmake
~~~

Reconfigure after adding test files (the unit target uses source globbing).
Focused tests are GoogleTest filters, NOT individual CTest registrations.
Configure CMAKE_INSTALL_PREFIX to the final staging path when generating the Inno
script; inspect the generated script before compiling it with ISCC. Never package
from a stage that fails the dependency gate. Package verification is not runtime
or license acceptance; the current manifest targets this specific Qt6/OpenSSL3 setup.

Historical checkpoint: 17/17 focused backend tests, 147/148 full-suite tests (Windows
--daemon argument rejection was the known failure); standalone GUI harness 10/10
including init/cleanup. These counts are historical evidence, not fresh build claims.
Keep local build logs, machine profiles, certificates, installer binaries and private
handoff receipts out of commits. Source publication is not a release announcement.

## Publication checkpoint verification (2026-11-08)
Existing Windows Release unittests target built successfully; 17/17 focused geometry
and logging-wrapper tests freshly passed. No fresh full-suite, GUI, installer or
hardware acceptance is claimed for this publication checkpoint.
