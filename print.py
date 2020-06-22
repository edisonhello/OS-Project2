import sys
import matplotlib.pyplot as plt
import os

if __name__ == "__main__":
    file_dir = os.listdir(sys.argv[1])

    times = []
    for files in file_dir:
        with open(sys.argv[1] + '/' + files, 'r') as datas:
            for line in datas:
                times.append(float(line.split(':')[1].split('ms')[0]))
    
    times = sorted(times)
    times = times[200:-200]
#    plt.title("Small fcntl-fcntl statistics")
#    plt.title("Small fcntl-mmap statistics")
#    plt.title("Small mmap-fcntl statistics")
    plt.title("Small mmap-mmap statistics")
    plt.xlabel("Time")
    plt.ylabel("Occurance")
    plt.hist(times)
    plt.show()
