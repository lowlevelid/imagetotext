cmake_minimum_required(VERSION 3.10)
# https://cmake.org/cmake/help/v3.0/policy/CMP0042.html
# cmake_policy(SET CMP0042 NEW)
set (CMAKE_CXX_STANDARD 14)

project (imagetotext)

include_directories(${CMAKE_JS_INC})

set(HEADER_FILES
    plugin/Base.hpp
    plugin/Logger.hpp
    plugin/Type.hpp
    plugin/NOpenCv.hpp
    plugin/ITTAsyncWorker.hpp
)

set(SOURCE_FILES 
    plugin/Plugin.cpp
)

# check dynamic lib ext for specific OS
if (APPLE)
    set(DYNAMIC_LIB_EXT "dylib")
endif (APPLE)

if (UNIX AND NOT APPLE)
    set(DYNAMIC_LIB_EXT "so")
endif (UNIX AND NOT APPLE)

# ------------------- Zlib ------------------
find_path(ZLIB_INCLUDE_DIR zlib.h
    ${PROJECT_SOURCE_DIR}/vendor/zlib/include/
)

set(ZLIB_LIBRARY ${PROJECT_SOURCE_DIR}/vendor/zlib/lib/libz.${DYNAMIC_LIB_EXT})

set(ZLIB_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/vendor/zlib/include)
set(ZLIB_LIBRARY ${ZLIB_LIBRARY})

find_package(ZLIB REQUIRED)

# ------------------- LibPNG ------------------
# find_path(PNG_INCLUDE_DIRS png.h
#     ${PROJECT_SOURCE_DIR}/vendor/png/include/libpng16
# )

# set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${PROJECT_SOURCE_DIR}/vendor/png/lib/pkgconfig")
set(PNG_LIBRARY ${PROJECT_SOURCE_DIR}/vendor/png/lib/libpng16.${DYNAMIC_LIB_EXT})
set(PNG_PNG_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/vendor/png/include/libpng16)
set(PNG_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/vendor/png/include/libpng16)
set(PNG_LIBRARIES ${PNG_LIBRARY})
set(PNG_VERSION_STRING "1.6.37")

add_definitions( -DHAVE_LIBZ=1 )

find_package(PNG REQUIRED)

# ------------------- LibJPEG ------------------
set(JPEG_LIBRARY ${PROJECT_SOURCE_DIR}/vendor/jpeg/lib/liblibjpeg.${DYNAMIC_LIB_EXT})

set(JPEG_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/vendor/jpeg/include)
set(JPEG_LIBRARIES ${JPEG_LIBRARY})

find_package(JPEG REQUIRED)

# -------------------- WebP --------------------

find_path(WEBP_INCLUDE_DIR decode.h
    ${PROJECT_SOURCE_DIR}/vendor/webp/include/webp/
)

set(WEBP_NAMES ${WEBP_NAMES} WebP)
find_library(WEBP_LIBRARY
  NAMES ${WEBP_NAMES}
  PATHS ${PROJECT_SOURCE_DIR}/vendor/webp/lib
)

if (WEBP_LIBRARY AND WEBP_INCLUDE_DIR)
  set(WEBP_FOUND "YES")
endif (WEBP_LIBRARY AND WEBP_INCLUDE_DIR)

set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${PROJECT_SOURCE_DIR}/vendor/webp/lib/pkgconfig")
set(WebP_DIR ${PROJECT_SOURCE_DIR}/vendor/webp/lib/WebP/cmake)
set(WebP_INCLUDE_DIRS ${WEBP_INCLUDE_DIR})
set(WebP_LIBRARIES ${WEBP_LIBRARY})

add_definitions( ${PNG_DEFINITIONS} )
# enable JPEG
add_definitions( -DWEBP_HAVE_JPEG=1 )

# enable PNG
add_definitions( -DWEBP_HAVE_PNG=1 )

find_package(WebP CONFIG REQUIRED)

