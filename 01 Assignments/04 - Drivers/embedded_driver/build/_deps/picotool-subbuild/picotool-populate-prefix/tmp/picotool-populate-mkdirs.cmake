# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/mnt/c/Users/Owner/CLionProjects/CEN-4907C_Computer-Engineering-Design-1/01 Assignments/04 - Drivers/embedded_driver/build/_deps/picotool-src"
  "/mnt/c/Users/Owner/CLionProjects/CEN-4907C_Computer-Engineering-Design-1/01 Assignments/04 - Drivers/embedded_driver/build/_deps/picotool-build"
  "/mnt/c/Users/Owner/CLionProjects/CEN-4907C_Computer-Engineering-Design-1/01 Assignments/04 - Drivers/embedded_driver/build/_deps/picotool-subbuild/picotool-populate-prefix"
  "/mnt/c/Users/Owner/CLionProjects/CEN-4907C_Computer-Engineering-Design-1/01 Assignments/04 - Drivers/embedded_driver/build/_deps/picotool-subbuild/picotool-populate-prefix/tmp"
  "/mnt/c/Users/Owner/CLionProjects/CEN-4907C_Computer-Engineering-Design-1/01 Assignments/04 - Drivers/embedded_driver/build/_deps/picotool-subbuild/picotool-populate-prefix/src/picotool-populate-stamp"
  "/mnt/c/Users/Owner/CLionProjects/CEN-4907C_Computer-Engineering-Design-1/01 Assignments/04 - Drivers/embedded_driver/build/_deps/picotool-subbuild/picotool-populate-prefix/src"
  "/mnt/c/Users/Owner/CLionProjects/CEN-4907C_Computer-Engineering-Design-1/01 Assignments/04 - Drivers/embedded_driver/build/_deps/picotool-subbuild/picotool-populate-prefix/src/picotool-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/mnt/c/Users/Owner/CLionProjects/CEN-4907C_Computer-Engineering-Design-1/01 Assignments/04 - Drivers/embedded_driver/build/_deps/picotool-subbuild/picotool-populate-prefix/src/picotool-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/mnt/c/Users/Owner/CLionProjects/CEN-4907C_Computer-Engineering-Design-1/01 Assignments/04 - Drivers/embedded_driver/build/_deps/picotool-subbuild/picotool-populate-prefix/src/picotool-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
