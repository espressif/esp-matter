#
#   Copyright (c) 2014 - 2016 Kulykov Oleh <info@resident.name>
#
#   Permission is hereby granted, free of charge, to any person obtaining a copy
#   of this software and associated documentation files (the "Software"), to deal
#   in the Software without restriction, including without limitation the rights
#   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#   copies of the Software, and to permit persons to whom the Software is
#   furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included in
#   all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#   THE SOFTWARE.
#

cmake_minimum_required(VERSION 2.8)


project(librws)


set(PACKAGE "librws")
set(CPACK_PACKAGE_NAME "${PACKAGE}")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "2")
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
set(CPACK_PACKAGE_VENDOR "info@resident.name")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PACKAGE} ${PACKAGE_VERSION}")
set(SOVERSION "1.2.3")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(VERSION "${CPACK_PACKAGE_VERSION}")


include(CheckLibraryExists)
include(CheckFunctionExists)

include(CheckIncludeFile)
include(CheckIncludeFileCXX)

include(CheckTypeSize)
include(CheckSymbolExists)

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

# COMMAND LINE OPTIONS
option(RWS_OPT_SHARED "Build shared lib" ON)
option(RWS_OPT_STATIC "Build static lib" ON)
option(RWS_OPT_TESTS "Build librws tests" ON)

option(RWS_OPT_APPVEYOR_CI "Build with appveyor ci" OFF)

# C with -fPIC
check_c_compiler_flag("-fPIC" WITH_FPIC_C)
if(WITH_FPIC_C)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
endif(WITH_FPIC_C)


# CXX with -fPIC
check_cxx_compiler_flag("-fPIC" WITH_FPIC_CXX)
if(WITH_FPIC_CXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif(WITH_FPIC_CXX)


if(WIN32)
	add_definitions(-DWIN32)
	add_definitions(-D_WIN32)
	set(RWS_OS_WINDOWS 1)
endif(WIN32)


if(MINGW)
	set(RWS_COMPILER_MINGW 1)
endif(MINGW)


add_definitions(-DCMAKE_BUILD)

if (RWS_OPT_APPVEYOR_CI)
	add_definitions(-DRWS_APPVEYOR_CI)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

#check_include_file("netdb.h" RWS_HAVE_NETDB_H)
#check_include_file("sys/socket.h" RWS_HAVE_SYS_SOCKET_H)
#check_include_file("sys/types.h" RWS_HAVE_SYS_TYPES_H)
#check_include_file("netinet/tcp.h" RWS_HAVE_NETINET_TCP_H)
#check_include_file("unistd.h" RWS_HAVE_UNISTD_H)
#check_include_file("fcntl.h" RWS_HAVE_FCNTL_H)
check_include_file("pthread.h" RWS_HAVE_PTHREAD_H)

message(CMAKE_EXTRA_INCLUDE_FILES ": " ${CMAKE_EXTRA_INCLUDE_FILES})

# To check the size of a primitive type:
check_type_size("void*" RWS_SIZEOF_VOIDP_T)
if(RWS_SIZEOF_VOIDP_T AND (NOT RWS_VOIDP_TYPE))
	set(RWS_VOIDP_TYPE void*)
endif()

if(RWS_SIZEOF_VOIDP_T EQUAL 4)
	set(RWS_32BIT_PLATFORM 1)
endif()
if(RWS_SIZEOF_VOIDP_T EQUAL 8)
	set(RWS_64BIT_PLATFORM 1)
endif()


# functions
#check_function_exists(usleep RWS_HAVE_FUNCTION_USLEEP)

#if(RWS_HAVE_PTHREAD_H)
#check_function_exists(pthread_attr_setscope RWS_HAVE_FUNCTION_PTHREAD_ATTR_SETSCOPE)
#check_function_exists(pthread_attr_setstacksize RWS_HAVE_FUNCTION_PTHREAD_ATTR_SETSTACKSIZE)
#check_function_exists(pthread_cancel RWS_HAVE_FUNCTION_PTHREAD_CANCEL)
#check_function_exists(pthread_setname_np RWS_HAVE_FUNCTION_PTHREAD_SETNAME_NP)
#check_function_exists(pthread_set_name_np RWS_HAVE_FUNCTION_PTHREAD_SET_NAME_NP)
#endif(RWS_HAVE_PTHREAD_H)


if((NOT RWS_HAVE_PTHREAD_H) AND (NOT WIN32))
	message(FATAL_ERROR "Can't build librws without any threading support")
endif()


if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
	set(CMAKE_INSTALL_LIBDIR lib)
endif(NOT DEFINED CMAKE_INSTALL_LIBDIR)

# Now make sure that you the the build directory on your "Include" path when compiling
include_directories(${PROJECT_BINARY_DIR})


set(LIBRWS_SOURCES src/rws_common.c
		src/rws_error.c
		src/rws_frame.c
		src/librws.c
		src/rws_list.c
		src/rws_memory.c
		src/rws_socketpriv.c
		src/rws_socketpub.c
		src/rws_string.c
		src/rws_thread.c)
				

set(LIBRWS_HEADERS librws.h)


add_definitions(-DRWS_BUILD)


if(RWS_OPT_SHARED)
	add_library(rws SHARED ${LIBRWS_SOURCES} ${LIBRWS_HEADERS})
	if(MSVC)
    	# msvc does not append 'lib' - do it here to have consistent name
	    set_property(TARGET rws PROPERTY PREFIX "lib")
	set_property(TARGET rws PROPERTY IMPORT_PREFIX "lib")
	endif()
endif()

if(RWS_OPT_STATIC)
	add_library(rws_static STATIC ${LIBRWS_SOURCES} ${LIBRWS_HEADERS})
	set_property(TARGET rws_static APPEND PROPERTY COMPILE_FLAGS -DLIBRWS_STATIC)
	if(MSVC)
    	# msvc does not append 'lib' - do it here to have consistent name
	    set_target_properties(rws_static PROPERTIES PREFIX "lib")
	endif()
endif()



if(RWS_HAVE_PTHREAD_H)
	target_link_libraries(rws pthread)
endif(RWS_HAVE_PTHREAD_H)


if(WIN32)
	target_link_libraries(rws ws2_32)
endif(WIN32)


install(TARGETS rws
		DESTINATION lib)

install(TARGETS rws_static
		DESTINATION lib)

install(FILES librws.h 
		DESTINATION include)


if(RWS_OPT_TESTS)
	enable_testing()
	add_subdirectory(test)

	# This must always be last!
	include(CPack)
endif()

