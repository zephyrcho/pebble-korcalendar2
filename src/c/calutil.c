#include <pebble.h>
#include "calutil.h"

#define	MAX_COUNT_LUNAR_DAY     119
#define LEAP_MONTH    100

typedef struct tag_HOLIDAY {
  int   year;
	int		month;
	int		day;
	int   type;
} HOLIDAY_T;

#define NUM_HOLIDAY				    77

HOLIDAY_T sHolidayList[NUM_HOLIDAY] = {
  #include "HoliTable.h"
};

int sGetMaxDayOfMonth( int nYear, int nMonth )
{
    int pnMaxDay[12] = {31,28,31,30,31,30,31,31,30,31,30,31}; // 1?꾩쓽 媛??ъ뿉 ??????섎? ??ν븳 諛곗뿴
    // 洹몃젅怨좊━?μ쑝濡?怨꾩궛?섎뒗 寃쎌슦 4?꾨쭏???ㅻ뀈???먯뼱 2?붿씠 29?쇱씠??100?쇰줈 ?섎늻?댁???400?쇰줈???섎늻?댁?吏 ?딅뒗 ?대뒗 ?됰뀈?쇰줈 怨꾩궛
    if (nMonth > 12)  //rys20080115
    {
        nYear += nMonth / 12;
        nMonth = nMonth % 12;
    }
    return pnMaxDay[nMonth-1] + ( nMonth==2 && (nYear % 400 == 0 || (nYear % 100 && nYear % 4 == 0 ))? 1: 0 ); // ?ㅻ뀈 2?붿씪 寃쎌슦?먮뒗 29?쇱씠 諛섑솚?섎룄濡?
}

int sGetDayOfWeek(int dyear, int dmonth, int dday)
{
    int iTotalDay, i = 1;
    int year;

    year = dyear - 1;
    iTotalDay = year * 365;
    iTotalDay += year / 4;
    iTotalDay -= year / 100;
    iTotalDay += year / 400;

    while (i < dmonth)
    {
        iTotalDay += sGetMaxDayOfMonth(year+1, i);   //洹??ъ쓽 ?????뚯븘??
        i++;
    }

    return ((iTotalDay + dday) % 7);
}


void sGetCalcDate(struct tm OriDate, int nDiff, struct tm *OutDate) {
  
	if(nDiff < 0)
	{
    OutDate->tm_mday = OriDate.tm_mday + nDiff;
    if (OutDate->tm_mday < 1) {
      OutDate->tm_mon = OriDate.tm_mon - 1;
      if (OutDate->tm_mon < 1) {
        OutDate->tm_year = OriDate.tm_year - 1;
        OutDate->tm_mon += 12;
      } else {
        OutDate->tm_year = OriDate.tm_year;
      }
      
      OutDate->tm_mday += sGetMaxDayOfMonth(OutDate->tm_year, OutDate->tm_mon);
    } else {
      OutDate->tm_mon = OriDate.tm_mon;
      OutDate->tm_year = OriDate.tm_year;
    }
  } else if (nDiff > 0) {
    int nMaxDate;
    
    OutDate->tm_year = OriDate.tm_year;
    OutDate->tm_mon = OriDate.tm_mon;
    OutDate->tm_mday = OriDate.tm_mday + nDiff;
    
    nMaxDate = sGetMaxDayOfMonth(OutDate->tm_year, OutDate->tm_mon);
    if (OutDate->tm_mday > nMaxDate) {
      OutDate->tm_mday -= nMaxDate;
      OutDate->tm_mon += 1;

      if (OutDate->tm_mon > 12) {
        OutDate->tm_mon -= 12;
        OutDate->tm_year += 1;
      }
    }
  } else {
    OutDate->tm_year = OriDate.tm_year;
    OutDate->tm_mon = OriDate.tm_mon;
    OutDate->tm_mday = OriDate.tm_mday;
  } 
}

int sIsHoliday(int nYear, int nMonth, int nDay) {
  int i;
  
  for (i = 0; i < NUM_HOLIDAY; i++) {
    if (sHolidayList[i].year == nYear && 
        sHolidayList[i].month == nMonth &&
        sHolidayList[i].day == nDay) {
      return sHolidayList[i].type;
    }
  }
  return 0;
}
