sudo ./user_program/slave 1 \
sample2_1 \
fcntl 127.0.0.1 2> /dev/null

sudo diff sample2_1 input/sample_input_2/target_file
