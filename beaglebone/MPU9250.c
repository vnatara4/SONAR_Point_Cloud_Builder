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

#include "MPU9250.h"

#define G_SI 9.80665
#define PI  3.14159

extern int i2c_file;
int testConnection()
{
    unsigned int response;
    response= set_i2c_register(i2c_file, DEVICE_ADDR,MPUREG_WHOAMI|READ_FLAG, 0x00);

    if (response == 0x68)
    {
    	printf("Connected to MPU-9250 device\n");
        return 1;
    }
    else
    {
    	printf("Connection Failed !\n");
        return 0;
    }
}

#define MPU_InitRegNum 17

int initialize(int sample_rate_div, int low_pass_filter)
{
    
    if (i2c_file < 0) 
    {
    	perror("Unable to open i2c control file");
    	return 0;
    }
    uint8_t i = 0;
    uint8_t MPU_Init_Data[MPU_InitRegNum][2] = 
    {
        //{0x80, MPUREG_PWR_MGMT_1},     // Reset Device - Disabled because it seems to corrupt initialisation of AK8963
        {0x01, MPUREG_PWR_MGMT_1},     // Clock Source
        {0x00, MPUREG_PWR_MGMT_2},     // Enable Acc & Gyro
        {low_pass_filter, MPUREG_CONFIG},         // Use DLPF set Gyroscope bandwidth 184Hz, temperature bandwidth 188Hz
        {0x18, MPUREG_GYRO_CONFIG},    // +-2000dps
        {0x08, MPUREG_ACCEL_CONFIG},   // +-4G
        {0x09, MPUREG_ACCEL_CONFIG_2}, // Set Acc Data Rates, Enable Acc LPF , Bandwidth 184Hz
        {0x30, MPUREG_INT_PIN_CFG},    //
        //{0x40, MPUREG_I2C_MST_CTRL},   // I2C Speed 348 kHz
        //{0x20, MPUREG_USER_CTRL},      // Enable AUX
        {0x20, MPUREG_USER_CTRL},       // I2C Master mode
        {0x0D, MPUREG_I2C_MST_CTRL}, //  I2C configuration multi-master  IIC 400KHz

        {AK8963_I2C_ADDR, MPUREG_I2C_SLV0_ADDR},  //Set the I2C slave addres of AK8963 and set for write.
        //{0x09, MPUREG_I2C_SLV4_CTRL},
        //{0x81, MPUREG_I2C_MST_DELAY_CTRL}, //Enable I2C delay

        {AK8963_CNTL2, MPUREG_I2C_SLV0_REG}, //I2C slave 0 register address from where to begin data transfer
        {0x01, MPUREG_I2C_SLV0_DO}, // Reset AK8963
        {0x81, MPUREG_I2C_SLV0_CTRL},  //Enable I2C and set 1 byte

        {AK8963_CNTL1, MPUREG_I2C_SLV0_REG}, //I2C slave 0 register address from where to begin data transfer
        {0x12, MPUREG_I2C_SLV0_DO}, // Register value to continuous measurement in 16bit
        {0x81, MPUREG_I2C_SLV0_CTRL},  //Enable I2C and set 1 byte
        {MPUREG_INT_PIN_CFG, 0x22}		// Enable Magnetometer by setting bypassing bit

    };
    //spi.format(8,0);
    //spi.frequency(1000000);

    for(i=0; i<MPU_InitRegNum; i++) {
        set_i2c_register(i2c_file,DEVICE_ADDR,MPU_Init_Data[i][1], MPU_Init_Data[i][0]);
        usleep(100000);  //I2C must slow down the write speed, otherwise it won't work
    }

    set_acc_scale(BITS_FS_16G);
    set_gyro_scale(BITS_FS_2000DPS);

    calib_mag();
    return 1;
}

unsigned int set_acc_scale(int scale)
{
    unsigned int temp_scale;
    set_i2c_register(i2c_file,DEVICE_ADDR,MPUREG_ACCEL_CONFIG, scale);

    switch (scale){
        case BITS_FS_2G:
            acc_divider=16384;
        break;
        case BITS_FS_4G:
            acc_divider=8192;
        break;
        case BITS_FS_8G:
            acc_divider=4096;
        break;
        case BITS_FS_16G:
            acc_divider=2048;
        break;
    }
    temp_scale= set_i2c_register(i2c_file,DEVICE_ADDR,MPUREG_ACCEL_CONFIG|READ_FLAG, 0x00);

    switch (temp_scale){
        case BITS_FS_2G:
            temp_scale=2;
        break;
        case BITS_FS_4G:
            temp_scale=4;
        break;
        case BITS_FS_8G:
            temp_scale=8;
        break;
        case BITS_FS_16G:
            temp_scale=16;
        break;
    }
    return temp_scale;
}


