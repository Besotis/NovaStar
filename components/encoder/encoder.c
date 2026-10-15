#include "encoder.h"
#include "driver/gpio.h"
#include <stdint.h>
#include "esp_check.h"
#define CLK GPIO_NUM_36
#define DT GPIO_NUM_39
#define SW GPIO_NUM_35
static int prev,accum,sw_last=1,sw_stable=1,sw_count;
void encoder_init(void){gpio_config_t g={.pin_bit_mask=(1ULL<<CLK)|(1ULL<<DT)|(1ULL<<SW),.mode=GPIO_MODE_INPUT,.pull_up_en=GPIO_PULLUP_DISABLE,.pull_down_en=GPIO_PULLDOWN_DISABLE};ESP_ERROR_CHECK(gpio_config(&g));prev=(gpio_get_level(CLK)<<1)|gpio_get_level(DT);}
int encoder_read_step(void){static const int8_t delta[16]={0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};int now=(gpio_get_level(CLK)<<1)|gpio_get_level(DT);accum+=delta[(prev<<2)|now];prev=now;if(accum>=4){accum=0;return 1;}if(accum<=-4){accum=0;return -1;}return 0;}
int encoder_pressed(void){int sw=gpio_get_level(SW);if(sw==sw_last){if(sw_count<5)sw_count++;}else{sw_count=0;sw_last=sw;}if(sw_count==5&&sw!=sw_stable){sw_stable=sw;return sw==0;}return 0;}
