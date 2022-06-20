cmake_minimum_required(VERSION 3.18)
project(wav2mp3)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-O3 -Wall)
    set(CMAKE_CXX_FLAGS "-fsanitize=thread")
endif()

if (UNIX)
    add_definitions(-DUSE_LOCAL_LAME_LIB=FALSE)
endif()

set(CMAKE_CXX_STANDARD 14)

if (WIN32)
    include_directories(include pthread)
else()
    include_directories(include)
endif()

add_library(libmp3lame STATIC IMPORTED)
add_executable(wav2mp3 src/import/wav_reader.cpp src/export/mp3_writer.cpp src/utils/mutex.cpp src/utils/wav_file_path_resolver.cpp src/utils/utils.cpp main.cpp)

function(copy_dll_to_bin_dir PTHREAD_DLL_PATH MSVCR_DLL_PATH)
    add_custom_command(
                TARGET wav2mp3 POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/${PTHREAD_DLL_PATH}
                ${CMAKE_SOURCE_DIR}/${MSVCR_DLL_PATH}
                ${CMAKE_CURRENT_BINARY_DIR})
endfunction()

if (WIN32)
    add_library(pthread STATIC IMPORTED)
    if ("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8") #x64
        set_target_properties(libmp3lame PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/x64/libmp3lame64.lib)
        set_target_properties(pthread PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/x64/pthreadVC2.lib)
        copy_dll_to_bin_dir(lib/x64/pthreadVC2.dll lib/x64/msvcr100.dll)
            
    else() #x86
        set_target_properties(libmp3lame PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/x86/libmp3lame.lib)
        set_target_properties(pthread PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/x86/pthreadVSE2.lib)
        copy_dll_to_bin_dir(lib/x86/pthreadVSE2.dll lib/x86/msvcr100.dll)
    endif()
else()
    if (USE_LOCAL_LAME_LIB)
        set_target_properties(libmp3lame PROPERTIES IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/libmp3lame.a)
    else()
        set_target_properties(libmp3lame PROPERTIES IMPORTED_LOCATION /usr/local/bin/libmp3lame.a)
    endif()
endif()

target_link_libraries(wav2mp3 pthread libmp3lame)