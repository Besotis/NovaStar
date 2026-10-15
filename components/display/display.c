#include "display.h"
#include <stdint.h>
#include <string.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define W 170
#define H 320
#define XOFF 35
#define YOFF 0
#define PIN_SCK 14
#define PIN_MOSI 15
#define PIN_DC 33
#define PIN_RST 32
#define PIN_CS 4
#define PIN_BLK 17
static spi_device_handle_t spi;
static uint8_t lines[W * 20 * 2];
static void tx(int is_data,const void *buf,size_t len){gpio_set_level(PIN_DC,is_data);spi_transaction_t t={.length=len*8,.tx_buffer=buf};ESP_ERROR_CHECK(spi_device_polling_transmit(spi,&t));}
static void command(uint8_t c){tx(0,&c,1);}
static void data(uint8_t c){tx(1,&c,1);}
static void area(int x0,int y0,int x1,int y1){x0+=XOFF;x1+=XOFF;y0+=YOFF;y1+=YOFF;command(0x2A);data(x0>>8);data(x0);data(x1>>8);data(x1);command(0x2B);data(y0>>8);data(y0);data(y1>>8);data(y1);command(0x2C);}
static void flush(lv_display_t *disp,const lv_area_t *a,uint8_t *px){
  int w=a->x2-a->x1+1,h=a->y2-a->y1+1;
  area(a->x1,a->y1,a->x2,a->y2);
  // LVGL RGB565 stores bytes in little-endian; ST7789 expects high byte first.
  for(int y=0;y<h;){int n=h-y;if(n>20)n=20;
    for(int i=0;i<w*n;i++){lines[i*2]=px[(y*w+i)*2+1];lines[i*2+1]=px[(y*w+i)*2];}
    tx(1,lines,w*n*2);y+=n;
  }
  lv_display_flush_ready(disp);
}
void display_init(void){
 gpio_config_t g={.pin_bit_mask=(1ULL<<PIN_DC)|(1ULL<<PIN_RST)|(1ULL<<PIN_BLK),.mode=GPIO_MODE_OUTPUT};ESP_ERROR_CHECK(gpio_config(&g));
 gpio_set_level(PIN_BLK,0);
 spi_bus_config_t bus={.mosi_io_num=PIN_MOSI,.miso_io_num=-1,.sclk_io_num=PIN_SCK,.quadwp_io_num=-1,.quadhd_io_num=-1,.max_transfer_sz=sizeof(lines)};
 ESP_ERROR_CHECK(spi_bus_initialize(SPI3_HOST,&bus,SPI_DMA_CH_AUTO));
 spi_device_interface_config_t dev={.clock_speed_hz=20*1000*1000,.mode=0,.spics_io_num=PIN_CS,.queue_size=1};ESP_ERROR_CHECK(spi_bus_add_device(SPI3_HOST,&dev,&spi));
 gpio_set_level(PIN_RST,0);vTaskDelay(pdMS_TO_TICKS(30));gpio_set_level(PIN_RST,1);vTaskDelay(pdMS_TO_TICKS(120));
 command(0x01);vTaskDelay(pdMS_TO_TICKS(150));command(0x11);vTaskDelay(pdMS_TO_TICKS(120));command(0x3A);data(0x55);command(0x36);data(0x00);command(0x21);command(0x29);vTaskDelay(pdMS_TO_TICKS(20));
 static uint8_t drawbuf[W*20*2];
 lv_display_t *d=lv_display_create(W,H);lv_display_set_color_format(d,LV_COLOR_FORMAT_RGB565);lv_display_set_buffers(d,drawbuf,NULL,sizeof(drawbuf),LV_DISPLAY_RENDER_MODE_PARTIAL);lv_display_set_flush_cb(d,flush);
 gpio_set_level(PIN_BLK,1);
}
