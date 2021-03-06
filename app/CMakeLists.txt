# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             ${PROJECT_SOURCE_DIR}/src/main/cpp/LocalApp.cpp
             ${PROJECT_SOURCE_DIR}/libs/framework/svrApplication.cpp
             ${PROJECT_SOURCE_DIR}/libs/framework/svrAndroidMain.cpp
             ${PROJECT_SOURCE_DIR}/libs/framework/svrUtil.cpp
             ${PROJECT_SOURCE_DIR}/libs/framework/svrContainers.cpp
             ${PROJECT_SOURCE_DIR}/libs/framework/svrConfig.cpp
             ${PROJECT_SOURCE_DIR}/libs/framework/svrCpuTimer.cpp
             ${PROJECT_SOURCE_DIR}/libs/private/svrApiHelper.cpp
             ${PROJECT_SOURCE_DIR}/libs/private/svrApiPredictiveSensor.cpp
             ${PROJECT_SOURCE_DIR}/libs/private/svrApiCore.cpp
             ${PROJECT_SOURCE_DIR}/libs/private/svrApiVersion.cpp
             ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
             )

add_library( qvrservice_client
             #svrapi
             SHARED
             IMPORTED )



include_directories( libs/framework )
include_directories( libs )
include_directories( libs/glm-0.9.7.0 )
include_directories( libs/qvr/inc )
include_directories( libs/inc )


find_library( # Sets the name of the path variable.
              android-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              android)
find_library( # Sets the name of the path variable.
                            EGL-lib

                            # Specifies the name of the NDK library that
                            # you want CMake to locate.
                            EGL)
find_library( # Sets the name of the path variable.
                            GLESv3-lib

                            # Specifies the name of the NDK library that
                            # you want CMake to locate.
                            GLESv3)



# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

#add third party library
set_target_properties( # Specifies the target library.
                        qvrservice_client
                        #svrapi

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${PROJECT_SOURCE_DIR}/libs/qvr/lib/libqvrservice_client.so
#                       ${PROJECT_SOURCE_DIR}/libs/qvr/lib/libsvrapi.so
                       )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib

                        qvrservice_client
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       ${android-lib}
                       ${EGL-lib}
                       ${GLESv3-lib})