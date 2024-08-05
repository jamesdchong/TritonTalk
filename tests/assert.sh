#!/bin/bash

dir=${1}
if [[ ! -d "${dir}output" ]]; then
    mkdir "${dir}output"
fi
touch "${dir}output/output.txt"; touch "${dir}output/log.txt"

cat "${dir}input.txt" | ./delayPipe.sh  | ../tritontalk $(<"${dir}cmd.txt") 1>"${dir}output/output.txt" 2>"${dir}output/log.txt"

echo "Diff (sorted, used to check presence not order):"
# sorting output
touch "${dir}output/sorted_output.txt"
#sort -t':' -k1V,1 -s "${dir}output/output.txt" > "${dir}output/sorted_output.txt"
sort -V "${dir}output/output.txt" > "${dir}output/sorted_output.txt"
# sorting expect
touch "${dir}sorted_expect.txt"
#sort -t':' -k1V,1 -s "${dir}expect.txt" > "${dir}sorted_expect.txt"
sort -V "${dir}expect.txt" > "${dir}sorted_expect.txt"

mv "${dir}sorted_expect.txt" "${dir}expect.txt"
diff "${dir}output/sorted_output.txt" "${dir}expect.txt"
# run valgrind if found
if [[ $(command -v valgrind) ]]; then
    echo "-------------------------------------------------------------"
    echo "Valgrind:"
    touch "${dir}output/valgrind_output.txt"
    valgrind --leak-check=full --show-leak-kinds=all \
     --track-origins=yes --log-file="${dir}output/valgrind.txt" \
    cat "${dir}input.txt" | ./delayPipe.sh | ../tritontalk $(< "${dir}cmd.txt") &> "${dir}output/valgrind_output.txt"
    tail -n 1 "${dir}output/valgrind.txt"
fi