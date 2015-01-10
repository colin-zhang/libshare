#include "file.h"


#define LED_GREEN(d) "/sys/devices/platform/s3c24xx_led.1/leds/led1/"#d


void led_green_on(){
	std_write_file(LED_GREEN(brightness),"0",1);
	std_write_file(LED_GREEN(brightness),"1",1);
}
void led_green_off(){
	std_write_file(LED_GREEN(brightness),"0",1);
}

void led_green_flash(){
	std_write_file(LED_GREEN(brightness),"0",1);
	std_write_file(LED_GREEN(trigger),"heartbeat",9);
}

int main(){
	int ret =0;
	ret = std_write_file("/opt/colin/zhang/hi","hello\n",6);
	printf("ret = %d\n",ret);
	ret = std_write_file("/opt/zhang","hello\n",6);
	printf("ret = %d\n", ret);
	ret = std_write_file("hi","hello\n",6);
	printf("ret = %d\n",ret );
	//led_green_on();
	std_write_file("/sys/devices/platform/s3c24xx_led.1/leds/led1/brightness","0",1);

	return 0;
}