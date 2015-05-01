# FindTBB.cmake - try to find TBB libs


OPTION(TBB_USE_TBBMALLOC "Use TBBMALLOC" ON)

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

# --- Liste des bibliothèques à trouver ---
# --- TBB_LIBS/TBB_LIBS_DEBUG ---

SET(TBB_LIBS       optimized tbb debug tbb_debug)

IF(TBB_USE_TBBMALLOC)
	SET(TBB_LIBS       ${TBB_LIBS}  optimized tbbmalloc_proxy  optimized tbbmalloc)
	SET(TBB_LIBS       ${TBB_LIBS}  debug tbbmalloc_proxy_debug  debug tbbmalloc_debug)
ENDIF(TBB_USE_TBBMALLOC)

MESSAGE(STATUS "TBB_LIBS=${TBB_LIBS}")

# -- try to guess TBBROOT from the PATH -- 

# try to find "tbb.dll/so" in the PATH
find_library(FOUND_LIB "tbb" PATHS "" ENV LD_LIBRARY_PATH)
MESSAGE(STATUS "FOUND_LIB=${FOUND_LIB}")
IF("${FOUND_LIB}" STREQUAL "FOUND_LIB-NOTFOUND")
	MESSAGE(FATAL_ERROR "!!!!!!!! tbb${CMAKE_SHARED_LIBRARY_SUFFIX} should be in PATH/LD_LIBRARY_PATH !!!!!!!!!")
ENDIF()

get_filename_component(TBB_LIB_PATH ${FOUND_LIB} PATH)
MESSAGE(STATUS "TBB_LIB_PATH=${TBB_LIB_PATH}")


# debug
find_library(FOUND_LIBD "tbb_debug" PATHS "" ENV LD_LIBRARY_PATH)
MESSAGE(STATUS "FOUND_LIBD=${FOUND_LIBD}")
IF("${FOUND_LIB}" STREQUAL "FOUND_LIBD-NOTFOUND")
        MESSAGE(FATAL_ERROR "!!!!!!!! tbb_debug${CMAKE_SHARED_LIBRARY_SUFFIX} should be in PATH/LD_LIBRARY_PATH !!!!!!!!!")
ENDIF()

get_filename_component(TBB_DEBUG_LIB_PATH ${FOUND_LIBD} PATH)
MESSAGE(STATUS "TBB_DEBUG_LIB_PATH=${TBB_DEBUG_LIB_PATH}")

# search include dir

SET(TBB_CANDIDATE_DIR "")
WHILE("${TBB_CANDIDATE_DIR}" STREQUAL "")
	get_filename_component(DIRNAME ${FOUND_LIB} PATH)
	#MESSAGE(STATUS "DIRNAME=${DIRNAME}")	
	get_filename_component(BASENAME ${FOUND_LIB} NAME)
	#MESSAGE(STATUS "BASENAME=${BASENAME}") # on remonte jusqu'à un "lib" ou "build"

	IF("${BASENAME}" STREQUAL "")
		SET(TBB_CANDIDATE_DIR "NOT-FOUND")
	ENDIF()
	
	IF( ("${BASENAME}" STREQUAL "lib") OR ("${BASENAME}" STREQUAL "build") )
		SET(TBB_CANDIDATE_DIR ${DIRNAME})
	ELSE()
		SET(FOUND_LIB ${DIRNAME})
		#MESSAGE(STATUS "FOUND_LIB=${FOUND_LIB}")
	ENDIF()
ENDWHILE()

MESSAGE(STATUS "TBB_CANDIDATE_DIR=${TBB_CANDIDATE_DIR}")
MESSAGE(STATUS "TBBROOT=$ENV{TBBROOT}")



# --- TBB_INCLUDE_PATH ---

SET(TBB_INCLUDE_PATH TBB_INCLUDE_PATH-NOTFOUND CACHE PATH "" FORCE) # enlever le "FORCE" par la suite
FIND_PATH(TBB_INCLUDE_PATH 
             NAMES  tbb/parallel_for.h
			 PATHS "${TBB_CANDIDATE_DIR}"
			 PATH_SUFFIXES include
            )
MESSAGE(STATUS "TBB_INCLUDE_PATH=${TBB_INCLUDE_PATH}")

