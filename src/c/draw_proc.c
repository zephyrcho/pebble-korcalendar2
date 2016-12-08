#include <pebble.h>
#include "draw_proc.h"
#include "calutil.h"

//#define TEST_VIEW

CAL_INFO_T sCalInfo;

static bool bColor = true;

static GBitmap *s_bitmap_mday[7];

static bool s_bt_connected;
static GBitmap *s_bitmap_bt_ok;
static GBitmap *s_bitmap_bt_fail;
static GBitmap *s_bitmap_bt_fail_BW;

static GBitmap *s_bitmap_holi[15];

static int s_battery_level;
static char szBattery[6];
static GFont fontBatt;
static GBitmap *s_bitmap_batt;
static GBitmap *s_bitmap_batt_low;
static GBitmap *s_bitmap_batt_low2;

static char szTime[10];
static GFont fontTime1;
static GFont fontTime2;
static GFont fontTime3;
static GFont fontTime4;
static GFont fontTime5;

static char szmday[7][3];
static char szDate[10];
static GFont fontDate;
static GFont fontWDay;
static GFont fontMDay;
static GFont fontMDayB;
static GFont fontToday;

void bg_update_proc(Layer *layer, GContext *ctx) {
  GRect layer_bounds = layer_get_bounds(layer);
  
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  
  time_t now = time(NULL);
  #ifdef TEST_VIEW
  static int test_day = 0;
  now += test_day * 86400;
  test_day++;
  #endif
  struct tm *t = localtime(&now);
// for test
//  t->tm_year += 1;
//  t->tm_mon = 9;
//  t->tm_mday = 3;
//  t->tm_wday = 2;
//  t->tm_hour = 22;
  
#if defined(PBL_ROUND)
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Layer bounds w : %d , h : %d", layer_bounds.size.w, layer_bounds.size.h);
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Draw Bluetooth connect");
  {
//    s_bt_connected = false;
    
    if (s_bt_connected == false || s_battery_level <= 20) {
      graphics_context_set_fill_color(ctx, GColorRed);
      graphics_fill_rect(ctx, GRect(0, 0, 180, 28), 0, GCornerNone);
      
      if (s_bt_connected == false && s_battery_level <= 20) {
        graphics_draw_bitmap_in_rect(ctx, s_bitmap_bt_fail, GRect(52, 8, 18, 18));  

        snprintf(szBattery, sizeof(szBattery), "%d%%", s_battery_level);
  
        GSize text_size = graphics_text_layout_get_content_size(szBattery, fontBatt, layer_bounds,
                                                                GTextOverflowModeWordWrap, GTextAlignmentCenter);
        GRect bounds_batt = GRect(74, 4, text_size.w, text_size.h);
        
        graphics_context_set_text_color(ctx, GColorBlack);
        graphics_context_set_fill_color(ctx, GColorDarkGray);
        graphics_draw_text(ctx, szBattery, fontBatt, bounds_batt, GTextOverflowModeWordWrap, GTextAlignmentRight, NULL); 
      
        graphics_draw_bitmap_in_rect(ctx, s_bitmap_batt_low, GRect(104, 7, 25, 18));
        
        // Draw the bar
        graphics_fill_rect(ctx, GRect(108, 11, 15 * s_battery_level / 100, 10), 0, GCornerNone);
      } else if (s_bt_connected == false) {
        graphics_draw_bitmap_in_rect(ctx, s_bitmap_bt_fail, GRect(80, 8, 18, 18));  
      } else {
        snprintf(szBattery, sizeof(szBattery), "%d%%", s_battery_level);
  
        GSize text_size = graphics_text_layout_get_content_size(szBattery, fontBatt, layer_bounds,
                                                                GTextOverflowModeWordWrap, GTextAlignmentCenter);
        GRect bounds_batt = GRect(63, 4, text_size.w, text_size.h);
        
        graphics_context_set_text_color(ctx, GColorBlack);
        graphics_context_set_fill_color(ctx, GColorDarkGray);
        graphics_draw_text(ctx, szBattery, fontBatt, bounds_batt, GTextOverflowModeWordWrap, GTextAlignmentRight, NULL); 
      
        graphics_draw_bitmap_in_rect(ctx, s_bitmap_batt_low, GRect(93, 7, 25, 18));        
        
        // Draw the bar
        graphics_fill_rect(ctx, GRect(97, 11, 15 * s_battery_level / 100, 10), 0, GCornerNone);
      }
    } else {
      GRect bounds_batt;
      snprintf(szBattery, sizeof(szBattery), "%d%%", s_battery_level);

      GSize text_size = graphics_text_layout_get_content_size(szBattery, fontBatt, layer_bounds,
                                                              GTextOverflowModeWordWrap, GTextAlignmentCenter);
      if (s_battery_level == 100) {
        bounds_batt = GRect(57, 4, text_size.w, text_size.h);
        
        graphics_context_set_text_color(ctx, GColorWhite);
        graphics_context_set_fill_color(ctx, GColorLightGray);
        graphics_draw_text(ctx, szBattery, fontBatt, bounds_batt, GTextOverflowModeWordWrap, GTextAlignmentRight, NULL); 
  
        graphics_draw_bitmap_in_rect(ctx, s_bitmap_batt, GRect(95, 7, 25, 18));        
  
        // Draw the bar
        graphics_fill_rect(ctx, GRect(99, 11, 15 * s_battery_level / 100, 10), 0, GCornerNone);      
      }
      else {
        bounds_batt = GRect(63, 4, text_size.w, text_size.h);

        graphics_context_set_text_color(ctx, GColorWhite);
        graphics_context_set_fill_color(ctx, GColorLightGray);
        graphics_draw_text(ctx, szBattery, fontBatt, bounds_batt, GTextOverflowModeWordWrap, GTextAlignmentRight, NULL); 
  
        graphics_draw_bitmap_in_rect(ctx, s_bitmap_batt, GRect(93, 7, 25, 18));        
  
        // Draw the bar
        graphics_fill_rect(ctx, GRect(97, 11, 15 * s_battery_level / 100, 10), 0, GCornerNone);      
      }
    }
  }
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Draw Time %d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
  {
    int hour;
  
    #ifdef TEST_VIEW
    if (clock_is_24h_style() == false) {
      hour = t->tm_min % 12;
      if (hour == 0) hour = 12;
      snprintf(szTime, sizeof(szTime), "%d:%02d", hour, t->tm_sec);
    } else {
      hour = t->tm_min % 24;
      snprintf(szTime, sizeof(szTime), "%d:%02d", hour, t->tm_sec);
    }
    #else
    if (clock_is_24h_style() == false) {
      hour = t->tm_hour % 12;
      if (hour == 0) hour = 12;
      snprintf(szTime, sizeof(szTime), "%d:%02d", hour, t->tm_min);
    } else {
      snprintf(szTime, sizeof(szTime), "%d:%02d", t->tm_hour, t->tm_min);
    }
    #endif
    
// for Test
//    snprintf(szTime, sizeof(szTime), "20:00");
//    snprintf(szTime, sizeof(szTime), "%d:%02d", t->tm_min, t->tm_sec);

    GSize text_size = graphics_text_layout_get_content_size(szTime, fontTime1, GRect(0, 0, 300, 300),
                              GTextOverflowModeWordWrap, GTextAlignmentCenter);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Time Size1 %d %d", text_size.w, text_size.h);
    
    if (text_size.w < 144) {
      GRect bounds_time = GRect(0, 22, 180, text_size.h);
    
      graphics_context_set_text_color(ctx, GColorWhite);
      graphics_draw_text(ctx, szTime, fontTime1, bounds_time, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    } else {
      text_size = graphics_text_layout_get_content_size(szTime, fontTime2, GRect(0, 0, 300, 300),
                                GTextOverflowModeWordWrap, GTextAlignmentCenter);
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Time Size2 %d %d", text_size.w, layer_bounds.size.w);
      if (text_size.w < 144) {
        GRect bounds_time = GRect(0, 24, 180, text_size.h);
      
        graphics_context_set_text_color(ctx, GColorWhite);
        graphics_draw_text(ctx, szTime, fontTime2, bounds_time, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      } else {
        text_size = graphics_text_layout_get_content_size(szTime, fontTime3, GRect(0, 0, 300, 300),
                                  GTextOverflowModeWordWrap, GTextAlignmentCenter);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Time Size3 %d %d", text_size.w, layer_bounds.size.w);
        if (text_size.w < 144) {
          GRect bounds_time = GRect(0, 26, 180, text_size.h);
        
          graphics_context_set_text_color(ctx, GColorWhite);
          graphics_draw_text(ctx, szTime, fontTime3, bounds_time, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
        } else {
          text_size = graphics_text_layout_get_content_size(szTime, fontTime4, GRect(0, 0, 300, 300),
                                    GTextOverflowModeWordWrap, GTextAlignmentCenter);
          APP_LOG(APP_LOG_LEVEL_DEBUG, "Time Size4 %d %d", text_size.w, layer_bounds.size.w);
          if (text_size.w < 144) {
            GRect bounds_time = GRect(0, 28, 180, text_size.h);
            
            graphics_context_set_text_color(ctx, GColorWhite);
            graphics_draw_text(ctx, szTime, fontTime4, bounds_time, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
          } else {
    			  text_size = graphics_text_layout_get_content_size(szTime, fontTime5, GRect(0, 0, 300, 300),
    				  GTextOverflowModeWordWrap, GTextAlignmentCenter);
    			  APP_LOG(APP_LOG_LEVEL_DEBUG, "Time Size5 %d %d", text_size.w, layer_bounds.size.w);
    
    			  GRect bounds_time = GRect(0, 30, 180, text_size.h);
    			  graphics_context_set_text_color(ctx, GColorWhite);
    			  graphics_draw_text(ctx, szTime, fontTime5, bounds_time, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
          }        
        }
      }        
    }
  }
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Draw Calendar %d.%d", t->tm_mon, t->tm_mday);
  {
    int i, j, totalWidth, holiday_today;
    
    graphics_context_set_stroke_color(ctx, GColorLightGray);
    graphics_draw_line(ctx, GPoint(10, 110), GPoint(170, 110));    
    
    snprintf(szDate, sizeof(szDate), "%d.%02d", t->tm_mon + 1, t->tm_mday);
    graphics_context_set_text_color(ctx, GColorWhite);
    
    GSize text_size = graphics_text_layout_get_content_size(szDate, fontDate, layer_bounds,
                              GTextOverflowModeWordWrap, GTextAlignmentCenter);
    
    holiday_today = sIsHoliday(t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    
    
    if (holiday_today) {
      GRect bitmap_size = gbitmap_get_bounds(s_bitmap_holi[holiday_today - 1]);
    
      totalWidth = text_size.w + 2 + 18 + 2 + bitmap_size.size.w;
      
      GRect bounds_date = GRect((180 - totalWidth) / 2, 86, text_size.w, text_size.h);

      graphics_draw_text(ctx, szDate, fontDate, bounds_date, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      graphics_draw_bitmap_in_rect(ctx, s_bitmap_mday[t->tm_wday], 
                                 GRect((180 - totalWidth + 2) / 2 + text_size.w + 2, 86 + text_size.h - 12, 18, 12));
      graphics_draw_bitmap_in_rect(ctx, s_bitmap_holi[holiday_today - 1], 
                                 GRect((180 - totalWidth + 2) / 2 + text_size.w + 2 + 18 + 2, 86 + text_size.h - 12, bitmap_size.size.w, bitmap_size.size.h));
    } else {
      totalWidth = text_size.w + 2 + 18;
      
      GRect bounds_date = GRect((180 - totalWidth) / 2, 86, text_size.w, text_size.h);
      
      graphics_draw_text(ctx, szDate, fontDate, bounds_date, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      graphics_draw_bitmap_in_rect(ctx, s_bitmap_mday[t->tm_wday], 
                                 GRect((180 - totalWidth) / 2 + text_size.w + 2, 86 + text_size.h - 12, 18, 12));
    }

    /*
    graphics_context_set_text_color(ctx, GColorWhite);
    for (i = 0; i < 7; i++) {
      GSize text_size = graphics_text_layout_get_content_size(szmday[i], fontWDay, layer_bounds,
                                GTextOverflowModeWordWrap, GTextAlignmentCenter);
      GRect bounds_wday = GRect(18 + 2 + i*20 + (10 - text_size.w / 2), 90 + 36 - text_size.h,
                             text_size.w, text_size.h);
      graphics_draw_text(ctx, szmday[i], fontWDay, bounds_wday, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);      
    }
    */
    
    graphics_context_set_fill_color(ctx, GColorYellow);
    graphics_fill_circle(ctx, GPoint(18 + 11 + 20*t->tm_wday, 90 + 34), 10);
    
    if (t->tm_mday != sCalInfo.nDate[0][t->tm_wday].tm_mday ||
        t->tm_mon != sCalInfo.nDate[0][t->tm_wday].tm_mon) {
      // make calendar
      sCalInfo.nDate[0][t->tm_wday].tm_mday = t->tm_mday;
      sCalInfo.nDate[0][t->tm_wday].tm_mon = t->tm_mon + 1;
      sCalInfo.nDate[0][t->tm_wday].tm_year = t->tm_year + 1900;
      
      for (i = 0; i < t->tm_wday; i++) {
        sGetCalcDate(sCalInfo.nDate[0][t->tm_wday], (-1 - i), &(sCalInfo.nDate[0][t->tm_wday - 1 - i]));
      }
          
      for (i = 0; i < 7 - t->tm_wday - 1; i++) {
        sGetCalcDate(sCalInfo.nDate[0][t->tm_wday], (1 + i), &(sCalInfo.nDate[0][t->tm_wday + 1 + i]));
      }
        
      for (i = 0; i < 7; i++) {
          sGetCalcDate(sCalInfo.nDate[0][i], 7 , &(sCalInfo.nDate[1][i]));
      }
    
      for (i = 0; i < 2; i++) {
        for (j = 0; j < 7; j++) {
            sCalInfo.nAnniv[i][j] = sIsHoliday(sCalInfo.nDate[i][j].tm_year, sCalInfo.nDate[i][j].tm_mon, sCalInfo.nDate[i][j].tm_mday);
        }
      }
    }
    
    for (i = 0; i < 7; i++) {
      if (i == 0 || sCalInfo.nAnniv[0][i] != 0) {
        graphics_context_set_text_color(ctx, GColorRed);
      } else {
        if (i == t->tm_wday)
          graphics_context_set_text_color(ctx, GColorBlack);
        else if (i != 6)
          graphics_context_set_text_color(ctx, GColorWhite);
        else
          graphics_context_set_text_color(ctx, GColorBlue);
      }      
      snprintf(sCalInfo.szDate[0][i], 3, "%d", sCalInfo.nDate[0][i].tm_mday);
      if (sCalInfo.nDate[0][i].tm_mday == t->tm_mday) {
        GSize cal_text_size1 = graphics_text_layout_get_content_size(sCalInfo.szDate[0][i], fontToday, layer_bounds,
                                  GTextOverflowModeWordWrap, GTextAlignmentCenter);
        GRect cal_bounds1 = GRect(18 + 2 + i*20 + (10 - cal_text_size1.w / 2), 90 + 40 - cal_text_size1.h,
                               cal_text_size1.w, cal_text_size1.h);
        graphics_draw_text(ctx, sCalInfo.szDate[0][i], fontToday, cal_bounds1, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);      
      } else {
        GSize cal_text_size1 = graphics_text_layout_get_content_size(sCalInfo.szDate[0][i], fontMDay, layer_bounds,
                                                                     GTextOverflowModeWordWrap, GTextAlignmentCenter);
        GRect cal_bounds1 = GRect(18 + 2 + i*20 + (10 - cal_text_size1.w / 2), 90 + 40 - cal_text_size1.h,
                                  cal_text_size1.w, cal_text_size1.h);
        graphics_draw_text(ctx, sCalInfo.szDate[0][i], fontMDay, cal_bounds1, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);      
      }

      if (i == 0 || sCalInfo.nAnniv[1][i] != 0) {
        graphics_context_set_text_color(ctx, GColorRed);
      } else {
        if (i != 6)
          graphics_context_set_text_color(ctx, GColorWhite);
        else
          graphics_context_set_text_color(ctx, GColorBlue);
      }      
      snprintf(sCalInfo.szDate[1][i], 3, "%d", sCalInfo.nDate[1][i].tm_mday);

      GSize cal_text_size1 = graphics_text_layout_get_content_size(sCalInfo.szDate[1][i], fontMDay, layer_bounds,
                                                                     GTextOverflowModeWordWrap, GTextAlignmentCenter);
      GRect cal_bounds1 = GRect(18 + 2 + i*20 + (10 - cal_text_size1.w / 2), 90 + 57 - cal_text_size1.h,
                                  cal_text_size1.w, cal_text_size1.h);
      graphics_draw_text(ctx, sCalInfo.szDate[1][i], fontMDay, cal_bounds1, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);      
    }
  }
  
#else
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Draw Bluetooth connect");
  {
    if (s_bt_connected) {
      graphics_draw_bitmap_in_rect(ctx, s_bitmap_bt_ok, 
                                   GRect(0, 0, 18, 18));       
    } else {
      if (bColor) 
        graphics_draw_bitmap_in_rect(ctx, s_bitmap_bt_fail, GRect(0, 0, 18, 18));       
      else
        graphics_draw_bitmap_in_rect(ctx, s_bitmap_bt_fail_BW, GRect(0, 0, 18, 18));       
    }
  }
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Draw Batt Level %d", s_battery_level);
  {
    snprintf(szBattery, sizeof(szBattery), "%d%%", s_battery_level);
  
    GSize text_size = graphics_text_layout_get_content_size(szBattery, fontBatt, layer_bounds,
                              GTextOverflowModeWordWrap, GTextAlignmentCenter);

    GRect bounds_batt = GRect(144 - text_size.w - 25, (18 - text_size.h) - 4,
                              text_size.w, text_size.h);

    if (s_battery_level <= 20) {
      if (bColor) {
        graphics_context_set_fill_color(ctx, GColorRed);
        graphics_fill_rect(ctx, GRect(144 - text_size.w - 25 - 2, 0, text_size.w +100, 18), 0, GCornerNone);
        
        graphics_context_set_text_color(ctx, GColorBlack);
        graphics_context_set_fill_color(ctx, GColorDarkGray);
        graphics_draw_text(ctx, szBattery, fontBatt, bounds_batt, GTextOverflowModeWordWrap, GTextAlignmentRight, NULL); 
      
        graphics_draw_bitmap_in_rect(ctx, s_bitmap_batt_low, GRect((144 - 25), 0, 25, 18));
      } else {
        graphics_context_set_fill_color(ctx, GColorWhite);
        graphics_fill_rect(ctx, GRect(144 - text_size.w - 25 - 2, 0, text_size.w +100, 18), 0, GCornerNone);
        
        graphics_context_set_text_color(ctx, GColorBlack);
        graphics_context_set_fill_color(ctx, GColorDarkGray);
        graphics_draw_text(ctx, szBattery, fontBatt, bounds_batt, GTextOverflowModeWordWrap, GTextAlignmentRight, NULL); 
      
        graphics_draw_bitmap_in_rect(ctx, s_bitmap_batt_low2, GRect((144 - 25), 0, 25, 18));        
      }
    } else {
      graphics_context_set_text_color(ctx, GColorLightGray);      
      graphics_context_set_fill_color(ctx, GColorLightGray);
      graphics_draw_text(ctx, szBattery, fontBatt, bounds_batt, GTextOverflowModeWordWrap, GTextAlignmentRight, NULL); 
    
      graphics_draw_bitmap_in_rect(ctx, s_bitmap_batt, GRect((144 - 25), 0, 25, 18));
    }
    
    // Draw the bar
    graphics_fill_rect(ctx, GRect(144 - 18 + 2 - 5, 4, 15 * s_battery_level / 100, 10), 0, GCornerNone);
  }
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Draw Time %d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
  {
    int hour;
    
    if (clock_is_24h_style() == false) {
      hour = t->tm_hour % 12;
      if (hour == 0) hour = 12;
      snprintf(szTime, sizeof(szTime), "%d:%02d", hour, t->tm_min);
    } else {
      snprintf(szTime, sizeof(szTime), "%d:%02d", t->tm_hour, t->tm_min);
    }
// for Test
//    snprintf(szTime, sizeof(szTime), "20:00");
//    snprintf(szTime, sizeof(szTime), "%d:%02d", t->tm_min, t->tm_sec);

    GSize text_size = graphics_text_layout_get_content_size(szTime, fontTime1, GRect(0, 0, 200, 200),
                              GTextOverflowModeWordWrap, GTextAlignmentCenter);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Time Size1 %d %d", text_size.w, layer_bounds.size.w);
    
    if (text_size.w < 144) {
      GRect bounds_time = GRect(0, 16, 144, text_size.h);
    
      graphics_context_set_text_color(ctx, GColorWhite);
      graphics_draw_text(ctx, szTime, fontTime1, bounds_time, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    } else {
      text_size = graphics_text_layout_get_content_size(szTime, fontTime2, GRect(0, 0, 200, 200),
                                GTextOverflowModeWordWrap, GTextAlignmentCenter);
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Time Size2 %d %d", text_size.w, layer_bounds.size.w);
      if (text_size.w < 144) {
        GRect bounds_time = GRect(0, 18, 144, text_size.h);
      
        graphics_context_set_text_color(ctx, GColorWhite);
        graphics_draw_text(ctx, szTime, fontTime2, bounds_time, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      } else {
        text_size = graphics_text_layout_get_content_size(szTime, fontTime3, GRect(0, 0, 200, 200),
                                  GTextOverflowModeWordWrap, GTextAlignmentCenter);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Time Size3 %d %d", text_size.w, layer_bounds.size.w);
        if (text_size.w < 144) {
          GRect bounds_time = GRect(0, 20, 144, text_size.h);
        
          graphics_context_set_text_color(ctx, GColorWhite);
          graphics_draw_text(ctx, szTime, fontTime3, bounds_time, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
        } else {
          text_size = graphics_text_layout_get_content_size(szTime, fontTime4, GRect(0, 0, 200, 200),
                                    GTextOverflowModeWordWrap, GTextAlignmentCenter);
          APP_LOG(APP_LOG_LEVEL_DEBUG, "Time Size4 %d %d", text_size.w, layer_bounds.size.w);
          if (text_size.w < 144) {
            GRect bounds_time = GRect(0, 22, 144, text_size.h);
            
            graphics_context_set_text_color(ctx, GColorWhite);
            graphics_draw_text(ctx, szTime, fontTime4, bounds_time, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
          } else {
    			  text_size = graphics_text_layout_get_content_size(szTime, fontTime5, GRect(0, 0, 200, 200),
    				  GTextOverflowModeWordWrap, GTextAlignmentCenter);
    			  APP_LOG(APP_LOG_LEVEL_DEBUG, "Time Size5 %d %d", text_size.w, layer_bounds.size.w);
    
    			  GRect bounds_time = GRect(0, 24, 144, text_size.h);
    			  graphics_context_set_text_color(ctx, GColorWhite);
    			  graphics_draw_text(ctx, szTime, fontTime5, bounds_time, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
          }        
        }
      }        
    }
  }
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Draw Calendar %d.%d", t->tm_mon, t->tm_mday);
  {
    int i, j, totalWidth, holiday_today;
    
    graphics_context_set_stroke_color(ctx, GColorLightGray);
    graphics_draw_line(ctx, GPoint(10, 104), GPoint(134, 104));    
    
    snprintf(szDate, sizeof(szDate), "%d.%02d", t->tm_mon + 1, t->tm_mday);
    graphics_context_set_text_color(ctx, GColorWhite);
    
    GSize text_size = graphics_text_layout_get_content_size(szDate, fontDate, layer_bounds,
                              GTextOverflowModeWordWrap, GTextAlignmentCenter);
    
    holiday_today = sIsHoliday(t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    
    
    if (holiday_today) {
      GRect bitmap_size = gbitmap_get_bounds(s_bitmap_holi[holiday_today - 1]);
    
      totalWidth = text_size.w + 2 + 18 + 2 + bitmap_size.size.w;
      
      GRect bounds_date = GRect((144 - totalWidth) / 2, 80, text_size.w, text_size.h);

      graphics_draw_text(ctx, szDate, fontDate, bounds_date, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      graphics_draw_bitmap_in_rect(ctx, s_bitmap_mday[t->tm_wday], 
                                 GRect((144 - totalWidth + 2) / 2 + text_size.w + 2, 80 + text_size.h - 12, 18, 12));
      graphics_draw_bitmap_in_rect(ctx, s_bitmap_holi[holiday_today - 1], 
                                 GRect((144 - totalWidth + 2) / 2 + text_size.w + 2 + 18 + 2, 80 + text_size.h - 12, bitmap_size.size.w, bitmap_size.size.h));
    } else {
      totalWidth = text_size.w + 2 + 18;
      
      GRect bounds_date = GRect((144 - totalWidth) / 2, 80, text_size.w, text_size.h);
      
      graphics_draw_text(ctx, szDate, fontDate, bounds_date, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
      graphics_draw_bitmap_in_rect(ctx, s_bitmap_mday[t->tm_wday], 
                                 GRect((144 - totalWidth) / 2 + text_size.w + 2, 80 + text_size.h - 12, 18, 12));
    }
    
    graphics_context_set_text_color(ctx, GColorWhite);
    for (i = 0; i < 7; i++) {
      GSize text_size = graphics_text_layout_get_content_size(szmday[i], fontWDay, layer_bounds,
                                GTextOverflowModeWordWrap, GTextAlignmentCenter);
      GRect bounds_wday = GRect(2 + i*20 + (10 - text_size.w / 2), 84 + 36 - text_size.h,
                             text_size.w, text_size.h);
      graphics_draw_text(ctx, szmday[i], fontWDay, bounds_wday, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);      
    }
    
    graphics_context_set_fill_color(ctx, GColorYellow);
    graphics_fill_circle(ctx, GPoint(11 + 20*t->tm_wday, 84 + 51), 10);
    
    if (t->tm_mday != sCalInfo.nDate[0][t->tm_wday].tm_mday ||
        t->tm_mon != sCalInfo.nDate[0][t->tm_wday].tm_mon) {
      // make calendar
      sCalInfo.nDate[0][t->tm_wday].tm_mday = t->tm_mday;
      sCalInfo.nDate[0][t->tm_wday].tm_mon = t->tm_mon + 1;
      sCalInfo.nDate[0][t->tm_wday].tm_year = t->tm_year + 1900;
      
      for (i = 0; i < t->tm_wday; i++) {
        sGetCalcDate(sCalInfo.nDate[0][t->tm_wday], (-1 - i), &(sCalInfo.nDate[0][t->tm_wday - 1 - i]));
      }
          
      for (i = 0; i < 7 - t->tm_wday - 1; i++) {
        sGetCalcDate(sCalInfo.nDate[0][t->tm_wday], (1 + i), &(sCalInfo.nDate[0][t->tm_wday + 1 + i]));
      }
        
      for (i = 0; i < 7; i++) {
          sGetCalcDate(sCalInfo.nDate[0][i], 7 , &(sCalInfo.nDate[1][i]));
      }
    
      for (i = 0; i < 2; i++) {
        for (j = 0; j < 7; j++) {
            sCalInfo.nAnniv[i][j] = sIsHoliday(sCalInfo.nDate[i][j].tm_year, sCalInfo.nDate[i][j].tm_mon, sCalInfo.nDate[i][j].tm_mday);
        }
      }
    }
    
    for (i = 0; i < 7; i++) {
      if (i == 0 || sCalInfo.nAnniv[0][i] != 0) {
        if (bColor)
          graphics_context_set_text_color(ctx, GColorRed);
        else
          graphics_context_set_text_color(ctx, GColorBlack);          
      } else {
        if (i == t->tm_wday)
          graphics_context_set_text_color(ctx, GColorBlack);
        else
          graphics_context_set_text_color(ctx, GColorWhite);
      }      
      snprintf(sCalInfo.szDate[0][i], 3, "%d", sCalInfo.nDate[0][i].tm_mday);
      if (sCalInfo.nDate[0][i].tm_mday == t->tm_mday) {
        GSize cal_text_size1 = graphics_text_layout_get_content_size(sCalInfo.szDate[0][i], fontToday, layer_bounds,
                                  GTextOverflowModeWordWrap, GTextAlignmentCenter);
        GRect cal_bounds1 = GRect(2 + i*20 + (10 - cal_text_size1.w / 2), 84 + 57 - cal_text_size1.h,
                               cal_text_size1.w, cal_text_size1.h);
        graphics_draw_text(ctx, sCalInfo.szDate[0][i], fontToday, cal_bounds1, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);      
      } else {
        if (bColor == false && (i == 0 || sCalInfo.nAnniv[0][i] != 0)) {
          graphics_context_set_fill_color(ctx, GColorDarkGray);
          graphics_fill_rect(ctx, GRect(3 + 20 * i, 84 + 42, 18, 18), 0, GCornerNone);
          GSize cal_text_size1 = graphics_text_layout_get_content_size(sCalInfo.szDate[0][i], fontMDayB, layer_bounds,
                                    GTextOverflowModeWordWrap, GTextAlignmentCenter);
          GRect cal_bounds1 = GRect(2 + i*20 + (10 - cal_text_size1.w / 2), 84 + 57 - cal_text_size1.h,
                                 cal_text_size1.w, cal_text_size1.h);
          graphics_draw_text(ctx, sCalInfo.szDate[0][i], fontMDayB, cal_bounds1, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);      
        } else {
          GSize cal_text_size1 = graphics_text_layout_get_content_size(sCalInfo.szDate[0][i], fontMDay, layer_bounds,
                                    GTextOverflowModeWordWrap, GTextAlignmentCenter);
          GRect cal_bounds1 = GRect(2 + i*20 + (10 - cal_text_size1.w / 2), 84 + 57 - cal_text_size1.h,
                                 cal_text_size1.w, cal_text_size1.h);
          graphics_draw_text(ctx, sCalInfo.szDate[0][i], fontMDay, cal_bounds1, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);      
        }
      }
      
      if (i == 0 || sCalInfo.nAnniv[1][i] != 0) {
        graphics_context_set_text_color(ctx, PBL_IF_COLOR_ELSE(GColorRed, GColorBlack));
      } else {
        graphics_context_set_text_color(ctx, GColorWhite);
      }
      snprintf(sCalInfo.szDate[1][i], 3, "%d", sCalInfo.nDate[1][i].tm_mday);
      if (bColor == false && (i == 0 || sCalInfo.nAnniv[1][i] != 0)) {
        graphics_context_set_fill_color(ctx, GColorDarkGray);
        graphics_fill_rect(ctx, GRect(3 + 20 * i, 84 + 42 + 19, 18, 18), 0, GCornerNone);
        
        GSize cal_text_size2 = graphics_text_layout_get_content_size(sCalInfo.szDate[1][i], fontMDayB, layer_bounds,
                                  GTextOverflowModeWordWrap, GTextAlignmentCenter);
        GRect cal_bounds2 = GRect(2 + i*20 + (10 - cal_text_size2.w / 2), 84 + 76 - cal_text_size2.h,
                               cal_text_size2.w, cal_text_size2.h);
        graphics_draw_text(ctx, sCalInfo.szDate[1][i], fontMDayB, cal_bounds2, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);      
      } else {
        GSize cal_text_size2 = graphics_text_layout_get_content_size(sCalInfo.szDate[1][i], fontMDay, layer_bounds,
                                  GTextOverflowModeWordWrap, GTextAlignmentCenter);
        GRect cal_bounds2 = GRect(2 + i*20 + (10 - cal_text_size2.w / 2), 84 + 76 - cal_text_size2.h,
                               cal_text_size2.w, cal_text_size2.h);
        graphics_draw_text(ctx, sCalInfo.szDate[1][i], fontMDay, cal_bounds2, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);              
      }
    }
  }
#endif
}

void load_resource() {
  bColor = PBL_IF_COLOR_ELSE(true, false);
  
  fontTime1 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CHALET_LONDON_60));
  fontTime2 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CHALET_LONDON_55));
  fontTime3 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CHALET_LONDON_50));
  fontTime4 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CHALET_LONDON_45));
  fontTime5 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CHALET_LONDON_40));
  fontDate = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  fontWDay = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
  #if defined(PBL_ROUND)
  fontMDay = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  #else
  fontMDay = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  #endif
  fontMDayB = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  fontToday = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  fontBatt = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  
  strncpy(szmday[0], "S", sizeof(szmday[0]));
  strncpy(szmday[1], "M", sizeof(szmday[0]));
  strncpy(szmday[2], "T", sizeof(szmday[0]));
  strncpy(szmday[3], "W", sizeof(szmday[0]));
  strncpy(szmday[4], "T", sizeof(szmday[0]));
  strncpy(szmday[5], "F", sizeof(szmday[0]));
  strncpy(szmday[6], "S", sizeof(szmday[0]));
  
  s_bitmap_mday[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SUN_SYM);
  s_bitmap_mday[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MON_SYM);
  s_bitmap_mday[2] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TUE_SYM);
  s_bitmap_mday[3] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WED_SYM);
  s_bitmap_mday[4] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_THU_SYM);
  s_bitmap_mday[5] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FRI_SYM);
  s_bitmap_mday[6] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SAT_SYM);
  
  s_bitmap_holi[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLIDAY_1);
  s_bitmap_holi[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLIDAY_2);
  s_bitmap_holi[2] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLIDAY_3);
  s_bitmap_holi[3] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLIDAY_4);
  s_bitmap_holi[4] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLIDAY_5);
  s_bitmap_holi[5] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLIDAY_6);
  s_bitmap_holi[6] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLIDAY_7);
  s_bitmap_holi[7] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLIDAY_8);
  s_bitmap_holi[8] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLIDAY_9);
  s_bitmap_holi[9] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLIDAY_10);
  s_bitmap_holi[10] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLIDAY_11);
  s_bitmap_holi[11] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLIDAY_12);
  s_bitmap_holi[12] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLIDAY_13);
  s_bitmap_holi[13] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLIDAY_14);
  s_bitmap_holi[14] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOLIDAY_99);
  
  s_bitmap_batt = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_LIGHT);
  s_bitmap_batt_low = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_DARK);
  s_bitmap_batt_low2 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATT_DARK2);
  s_bitmap_bt_ok = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_OK1);
  s_bitmap_bt_fail = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_FAIL1);
  s_bitmap_bt_fail_BW = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_FAIL2);
}

void unload_resource() {
  int i;
  
  // Unload GFont
  fonts_unload_custom_font(fontTime1);
  fonts_unload_custom_font(fontTime2);
  fonts_unload_custom_font(fontTime3);
  fonts_unload_custom_font(fontTime4);
  fonts_unload_custom_font(fontTime5);
  
  for (i = 0; i < 7; i++) {
    gbitmap_destroy(s_bitmap_mday[i]);
  }
  for (i = 0; i < 15; i++) {
    gbitmap_destroy(s_bitmap_holi[i]);
  }
  gbitmap_destroy(s_bitmap_batt);
  gbitmap_destroy(s_bitmap_batt_low);
  gbitmap_destroy(s_bitmap_batt_low2);
  gbitmap_destroy(s_bitmap_bt_ok);
  gbitmap_destroy(s_bitmap_bt_fail);
  gbitmap_destroy(s_bitmap_bt_fail_BW);
}

void batt_save_level(int nLevel) {
  s_battery_level = nLevel;
}

void bt_set_connected(bool connected) {
  s_bt_connected = connected;
}

