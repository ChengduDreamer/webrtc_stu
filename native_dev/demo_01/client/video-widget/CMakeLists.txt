#cmake_minimum_required(VERSION 3.16)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)
#
#include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#
#aux_source_directory(. SRCS)
#set(FULL_PATH_SRCS )
#
#foreach(FILE IN LISTS SRCS)
#	list(APPEND FULL_PATH_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/${FILE})
#endforeach()
#
#set(VIDEO_WIDGET_SRCS
#	${FULL_PATH_SRCS}
#	PARENT_SCOPE
#)
