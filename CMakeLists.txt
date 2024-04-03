cmake_minimum_required(VERSION 2.8)

project(sylar)

# include (cmake/utils.camke)

set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -o0 -g -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

# set(LIB_SRC
#     sylar/log.cc
#     sylar/util.cc
# )
include_directories(.)
# include_directories(./sylar)
# include_directories(/apps/oncelane/include)
# link_directories(/apps/oncelane/lib)


file(GLOB LIB_SRC
    "sylar/*.h"
    "sylar/*.cc"
    "sylar/http/*.cc"
    "sylar/http/*.h"
    "sylar/util/*.h"
    "sylar/util/*.cc"
)


add_library(sylar SHARED ${LIB_SRC})

find_library(YAMLCPP yaml-cpp)
find_library(PTHREAD pthread)

set(LIBS 
        sylar
        pthread
        yaml-cpp
        dl
        ssl
        json
        protobuf)

message("***", ${LIBS})




#add_library(sylar_static STATIC ${LIB_SRC})
#SET_TARGET_PROPERTIES (sylar_static PROPERTIES OUTPUT_NAME "sylar")


add_executable(test tests/test.cc)
add_dependencies(test sylar)
target_link_libraries(test sylar ${LIBS})

add_executable(test_config tests/test_config.cc)
add_dependencies(test_config sylar)
target_link_libraries(test_config sylar ${LIBS})

add_executable(test_thread tests/test_thread.cc)
add_dependencies(test_thread sylar)
target_link_libraries(test_thread sylar ${LIBS})

add_executable(test_util tests/test_util.cc)
add_dependencies(test_util sylar)
target_link_libraries(test_util sylar ${LIBS})

add_executable(test_fiber tests/test_fiber.cc)
add_dependencies(test_fiber sylar)
target_link_libraries(test_fiber sylar ${LIBS})

add_executable(test_scheduler tests/test_scheduler.cc)
add_dependencies(test_scheduler sylar)
target_link_libraries(test_scheduler sylar ${LIBS})

add_executable(test_io tests/test_io.cc)
add_dependencies(test_io sylar)
target_link_libraries(test_io sylar ${LIBS})

add_executable(test_timer tests/test_timer.cc)
add_dependencies(test_timer sylar)
target_link_libraries(test_timer sylar ${LIBS})

add_executable(test_hook tests/test_hook.cc)
add_dependencies(test_hook sylar)
target_link_libraries(test_hook sylar ${LIBS})

add_executable(test_address tests/test_address.cc)
add_dependencies(test_address sylar)
target_link_libraries(test_address sylar ${LIBS})

add_executable(test_socket tests/test_socket.cc)
add_dependencies(test_socket sylar)
target_link_libraries(test_socket sylar ${LIBS})

# add_executable(test_array tests/test_array.cc)
# add_dependencies(test_array sylar)
# target_link_libraries(test_array sylar ${LIBS})

add_executable(test_bytearray tests/test_bytearray.cc)
add_dependencies(test_bytearray sylar)
target_link_libraries(test_bytearray sylar ${LIBS})

add_executable(test_http tests/test_http.cc)
add_dependencies(test_http sylar)
target_link_libraries(test_http sylar ${LIBS})

add_executable(test_http_parser tests/test_http_parser.cc)
add_dependencies(test_http_parser sylar)
target_link_libraries(test_http_parser sylar ${LIBS})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)