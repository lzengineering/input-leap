# VerifyWindowsPackage.cmake -- offline packaging gate for InputLeap on Windows.
#
# Verifies a staged install tree (the CMAKE_INSTALL_PREFIX output used as the
# installer payload) for self-contained offline use: on a bare Windows 11
# target machine with no dev tools, no Qt, and no separately installed
# OpenSSL, the executables still start.
#
# Fail-closed: every required component must exist inside the stage itself. A
# DLL resolving only through dev-machine locations does not count. A fully
# static OpenSSL build is supported with -DILOPENSSL_STATIC=1.
#
# Usage (from the repo root):
#   cmake -DSTAGE_DIR=<stage> [-DILOPENSSL_STATIC=1] [-DIL_SKIP_GUI=1]
#       [-DIL_DEEP_SCAN=1] -P cmake/VerifyWindowsPackage.cmake
cmake_minimum_required(VERSION 3.16)

if(POLICY CMP0207)
    cmake_policy(SET CMP0207 NEW)
endif()


if(NOT STAGE_DIR)
    message(FATAL_ERROR "VerifyWindowsPackage: missing -DSTAGE_DIR=... variable")
endif()
get_filename_component(_vw_stage "${STAGE_DIR}" ABSOLUTE)
if(NOT IS_DIRECTORY "${_vw_stage}")
    message(FATAL_ERROR "VerifyWindowsPackage: stage directory not found: ${_vw_stage}")
endif()

set(_vw_missing "")

set(_vw_required_exes input-leaps.exe input-leapc.exe input-leapd.exe)
if(NOT IL_SKIP_GUI)
    list(APPEND _vw_required_exes input-leap.exe)
endif()

set(_vw_required_dlls
    Qt6Core.dll
    Qt6Gui.dll
    Qt6Network.dll
    Qt6Widgets.dll
    Qt6Svg.dll
    msvcp140.dll
    msvcp140_1.dll
    msvcp140_2.dll
    msvcp140_atomic_wait.dll
    msvcp140_codecvt_ids.dll
    vcruntime140.dll
    vcruntime140_1.dll
    concrt140.dll
    ucrtbase.dll
)
if(NOT ILOPENSSL_STATIC)
    list(APPEND _vw_required_dlls libcrypto-3-x64.dll libssl-3-x64.dll)
endif()

set(_vw_required_plugins
    platforms/qwindows.dll
    iconengines/qsvgicon.dll
)

set(_vw_soft_plugins
    imageformats/qgif.dll
    imageformats/qico.dll
    imageformats/qjpeg.dll
    imageformats/qsvg.dll
    styles/qwindowsvistastyle.dll
    tls/qcertonlybackend.dll
    tls/qopensslbackend.dll
    tls/qschannelbackend.dll
    networkinformation/qnetworklistmanager.dll
    opengl32sw.dll
)

foreach(_vw_name IN LISTS _vw_required_exes)
    if(NOT EXISTS "${_vw_stage}/${_vw_name}")
        list(APPEND _vw_missing "${_vw_name} (executable)")
    endif()
endforeach()
foreach(_vw_name IN LISTS _vw_required_dlls)
    if(NOT EXISTS "${_vw_stage}/${_vw_name}")
        list(APPEND _vw_missing "${_vw_name} (runtime DLL)")
    endif()
endforeach()
foreach(_vw_name IN LISTS _vw_required_plugins)
    if(NOT EXISTS "${_vw_stage}/${_vw_name}")
        list(APPEND _vw_missing "${_vw_name} (Qt plugin)")
    endif()
endforeach()

if(_vw_missing)
    set(_vw_report "")
    foreach(_vw_item IN LISTS _vw_missing)
        set(_vw_report "${_vw_report}  ${_vw_item}\n")
    endforeach()
    message(FATAL_ERROR "VerifyWindowsPackage: Fail - stage incomplete. Missing from ${_vw_stage}:\n${_vw_report}Do NOT compile or ship an installer from an incomplete stage.")
endif()

message(STATUS "VerifyWindowsPackage: OK - all required executables, runtime DLLs and Qt plugins are present inside ${_vw_stage}")

set(_vw_soft_missing "")
foreach(_vw_name IN LISTS _vw_soft_plugins)
    if(NOT EXISTS "${_vw_stage}/${_vw_name}")
        list(APPEND _vw_soft_missing ${_vw_name})
    endif()
endforeach()
if(_vw_soft_missing)
    message(STATUS "VerifyWindowsPackage: note - optional (graceful degrade) Qt components absent: ${_vw_soft_missing}")
endif()

