sudo ./user_program/slave 3 \
sample3_1 \
sample3_2 \
sample3_3 \
mmap 127.0.0.1 2> /dev/null

sudo diff sample3_1  input/input_3/target_file_v3_1
sudo diff sample3_2  input/input_3/target_file_v3_2
sudo diff sample3_3  input/input_3/target_file_v3_3
