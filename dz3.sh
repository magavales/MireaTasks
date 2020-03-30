#!/usr/bin/env bash
if ! [ -f /home/denis/dz/sum_out.txt ]; then
cat *.txt > sum_out.txt
else 
echo "Error"
fi
