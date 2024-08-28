cmake_minimum_required(VERSION 3.1.0)

project(webrtc_client_01 VERSION 1.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(env.cmake)

set(CMAKE_PREFIX_PATH ${QT_ROOT}/lib/cmake)



include(InstallRequiredSystemLibraries)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#add_compile_definitions(ASIO2_ENABLE_SSL ASIO2_ENABLE_LOG)

add_definitions(-DUNICODE -D_UNICODE -DWIN32_LEAN_AND_MEAN)

add_subdirectory(3rdparty)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

if(${CMAKE_SIZEOF_VOID_P} STREQUAL "4")
	set(PLATFORM_WIN64 FALSE)
else()
	set(PLATFORM_WIN64 TRUE)
endif()


message("----------------------------------------------------=>" ${CMAKE_SOURCE_DIR}/3rdparty/libmediasoupclient)

include_directories(${CMAKE_SOURCE_DIR}
					${CMAKE_SOURCE_DIR}/client
					${CMAKE_SOURCE_DIR}/server
					${CMAKE_SOURCE_DIR}/common
					#${OPENSSL_ROOT}/include
					${CMAKE_SOURCE_DIR}/3rdparty/asio2/include
					${CMAKE_SOURCE_DIR}/3rdparty/asio2/3rd
)

link_directories(
		${OPENSSL_ROOT}/lib
)

set(common_SOURCES
	common/common.cpp
	common/yk_logger.cpp
)

set(client_SOURCES
	client/main.cpp
	client/context.cpp
	client/web_socket_client.cpp
)

set(server_SOURCES
	server/main.cpp
	server/web_socket_server.cpp
)

if(MSVC)
	#add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)  //修改调试级别
	set(CompilerFlags
			CMAKE_CXX_FLAGS
			CMAKE_CXX_FLAGS_DEBUG
			CMAKE_CXX_FLAGS_RELEASE
			CMAKE_CXX_FLAGS_RELWITHDEBINFO
			CMAKE_C_FLAGS
			CMAKE_C_FLAGS_DEBUG
			CMAKE_C_FLAGS_RELEASE
			CMAKE_C_FLAGS_RELWITHDEBINFO
			)
	foreach(CompilerFlag ${CompilerFlags})
	  string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
	endforeach()

	add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/bigobj>")
endif()


add_executable(rtc_client
		${client_SOURCES}
		${common_SOURCES}
)

add_executable(rtc_server
		${server_SOURCES}
		${common_SOURCES}
)

#add_subdirectory(${CMAKE_SOURCE_DIR}/test)

SET_TARGET_PROPERTIES(rtc_client PROPERTIES LINK_FLAGS "/MANIFESTUAC:\"level=\'requireAdministrator\' uiAccess=\'false\'\"")

#target_link_libraries(${PROJECT_NAME} Qt6::Widgets Qt6::Svg elawidgettools mediasoupclient)
#target_link_libraries(${PROJECT_NAME} Qt6::Widgets Qt6::Svg mediasoupclient)

#target_link_libraries(${PROJECT_NAME} mediasoupclient)

target_link_libraries(rtc_client spdlog::spdlog_header_only Boost::signals2 Qt6::Widgets)

target_link_libraries(rtc_server spdlog::spdlog_header_only Boost::signals2 Qt6::Widgets)


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_custom_command(
					TARGET rtc_client
					POST_BUILD
					COMMAND ${QT_ROOT}/bin/windeployqt.exe --debug rtc_client.exe
					WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	)
else()
	add_custom_command(
					TARGET rtc_client
					POST_BUILD
					COMMAND ${QT_ROOT}/bin/windeployqt.exe --release rtc_client.exe
					WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	)
endif()


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_custom_command(
					TARGET rtc_server
					POST_BUILD
					COMMAND ${QT_ROOT}/bin/windeployqt.exe --debug rtc_server.exe
					WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	)
else()
	add_custom_command(
					TARGET rtc_server
					POST_BUILD
					COMMAND ${QT_ROOT}/bin/windeployqt.exe --release rtc_server.exe
					WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
	)
endif()

