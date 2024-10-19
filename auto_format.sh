#!/bin/bash

# Find all .c files and run c_formatter_42 on them
find . -name "*.c" | while read file; do
    c_formatter_42 "$file"
    echo "Formatted: $file"
done

