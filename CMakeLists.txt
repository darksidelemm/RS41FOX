cmake_minimum_required(VERSION 3.6)
include(CMakeForceCompiler)
SET(CMAKE_SYSTEM_NAME "Generic")
SET(CMAKE_SYSTEM_VERSION 1)


#
# Set toolchain path below.
#

if (UNIX)
    set(TOOLCHAIN_DIR "$ENV{HOME}/opt/gcc-arm-none-eabi/bin")
    CMAKE_FORCE_C_COMPILER(${TOOLCHAIN_DIR}/arm-none-eabi-gcc GNU)
    CMAKE_FORCE_CXX_COMPILER(${TOOLCHAIN_DIR}/arm-none-eabi-g++ GNU)
else ()
    set(TOOLCHAIN_DIR "D:/Programy/GNU Tools ARM Embedded/5.4 2016q3/bin")
    CMAKE_FORCE_C_COMPILER(${TOOLCHAIN_DIR}/arm-none-eabi-gcc.exe GNU)
    CMAKE_FORCE_CXX_COMPILER(${TOOLCHAIN_DIR}/arm-none-eabi-g++.exe GNU)
endif ()


#
# Select a method of programming below.
#

set(USE_OPENOCD "true")
#set(USE_ST_FLASH "true")
#set(USE_BLACK_MAGIC "true")


project(RS41FOX C CXX)

add_definitions(-DSTM32F100C8)
add_definitions(-DSTM32F10X_MD_VL)
add_definitions(-DUSE_STDPERIPH_DRIVER)
add_definitions(-DSUPPORT_CPLUSPLUS)
add_definitions(-D__ASSEMBLY__)

SET(LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/arm-gcc-link.ld)
SET(COMMON_FLAGS " -mcpu=cortex-m3 -mthumb -Wall -ffunction-sections -g -O3 -g -nostartfiles ")
SET(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=c++11")
SET(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99")
SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map -lstdc++ -O3 -Wl,--gc-sections --specs=nano.specs -T ${LINKER_SCRIPT}")

file(GLOB_RECURSE USER_SOURCES "*.c")
file(GLOB_RECURSE USER_SOURCES_CXX "*.cpp")
file(GLOB_RECURSE USER_HEADERS "*.h")

include_directories(cmsis
        cmsis_boot
        stm_lib/inc
        .)

add_executable(${PROJECT_NAME}.elf ${USER_SOURCES} ${USER_SOURCES_CXX} ${USER_HEADERS} ${HAL_SOURCES} ${LINKER_SCRIPT})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map")
set(HEX_FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin)
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}"
        COMMAND ${TOOLCHAIN_DIR}/arm-none-eabi-size ${PROJECT_NAME}.elf)

set(CMAKE_CXX_STANDARD 11)


#
# make erase
#
# Does a security unlock, followed by a flash erase.
#
# Just doing an unlock on its own would only erase the flash, if the device had
# previously been locked. To avoid surprising the user, we always erase the
# flash.
#
# Note that security unlock is currently only supported if using openocd.
#
#
# make halt
#
# Resets and then halts the device.
#
#
# make run
#
# Resets and then runs the device.
#
#
# make program-halt
#
# Programs the device, and leaves it halted.
#
# Does not require the device to be erased first, although it must not be
# security locked.
#
#
# make program
#
# Programs the device, and allows it to run after reset.
#
# Does not require the device to be erased first, although it must not be
# security locked.
#


if (USE_OPENOCD)

add_custom_target(erase
        COMMENT "Unlocking and erasing device"
        COMMAND openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg -c \"init\; reset halt\; stm32f1x unlock 0\; reset halt\; flash protect 0 0 last off\; reset halt\; flash erase_sector 0 0 last\; reset halt\; exit\"
)
add_custom_target(halt
        COMMENT "Resetting and halting device"
        COMMAND openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg -c \"init\; reset halt\; exit\"
)
add_custom_target(run
        COMMENT "Resetting device and then running"
        COMMAND openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg -c \"init\; reset\; exit\"
)
add_custom_target(program-halt
        DEPENDS ${PROJECT_NAME}.elf
        COMMENT "Programming device with ${HEX_FILE}, and then halting"
        COMMAND openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg -c \"program ${HEX_FILE} verify exit\"
)
add_custom_target(program
        DEPENDS ${PROJECT_NAME}.elf
        COMMENT "Programming device with ${HEX_FILE}, and then running"
        COMMAND openocd -f interface/stlink-v2.cfg -f target/stm32f1x.cfg -c \"program ${HEX_FILE} verify reset exit\"
)

elseif (USE_ST_FLASH)

add_custom_target(erase
        COMMENT "Erasing device"
        COMMAND st-flash erase
        COMMAND echo If unlocking is required, please select openocd in CMakeLists.txt
)
add_custom_target(halt
        COMMENT "Halting the device is not supported by st-link"
        COMMAND echo If halting is required, please select openocd in CMakeLists.txt
)
add_custom_target(run
        COMMENT "Resetting device and then running"
        COMMAND st-flash reset
)
add_custom_target(program-halt
        COMMENT "Halting the device is not supported by st-link"
        COMMAND echo If halting is required, please select openocd in CMakeLists.txt
)
add_custom_target(program
        DEPENDS ${PROJECT_NAME}.elf
        COMMENT "Programming device with ${HEX_FILE}, and then running"
        COMMAND st-flash --reset --format ihex write ${HEX_FILE}
)

elseif (USE_BLACK_MAGIC)

add_custom_target(erase
        COMMENT "Erasing the device is not supported with Black Magic Probe"
        COMMAND echo If erasing is required, please select another programming method in CMakeLists.txt
)
add_custom_target(halt
        COMMENT "Halting the device is not supported with Black Magic Probe"
        COMMAND echo If halting is required, please select openocd in CMakeLists.txt
)
add_custom_target(run
        COMMENT "make run is not supported with Black Magic Probe"
        COMMAND echo If erasing is required, please select another programming method in CMakeLists.txt
)
add_custom_target(program-halt
        COMMENT "Halting the device is not supported with Black Magic Probe"
        COMMAND echo If halting is required, please select openocd in CMakeLists.txt
)
add_custom_target(program
        DEPENDS ${PROJECT_NAME}.elf
        COMMENT "Programming device with ${PROJECT_NAME}.elf, and then running"
        COMMAND chmod u+x flash.sh
        COMMAND ./flash.sh
)

else ()

add_custom_target(erase
        COMMENT "No method of programming is available"
        COMMAND echo Edit CMakeLists.txt and select a method of programming.
)
add_custom_target(halt
        COMMENT "No method of programming is available"
        COMMAND echo Edit CMakeLists.txt and select a method of programming.
)
add_custom_target(run
        COMMENT "No method of programming is available"
        COMMAND echo Edit CMakeLists.txt and select a method of programming.
)
add_custom_target(program-halt
        COMMENT "No method of programming is available"
        COMMAND echo Edit CMakeLists.txt and select a method of programming.
)
add_custom_target(program
        COMMENT "No method of programming is available"
        COMMAND echo Edit CMakeLists.txt and select a method of programming.
)

endif ()
