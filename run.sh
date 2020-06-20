#!/bin/bash

N=$(ls $1 | wc | awk '{ printf "%d", $1 }')
F=$(ls $1)

cd $1 && ../user_program/master $N $F $2 &
master=$!
./user_program/slave $N $F $2 127.0.0.1 &
slave=$!

wait $master
wait $slave

for file in $F; do
  diff $file $1/$file
  rm $file
done
