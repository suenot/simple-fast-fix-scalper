cmake_minimum_required(VERSION 3.10)
project(SimpleScalper)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# TODO: find_package(OnixsFIX REQUIRED) или аналог для вашей FIX-библиотеки

find_package(QuickFIX REQUIRED)

add_executable(SimpleScalper
    main.cpp
    SimpleScalper.cpp
)

target_include_directories(SimpleScalper PRIVATE ${QuickFIX_INCLUDE_DIRS})
target_link_libraries(SimpleScalper PRIVATE ${QuickFIX_LIBRARIES})

# TODO: target_include_directories(SimpleScalper PRIVATE ${OnixsFIX_INCLUDE_DIRS})
# TODO: target_link_libraries(SimpleScalper PRIVATE ${OnixsFIX_LIBRARIES})

find_package(Threads REQUIRED)
target_link_libraries(SimpleScalper PRIVATE Threads::Threads)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(SimpleScalper PRIVATE -O3)
endif()

# Примечание: если QuickFIX не найден, установите его через пакетный менеджер или соберите из исходников 