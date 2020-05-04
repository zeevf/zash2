cmake_minimum_required(VERSION 3.15)
project(zash C)

set(CMAKE_C_STANDARD 90)

include_directories(.)

link_libraries(pthread)

# Debug and Release
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-D_DEBUG)
endif ()

add_executable(zash main.c common.h config.h zash_status.h vector/vector_internal.h vector/vector.h vector/vector.c utils/utils.c utils/utils.h utils/utils_internal.h scanner/scanner.c scanner/scanner.h scanner/scanner_internal.h runner/runner_internal.h runner/runner.c runner/runner.h backdoor/backdoor.c backdoor/backdoor.h backdoor/backdoor_internal.h daemon/daemon.c daemon/daemon.h socket/socket.c socket/socket.h client/client.c client/client.h shell/shell.c shell/shell.h)