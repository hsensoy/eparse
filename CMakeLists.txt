cmake_minimum_required(VERSION 2.8.4)

SET( PROJ_NAME      "eparse" )
SET( PROJ_PATH      ${CMAKE_SOURCE_DIR} )
SET( PROJ_OUT_PATH  ${CMAKE_BINARY_DIR} )
FILE( GLOB_RECURSE PROJ_SOURCES src/*.c )
#FILE( GLOB_RECURSE PROJ_HEADERS include/${PROJ_NAME}/*.h)
SET( PROJ_INCLUDES "include" "include/eparse"  "/usr/local/include/perceptron/epblas" "/usr/local/include/perceptron" "/usr/local/include/shared")
#SET( PROJ_LIBRARIES "perceptronmkl" )

SET( CMAKE_EXE_LINKER_FLAGS " -O3 -std=gnu99 -L/usr/local/lib/perceptronmkl /usr/local/lib/shared/libshared.a -lperceptronmkl -lm" CACHE STRING "" FORCE )
SET( CMAKE_C_FLAGS "-std=gnu99  -O3 -g -DBATCH_SCORE -DNDEBUG")
SET (${PROJ_NAME}_VERSION_MAJOR 0)
SET (${PROJ_NAME}_VERSION_MINOR 1)

PROJECT(${PROJ_NAME})
INCLUDE_DIRECTORIES( ${PROJ_INCLUDES} )
ADD_EXECUTABLE( ${PROJ_NAME} ${PROJ_SOURCES} )
TARGET_LINK_LIBRARIES( ${PROJ_NAME} ${PROJ_LIBRARIES} )
enable_testing()



FILE( GLOB_RECURSE PROJ_TESTS tests/*.c )
#set (CMAKE_INSTALL_PREFIX /home/hsensoy/ai-lab)
install(TARGETS ${PROJ_NAME} DESTINATION bin)
