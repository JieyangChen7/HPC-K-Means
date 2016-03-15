# Install script for directory: /home/jchen098/KMEANS/mlpack-2.0.1/src/mlpack/methods

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
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/adaboost/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/amf/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/cf/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/decision_stump/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/det/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/emst/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/fastmks/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/gmm/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/hmm/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/hoeffding_trees/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/kernel_pca/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/kmeans/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/mean_shift/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/lars/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/linear_regression/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/local_coordinate_coding/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/logistic_regression/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/lsh/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/matrix_completion/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/naive_bayes/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/nca/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/neighbor_search/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/nmf/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/pca/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/perceptron/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/quic_svd/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/radical/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/range_search/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/rann/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/regularized_svd/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/softmax_regression/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/sparse_autoencoder/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/sparse_coding/cmake_install.cmake")
  include("/home/jchen098/KMEANS/mlpack-2.0.1/build/src/mlpack/methods/nystroem_method/cmake_install.cmake")

endif()

