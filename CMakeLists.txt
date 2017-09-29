cmake_minimum_required(VERSION 2.8)
project(pic2mcmap)

set(IUPDIR . CACHE PATH "IUP directory")
set(IMDIR . CACHE PATH "IM directory")
set(NBTDIR ${CMAKE_CURRENT_SOURCE_DIR}/cNBT-master CACHE PATH "NBT library path")

include_directories(${IUPDIR}/include ${IMDIR}/include include)
file(GLOB SOURCES "src/*.c" "src/*.cpp")
add_executable(pic2mcmap ${SOURCES})
target_link_libraries(pic2mcmap ${IUPDIR}/libiup.a ${IUPDIR}/libiupim.a ${IMDIR}/libim.a ${IMDIR}/libim_process.a ${IMDIR}/libz.a gdi32 comdlg32 comctl32 uuid ole32 ${NBTDIR}/libnbt.a)