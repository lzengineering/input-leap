# Windows development setup

## CI reference

The upstream workflow builds Windows Server 2019 + Qt 5.15 and Windows Server 2022 + Qt 6.6 with Visual Studio generators, CMake, the Bonjour SDK-like archive, and uploaded ZIP artifacts.

## Recommended local baseline

- Windows 11 x64
- Git
- Visual Studio 2022 Build Tools with Desktop development with C++ and a Windows SDK
- CMake 3.21 or newer
- Qt 6.6 MSVC x64 for the primary path
- Bonjour SDK-like dependency used by `.github/workflows/builds.yml`
- Inno Setup 6 only when producing an installer

Use a separate checkout/build directory and do not overwrite the production Input Leap installation. First prove a Debug ZIP build, then run it from an isolated directory with a copied test configuration.

## Remote automation baseline

Before provisioning dependencies, inventory each Windows test host for Git, PowerShell, CMake, Ninja, MSBuild/CL, Python, Qt, and packaging tools. Keep host names, addresses, controller details, and capability inventories in private operational documentation rather than this public repository.

Long-running builds should use a local bounded job wrapper with job ID, log/status files, and explicit cancellation; synchronous remote commands are not a durable background-job supervisor.