# -------------------- WebP Imageio --------------------
set(WEBPIMAGEIO_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/vendor/webp/libwebp-1.2.1/imageio)
file(GLOB WEBPIMAGEIO_HEADERS 
  ${PROJECT_SOURCE_DIR}/vendor/webp/libwebp-1.2.1/imageio/*.h
)

file(GLOB WEBPIMAGEIO_SOURCES 
  ${PROJECT_SOURCE_DIR}/vendor/webp/libwebp-1.2.1/imageio/*.c
)

# -------------------- Image Magick --------------------
set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${PROJECT_SOURCE_DIR}/vendor/image_magick/lib/pkgconfig")
set(ImageMagick_EXECUTABLE_DIR ${PROJECT_SOURCE_DIR}/vendor/image_magick/bin)
set(ImageMagick_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/vendor/image_magick/include/ImageMagick-7)
set(ImageMagick_LIBRARIES ${PROJECT_SOURCE_DIR}/vendor/image_magick/lib)
set(ImageMagick_Magick++_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/vendor/image_magick/include/ImageMagick-7)
set(ImageMagick_Magick++_LIBRARY ${PROJECT_SOURCE_DIR}/vendor/image_magick/lib/libMagick++-7.Q16HDRI.${DYNAMIC_LIB_EXT})
set(ImageMagick_MagickCore_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/vendor/image_magick/include/ImageMagick-7)
set(ImageMagick_MagickCore_LIBRARY ${PROJECT_SOURCE_DIR}/vendor/image_magick/lib/libMagickCore-7.Q16HDRI.${DYNAMIC_LIB_EXT})

add_definitions( -DMAGICKCORE_QUANTUM_DEPTH=16 )
add_definitions( -DMAGICKCORE_HDRI_ENABLE=0 )

find_package(ImageMagick REQUIRED COMPONENTS convert Magick++ MagickCore)

# find_program(MAGICK_CONFIG "${ImageMagick_EXECUTABLE_DIR}/Magick++-config")

# # Ask about CXX and lib flags/locations
# execute_process(COMMAND "${MAGICK_CONFIG}" "--cxxflags" OUTPUT_VARIABLE MAGICK_CXX_FLAGS)
# execute_process(COMMAND "${MAGICK_CONFIG}" "--libs" OUTPUT_VARIABLE MAGICK_LD_FLAGS)
# # Remove trailing whitespace (CMAKE warns about this)
# string(STRIP "${MAGICK_CXX_FLAGS}" MAGICK_CXX_FLAGS)
# string(STRIP "${MAGICK_LD_FLAGS}" MAGICK_LD_FLAGS)
# # Append all flags to CMAKE_CXX_FLAGS
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ${MAGICK_CXX_FLAGS} ${MAGICK_LD_FLAGS}")

# -------------------- Leptonica --------------------
set(Leptonica_DIR ${PROJECT_SOURCE_DIR}/vendor/leptonica/lib/cmake/leptonica)
set(Leptonica_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/vendor/leptonica/include/leptonica)
# set(Leptonica_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/vendor/leptonica/include/leptonica)
set(Leptonica_LIBRARY ${PROJECT_SOURCE_DIR}/vendor/leptonica/lib/libleptonica.${DYNAMIC_LIB_EXT})

add_definitions( ${PNG_DEFINITIONS} )
# enable JPEG
add_definitions( -DHAVE_LIBJPEG=1 )

# enable PNG
add_definitions( -DHAVE_LIBPNG=1 )

# enable WebP
add_definitions( -DHAVE_LIBWEBP=1 )

# enable Zlib
add_definitions( -DHAVE_LIBZ=1 )

find_package(Leptonica REQUIRED)

# -------------------- Tesseract --------------------
set(Tesseract_DIR ${PROJECT_SOURCE_DIR}/vendor/tesseract/lib/cmake/tesseract)
set(Tesseract_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/vendor/tesseract/include/tesseract/)
set(Tesseract_LIBRARY ${PROJECT_SOURCE_DIR}/vendor/tesseract/lib/libtesseract.${DYNAMIC_LIB_EXT})

find_package( Tesseract REQUIRED ) # 3.05 is currently the latest version of the git repository.

# -------------------- OPEN CV --------------------

# set OpenCV_DIR variable equal to the path to the cmake
# files within the previously installed opencv program
set(OpenCV_DIR ${PROJECT_SOURCE_DIR}/vendor/opencv/install/lib/cmake/opencv4)

# Tell compiler to use C++ 14 features which is needed because
# Clang version is often behind in the XCode installation
set(CMAKE_CXX_STANDARD 14)

# configure the necessary common CMake environment variables
# needed to include and link the OpenCV program into this
# demo project, namely OpenCV_INCLUDE_DIRS and OpenCV_LIBS
find_package( OpenCV REQUIRED )

# tell the build to include the headers from OpenCV
include_directories( ${OpenCV_INCLUDE_DIRS}  
        ${ImageMagick_INCLUDE_DIRS} 
        ${WebP_INCLUDE_DIRS} 
        ${WEBPIMAGEIO_INCLUDE_DIRS} 
        ${JPEG_INCLUDE_DIR} 
        ${ZLIB_INCLUDE_DIR} 
        ${PNG_INCLUDE_DIRS}
        ${Leptonica_INCLUDE_DIRS}
        ${Tesseract_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME} SHARED 
        ${HEADER_FILES} 
        ${SOURCE_FILES} 
        ${CMAKE_JS_SRC} 
        ${WEBPIMAGEIO_HEADERS} 
        ${WEBPIMAGEIO_SOURCES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

target_link_libraries(${PROJECT_NAME} 
        ${CMAKE_JS_LIB} 
        ${OpenCV_LIBS} 
        -L${ImageMagick_LIBRARIES} 
        -L${WebP_LIBRARIES} 
        ${ZLIB_LIBRARY} 
        ${JPEG_LIBRARIES}
        ${PNG_LIBRARIES}
        ${Leptonica_LIBRARY}
        ${Tesseract_LIBRARY}
)

# Include Node-API wrappers
execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
string(REGEX REPLACE "[\r\n\"]" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})

MESSAGE( [Main] " PROJECT_NAME = ${PROJECT_NAME}")
MESSAGE( [Main] " CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")
MESSAGE( [Main] " CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
MESSAGE( [Main] " PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}")

target_include_directories(${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_DIR})

# define NPI_VERSION
add_definitions(-DNAPI_VERSION=4)