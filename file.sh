#!/bin/bash

for file in parsing/*; do
    if norminette "$file" | grep -q "25 lines"; then
        echo "File: $file"

        norminette "$file" | grep "25 lines"
    fi
done

