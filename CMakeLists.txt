cmake_minimum_required (VERSION 3.8)
project(Settlers5GameSpeedTrainer)
set(CMAKE_CXX_STANDARD 17)
#set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(HEADERS "include/Button.hpp"
			"include/MemoryManager.hpp")

set(SOURCES "src/Button.cpp"
			"src/MemoryManager.cpp"
			"src/main.cpp")

#source_group("" FILES ${miscFiles})


add_executable(Settlers5GameSpeedTrainer ${HEADERS} ${SOURCES})
target_include_directories(Settlers5GameSpeedTrainer PUBLIC include)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Settlers5GameSpeedTrainer)

SET_TARGET_PROPERTIES(Settlers5GameSpeedTrainer PROPERTIES LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator'")