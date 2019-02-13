cmake_minimum_required(VERSION 3.9)
project(symbiote)

set(CMAKE_CXX_STANDARD 14)

# Symbiote library
add_library(symbiote
        src/core/entitymanager.cpp              include/core/entitymanager.hpp
        src/core/system.cpp                     include/core/system.hpp
        src/core/entity.cpp                     include/core/entity.hpp
        src/core/component.cpp                  include/core/component.hpp

        src/game/components/rigidbody.cpp       include/game/components/rigidbody.hpp
        src/game/components/transform.cpp       include/game/components/transform.hpp
        src/game/systems/physics.cpp            include/game/systems/physics.hpp
        src/game/systems/renderer.cpp            include/game/systems/renderer.hpp
)
target_include_directories(symbiote PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include> PRIVATE src)
target_compile_options(symbiote PRIVATE "-Wall")
target_compile_options(symbiote PRIVATE "-ansi")
target_compile_definitions(symbiote PUBLIC _DEBUG=1)

# Symbiote vendors
add_subdirectory(vendors/glm/glm)
target_link_libraries(symbiote glm_static)
target_include_directories(symbiote PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/vendors/glm>)

# Symbiote SDL2
find_package(SDL2 REQUIRED)
target_link_libraries(symbiote ${SDL2_LIBRARIES})
target_include_directories(symbiote PUBLIC ${SDL2_INCLUDE_DIRS})

# Symbiote main
add_executable(symbiote_main src/main.cpp)
target_link_libraries(symbiote_main symbiote)

# Symbiote tests
enable_testing()
add_executable(symbiote_test
        tests/test_systems/systems.cpp
        tests/test_systems/systems.hpp
        tests/test_components/components.cpp
        tests/test_components/components.hpp
        tests/test_systems.cpp
        tests/test_entities.cpp
        tests/test_lifecycle.cpp
        tests/test_performance.cpp
        tests/test_entitymanager.cpp)
add_subdirectory(tests/googletest)
target_link_libraries(symbiote_test symbiote gtest_main)
target_include_directories(symbiote_test PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_include_directories(symbiote_test PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tests/1.8.0/googletest/include>)