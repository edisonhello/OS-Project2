mkdir -p data_$1_$2
for i in $(seq 1 10);
do
    echo -n $i
    sudo ./user_program/master 10 \
    input/sample_input_1/target_file_1 \
    input/sample_input_1/target_file_2 \
    input/sample_input_1/target_file_3 \
    input/sample_input_1/target_file_4 \
    input/sample_input_1/target_file_5 \
    input/sample_input_1/target_file_6 \
    input/sample_input_1/target_file_7 \
    input/sample_input_1/target_file_8 \
    input/sample_input_1/target_file_9 \
    input/sample_input_1/target_file_10 \
    $1  > data_$1_$2/master_out$i &

    sudo ./user_program/slave 10 \
    sample1_1 \
    sample1_2 \
    sample1_3 \
    sample1_4 \
    sample1_5 \
    sample1_6 \
    sample1_7 \
    sample1_8 \
    sample1_9 \
    sample1_10 \
    $2 127.0.0.1 > /dev/null # > data/slave_out$i 2> /dev/null
    sudo diff sample1_1  input/sample_input_1/target_file_1
    sudo diff sample1_2  input/sample_input_1/target_file_2
    sudo diff sample1_3  input/sample_input_1/target_file_3
    sudo diff sample1_4  input/sample_input_1/target_file_4
    sudo diff sample1_5  input/sample_input_1/target_file_5
    sudo diff sample1_6  input/sample_input_1/target_file_6
    sudo diff sample1_7  input/sample_input_1/target_file_7
    sudo diff sample1_8  input/sample_input_1/target_file_8
    sudo diff sample1_9  input/sample_input_1/target_file_9
    sudo diff sample1_10 input/sample_input_1/target_file_10

done

rm sample1_1 -f
rm sample1_2 -f
rm sample1_3 -f
rm sample1_4 -f
rm sample1_5 -f
rm sample1_6 -f
rm sample1_7 -f
rm sample1_8 -f
rm sample1_9 -f
rm sample1_10 -f
