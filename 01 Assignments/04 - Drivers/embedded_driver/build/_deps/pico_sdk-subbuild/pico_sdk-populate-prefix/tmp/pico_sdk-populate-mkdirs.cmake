# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/mnt/c/Users/Owner/CLionProjects/CEN-4907C_Computer-Engineering-Design-1/01 Assignments/04 - Drivers/embedded_driver/build/_deps/pico_sdk-src"
  "/mnt/c/Users/Owner/CLionProjects/CEN-4907C_Computer-Engineering-Design-1/01 Assignments/04 - Drivers/embedded_driver/build/_deps/pico_sdk-build"
  "/mnt/c/Users/Owner/CLionProjects/CEN-4907C_Computer-Engineering-Design-1/01 Assignments/04 - Drivers/embedded_driver/build/_deps/pico_sdk-subbuild/pico_sdk-populate-prefix"
  "/mnt/c/Users/Owner/CLionProjects/CEN-4907C_Computer-Engineering-Design-1/01 Assignments/04 - Drivers/embedded_driver/build/_deps/pico_sdk-subbuild/pico_sdk-populate-prefix/tmp"
  "/mnt/c/Users/Owner/CLionProjects/CEN-4907C_Computer-Engineering-Design-1/01 Assignments/04 - Drivers/embedded_driver/build/_deps/pico_sdk-subbuild/pico_sdk-populate-prefix/src/pico_sdk-populate-stamp"
  "/mnt/c/Users/Owner/CLionProjects/CEN-4907C_Computer-Engineering-Design-1/01 Assignments/04 - Drivers/embedded_driver/build/_deps/pico_sdk-subbuild/pico_sdk-populate-prefix/src"
  "/mnt/c/Users/Owner/CLionProjects/CEN-4907C_Computer-Engineering-Design-1/01 Assignments/04 - Drivers/embedded_driver/build/_deps/pico_sdk-subbuild/pico_sdk-populate-prefix/src/pico_sdk-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/mnt/c/Users/Owner/CLionProjects/CEN-4907C_Computer-Engineering-Design-1/01 Assignments/04 - Drivers/embedded_driver/build/_deps/pico_sdk-subbuild/pico_sdk-populate-prefix/src/pico_sdk-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/mnt/c/Users/Owner/CLionProjects/CEN-4907C_Computer-Engineering-Design-1/01 Assignments/04 - Drivers/embedded_driver/build/_deps/pico_sdk-subbuild/pico_sdk-populate-prefix/src/pico_sdk-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
