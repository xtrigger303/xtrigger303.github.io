cmake_minimum_required(VERSION 3.15)

##################################################
# CONFIGURATION
##################################################

# project name dictates everything
project(kick)

# target name = project
add_library( ${PROJECT_NAME} STATIC )

# all sources files
target_sources( ${PROJECT_NAME} PRIVATE
    ./src/stuff.cpp
)

# c++ standard, emscripten needs to set it as compile option, see below
set( CMAKE_CXX_STANDARD 17 )

##################################################
# IMPLEMENTATION
##################################################

#includes
include_directories(./include)

# compile options
target_compile_options( ${PROJECT_NAME}  PRIVATE -std=c++${CMAKE_CXX_STANDARD} ) # need to set this, global one not enough
target_compile_options( ${PROJECT_NAME} PRIVATE -Wno-deprecated-declarations )

##################################################

if( EMSCRIPTEN )

    ##################################################
    # EMSCRIPTEN
    ##################################################

    message( "EMSCRIPTEN" )

    # compile options
    target_compile_options( ${PROJECT_NAME}  PRIVATE -s USE_SDL=2 )

    # link options
    target_link_options( ${PROJECT_NAME}  PRIVATE -s USE_SDL=2 )

else()

    ##################################################
    # NATIVE
    ##################################################

    # here I assume macos
    message( "NATIVE" )

    # need to link to OpenGL
    find_package(OpenGL REQUIRED)
    target_include_directories(${PROJECT_NAME} PRIVATE ${OPENGL_INCLUDE_DIRS} )
    target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES}  )

    # need SDL2, must be installed in a "proper" director, e.g. /Library/Frameworks
    find_library( SDL2_FRAMEWORK SDL2 )
    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_FRAMEWORK}/Headers )
    target_link_libraries(${PROJECT_NAME} ${SDL2_FRAMEWORK}  )
    
endif()

