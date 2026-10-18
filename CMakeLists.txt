cmake_minimum_required(VERSION 3.16)
project(SC-MotorAg-CPort C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(sc-motor-ag
    src/controller.c
    src/dc_motor_sim.c
    src/ga_operator.c
    src/main.c
    src/simulator_operator.c
)

target_include_directories(sc-motor-ag PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
)

target_compile_options(sc-motor-ag PRIVATE
    -Wall
    -Wextra
)

target_link_libraries(sc-motor-ag PRIVATE m)