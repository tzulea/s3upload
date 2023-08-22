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

set(AWS_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/install/lib)

set(AWS_CORE_LIBRARY ${AWS_INSTALL_DIR}/aws-cpp-sdk-core${CMAKE_STATIC_LIBRARY_SUFFIX})
set(AWS_S3_LIBRARY ${AWS_INSTALL_DIR}/aws-cpp-sdk-s3${CMAKE_STATIC_LIBRARY_SUFFIX})
set(AWS_TRANSFER ${AWS_INSTALL_DIR}/aws-cpp-sdk-transfer${CMAKE_STATIC_LIBRARY_SUFFIX})
set(AWS_CRT ${AWS_INSTALL_DIR}/aws-crt-cpp${CMAKE_STATIC_LIBRARY_SUFFIX})
set(AWS_C_COMMON ${AWS_INSTALL_DIR}/aws-c-common${CMAKE_STATIC_LIBRARY_SUFFIX})
set(AWS_C_STREAM ${AWS_INSTALL_DIR}/aws-c-event-stream${CMAKE_STATIC_LIBRARY_SUFFIX})
set(AWS_C_IO ${AWS_INSTALL_DIR}/aws-c-io${CMAKE_STATIC_LIBRARY_SUFFIX})
set(AWS_C_HTTP ${AWS_INSTALL_DIR}/aws-c-http${CMAKE_STATIC_LIBRARY_SUFFIX})
set(AWS_C_AUTH ${AWS_INSTALL_DIR}/aws-c-auth${CMAKE_STATIC_LIBRARY_SUFFIX})
set(AWS_C_CAL ${AWS_INSTALL_DIR}/aws-c-cal${CMAKE_STATIC_LIBRARY_SUFFIX})
set(AWS_CHECKSUM ${AWS_INSTALL_DIR}/aws-checksums${CMAKE_STATIC_LIBRARY_SUFFIX})
set(AWS_SDK_UTILS ${AWS_INSTALL_DIR}/aws-c-sdkutils${CMAKE_STATIC_LIBRARY_SUFFIX})
set(AWS_C_MQTT ${AWS_INSTALL_DIR}/aws-c-mqtt${CMAKE_STATIC_LIBRARY_SUFFIX})
set(AWS_C_S3 ${AWS_INSTALL_DIR}/aws-c-s3${CMAKE_STATIC_LIBRARY_SUFFIX})
set(AWS_C_COMPRESSION ${AWS_INSTALL_DIR}/aws-c-compression${CMAKE_STATIC_LIBRARY_SUFFIX})

include_directories(${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/install/include)

# Source files go here.
set(PROJECT_SRC
    main.cpp
    CloudBackup.cpp
    CloudBackup.h
    constants.h
    IWatcher.h
)

if(WIN32)
  set(PLATFORM_SRC
    WatcherWin.h
    WatcherWin.cpp
  )
endif()

add_executable(${PROJECT_NAME} ${PROJECT_SRC} ${PLATFORM_SRC})

link_directories(${CMAKE_CURRENT_BINARY_DIR}/aws-sdk-cpp/install/lib/aws-crt-cpp)

target_link_libraries(${PROJECT_NAME} ${AWS_CORE_LIBRARY} ${AWS_S3_LIBRARY} ${AWS_TRANSFER} ${AWS_CRT} ${AWS_SDK_UTILS} ${AWS_C_COMPRESSION} ${AWS_C_MQTT} ${AWS_C_S3} ${AWS_C_COMMON} ${AWS_C_STREAM} ${AWS_C_IO} ${AWS_C_HTTP} ${AWS_C_HTTP} ${AWS_C_AUTH} ${AWS_C_CAL} ${AWS_CHECKSUM})
if(WIN32)
  set(PLATFORM_LIBS
      Wtsapi32
      Wininet
      Bcrypt
      WinHttp
      Ws2_32
      Version
      Userenv
      Crypt32
      Shlwapi
      Secur32
      Ncrypt
  )
  target_link_libraries(${PROJECT_NAME} ${PLATFORM_LIBS})
endif()
