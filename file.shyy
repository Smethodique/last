#!/bin/bash

for file in parsing/* ; do
    if norminette "$file" | grep -q "Too many functions in file"; then
        echo "File: $file"

        norminette "$file" | grep "Too many functions in file"
    fi
done

