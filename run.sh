#!/bin/bash

cpp_files="src/main.cpp"
for file in $(find src -name "*.cpp" -type f | grep -v "_test.cpp" | grep -v "test_"); do
    cpp_files="$cpp_files $file"
done

include_flags=""
for dir in $(find src -type d); do
    include_flags="$include_flags -I$dir"
done

library_flags=""

# Проверяем наличие библиотек и добавляем их
if pkg-config --exists poppler-cpp; then
    library_flags="$library_flags $(pkg-config --libs --cflags poppler-cpp)"
    echo "Using poppler-cpp for PDF parsing"
else
    echo "Warning: poppler-cpp not found, PDF support may be limited"
fi

if pkg-config --exists libzip; then
    library_flags="$library_flags $(pkg-config --libs --cflags libzip)"
    echo "Using libzip for DOCX parsing"
else
    # Пытаемся найти libzip вручную
    if [ -f /usr/lib/x86_64-linux-gnu/libzip.so ] || [ -f /usr/lib/libzip.so ]; then
        library_flags="$library_flags -lzip"
        echo "Using system libzip"
    else
        echo "Warning: libzip not found, using mock DOCX parser"
        include_flags="$include_flags -DUSE_MOCK_DOCX"
    fi
fi

if pkg-config --exists pugixml; then
    library_flags="$library_flags $(pkg-config --libs --cflags pugixml)"
    echo "Using pugixml for XML parsing"
else
    # Пытаемся найти pugixml вручную
    if [ -f /usr/lib/x86_64-linux-gnu/libpugixml.so ] || [ -f /usr/lib/libpugixml.so ]; then
        library_flags="$library_flags -lpugixml"
        echo "Using system pugixml"
    else
        echo "Warning: pugixml not found, using mock XML parser"
        include_flags="$include_flags -DUSE_MOCK_DOCX"
    fi
fi


library_flags="$library_flags -lstdc++fs -pthread"

g++ $include_flags $cpp_files -std=c++17 $library_flags -Wall -Wextra