#!/bin/bash

# Проверяем, установлен ли clang-format
if ! command -v clang-format &> /dev/null; then
    echo "Error: clang-format is not installed."
    exit 1
fi

echo "Checking code style with clang-format..."

# Ищем все файлы .cpp и .h в папке src (включая все подпапки) и test
FILES=$(find src test -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) 2>/dev/null)

if [ -z "$FILES" ]; then
    echo "No .cpp, .h or .hpp files found in src/ or test/ directories."
    echo "Found files:"
    find src test -type f -name "*" 2>/dev/null | head -10
    exit 0
fi

echo "Found $(echo "$FILES" | wc -l) files to check:"
echo "$FILES" 
echo ""

# Проверяем существование каждого файла перед проверкой
VALID_FILES=""
for file in $FILES; do
    if [ -f "$file" ]; then
        VALID_FILES="$VALID_FILES $file"
    else
        echo "Warning: File '$file' not found, skipping"
    fi
done

# Если файлов для проверки нет, выходим
if [ -z "$VALID_FILES" ]; then
    echo "No valid files to check."
    exit 0
fi

# Запускаем проверку:
# --dry-run: не менять файлы, только показать, что изменилось бы
# -Werror: считать предупреждения ошибками (вернуть код != 0)
# --style=file: использовать наш .clang-format
echo "Running clang-format check..."
clang-format --dry-run -Werror --style=file $VALID_FILES

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
    echo "Files with issues:"
    for file in $VALID_FILES; do
        if clang-format --dry-run --style=file "$file" 2>&1 | grep -q "clang-format"; then
            echo "  - $file"
        fi
    done
    echo ""
    echo "To fix them automatically, run:"
    echo "  clang-format -i --style=file $VALID_FILES"
    echo ""
    echo "Or to fix specific files, run:"
    echo "  find src test -type f \( -name \"*.cpp\" -o -name \"*.h\" -o -name \"*.hpp\" \) -exec clang-format -i --style=file {} \;"
    exit 1
fi