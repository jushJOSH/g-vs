# cmake_minimum_required(VERSION 3.1)
# project(videoserver)

# set(CMAKE_CXX_STANDARD 23)

# MACRO(HEADER_DIRECTORIES return_list)
#     FILE(GLOB_RECURSE new_list *.hpp *.cpp)
#     SET(dir_list "")
#     FOREACH(file_path ${new_list})
#         GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
#         SET(dir_list ${dir_list} ${dir_path})
#     ENDFOREACH()
#     LIST(REMOVE_DUPLICATES dir_list)
#     SET(${return_list} ${dir_list})
# ENDMACRO()

# HEADER_DIRECTORIES(libs)
# add_library(videoserver-lib ${libs})

# ## include directories

# target_include_directories(videoserver-lib PUBLIC include)

# ## link libs
# find_package(PkgConfig            REQUIRED) 
# find_package(oatpp          1.3.0 REQUIRED)
# find_package(oatpp-swagger  1.3.0 REQUIRED)
# find_package(oatpp-sqlite   1.3.0 REQUIRED)

# pkg_search_module(GLIB      REQUIRED glib-2.0) 
# pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
# pkg_check_modules(GST_APP   REQUIRED gstreamer-app-1.0)
# pkg_check_modules(GST_VIDEO REQUIRED gstreamer-video-1.0)
# pkg_check_modules(JSON      REQUIRED nlohmann_json)

# target_link_libraries(videoserver-lib 
#         # Oat++
#         PUBLIC oatpp::oatpp
#         PUBLIC oatpp::oatpp-swagger
#         PUBLIC oatpp::oatpp-sqlite
        
#         # GStreamer
#         PUBLIC PkgConfig::GLIB
#         PUBLIC PkgConfig::GSTREAMER
#         PUBLIC PkgConfig::GST_APP
#         PUBLIC PkgConfig::GST_VIDEO
# )

# add_definitions(
#         ## define path to swagger-ui static resources folder
#         -DOATPP_SWAGGER_RES_PATH="${oatpp-swagger_INCLUDE_DIRS}/../bin/oatpp-swagger/res"

#         ## SQLite database file
#         -DDATABASE_FILE="${CMAKE_CURRENT_SOURCE_DIR}/db.sqlite"
#         ## SQLite database test file
#         -DTESTDATABASE_FILE="${CMAKE_CURRENT_SOURCE_DIR}/test-db.sqlite"

#         ## Path to database migration scripts
#         -DDATABASE_MIGRATIONS="${CMAKE_CURRENT_SOURCE_DIR}/sql"
# )

# if(CMAKE_SYSTEM_NAME MATCHES Linux)
#     target_link_libraries(videoserver-lib INTERFACE Threads::Threads ${CMAKE_DL_LIBS})
# endif()

# ## add executables

# add_executable(videoserver-exe main.cpp)
# target_link_libraries(videoserver-exe videoserver-lib)


cmake_minimum_required(VERSION 3.1)
project(videoserver)

# Компилятор для проекта
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск и установка библиотек с помощью pkg-config
find_package(PkgConfig REQUIRED) 
pkg_check_modules(GST REQUIRED gstreamer-1.0)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GST_APP REQUIRED gstreamer-app-1.0)
pkg_check_modules(JSON REQUIRED nlohmann_json)

find_package(oatpp          1.3.0 REQUIRED)
find_package(oatpp-swagger  1.3.0 REQUIRED)
find_package(oatpp-postgresql 1.3.0 REQUIRED)
find_package(Threads REQUIRED)

# Включение исходных файлов
MACRO(HEADER_DIRECTORIES return_list directory)
    FILE(GLOB_RECURSE new_list ${directory}/*.hpp ${directory}/*.cpp)
    SET(file_list "")
    FOREACH(file_path ${new_list})
        LIST(APPEND file_list ${file_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES file_list)
    SET(${return_list} ${file_list})
ENDMACRO()

HEADER_DIRECTORIES(includes include)
message(STATUS "[INFO] Found these files: ${includes}")

set(SOURCES
    main.cpp
    ${includes}
)

# Добавление исходных файлов и подключаемых директорий
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${GST_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS} ${GST_APP_INCLUDE_DIRS} ${JSON_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${GST_LIBRARIES} ${GLIB_LIBRARIES} ${GST_APP_LIBRARIES} ${JSON_LIBRARIES} oatpp::oatpp oatpp::oatpp-swagger oatpp::oatpp-postgresql Threads::Threads)