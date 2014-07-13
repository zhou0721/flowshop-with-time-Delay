PROJECT(PSE)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/src/srcbornesup")
#add_definitions(-D_CRAY)
add_definitions(-Dg77Fortran)
enable_language (Fortran)
FIND_PACKAGE(GLPK)
INCLUDE_DIRECTORIES(${GLPK_INCLUDE_DIR})
ADD_EXECUTABLE(pse 
	${PROJECT_SOURCE_DIR}/src/fdminmax.c
	${PROJECT_SOURCE_DIR}/src/newcdt.f
	${PROJECT_SOURCE_DIR}/src/srcbornesup/chaine.c
	${PROJECT_SOURCE_DIR}/src/srcbornesup/LR.c
	${PROJECT_SOURCE_DIR}/src/srcbornesup/NEH.c
	${PROJECT_SOURCE_DIR}/src/srcbornesup/NEH2.c
	${PROJECT_SOURCE_DIR}/src/srcbornesup/PR1.c
	${PROJECT_SOURCE_DIR}/src/srcbornesup/PR2.c
	${PROJECT_SOURCE_DIR}/src/srcbornesup/PR4.c
	${PROJECT_SOURCE_DIR}/src/srcbornesup/RZ.c
	${PROJECT_SOURCE_DIR}/src/srcbornesup/VNS.c
	)
TARGET_LINK_LIBRARIES(pse m  ${GLPK_LIBRARY})



