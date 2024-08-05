#!/bin/bash
echo "-------------------------------------------------------------"
echo "Valgrind:"
valgrind --leak-check=full --show-leak-kinds=all \
    --track-origins=yes \
../tritontalk $(< "${1}cmd.txt") < "${1}input.txt" 