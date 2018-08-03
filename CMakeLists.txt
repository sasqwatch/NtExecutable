cmake_minimum_required(VERSION 3.12)
project(NtExecutable)

set(CMAKE_CXX_STANDARD 11)

if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif ()

add_executable(NtExecute NtExecute.cpp)
target_link_libraries(NtExecute ntdll.lib)

add_executable(NtExecutable NtExecutable.cpp)
target_compile_options(NtExecutable PUBLIC -fno-rtti -fno-exceptions)
target_link_libraries(NtExecutable ntdll.lib)
set_target_properties(NtExecutable PROPERTIES LINK_FLAGS "-nostdlib -Wl,--subsystem,native,-e,entry_point")
