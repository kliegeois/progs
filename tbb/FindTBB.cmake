# FindTBB.cmake - try to find TBB libs
#   (RoBo)
#
# input:
#    ENABLE_TBBMALLOC : ON/OFF
# output:
#    TBB_LIBS     : list of tbb libraries to link with
#    TBB_LIB_PATH : where the .so/.lib are
#    TBB_INC_PATH : where the tbb/*.h are

OPTION(ENABLE_TBBMALLOC "Use TBBMALLOC" ON)

IF(0)  # Debug
	MESSAGE(STATUS "-------------------------------------")
	MESSAGE(STATUS " FindTBB")
	MESSAGE(STATUS "-------------------------------------")

	MESSAGE(STATUS "CMAKE_SHARED_LIBRARY_SUFFIX=${CMAKE_SHARED_LIBRARY_SUFFIX}")
	MESSAGE(STATUS "CMAKE_SHARED_MODULE_SUFFIX=${CMAKE_SHARED_MODULE_SUFFIX}")
	MESSAGE(STATUS "CMAKE_FIND_LIBRARY_SUFFIX=${CMAKE_FIND_LIBRARY_SUFFIX}")
	MESSAGE(STATUS "TBB_FIND_REQUIRED=${TBB_FIND_REQUIRED}") # automatiquement à 1 si REQUIRED
	MESSAGE(STATUS "TBB_USE_TBBMALLOC=${TBB_USE_TBBMALLOC}")
	MESSAGE(STATUS "WIN32=${WIN32}")
	MESSAGE(STATUS "MSVC=${MSVC}")
	MESSAGE(STATUS "MINGW=${MINGW}")
ENDIF()

# --- Liste des bibliothèques à trouver ---

SET(TBB_LIBS  optimized tbb debug tbb_debug)
IF(ENABLE_TBBMALLOC)
	LIST(APPEND TBB_LIBS optimized tbbmalloc_proxy optimized tbbmalloc)
	LIST(APPEND TBB_LIBS debug tbbmalloc_proxy_debug debug tbbmalloc_debug)
ENDIF()
MESSAGE(STATUS "TBB_LIBS=${TBB_LIBS}")

# --- TBB_LIB_PATH ---

SET(TBB_LIB_PATH TBB_LIB_PATH-NOTFOUND CACHE PATH "")

IF(NOT TBB_LIB_PATH)
	# try to find "tbb.dll/so" in the PATH
	find_library(TBB_TBBDLL "tbb" PATHS "" ENV LD_LIBRARY_PATH)
	MESSAGE(STATUS "TBB_TBBDLL=${TBB_TBBDLL}")
	IF("${TBB_TBBDLL}" STREQUAL "TBB_TBBDLL-NOTFOUND")
		MESSAGE(FATAL_ERROR "!!!!!!!! tbb${CMAKE_SHARED_LIBRARY_SUFFIX} should be in PATH/LD_LIBRARY_PATH !!!!!!!!!")
	ENDIF()

	get_filename_component(TBB_LIB_PATH ${TBB_TBBDLL} PATH CACHE)
	unset(TBB_TBBDLL CACHE)
	MESSAGE(STATUS "TBB_LIB_PATH=${TBB_LIB_PATH}")
ENDIF()

# debug (TBB_LIB_PATH_DEBUG)

SET(TBB_LIB_PATH_DEBUG TBB_LIB_PATH_DEBUG-NOTFOUND CACHE PATH "")

IF(NOT TBB_LIB_PATH_DEBUG)
	find_library(TBB_TBBDLL_DEBUG "tbb_debug" PATHS "" ENV LD_LIBRARY_PATH)
	MESSAGE(STATUS "TBB_TBBDLL_DEBUG=${TBB_TBBDLL_DEBUG}")
	IF(NOT TBB_TBBDLL_DEBUG)
		MESSAGE(FATAL_ERROR "!!!!!!!! tbb_debug${CMAKE_SHARED_LIBRARY_SUFFIX} should be in PATH/LD_LIBRARY_PATH !!!!!!!!!")
	ENDIF()

	get_filename_component(TBB_LIB_PATH_DEBUG ${TBB_TBBDLL_DEBUG} PATH CACHE)
	unset(TBB_TBBDLL_DEBUG CACHE)
	MESSAGE(STATUS "TBB_LIB_PATH_DEBUG=${TBB_LIB_PATH_DEBUG}")
ENDIF()

# TBB_INC_PATH: search include dir

SET(TBB_INC_PATH TBB_INC_PATH-NOTFOUND CACHE PATH "")

IF(NOT TBB_INC_PATH)
	SET(VAR1 ${TBB_LIB_PATH})
	SET(BASENAME ${VAR1})
	SET(TBB_TBBROOT "TBB_TBBROOT-NOTFOUND")
	WHILE( (NOT TBB_TBBROOT) OR (NOT BASENAME) )
		get_filename_component(DIRNAME ${VAR1} PATH)
		#MESSAGE(STATUS "DIRNAME=${DIRNAME}")	
		get_filename_component(BASENAME ${VAR1} NAME)
		#MESSAGE(STATUS "BASENAME=${BASENAME}") # on remonte jusqu'à un "lib" ou "build"
		
		IF( ("${BASENAME}" STREQUAL "lib") OR ("${BASENAME}" STREQUAL "build") )
			SET(TBB_TBBROOT ${DIRNAME})
		ELSE()
			SET(VAR1 ${DIRNAME})
			#MESSAGE(STATUS "VAR1=${VAR1}")
		ENDIF()
	ENDWHILE()

	MESSAGE(STATUS "TBB_TBBROOT=${TBB_TBBROOT}")
	MESSAGE(STATUS "TBBROOT=$ENV{TBBROOT}")

	FIND_PATH(TBB_INC_PATH 
				 NAMES  tbb/parallel_for.h
				 PATHS "${TBB_TBBROOT}"
				 PATH_SUFFIXES include
				)
	MESSAGE(STATUS "TBB_INC_PATH=${TBB_INC_PATH}")
ENDIF()


