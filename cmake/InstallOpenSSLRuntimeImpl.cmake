# Install-time scan invoked via install(SCRIPT ...) from
# input_leap_install_openssl_runtime() in WindowsRuntimeDeps.cmake. Expects
# IL_RUNTIME_TARGET_FILE, IL_RUNTIME_TARGET_NAME, and IL_RUNTIME_SEARCH_DIRS
# to already be set by the preceding install(CODE ...) in the same directory.

if(POLICY CMP0207)
    cmake_policy(SET CMP0207 NEW)
endif()

set(_il_openssl_pattern "^(lib)?(ssl|crypto)-[0-9].*\\.dll$")

file(GET_RUNTIME_DEPENDENCIES
    EXECUTABLES "${IL_RUNTIME_TARGET_FILE}"
    RESOLVED_DEPENDENCIES_VAR _il_resolved_deps
    UNRESOLVED_DEPENDENCIES_VAR _il_unresolved_deps
    DIRECTORIES ${IL_RUNTIME_SEARCH_DIRS}
)

foreach(_il_dep IN LISTS _il_resolved_deps)
    get_filename_component(_il_dep_name "${_il_dep}" NAME)
    if (_il_dep_name MATCHES "${_il_openssl_pattern}")
        message(STATUS "[${IL_RUNTIME_TARGET_NAME}] packaging OpenSSL runtime dependency: ${_il_dep_name}")
        file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}" TYPE FILE FILES "${_il_dep}")
    endif()
endforeach()

foreach(_il_dep_name IN LISTS _il_unresolved_deps)
    get_filename_component(_il_dep_name "${_il_dep_name}" NAME)
    if (_il_dep_name MATCHES "${_il_openssl_pattern}")
        message(FATAL_ERROR
            "[${IL_RUNTIME_TARGET_NAME}] required OpenSSL runtime DLL '${_il_dep_name}' could not be "
            "located for packaging (searched: ${IL_RUNTIME_SEARCH_DIRS}). Refusing to produce "
            "an incomplete installer -- point OPENSSL_SSL_LIBRARY/OPENSSL_CRYPTO_LIBRARY "
            "at a tree that also contains the matching DLLs, or build/link OpenSSL fully "
            "statically.")
    endif()
endforeach()
