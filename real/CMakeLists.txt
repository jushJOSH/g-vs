cmake_minimum_required(VERSION 3.1)
project(videoserver)

# Компилятор для проекта
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск и установка библиотек с помощью pkg-config
find_package(PkgConfig REQUIRED) 
pkg_check_modules(GST REQUIRED gstreamer-1.0)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GIO REQUIRED gio-2.0)
pkg_check_modules(GRTSP REQUIRED gstreamer-rtsp-1.0)
pkg_check_modules(GST_APP REQUIRED gstreamer-app-1.0)
pkg_check_modules(GST_PBUTILS REQUIRED gstreamer-pbutils-1.0)

find_package(oatpp          1.3.0 REQUIRED)
find_package(oatpp-swagger  1.3.0 REQUIRED)
find_package(oatpp-postgresql 1.3.0 REQUIRED)
find_package(Threads REQUIRED)
find_package(jwt-cpp REQUIRED)

# Включение исходных файлов
MACRO(HEADER_DIRECTORIES return_list directory)
    FILE(GLOB_RECURSE new_list ${directory}/*.hpp ${directory}/*.cpp ${directory}/*.h)
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
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${GST_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS} ${GST_APP_INCLUDE_DIRS} ${JSON_INCLUDE_DIRS} ${GIO_INCLUDE_DIRS} ${GST_PBUTILS_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${GST_LIBRARIES} ${GLIB_LIBRARIES} ${GST_APP_LIBRARIES} ${JSON_LIBRARIES} ${GIO_LIBRARIES} ${GST_PBUTILS_LIBRARIES} oatpp::oatpp oatpp::oatpp-swagger oatpp::oatpp-postgresql Threads::Threads jwt-cpp::jwt-cpp)