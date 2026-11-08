# Resolves the actual runtime DLL dependencies of a Windows executable at
# install time and copies any OpenSSL shared libraries it needs.
#
# OPENSSL_USE_STATIC_LIBS only controls which .lib files find_package(OpenSSL)
# picks; several common prebuilt Windows OpenSSL distributions ship a "static"
# import library that still links against libssl-3-x64.dll / libcrypto-3-x64.dll
# at runtime. Without this, those DLLs are silently missing from the staged
# install tree and the packaged executables fail to start on a target machine
# that doesn't have OpenSSL installed separately.
#
# The actual install-time scan lives in InstallOpenSSLRuntimeImpl.cmake (run
# via install(SCRIPT ...)); it fails the install step (fail-closed) if a
# resolved OpenSSL DLL dependency cannot be located anywhere near the
# discovered OpenSSL libraries, rather than shipping an incomplete installer.
# A genuinely static OpenSSL build (no libssl/libcrypto DLL in the dependency
# list) is left untouched.
set(_il_impl_script "${CMAKE_CURRENT_LIST_DIR}/InstallOpenSSLRuntimeImpl.cmake")

function(input_leap_install_openssl_runtime target)
    if (NOT WIN32)
        return()
    endif()

    get_filename_component(_il_openssl_ssl_dir "${OPENSSL_SSL_LIBRARY}" DIRECTORY)
    get_filename_component(_il_openssl_crypto_dir "${OPENSSL_CRYPTO_LIBRARY}" DIRECTORY)
    get_filename_component(_il_openssl_root "${_il_openssl_ssl_dir}" DIRECTORY)

    set(_il_search_dirs
        "${_il_openssl_ssl_dir}"
        "${_il_openssl_crypto_dir}"
        "${_il_openssl_root}/bin"
        "${_il_openssl_root}"
    )
    list(REMOVE_DUPLICATES _il_search_dirs)

    install(CODE "set(IL_RUNTIME_TARGET_FILE \"$<TARGET_FILE:${target}>\")
set(IL_RUNTIME_TARGET_NAME \"${target}\")
set(IL_RUNTIME_SEARCH_DIRS \"${_il_search_dirs}\")"
        COMPONENT Runtime)
    install(SCRIPT "${_il_impl_script}" COMPONENT Runtime)
endfunction()
