#!/bin/bash
mkdir -p medium_data_$1_$2
for i in $(seq 1 3000);
do
    echo -n $i

    sudo ./user_program/master 1 \
    input/sample_input_2/target_file \
    $1  > medium_data_$1_$2/master_out$i &
    # exit
    sleep 0.1
    sudo ./user_program/slave 1 \
    sample2_1 \
    $2 127.0.0.1 > /dev/null
    sleep 0.1    
    wait
    sudo diff sample2_1 input/sample_input_2/target_file || break
    rm sample2_1 -f

done

