cmake_minimum_required(VERSION 3.14)
project(document_processor)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# ============================================
# 0. ОПЦИИ КОНФИГУРАЦИИ
# ============================================

option(BUILD_TESTS "Build tests" ON)

# ============================================
# 1. НАЙТИ ВСЕ ФАЙЛЫ
# ============================================

# Файлы для основной программы (ВКЛЮЧАЯ main.cpp)
file(GLOB_RECURSE ALL_SRC_FILES "src/*.cpp" "src/*/*.cpp")
set(MAIN_SRC_FILES "")
foreach(file ${ALL_SRC_FILES})
    if(NOT file MATCHES ".*[Tt]est.*")
        list(APPEND MAIN_SRC_FILES ${file})
    endif()
endforeach()

# Файлы для тестов (БЕЗ main.cpp)
set(TEST_SRC_FILES "")
foreach(file ${ALL_SRC_FILES})
    if(NOT file MATCHES ".*[Tt]est.*" AND NOT file MATCHES ".*/main\.cpp$")
        list(APPEND TEST_SRC_FILES ${file})
    endif()
endforeach()

# Найти все заголовочные файлы
file(GLOB_RECURSE HEADER_FILES "src/*.h" "src/*.hpp" "src/*/*.h" "src/*/*.hpp")

# Сообщить что нашли
message("Найдено основных файлов: ${MAIN_SRC_FILES}")
message("Найдено файлов для тестов: ${TEST_SRC_FILES}")
message("Найдено заголовочных файлов: ${HEADER_FILES}")

# ============================================
# 2. СОЗДАТЬ ОСНОВНОЙ ИСПОЛНЯЕМЫЙ ФАЙЛ
# ============================================

add_executable(main ${MAIN_SRC_FILES})

# Добавить include директории
target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE DIRS LIST_DIRECTORIES true "src/*")
foreach(dir ${DIRS})
    if(IS_DIRECTORY ${dir})
        target_include_directories(main PRIVATE ${dir})
    endif()
endforeach()

# ============================================
# 3. НАЙТИ БИБЛИОТЕКИ
# ============================================

set(LIBS "")

# PDF: Poppler
find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(POPPLER poppler-cpp)
    if(POPPLER_FOUND)
        message("✅ Найден poppler-cpp")
        target_include_directories(main PRIVATE ${POPPLER_INCLUDE_DIRS})
        list(APPEND LIBS ${POPPLER_LIBRARIES})
        target_compile_definitions(main PRIVATE USE_POPPLER)
    else()
        message("⚠ poppler-cpp не найден, используем базовый парсер")
        target_compile_definitions(main PRIVATE USE_BASIC_PDF)
    endif()
endif()

# DOCX: libzip
find_library(LIBZIP zip)
if(LIBZIP)
    message("✅ Найден libzip")
    list(APPEND LIBS ${LIBZIP})
else()
    message("⚠ libzip не найден, используем мок для DOCX")
    target_compile_definitions(main PRIVATE USE_MOCK_DOCX)
endif()

# XML: pugixml
find_library(PUGIXML pugixml)
if(PUGIXML)
    message("✅ Найден pugixml")
    list(APPEND LIBS ${PUGIXML})
else()
    message("⚠ pugixml не найден, используем мок для XML")
    target_compile_definitions(main PRIVATE USE_MOCK_DOCX)
endif()

# Стандартные библиотеки
list(APPEND LIBS pthread stdc++fs)

# ============================================
# 4. СВЯЗАТЬ БИБЛИОТЕКИ
# ============================================

if(LIBS)
    target_link_libraries(main ${LIBS})
endif()

# ============================================
# 5. ФЛАГИ КОМПИЛЯЦИИ
# ============================================

target_compile_options(main PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(main PRIVATE -O2 -DNDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(main PRIVATE -g -O0 -DDEBUG)
endif()

# ============================================
# 6. ТЕСТИРОВАНИЕ
# ============================================

if(BUILD_TESTS)
    message(STATUS "Поиск тестовых файлов...")
    
    # Найти тестовые файлы в папке test
    file(GLOB_RECURSE TEST_FILES "test/*.cpp" "test/*/*.cpp")
    
    if(TEST_FILES)
        message("✅ Найдено тестовых файлов: ${TEST_FILES}")
        
        # Загружаем GoogleTest
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
        
        # Включаем GoogleTest
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
        
        if(googletest_POPULATED)
            message("✅ GoogleTest загружен и настроен")
            
            # Создать тестовый исполняемый файл (БЕЗ main.cpp)
            add_executable(tests ${TEST_FILES} ${TEST_SRC_FILES})
            
            # Настройки для тестов
            target_include_directories(tests PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/src
                ${CMAKE_CURRENT_SOURCE_DIR}/test
                ${gtest_SOURCE_DIR}/include
                ${gmock_SOURCE_DIR}/include
            )
            
            # Линковка для тестов
            target_link_libraries(tests
                ${LIBS}
                gtest
                gtest_main
                gmock
            )
            
            # Добавить определение для тестов
            target_compile_definitions(tests PRIVATE RUNNING_TESTS)
            
            # Включить CTest
            enable_testing()
            add_test(NAME AllTests COMMAND tests)
            
            # Цель для запуска тестов
            add_custom_target(run_tests
                COMMAND ./tests --gtest_color=yes
                DEPENDS tests
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Запуск тестов..."
            )
            
        else()
            message("⚠ Не удалось загрузить GoogleTest")
            add_custom_target(run_tests
                COMMAND ${CMAKE_COMMAND} -E echo "Не удалось загрузить GoogleTest"
                COMMENT "GoogleTest не загружен"
            )
        endif()
        
    else()
        message("⚠ Тестовые файлы не найдены в папке test/")
        add_custom_target(run_tests
            COMMAND ${CMAKE_COMMAND} -E echo "Тестовые файлы не найдены в папке test/"
            COMMENT "Нет тестов"
        )
    endif()
else()
    message("⚠ Тесты отключены (BUILD_TESTS=OFF)")
    add_custom_target(run_tests
        COMMAND ${CMAKE_COMMAND} -E echo "Тесты отключены. Соберите с -DBUILD_TESTS=ON"
        COMMENT "Тесты отключены"
    )
endif()

# ============================================
# 7. ДОПОЛНИТЕЛЬНЫЕ ЦЕЛИ
# ============================================

add_custom_target(run
    COMMAND ./main
    DEPENDS main
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Запуск основной программы..."
)

add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "Очистка всех файлов сборки..."
)

# ============================================
# 8. ИНФОРМАЦИЯ О СБОРКЕ
# ============================================

message("====================================")
message("Основная программа: main")
message("Тестовая программа: ${BUILD_TESTS}")
message("Стандарт C++: ${CMAKE_CXX_STANDARD}")
message("Библиотеки: ${LIBS}")
message("====================================")