#!/usr/bin/env bash
echo "Сколько директорий вы хотите создать"
read N
A=1
until [[ "$N" -eq -1 ]] 
do 
	mkdir dir$N
	N=$(($N - $A))
done

