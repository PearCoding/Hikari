function(add_gen_lib TARGET SRC)
    add_library(${TARGET} SHARED ${SRC} ${ARGN})
    set_target_properties(${TARGET} PROPERTIES CXX_VISIBILITY_PRESET hidden)
endfunction(add_gen_lib)

function(add_layer TARGET SRC)
    add_gen_lib(${TARGET} ${SRC} ${ARGN})
endfunction(add_layer)

function(add_icd TARGET SRC)
    add_gen_lib(${TARGET} ${SRC} ${ARGN})
endfunction(add_icd)

SET(SRC
loader/device.cpp
loader/extension.cpp
loader/icd.cpp
loader/instance.cpp
loader/layer.cpp
loader/physical_device.cpp
loader/terminator.cpp)

SET(SRC_L_STD_validation
layer/HK_LAYER_STD_validation/global.cpp
layer/HK_LAYER_STD_validation/instance.cpp
layer/HK_LAYER_STD_validation/instanceprocaddr.cpp
utils/layer_utils.c)

SET(SRC_ICD_CPU
icd/cpu/device.cpp
icd/cpu/global.cpp
icd/cpu/instance.cpp
icd/cpu/procaddr.cpp
icd/cpu/physical_device.cpp)

add_gen_lib(hikari ${SRC})
target_link_libraries(hikari PRIVATE stdc++fs)
add_dependencies(hikari header_files)
set_source_files_properties(${CMAKE_BINARY_DIR}/hikari.h PROPERTIES GENERATED TRUE)
include_directories(${CMAKE_BINARY_DIR})

add_layer(HK_LAYER_STD_validation ${SRC_L_STD_validation})
add_icd(HK_ICD_cpu ${SRC_ICD_CPU})
