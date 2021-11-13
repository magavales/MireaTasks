#!/usr/bin/env bash
cd /home/denis/
ls
echo "Какой каталог вы хотите заархивировать"
read katalog
tar -zcvf $katalog.tar /home/denis/$katalog 

