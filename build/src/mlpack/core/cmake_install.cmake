# Install script for directory: /home/jchen098/KMEANS/mlpack-2.0.1/src/mlpack/core

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/core/arma_extend/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/core/boost_backport/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/core/data/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/core/dists/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/core/kernels/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/core/math/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/core/metrics/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/core/optimizers/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/core/tree/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/core/util/cmake_install.cmake")

endif()