unsigned int set_gyro_scale(int scale)
{
    unsigned int temp_scale;
    set_i2c_register(i2c_file,DEVICE_ADDR,MPUREG_GYRO_CONFIG, scale);
    switch (scale){
        case BITS_FS_250DPS:
            gyro_divider=131;
        break;
        case BITS_FS_500DPS:
            gyro_divider=65.5;
        break;
        case BITS_FS_1000DPS:
            gyro_divider=32.8;
        break;
        case BITS_FS_2000DPS:
            gyro_divider=16.4;
        break;
    }
    temp_scale=set_i2c_register(i2c_file,DEVICE_ADDR,MPUREG_GYRO_CONFIG|READ_FLAG, 0x00);
    switch (temp_scale){
        case BITS_FS_250DPS:
            temp_scale=250;
        break;
        case BITS_FS_500DPS:
            temp_scale=500;
        break;
        case BITS_FS_1000DPS:
            temp_scale=1000;
        break;
        case BITS_FS_2000DPS:
            temp_scale=2000;
        break;
    }
    return temp_scale;
}

unsigned int whoami()
{
    unsigned int response;
    response=set_i2c_register(i2c_file,DEVICE_ADDR,MPUREG_WHOAMI|READ_FLAG, 0x00);
    return response;
}


void read_acc()
{
    uint8_t response[6];
    int16_t bit_data;
    float data;
    int i;
    get_i2c_registers(i2c_file,DEVICE_ADDR,MPUREG_ACCEL_XOUT_H,6,response);
    for(i=0; i<3; i++) {
        bit_data = ((int16_t)response[i*2] << 8) | response[i*2+1];
        data = (float)bit_data;
        accelerometer_data[i] = G_SI * data / acc_divider;
    }

}

void read_gyro()
{
    uint8_t response[6];
    int16_t bit_data;
    float data;
    int i;
    get_i2c_registers(i2c_file,DEVICE_ADDR,MPUREG_GYRO_XOUT_H,6,response);
    for(i=0; i<3; i++) {
        bit_data = ((int16_t)response[i*2] << 8) | response[i*2+1];
        data = (float)bit_data;
        gyroscope_data[i] = (PI / 180) * data / gyro_divider;
    }

}

void calib_acc()
{
    uint8_t response[4];
    int temp_scale;
    //READ CURRENT ACC SCALE
    temp_scale=set_i2c_register(i2c_file,DEVICE_ADDR,MPUREG_ACCEL_CONFIG|READ_FLAG, 0x00);
    set_acc_scale(BITS_FS_8G);
    //ENABLE SELF TEST need modify
    //temp_scale=WriteReg(MPUREG_ACCEL_CONFIG, 0x80>>axis);

    get_i2c_registers(i2c_file,DEVICE_ADDR,MPUREG_SELF_TEST_X,4,response);
    calib_data[0]=((response[0]&11100000)>>3)|((response[3]&00110000)>>4);
    calib_data[1]=((response[1]&11100000)>>3)|((response[3]&00001100)>>2);
    calib_data[2]=((response[2]&11100000)>>3)|((response[3]&00000011));

    set_acc_scale(temp_scale);
}

//-----------------------------------------------------------------------------------------------

void calib_mag(){
    uint8_t response[3];
    float data;
    int i;

    set_i2c_register(i2c_file,DEVICE_ADDR,MPUREG_I2C_SLV0_ADDR,AK8963_I2C_ADDR|READ_FLAG); //Set the I2C slave addres of AK8963 and set for read.
    set_i2c_register(i2c_file,DEVICE_ADDR,MPUREG_I2C_SLV0_REG, AK8963_ASAX); //I2C slave 0 register address from where to begin data transfer
    set_i2c_register(i2c_file,DEVICE_ADDR,MPUREG_I2C_SLV0_CTRL, 0x83); //Read 3 bytes from the magnetometer

    //WriteReg(MPUREG_I2C_SLV0_CTRL, 0x81);    //Enable I2C and set bytes
    usleep(10000);
    //response[0]=WriteReg(MPUREG_EXT_SENS_DATA_01|READ_FLAG, 0x00);    //Read I2C
    get_i2c_registers(i2c_file,DEVICE_ADDR,MPUREG_EXT_SENS_DATA_00,3,response);

    //response=WriteReg(MPUREG_I2C_SLV0_DO, 0x00);    //Read I2C
    for(i=0; i<3; i++) {
        data=response[i];
        magnetometer_ASA[i]=((data-128)/256+1)*Magnetometer_Sensitivity_Scale_Factor;
    }
}

