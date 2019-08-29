cmake_minimum_required(VERSION 3.15)

##################################################
# CONFIGURATION
##################################################

# project name dictates everything
project(foo)

# target name = project
add_executable( ${PROJECT_NAME} )

# all sources files
target_sources( ${PROJECT_NAME} PRIVATE
    ./src/main.cpp
)

# relative path of common library
set( LIB_REL_PATH  ../KickLib )
# name of common library target
set( LIB_TARGET  kick )
# c++ standard, emscripten needs to set it as compile option, see below
set( CMAKE_CXX_STANDARD 17 )
# name of source html template file, relative to src directory
set( HTML_TEMPLATE template.html )

##################################################
# IMPLEMENTATION
##################################################

# compile options
target_compile_options( ${PROJECT_NAME}  PRIVATE -std=c++${CMAKE_CXX_STANDARD} ) # need to set this, global one not enough
target_compile_options( ${PROJECT_NAME} PRIVATE -Wno-deprecated-declarations )

##################################################

if( EMSCRIPTEN )

    ##################################################
    # EMSCRIPTEN
    ##################################################

    message( "EMSCRIPTEN" )

    # common library stuff
    add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_REL_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_REL_PATH}/embuild )
    target_include_directories( ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_REL_PATH}/include )
    target_link_directories( ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_REL_PATH}/embuild )
    target_link_libraries( ${PROJECT_NAME} ${LIB_TARGET} )

    # compile options
    target_compile_options( ${PROJECT_NAME}  PRIVATE -s USE_SDL=2 )

    # copy the html template and create dependency so that changing file will trigger relink
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/${HTML_TEMPLATE} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${HTML_TEMPLATE})

    # link options
    target_link_options( ${PROJECT_NAME}  PRIVATE -s USE_SDL=2 )
    target_link_options( ${PROJECT_NAME}  PRIVATE --shell-file ${CMAKE_CURRENT_BINARY_DIR}/${HTML_TEMPLATE} )

    # tried target_link_options( ${PROJECT_NAME}  PRIVATE -o ${PROJECT_NAME}.html ) but did not work, the following works
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    # copy to dist directoy relevant files
    add_custom_command( TARGET ${PROJECT_NAME} 
        POST_BUILD
        COMMAND mkdir -p ${CMAKE_CURRENT_SOURCE_DIR}/emdist
        COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.html ${CMAKE_CURRENT_SOURCE_DIR}/emdist
        COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.wasm ${CMAKE_CURRENT_SOURCE_DIR}/emdist
        COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.js ${CMAKE_CURRENT_SOURCE_DIR}/emdist
    )

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

    # common library stuff
    if( XCODE )
        set( LIB_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_REL_PATH}/xcodebuild )
    else(XCODE)
        set( LIB_BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_REL_PATH}/build )
    endif( XCODE)

    add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_REL_PATH} ${LIB_BUILD_DIR} )
    target_include_directories( ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_REL_PATH}/include )
    target_link_directories( ${PROJECT_NAME} PRIVATE ${LIB_BUILD_DIR} )
    target_link_libraries( ${PROJECT_NAME} ${LIB_TARGET} )
    
endif()

