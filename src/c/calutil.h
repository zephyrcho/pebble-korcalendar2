#pragma once

int sGetDayOfWeek(int dyear, int dmonth, int dday);

int sGetMaxDayOfMonth(int nYear, int nMonth);

void sGetCalcDate(struct tm OriDate, int nDiff, struct tm *OutDate);

int sIsHoliday(int nYear, int nMonth, int nDay);