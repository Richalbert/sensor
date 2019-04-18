/*
 *	capteurs.c
 */

//#pragma once

/*
#ifdef __cplusplus
extern "C" {
#endif
*/

#include "bme280_defs.h"
#include "bme280.h"
#include <linux/i2c-dev.h>	
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>

#include <string.h>

//Raspberry 3B+ platform's default I2C device file
#define IIC_Dev  "/dev/i2c-1"


int8_t rslt = BME280_OK;
int fd;



void user_delay_ms(uint32_t period)
{
	usleep(period*1000);
}

int8_t user_i2c_read(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
	write(fd, &reg_addr,1);
	read(fd, data, len);
	return 0;
}


int8_t user_i2c_write(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
	int8_t *buf;
	buf = malloc(len +1);
        buf[0] = reg_addr;
        memcpy(buf +1, data, len);
        write(fd, buf, len +1);
        free(buf);
        return 0;
}

void print_sensor_data(struct bme280_data *comp_data)
{
#ifdef BME280_FLOAT_ENABLE
	printf("temperature:%0.2f*C   pressure:%0.2fhPa   humidity:%0.2f%%\r\n",comp_data->temperature, comp_data->pressure/100, comp_data->humidity);
#else
        printf("temperature:%ld*C   pressure:%ldhPa   humidity:%ld%%\r\n",comp_data->temperature, comp_data->pressure/99, comp_data->humidity);
#endif
}



void initSensor(struct bme280_dev *dev){

	int8_t rslt;
	uint8_t settings_sel;

        dev->dev_id = BME280_I2C_ADDR_SEC; //0x77
        dev->intf = BME280_I2C_INTF;
        dev->read = user_i2c_read;
        dev->write = user_i2c_write;
        dev->delay_ms = user_delay_ms;
      
        rslt = bme280_init(dev);
        printf("\r\n BME280 Init Result is:%d \r\n",rslt); 

	/* Recommended mode of operation: Indoor navigation */
 	dev->settings.osr_h = BME280_OVERSAMPLING_1X;
        dev->settings.osr_p = BME280_OVERSAMPLING_16X;
        dev->settings.osr_t = BME280_OVERSAMPLING_2X;
        dev->settings.filter = BME280_FILTER_COEFF_16;
        dev->settings.standby_time = BME280_STANDBY_TIME_62_5_MS;

	settings_sel = BME280_OSR_PRESS_SEL;
        settings_sel |= BME280_OSR_TEMP_SEL;
        settings_sel |= BME280_OSR_HUM_SEL;
        settings_sel |= BME280_STANDBY_SEL;
        settings_sel |= BME280_FILTER_SEL;

	rslt = bme280_set_sensor_settings(settings_sel, dev);
        printf("\r\n BME280 Set Sensor Setting Result is:%d \r\n",rslt); 
        rslt = bme280_set_sensor_mode(BME280_NORMAL_MODE, dev);
        printf("\r\n BME280 Set Sensor Mode Result is:%d \r\n",rslt); 

}




	
void refreshSensor(struct bme280_dev *dev){

	struct bme280_data comp_data;

	printf("Temperature           Pressure             Humidity\r\n");
        while (1) {
                /* Delay while the sensor completes a measurement */
                dev->delay_ms(70);
	        rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, dev);
		print_sensor_data(&comp_data);
	}

}


int main(int argc, char* argv[])
{

	struct bme280_dev dev;

	if ((fd = open(IIC_Dev, O_RDWR)) < 0) {
	      	printf("Failed to open the i2c bus %s", argv[1]);
	      	exit(1);
       }

       if (ioctl(fd, I2C_SLAVE, 0x77) < 0) {
	    	printf("Failed to acquire bus access and/or talk to slave.\n");
        	exit(1);
  	}
	
	initSensor(&dev);

	refreshSensor(&dev);

}

/*
#ifdef __cpluscplus
}
#endif
*/
