cmake_minimum_required(VERSION 3.8)

project(mmServ)

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
add_definitions(-DBOOST_LOG_DYN_LINK -Wfatal-errors)

find_package(Boost 1.68 COMPONENTS system log REQUIRED)

add_executable(mmServ main.cpp
                      manager.cpp
                      manager.h
                      controller.h
                      controller.cpp
                      message_object.h
                      message_object.cpp
                      random_engine.h
                      random_engine.cpp
                      task_engine.h
                      task_engine.cpp
                      log.h
                      message.h
                      message.cpp
                      util.h
                      util.cpp)

target_include_directories(mmServ PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(mmServ PRIVATE ${Boost_LIBRARIES} ws2_32 mswsock)

target_compile_features(mmServ PRIVATE cxx_std_11)


configure_file(${CMAKE_SOURCE_DIR}/config/pio_config.xml ${CMAKE_BINARY_DIR}/config/pio_config.xml COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/config/ci_config.xml ${CMAKE_BINARY_DIR}/config/ci_config.xml COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/config/task.cim.json ${CMAKE_BINARY_DIR}/config/task.cim.json COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/config/task.overallstatus.json ${CMAKE_BINARY_DIR}/config/task.overallstatus.json COPYONLY)