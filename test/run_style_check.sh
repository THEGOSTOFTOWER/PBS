#!/bin/bash

# Проверяем, установлен ли clang-format
if ! command -v clang-format &> /dev/null; then
    echo "Error: clang-format is not installed."
    echo "Install: sudo apt-get install clang-format"
    exit 1
fi

echo "Checking code style with clang-format..."

# Ищем все файлы .cpp и .h в папке src (включая все подпапки) и test
FILES=$(find src test -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) 2>/dev/null)

if [ -z "$FILES" ]; then
    echo "No .cpp, .h or .hpp files found in src/ or test/ directories."
    exit 0
fi

echo "Found $(echo "$FILES" | wc -l) files to check"

# Запускаем проверку:
# --dry-run: не менять файлы, только показать, что изменилось бы
# -Werror: считать предупреждения ошибками (вернуть код != 0)
# --style=file: использовать наш .clang-format
clang-format --dry-run -Werror --style=file $FILES

# Проверяем код возврата
EXIT_CODE=$?
if [ $EXIT_CODE -eq 0 ]; then
    echo "---------------------------------------------------"
    echo -e "\033[0;32m[SUCCESS] Code style is correct.\033[0m"
    exit 0
else
    echo "---------------------------------------------------"
    echo -e "\033[0;31m[FAIL] Code style violations found!\033[0m"
    echo ""
    echo "To fix them automatically, run:"
    echo "  find src test -type f \( -name \"*.cpp\" -o -name \"*.h\" -o -name \"*.hpp\" \) -exec clang-format -i --style=file {} \;"
    exit 1
fi