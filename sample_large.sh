mkdir -p medium_data_$1_$2
for i in $(seq 1 3000);
do
    echo -n $i

    sudo ./user_program/master 3 \
    input/input_3/target_file_v3_1 \
    input/input_3/target_file_v3_2 \
    input/input_3/target_file_v3_3 \
    $1  > medium_data_$1_$2/master_out$i &
    # exit
        
    sudo ./user_program/slave 3 \
    sample3_1 \
    sample3_2 \
    sample3_3 \
    $2 127.0.0.1 > /dev/null
#    sleep 0.1
    wait
    sudo diff sample3_1 input/input_3/target_file_v3_1 || break
    sudo diff sample3_2 input/input_3/target_file_v3_2 || break
    sudo diff sample3_3 input/input_3/target_file_v3_3 || break
    
    rm sample3_1 -f
    rm sample3_2 -f
    rm sample3_3 -f

done
