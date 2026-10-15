#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lvgl.h"
#include "display.h"
#include "encoder.h"
#include "menu_controller.h"
#include "ui.h"
void app_main(void){
 lv_init();display_init();encoder_init();ui_init();menu_init();
 ESP_LOGI("NOVASTAR","SquareLine UI ready. BLACK selected. Ethernet disabled.");
 while(1){
   int step=encoder_read_step();if(step)menu_rotate(step);
   if(encoder_pressed())menu_press();
   lv_tick_inc(2);lv_timer_handler();
   vTaskDelay(pdMS_TO_TICKS(2));
 }
}
