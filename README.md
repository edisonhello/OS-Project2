# 2020 Spring NTU CSIE - Operating System Project 2

This is the source code of operating system project 2.

`./master_device`: the device module for master server
`./slave_device`  : the device module for slave client
`./ksocket`: the device module including the functions used for kernel socket
`./user_program` : the user program "master" and "slave"

## Members

* 資工二 b07902024 塗大為
* 資工二 b07902131 陳冠宇
* 資工二 b07902132 陳威翰
* 資工二 b07902133 彭道耘
* 資工二 b07902143 陳正康

## Kernel Version
**4.14.25**

## Usage

To use it, please:

1. Change to super user
2. Execute `./compile.sh` to compile codes and install modules
3. Follow the input instructions in the spec, i.e.,

```bash
./master 1 file1_in mmap
./slave 1 file1_out fcntl 127.0.0.1
```

Make sure that you are under the path `./user_program` when you execute user programs.
Though the execution order of user program `master` and `slave` does not matter,
it is suggested to execute "master" first to get more precise transmission time.

## Demo

* Demo script: under `./demo/` directory
* Demo video: `./demo/demo.mp4`

## Reproduce the figures

To reproduce the figures in the report, please:
1. Use the scripts in `./sample` to sample to data. Pass in the transmission methods for both master and slave device as the argument to the testing scripts. 
2. Execute `print.py [sample_data_dir]` to plot.
