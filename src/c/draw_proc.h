#pragma once

typedef struct tagCAL_INFO_T {
  struct tm nDate[2][7];
  
  int nAnniv[2][7];
  
  char szDate[2][7][3];
} CAL_INFO_T;

extern Layer *s_bg_layer; 

void bg_update_proc(Layer *layer, GContext *ctx);

void load_resource();
void unload_resource();

void batt_save_level(int nLevel);

void bt_set_connected(bool connected);