//-----------------------------------------------------------------------------------------------

void read_mag()
{
    uint8_t response[7];
    int16_t bit_data;
    float data;
    int i;

    set_i2c_register(i2c_file,DEVICE_ADDR,MPUREG_I2C_SLV0_ADDR,AK8963_I2C_ADDR|READ_FLAG); //Set the I2C slave addres of AK8963 and set for read.
    set_i2c_register(i2c_file,DEVICE_ADDR,MPUREG_I2C_SLV0_REG, AK8963_HXL); //I2C slave 0 register address from where to begin data transfer
    set_i2c_register(i2c_file,DEVICE_ADDR,MPUREG_I2C_SLV0_CTRL, 0x87); //Read 6 bytes from the magnetometer

    usleep(10000);
    get_i2c_registers(i2c_file,DEVICE_ADDR,MPUREG_EXT_SENS_DATA_00,7,response);
    //must start your read from AK8963A register 0x03 and read seven bytes so that upon read of ST2 register 0x09 the AK8963A will unlatch the data registers for the next measurement.
    for(i=0; i<3; i++) {
        bit_data=((int16_t)response[i*2+1]<<8)|response[i*2];
        data=(float)bit_data;
        magnetometer_data[i]=data*magnetometer_ASA[i];
    }
}

void read_all()
{
    uint8_t response[21];
    int16_t bit_data;
    float data;
    int i;

    //Send I2C command at first
    set_i2c_register(i2c_file,DEVICE_ADDR,MPUREG_I2C_SLV0_ADDR,AK8963_I2C_ADDR|READ_FLAG); //Set the I2C slave addres of AK8963 and set for read.
    set_i2c_register(i2c_file,DEVICE_ADDR,MPUREG_I2C_SLV0_REG, AK8963_HXL); //I2C slave 0 register address from where to begin data transfer
    set_i2c_register(i2c_file,DEVICE_ADDR,MPUREG_I2C_SLV0_CTRL, 0x87); //Read 7 bytes from the magnetometer
    //must start your read from AK8963A register 0x03 and read seven bytes so that upon read of ST2 register 0x09 the AK8963A will unlatch the data registers for the next measurement.

    //wait(0.001);
    get_i2c_registers(i2c_file,DEVICE_ADDR,MPUREG_ACCEL_XOUT_H,21,response);
    //Get accelerometer value
    for(i=0; i<3; i++) 
    {
        bit_data = ((int16_t)response[i*2] << 8)|response[i*2+1];
        data = (float)bit_data;
        accelerometer_data[i] = G_SI * data / acc_divider;
    }
   
    //Get gyroscope value
    for(i=4; i<7; i++) {
        bit_data = ((int16_t)response[i*2] << 8) | response[i*2+1];
        data = (float)bit_data;
        gyroscope_data[i-4] = (PI / 180) * data / gyro_divider;
    }
    //Get Magnetometer value
    for(i=7; i<10; i++) {
        bit_data = ((int16_t)response[i*2+1] << 8) | response[i*2];
        data = (float)bit_data;
        magnetometer_data[i-7] = data * magnetometer_ASA[i-7];
    }
}

/*-----------------------------------------------------------------------------------------------
                                         GET VALUES
usage: call this functions to read and get values
returns accel, gyro and mag values
-----------------------------------------------------------------------------------------------*/

void getMotion9(float *ax, float *ay, float *az, float *gx, float *gy, float *gz, float *mx, float *my, float *mz)
{
    read_all();
    *ax = accelerometer_data[0];
    *ay = accelerometer_data[1];
    *az = accelerometer_data[2];
    *gx = gyroscope_data[0];
    *gy = gyroscope_data[1];
    *gz = gyroscope_data[2];
    *mx = magnetometer_data[0];
    *my = magnetometer_data[1];
    *mz = magnetometer_data[2];
}

//-----------------------------------------------------------------------------------------------

void getMotion6(float *ax, float *ay, float *az, float *gx, float *gy, float *gz)
{
    read_acc();
    read_gyro();
    *ax = accelerometer_data[0];
    *ay = accelerometer_data[1];
    *az = accelerometer_data[2];
    *gx = gyroscope_data[0];
    *gy = gyroscope_data[1];
    *gz = gyroscope_data[2];
}


