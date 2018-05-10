CC = gcc
CFLAGS = -c -Wall -march=armv7-a -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a8 -mtune=cortex-a8 -O3 -ggdb
# Generic rule for compiling any C code
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

main: main.o MadgwickAHRS.o IMU.o MPU9250.o
	$(CC) main.o MadgwickAHRS.o IMU.o MPU9250.o -lrt -lm -static -o $@

clean:
	rm -f *.o main *.s
	
	
	
	

