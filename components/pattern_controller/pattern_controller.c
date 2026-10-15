#include "pattern_controller.h"
#include "esp_log.h"
static const char *names[]={"BLACK","RED","GREEN","BLUE","WHITE","GRID V","GRID H","GRID D"};
static int active=-1;
const char *pattern_name(int index){return (index>=0&&index<PATTERN_COUNT)?names[index]:"NONE";}
int pattern_active(void){return active;}
void pattern_activate(int index){if(index<0||index>=PATTERN_COUNT)return;active=index;ESP_LOGI("PATTERN","Activated %s (Ethernet TX not implemented)",pattern_name(index));}
