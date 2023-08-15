cmake_minimum_required(VERSION 3.15)
project(s3upload)
set(CMAKE_CXX_STANDARD 20)

ExternalProject_Add(aws-sdk-cpp
  PREFIX aws-sdk-cpp
  GIT_REPOSITORY "https://github.com/aws/aws-sdk-cpp.git"
  GIT_TAG "1.11.141"
  SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/src/aws-sdk-cpp"
  CMAKE_CACHE_ARGS
    -DBUILD_ONLY:STRING=s3
    -DBUILD_SHARED_LIBS:BOOL=OFF
    -DMINIMIZE_SIZE:BOOL=ON
    -DENABLE_TESTING:BOOL=OFF
    -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/install
)

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH}
    ${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/install/lib
find_package(AWSSDK REQUIRED COMPONENTS s3)

# Source files go here.
set(PROJECT_SRC
    main.cpp
)
add_executable(${PROJECT_NAME} ${PROJECT_SRC})
target_link_libraries(${PROJECT_NAME} ${AWSSDK_LINK_LIBRARIES})
