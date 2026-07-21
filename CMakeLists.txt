cmake_minimum_required(VERSION 3.16)
project(CheatDLL CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Sadece dllmain.cpp dosyasını derliyoruz (SDK dosyalarına ihtiyaç kalmadı)
add_library(Cheat SHARED dllmain.cpp)

set_target_properties(Cheat PROPERTIES PREFIX "")
