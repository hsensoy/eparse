cmake_minimum_required(VERSION 2.8.4)

SET( PROJ_NAME      "eparse" )
SET( PROJ_PATH      ${CMAKE_SOURCE_DIR} )
SET( PROJ_OUT_PATH  ${CMAKE_BINARY_DIR} )
FILE( GLOB_RECURSE PROJ_SOURCES src/*.c )

#FILE( GLOB_RECURSE PROJ_TEST_SOURCES src/*.c )
#list(REMOVE_ITEM PROJ_TEST_SOURCES "src/ai-parse.c")

#FILE( GLOB_RECURSE PROJ_TEST_SOURCES src/*.c )
#FILE( REMOVE PROJ_TEST_SOURCES src/ai-parse.c )
#FILE( GLOB_RECURSE PROJ_HEADERS include/${PROJ_NAME}/*.h)
SET( FRAMEWORK "mkl")
link_directories(/usr/local/lib/perceptronmkl)

SET( PROJ_INCLUDES "include" "include/eparse"  "/usr/local/include/perceptron/epblas" "/usr/local/include/perceptron" "/usr/local/include/shared")
SET( PROJ_LIBRARIES "/usr/local/lib/shared/libshared.a" "m"  )

IF( FRAMEWORK EQUAL "cuda" )
  SET( PROJ_LIBRARIES ${PROJ_LIBRARIES} "/usr/local/cuda/lib64/libcurand.so" )
  SET( CMAKE_EXE_LINKER_FLAGS " -O3 -std=gnu99 -lperceptron${FRAMEWORK} -lm -lcurand" CACHE STRING "" FORCE )
ELSE( FRAMEWORK EQUAL "cuda" )
    SET( CMAKE_EXE_LINKER_FLAGS " -O3 -std=gnu99 -lperceptron${FRAMEWORK} -lm" CACHE STRING "" FORCE )
ENDIF( FRAMEWORK EQUAL "cuda" )



SET( CMAKE_C_FLAGS "-std=gnu99  -O3 -g -DBATCH_SCORE -DNDEBUG")
#SET (${PROJ_NAME}_VERSION_MAJOR 0)
#SET (${PROJ_NAME}_VERSION_MINOR 1)

PROJECT(${PROJ_NAME})
INCLUDE_DIRECTORIES( ${PROJ_INCLUDES} )
ADD_EXECUTABLE( ${PROJ_NAME} ${PROJ_SOURCES} )
TARGET_LINK_LIBRARIES( ${PROJ_NAME} ${PROJ_LIBRARIES} )
enable_testing()

SET ( PROJ_TESTS
   "tFeatureTemplate"
   "tFeatureTemplateDiscrete")

foreach( t ${PROJ_TESTS} )
       add_executable( test_${PROJ_NAME}_${t} tests/${t}.c src/corpus.c src/feattemplate.c src/dependency.c src/parseutil.c src/conll.c src/murmurhash.c)
           target_link_libraries( test_${PROJ_NAME}_${t} "perceptron${FRAMEWORK}")

          target_link_libraries( test_${PROJ_NAME}_${t} "/usr/local/lib/shared/libshared.a")
          IF( FRAMEWORK EQUAL "cuda" )
          target_link_libraries( test_${PROJ_NAME}_${t} "/usr/local/cuda/lib64/libcurand.so")
          ENDIF(FRAMEWORK EQUAL "cuda")
               add_test( test_${PROJ_NAME}_${t}  ./test_${PROJ_NAME}_${t} )
endforeach(t)
#set (CMAKE_INSTALL_PREFIX /home/hsensoy/ai-lab)
install(TARGETS ${PROJ_NAME} DESTINATION bin)
