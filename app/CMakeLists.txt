# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library(
        media
        SHARED
        src/main/cpp/media_player.cpp
)

set(JNI_LIBS_DIR ${CMAKE_SOURCE_DIR}/src/main/jniLibs)

add_library(
        avcodec
        SHARED
        IMPORTED
)
set_target_properties(
        avcodec
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavcodec.so
)

add_library(
        avdevice
        SHARED
        IMPORTED
)
set_target_properties(
        avdevice
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavdevice.so
)

add_library(
        avfilter
        SHARED
        IMPORTED
)
set_target_properties(
        avfilter
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavfilter.so
)

add_library(
        avformat
        SHARED
        IMPORTED
)
set_target_properties(
        avformat
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavformat.so
)

add_library(
        avutil
        SHARED
        IMPORTED
)
set_target_properties(
        avutil
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libavutil.so
)

add_library(
        swresample
        SHARED
        IMPORTED
)
set_target_properties(
        swresample
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libswresample.so
)

add_library(
        swscale
        SHARED
        IMPORTED
)
set_target_properties(
        swscale
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libswscale.so
)

add_library(
        libpostproc
        SHARED
        IMPORTED
)
set_target_properties(
        libpostproc
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libpostproc.so
)

add_library(
        libhidapi
        SHARED
        IMPORTED
)
set_target_properties(
        libhidapi
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libhidapi.so
)

add_library(
        libSDL2
        SHARED
        IMPORTED
)
set_target_properties(
        libSDL2
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libSDL2.so
)

add_library(
        libx264
        SHARED
        IMPORTED
)
set_target_properties(
        libx264
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libx2641.so
)

add_library(
        libfdk-aac
        SHARED
        IMPORTED
)
set_target_properties(
        libfdk-aac
        PROPERTIES IMPORTED_LOCATION
        ${JNI_LIBS_DIR}/${ANDROID_ABI}/libfdk-aac.so
)


include_directories(${JNI_LIBS_DIR}/includes)


find_library(log-lib log)
find_library(android-lib android)
find_library(jnigraphics-lib jnigraphics)
find_library(GLESv2-lib GLESv2)

target_link_libraries(
        media
        avcodec
        avdevice
        avfilter
        avformat
        avutil
        swresample
        swscale
        libpostproc
        libhidapi
        libSDL2
        libx264
        libfdk-aac
        ${log-lib}
        ${android-lib}
        ${jnigraphics-lib}
        ${GLESv2-lib})