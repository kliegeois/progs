# Exe 

FILE(GLOB SRCS *.h *.cpp)

ADD_EXECUTABLE(genmai_console ${SRCS})
IF(WIN32)
    SET_TARGET_PROPERTIES(genmai_console PROPERTIES DEBUG_POSTFIX "_d")
ENDIF(WIN32)

# ---

INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/src 
)

TARGET_LINK_LIBRARIES(genmai_console genmai )
