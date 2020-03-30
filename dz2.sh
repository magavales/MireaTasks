#!/usr/bin/env bash 
N=0
B=1
mod=0
until [ "$N" -eq -1 ]
do
	if ! [ -d /home/denis/dz/dir$N ]; then 
		break
	else
		mod=$(($N % 5))
		if [ "$mod" -eq 0 ]; then
			rm -R dir$N
		fi	
	fi
	N=$(($N + $B))
done	
