cmake_minimum_required(VERSION 3.16)
project(CheatDLL CXX)

# Dumper-7 C++20 standardını zorunlu kılar
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tüm SDK başlık (header) dosyalarının bulunabilmesi için ana dizini dahil ediyoruz
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# dllmain.cpp dosyasını Cheat.dll olarak yapılandırıyoruz
add_library(Cheat SHARED dllmain.cpp)

# Çıktı dosyasının adının başında "lib" eki olmamasını sağlıyoruz (libCheat.dll yerine Cheat.dll)
set_target_properties(Cheat PROPERTIES PREFIX "")
