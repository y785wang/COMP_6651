# CMake entry point
#cmake_minimum_required (VERSION 3.0)
project (seg)

#find_package(opencv REQUIRED)
find_package(opencv)

if (opencv_FOUND)

# I could also use
# ${opencv_LIBRARIES} to set

set(ALL_LIBS
        opencv_core
        opencv_imgproc
        opencv_highgui
        opencv_imgcodecs
)

include_directories(${opencv_INCLUDE_DIRS})
link_directories(${opencv_LIB_DIRS})

else (opencv_FOUND)


set(ALL_LIBS
        opencv_core
        opencv_imgproc
        opencv_highgui
        opencv_imgcodecs
)


include_directories(/encs/pkg/opencv-3.3.0/root/include/)
link_directories(/encs/pkg/opencv-3.3.0/root/lib)

endif (opencv_FOUND)

add_executable(seg src/main.cpp)
target_link_libraries(seg ${ALL_LIBS})
