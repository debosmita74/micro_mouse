cmake_minimum_required(VERSION 3.20)

# ── Project ─────────────────────────────────────────────────
project(micro_mouse
    VERSION 0.1.0
    LANGUAGES C ASM
    DESCRIPTION "Autonomous maze-solving micromouse robot"
)

# ── Toolchain ───────────────────────────────────────────────
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ARM cross-compiler (adjust path as needed)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)

# ── MCU Flags ───────────────────────────────────────────────
set(MCU_FLAGS "-mcpu=cortex-m3 -mthumb")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${MCU_FLAGS} -Wall -Wextra -Os -ffunction-sections -fdata-sections")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MCU_FLAGS} -Wl,--gc-sections -T ${CMAKE_SOURCE_DIR}/configs/stm32f103c8.ld")

# ── Sources ─────────────────────────────────────────────────
file(GLOB SOURCES "src/*.c")

# ── Include Directories ─────────────────────────────────────
include_directories(include)

# ── Executable ──────────────────────────────────────────────
add_executable(${PROJECT_NAME}.elf ${SOURCES})

# ── Post-build: generate .hex and .bin ──────────────────────
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}.elf>
    COMMENT "Generating .hex and .bin files"
)

# ── Flash target ────────────────────────────────────────────
add_custom_target(flash
    COMMAND openocd -f ${CMAKE_SOURCE_DIR}/configs/openocd.cfg
        -c "program ${PROJECT_NAME}.elf verify reset exit"
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Flashing to STM32"
)
