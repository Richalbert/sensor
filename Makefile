#
# Makefile pour notre sensors.c
#
bme280:sensors.o bme280.o
	gcc -Wall -o bme280 sensors.o bme280.o -lwiringPi -std=gnu99

sensors.o: sensors.c bme280.h bme280_defs.h
	gcc -Wall -c sensors.c -lwiringPi -std=gnu99

bme280.o: bme280.c bme280.h bme280_defs.h
	gcc -Wall -c bme280.c -std=gnu99

clean:
	rm sensors.o bme280.o bme280

#EOF
