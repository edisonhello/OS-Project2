#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define PAGE_SIZE 4096
#define BUF_SIZE 512

void PrintUsage() {
  fprintf(stderr, "[Usage] ./slave N file1 file2 ... fileN fcntl/mmap ip\n");
  exit(1);
}

int main(int argc, char *argv[]) {
  char buf[BUF_SIZE];
  char file_name[50];
  char method[20];
  char ip[20];
  char *kernel_address, *file_address;

  if (argc == 1) PrintUsage();
  int num_file = atoi(argv[1]);
  if (argc != num_file + 4) PrintUsage();
  strcpy(method, argv[num_file + 2]);
  strcpy(ip, argv[num_file + 3]);

  int dev_fd = open("/dev/slave_device", O_RDWR);
  if (dev_fd < 0) {  // should be O_RDWR for PROT_WRITE when mmap()
    perror("failed to open /dev/slave_device\n");
    return 1;
  }
  if (ioctl(dev_fd, 0x12345677, ip) ==
      -1)  // 0x12345677 : connect to master in the device
  {
    perror("ioclt create slave socket error\n");
    return 1;
  }

  write(1, "ioctl success\n", 14);

  for (int i = 0; i < num_file; ++i) {
    int fd = open(argv[i + 2], O_RDWR | O_CREAT | O_TRUNC);
    struct timeval start;
    struct timeval end;
    gettimeofday(&start, NULL);

    int file_size = 0;
    switch (method[0]) {
      case 'f':  // fcntl : read()/write()
        assert(read(dev_fd, buf, 4) == 4);
        int num_byte = *(int *)(buf);
        while (num_byte) {
          int to_read = num_byte < BUF_SIZE ? num_byte : BUF_SIZE;
          assert(read(dev_fd, buf, to_read) == to_read);
          write(fd, buf, to_read);
          file_size += to_read;
        }
        break;
    }
    gettimeofday(&end, NULL);
    double trans_time = (end.tv_sec - start.tv_sec) * 1000 +
                        (end.tv_usec - start.tv_usec) * 0.0001;
    printf("Transmission time: %lf ms, File size: %d bytes\n", trans_time,
           file_size / 8);
    close(fd);
  }

  if (ioctl(dev_fd, 0x12345679) ==
      -1)  // end receiving data, close the connection
  {
    perror("ioclt client exits error\n");
    return 1;
  }

  close(dev_fd);
  return 0;
}

