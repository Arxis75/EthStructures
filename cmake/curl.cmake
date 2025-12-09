#set(CURL_PREFIX ${CMAKE_BINARY_DIR}-external/ethash-1.0.1)
set(CURL_PREFIX ${CMAKE_SOURCE_DIR}/build-external/curl-8.16.0)
set(CURL_INSTALL ${CURL_PREFIX})   #still unable to separate src/install
set(CURL_LIB_DIR ${CURL_INSTALL}/lib)
set(CURL_LIBCURL_LIBRARY ${CURL_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}curl-d${CMAKE_STATIC_LIBRARY_SUFFIX})
set(CURL_INCLUDE_DIR ${CURL_PREFIX}/include)

#The following test avoids the lib rebuilding when it is already available at the project level (parent host building it for example).
if(NOT TARGET libcurl)
    #The following test avoids to rebuild the lib when erasing the project buid folder.
    find_library(libcurl NAMES libcurl-d.a PATHS "${CURL_LIB_DIR}" NO_DEFAULT_PATH)
    if(NOT libcurl)
        message(STATUS "Third-party: creating target 'curl'")

        # Minimal HTTP-only
        set(HTTP_ONLY ON CACHE BOOL "" FORCE)

        # Protocols
        set(CURL_DISABLE_CRYPTO_AUTH    ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_DICT           ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_FILE           ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_FTP            ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_GOPHER         ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_HTTP_AUTH      ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_IMAP           ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_LDAP           ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_LDAPS          ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_LIBCURL_OPTION ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_MIME           ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_NETRC          ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_PARSEDATE      ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_POP3           ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_PROXY          ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_REDIR          ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_RTSP           ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_SMB            ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_SMTP           ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_SOCKETPAIR     ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_SOCKS          ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_TELOPT         ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_TFTP           ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_WEBSOCKETS     ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_MQTT           ON CACHE BOOL "" FORCE)

        # Build options
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
        set(BUILD_CURL_EXE    OFF CACHE BOOL "" FORCE)
        set(BUILD_TESTING     OFF CACHE BOOL "" FORCE)

        # Features / size reduction
        set(CURL_DISABLE_VERBOSE_STRINGS ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_PROGRESS_METER ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_COOKIES ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_ALTSVC ON CACHE BOOL "" FORCE)
        set(CURL_DISABLE_HSTS   ON CACHE BOOL "" FORCE)

        # Compression
        set(CURL_ZLIB   OFF CACHE BOOL "" FORCE)
        set(CURL_BROTLI OFF CACHE BOOL "" FORCE)
        set(CURL_ZSTD   OFF CACHE BOOL "" FORCE)

        # TLS / Crypto
        set(CURL_USE_OPENSSL        OFF CACHE BOOL "" FORCE)
        set(CURL_USE_MBEDTLS        OFF CACHE BOOL "" FORCE)
        set(CURL_USE_WOLFSSL        OFF CACHE BOOL "" FORCE)
        set(CURL_USE_SCHANNEL       OFF CACHE BOOL "" FORCE)
        set(CURL_USE_SECURETRANSPORT OFF CACHE BOOL "" FORCE)
        set(CURL_USE_GNUTLS         OFF CACHE BOOL "" FORCE)
        set(CURL_USE_BEARSSL        OFF CACHE BOOL "" FORCE)

        # Optional libraries / extras
        set(USE_LIBIDN2        OFF CACHE BOOL "" FORCE)
        set(CURL_USE_LIBPSL    OFF CACHE BOOL "" FORCE)
        set(CURL_USE_LIBSSH2   OFF CACHE BOOL "" FORCE)
        set(CURL_USE_LIBSSH    OFF CACHE BOOL "" FORCE)
        set(CURL_USE_GSSAPI    OFF CACHE BOOL "" FORCE)
        set(CURL_USE_LIBUV     OFF CACHE BOOL "" FORCE)
        set(USE_LIBRTMP        OFF CACHE BOOL "" FORCE)
        set(ENABLE_UNIX_SOCKETS OFF CACHE BOOL "" FORCE)

        # CA bundle (if no TLS)
        set(CURL_CA_BUNDLE "none" CACHE STRING "CA bundle path or none" FORCE)

        include(FetchContent)
        FetchContent_Declare(
                curl
                URL         https://github.com/curl/curl/releases/download/curl-8_16_0/curl-8.16.0.tar.gz
                URL_HASH    SHA256=a21e20476e39eca5a4fc5cfb00acf84bbc1f5d8443ec3853ad14c26b3c85b970
                UPDATE_DISCONNECTED true  # need this to avoid constant rebuild
                BINARY_DIR "${CURL_INSTALL}"
                SOURCE_DIR "${CURL_INSTALL}"
                SUBBUILD_DIR "${CURL_INSTALL}-subbuild"
            )
            FetchContent_MakeAvailable(curl)
    else()
        add_library(libcurl INTERFACE IMPORTED GLOBAL)
        add_dependencies(libcurl curl)
        target_include_directories(libcurl INTERFACE ${CURL_INCLUDE_DIR})
        target_link_libraries(libcurl INTERFACE ${CURL_LIBCURL_LIBRARY})
    endif()
endif()