cmake_minimum_required(VERSION 3.15)
project(s3upload VERSION 1.0.0 LANGUAGES CXX)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 20)

ExternalProject_Add(aws-sdk-cpp
  PREFIX aws-sdk-cpp
  GIT_REPOSITORY "https://github.com/aws/aws-sdk-cpp.git"
  GIT_TAG "1.11.141"
  SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/src/aws-sdk-cpp"
  CMAKE_CACHE_ARGS
    -DBUILD_ONLY:STRING=s3;transfer
    -DBUILD_SHARED_LIBS:BOOL=OFF
    -DMINIMIZE_SIZE:BOOL=ON
    -DENABLE_TESTING:BOOL=OFF
    -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/install
)


set(AWS_CORE_LIBRARY ${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/install/lib/aws-cpp-sdk-core${CMAKE_STATIC_LIBRARY_SUFFIX})
set(AWS_S3_LIBRARY ${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/install/lib/aws-cpp-sdk-s3${CMAKE_STATIC_LIBRARY_SUFFIX})
set(AWS_TRANSFER ${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/install/lib/aws-cpp-sdk-transfer${CMAKE_STATIC_LIBRARY_SUFFIX})

include_directories(${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/install/include)
# Source files go here.
set(PROJECT_SRC
    main.cpp
)
add_executable(${PROJECT_NAME} ${PROJECT_SRC})
target_link_libraries(${PROJECT_NAME} ${AWS_CORE_LIBRARY} ${AWS_S3_LIBRARY} ${AWS_TRANSFER})
