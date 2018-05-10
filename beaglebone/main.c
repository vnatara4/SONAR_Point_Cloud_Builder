#include <stdio.h>
#include <stdint.h>
#include <stdio.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <termios.h>
#include "MPU9250.h"
#include "MadgwickAHRS.h"
#include <time.h>
#include <math.h>

void delay(int milliseconds);
extern volatile float q0, q1, q2, q3;	// quaternion of sensor frame relative to auxiliary frame
int i2c_file;
int main()
{
  i2c_file = open("/dev/i2c-2", O_RDWR);
  int init_status=0;
  init_status = initialize(1,1);
  if(!init_status)
  {
  perror("Unable to open i2c control file");
  	return 0;
  }
  float ax, ay, az, gx, gy, gz, mx, my, mz;

  int file;

  if ((file = open("/dev/ttyO4", O_RDWR | O_NOCTTY))<0)
  {
    perror("UART: Failed to open the file.\n");
    return -1;
  }

  struct termios options;               //The termios structure is vital
  tcgetattr(file, &options);            //Sets the parameters associated with file

  options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
  options.c_iflag = IGNPAR | ICRNL;
  options.c_oflag = 0;

  options.c_lflag=0;
  options.c_cc[VTIME]=0;
  options.c_cc[VMIN]=5;

  tcflush(file, TCIFLUSH);             //discard file information not transmitted
  tcsetattr(file, TCSANOW, &options);  //changes occur immmediately
  unsigned char receive[100];
  unsigned char f_roll[6], f_pitch[6], f_yaw[6], f_altitude[6];
  float roll = 0.0 ,pitch = 0.0 ,yaw = 0.0, altitude = 0.0;

  FILE *fp;

  fp = fopen("data.txt","w");
  for(int i=0;i<1000;i++)
  {
    read(file,(void*)receive,4);
    receive[4] = '\0';
    getMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);
    MadgwickAHRSupdate(gx, gy, gz, ax, ay, az, mx, my, mz);

    altitude = asinf(sqrt((gx*gx + gy*gy + gz*gz)));
    yaw = atan2f(2.0*(q1*q3 + q0*q0), q0*q0 - q1*q1 - q2*q2 + q3*q3);
    pitch = asinf (-2.0*(q0*q3 - q0*q2));
    roll = atan2f(2.0*(q1*q2 + q0*q3), q0*q0 + q1*q1 - q2*q2 - q3*q3);

    snprintf(f_roll,6,"%f",roll);
    snprintf(f_pitch,6,"%f",pitch);
    snprintf(f_yaw,6,"%f",yaw);
    snprintf(f_altitude,6,"%f",altitude);

    fprintf(fp, "{%s, %s, %s }, \n", f_altitude, f_yaw, receive);
    delay(25);
   }

  fclose(fp);
  close(file);
  return 0;
}

void delay(int milliseconds)
{
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
        now = clock();
}
