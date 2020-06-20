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

size_t get_filesize(const char* filename);  // get the size of the input file

void PrintUsage() {
  fprintf(stderr, "[Usage] ./master N file1 file2 ... fileN fcntl/mmap\n");
  exit(1);
}

int main(int argc, char* argv[]) {
  char buf[BUF_SIZE];
  char file_name[50], method[20];
  char *kernel_address = NULL, *file_address = NULL;
  if (argc == 1) PrintUsage();
  int num_file = atoi(argv[1]);
  if (argc != num_file + 3) PrintUsage();
  strcpy(method, argv[num_file + 2]);

  int dev_fd = open("/dev/master_device", O_RDWR);
  if (dev_fd < 0) {
    perror("failed to open /dev/master_device\n");
    return 1;
  }

  if (ioctl(dev_fd, 0x12345677) == -1) { // 0x12345677 : create socket and accept
                                        // the connection from the slave
    perror("ioctl server create socket error\n");
    return 1;
  }

  for (int i = 0; i < num_file; ++i) {
    int fd = open(argv[i + 2], O_RDWR);
    if (fd < 0) {
      perror("failed to open input file\n");
      return 1;
    }
    int file_size = get_filesize(argv[i + 2]);
    if (file_size < 0) {
      perror("failed to get filesize\n");
      return 1;
    }
    printf("file_size = %d\n", file_size);
    struct timeval start;
    struct timeval end;
    gettimeofday(&start, NULL);

		int cur = 0;
		void *dfile, *ofile;
    switch (method[0]) {
      case 'f':  // fcntl : read()/write()
        assert(write(dev_fd, (const void*)&file_size, 4) == 4);
        int num_byte = 0;
        while (num_byte < file_size) {
          int to_write = min(file_size - num_byte, BUF_SIZE);
          assert(read(fd, buf, to_write) == to_write);
          assert(write(dev_fd, buf, to_write) ==
                 to_write);  // write to the the device
          num_byte += to_write;
        }
        break;
      case 'm':
				// write file size
				dfile = mmap(NULL, PAGE_SIZE, PROT_WRITE, MAP_SHARED, dev_fd, 0);
				if (dfile == (void *)-1) {
					perror("mapping dev error");
					return 1;
				}
				memcpy(dfile, (const void*)&file_size, 4);
				ioctl(dev_fd, 0x12345678, 4);
				munmap(dfile, 4);

				while (cur < file_size) {
					int len = cur + PAGE_SIZE > file_size ? file_size - cur : PAGE_SIZE;
					ofile = mmap(NULL, PAGE_SIZE, PROT_READ, 0, fd, cur);
					dfile = mmap(NULL, PAGE_SIZE, PROT_WRITE, MAP_SHARED, dev_fd, 0);
					if (ofile == (void *)-1) {
						perror("mapping file error\n");
						return 1;
					}
					if (dfile == (void *)-1) {
						perror("mapping dev error\n");
						return 1;
					}
					
					int cur2 = 0;
					while (len) {
						int len2 = cur2 + BUF_SIZE > len ? len - cur2 : BUF_SIZE;
						memcpy(dfile, ofile, len2);
						ioctl(dev_fd, 0x12345678, ((unsigned long)cur2 << 32) | len2);
						cur2 += len2;
					}

					cur += len;
					munmap(ofile, len);
					munmap(dfile, len);
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
      -1)  // end sending data, close the connection
  {
    perror("ioclt server exits error\n");
    return 1;
  }

  close(dev_fd);
  return 0;
}

size_t get_filesize(const char* filename) {
  struct stat st;
  stat(filename, &st);
  return st.st_size;
}
