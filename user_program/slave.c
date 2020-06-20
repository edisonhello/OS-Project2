// vim: ts=2:sw=2:sts=2:
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
#define min(x, y) (x < y ? x : y)

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
      -1) {  // 0x12345677 : connect to master in the device
    perror("ioclt create slave socket error\n");
    return 1;
  }

  write(1, "ioctl success\n", 14);
  printf("num_file = %d\n", num_file);

  for (int i = 0; i < num_file; ++i) {
    int fd = open(argv[i + 2], O_RDWR | O_CREAT | O_TRUNC);
    int num_byte = 0, file_size = 0;
    struct timeval start;
    struct timeval end;
    gettimeofday(&start, NULL);

    switch (method[0]) {
      case 'f': {
        assert(read(dev_fd, (void *)&num_byte, 4) == 4);
        printf("num_byte = %d\n", num_byte);
        while (file_size < num_byte) {
          int to_read = min(num_byte - file_size, BUF_SIZE);
          assert(read(dev_fd, buf, to_read) == to_read);
          assert(write(fd, buf, to_read) == to_read);
          file_size += to_read;
        }
        break;
      }
      case 'm': {
        printf("here\n");
        void *ptr = mmap(NULL, PAGE_SIZE, PROT_READ, MAP_SHARED, dev_fd, 0);
        if (ptr == MAP_FAILED) {
          perror("mmap");
        }
        printf("after mmap\n");
        ioctl(dev_fd, 0x12345678, 4);
        memcpy(&num_byte, (const void *)ptr, 4);
        printf("num_byte = %d\n", num_byte);
        lseek(fd, num_byte - 1, SEEK_SET);
        write(fd, "", 1);
        void *fptr = mmap(NULL, num_byte, PROT_WRITE, 0, fd, 0);
        while (file_size < num_byte) {
          ptr = mmap(NULL, PAGE_SIZE, PROT_READ, 0, dev_fd, 0);
          int to_write = min(num_byte - file_size, PAGE_SIZE);
          ioctl(dev_fd, 0x12345678, to_write);
          memcpy(fptr, (const void *)ptr, to_write);
          fptr += to_write;
          file_size += to_write;
        }
        munmap(ptr, PAGE_SIZE);
        munmap(fptr, num_byte);
        break;
      }
    }
    gettimeofday(&end, NULL);
    double trans_time = (end.tv_sec - start.tv_sec) * 1000 +
                        (end.tv_usec - start.tv_usec) * 0.0001;
    printf("Transmission time: %lf ms, File size: %d bytes\n", trans_time,
           file_size / 8);
    close(fd);
  }

  if (ioctl(dev_fd, 0x12345679) ==
      -1) {  // end receiving data, close the connection
    perror("ioclt client exits error\n");
    return 1;
  }

  close(dev_fd);
  return 0;
}
