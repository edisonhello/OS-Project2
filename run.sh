#!/bin/bash
mkdir -p output

N=$(ls $1 | wc | awk '{ printf "%d", $1 }')
F=$(ls $1)

cd $1 && ../user_program/master $N $F $2 > ../output/$1-$2-master.txt &
master=$!
./user_program/slave $N $F $2 127.0.0.1 > output/$1-$2-slave.txt &
slave=$!

wait $master
wait $slave

for file in $F; do
  diff $file $1/$file
  rm $file
done