if(IL_DEEP_SCAN)
    set(_vw_scan_exes)
    foreach(_vw_exe IN LISTS _vw_required_exes)
        list(APPEND _vw_scan_exes "${_vw_stage}/${_vw_exe}")
    endforeach()

    # Pass 1: resolve the full tree (stage first, then the Windows system
    # directory) to learn which dependencies are OS-provided. Sub-DLLs of
    # OS-provided DLLs are provided by the Windows loader on every target
    # machine and must not be demanded from the package.
    file(GET_RUNTIME_DEPENDENCIES
        EXECUTABLES ${_vw_scan_exes}
        RESOLVED_DEPENDENCIES_VAR _vw_resolved
        UNRESOLVED_DEPENDENCIES_VAR _vw_unresolved
        DIRECTORIES "${_vw_stage}" "C:/Windows/System32"
    )

    set(_vw_pre_excl "")
    foreach(_vw_dep IN LISTS _vw_resolved)
        string(TOLOWER "${_vw_dep}" _vw_dep_lower)
        string(FIND "${_vw_dep_lower}" "system32" _vw_in_system)
        if(NOT _vw_in_system EQUAL -1)
            get_filename_component(_vw_sys_name "${_vw_dep}" NAME)
            set(_vw_pat "")
            string(LENGTH "${_vw_sys_name}" _vw_len)
            set(_vw_i 0)
            while(_vw_i LESS _vw_len)
                string(SUBSTRING "${_vw_sys_name}" ${_vw_i} 1 _vw_ch)
                if(_vw_ch STREQUAL ".")
                    string(APPEND _vw_pat "\\.")
                elseif(_vw_ch MATCHES "[A-Za-z]")
                    string(TOUPPER "${_vw_ch}" _vw_cu)
                    string(TOLOWER "${_vw_ch}" _vw_cl)
                    string(APPEND _vw_pat "[${_vw_cu}${_vw_cl}]")
                else()
                    string(APPEND _vw_pat "${_vw_ch}")
                endif()
                math(EXPR _vw_i "${_vw_i} + 1")
            endwhile()
            list(APPEND _vw_pre_excl "${_vw_pat}")
        endif()
    endforeach()
    list(REMOVE_DUPLICATES _vw_pre_excl)

    # Pass 2: resolve only from the stage, with the OS-provided DLL names
    # excluded before scanning, so their OS-side sub-dependencies never get
    # misattributed to the package.
    file(GET_RUNTIME_DEPENDENCIES
        EXECUTABLES ${_vw_scan_exes}
        RESOLVED_DEPENDENCIES_VAR _vw_resolved
        UNRESOLVED_DEPENDENCIES_VAR _vw_unresolved
        PRE_EXCLUDE_REGEXES ${_vw_pre_excl}
        DIRECTORIES "${_vw_stage}"
    )

    set(_vw_problems "")
    string(TOLOWER "${_vw_stage}" _vw_stage_lower)
    foreach(_vw_dep IN LISTS _vw_unresolved)
        string(TOLOWER "${_vw_dep}" _vw_dep_lower)
        if(_vw_dep_lower MATCHES "api-ms-")
            continue()
        endif()
        if(_vw_dep_lower MATCHES "ext-ms-")
            continue()
        endif()
        if(EXISTS "C:/Windows/System32/${_vw_dep}")
            continue()
        endif()
        list(APPEND _vw_problems "unresolved dependency: ${_vw_dep}")
    endforeach()
    foreach(_vw_dep IN LISTS _vw_resolved)
        string(TOLOWER "${_vw_dep}" _vw_dep_lower)
        string(FIND "${_vw_dep_lower}" "system32" _vw_in_system)
        if(_vw_in_system EQUAL -1)
            string(FIND "${_vw_dep_lower}" "${_vw_stage_lower}" _vw_stage_pos)
            if(NOT _vw_stage_pos EQUAL 0)
                list(APPEND _vw_problems "dependency resolved OUTSIDE the stage: ${_vw_dep}")
            endif()
        endif()
    endforeach()

    if(_vw_problems)
        set(_vw_report "")
        foreach(_vw_item IN LISTS _vw_problems)
            set(_vw_report "${_vw_report}  ${_vw_item}\n")
        endforeach()
        message(FATAL_ERROR "VerifyWindowsPackage: Deep scan Fail - problems in ${_vw_stage}:\n${_vw_report}Resolve these into the stage via the CMake install rules before packaging.")
    endif()
    message(STATUS "VerifyWindowsPackage: deep scan OK - every non-system dependency of the executables resolves from inside the stage")
endif()
