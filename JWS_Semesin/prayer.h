#ifndef PRAYER_H
#define PRAYER_H

#include "trigonometri.h"

struct PrayerData
{
	int16_t lintang;
	int16_t bujur;
	int16_t zonaWaktu;
	int16_t sudutFajar;
	int16_t sudutTerbit;
	int16_t sudutDhuha;
	int16_t sudutMagrib;
	int16_t sudutIsya;
	int8_t aturDzuhur;
	int8_t aturAshar;

	uint8_t tanggal;
	uint8_t bulan;
	uint8_t tahun;
};

PrayerData prayerData;
double julian_date;
double waktuSholatDouble[9];
double sun_declination;
double equation_of_time;

double fix_angle(double a)
{
	a = a - 360.0 * floor(a / 360.0);
	a = a < 0.0 ? a + 360.0 : a;
	return a;
}

double fix_hour(double a)
{
	a = a - 24.0 * floor(a / 24.0);
	a = a < 0.0 ? a + 24.0 : a;
	return a;
}

double get_julian_date(int year, int month, int day)
{
  if (month <= 2)
  {
    year -= 1;
    month += 12;
  }

  double a = floor(year / 100.0);
  double b = 2 - a + floor(a / 4.0);

  return floor(365.25 * (year + 4716)) + floor(30.6001 * (month + 1)) + day + b - 1524.5;
}
static double time_diff(double time1, double time2)
{
	return fix_hour(time2 - time1);
}
double night_portion(double angle)
{
	return 1.0 * angle / (256.0 * 60.0);//****trick
}
static void get_float_time_parts(double time, int& hours, int& minutes)
{
	time = fix_hour(time + 0.5 / 60);		// add 0.5 minutes to round
	hours = floor(time);
	minutes = floor((time - hours) * 60);
}


void getSunEquation(double jd)
{
  double d = jd - 2451545.0;
  double g = fix_angle(357.529 + 0.98560028 * d);
  double q = fix_angle(280.459 + 0.98564736 * d);
  double la = fix_angle(q + 1.915 * dsin(g) + 0.020 * dsin(2 * g));

  double e = 23.439 - 0.00000036 * d;

  sun_declination = darcsin(dsin(e) * dsin(la));
  double ra = darctan2(dcos(e) * dsin(la), dcos(la)) / 15.0;
  ra = fix_hour(ra);
  equation_of_time = q / 15.0 - ra;
}
double compute_mid_day(double time)
{
  getSunEquation(julian_date + time);
  double z = fix_hour(12 - equation_of_time);
  return z;
}
double compute_time(double angle, double time)
{
  angle /= 256.0;//****trick
	
  float lintang = 1.0 * prayerData.lintang / 256;
  double z = compute_mid_day(time);
  getSunEquation(julian_date + time);
  double v = 1.0 / 15.0 * darccos((-dsin(angle) - dsin(sun_declination) * dsin(lintang)) / (dcos(sun_declination) * dcos(lintang)));
  return z + (angle > 90.0 ? - v :  v);
}
double compute_asr(int step, double time)  // Shafii: step=1, Hanafi: step=2
{
  float lintang = 1.0 * prayerData.lintang / 256;
  getSunEquation(julian_date + time);
  double g = -darccot(step + dtan(fabs(lintang - sun_declination)));
  return compute_time(g * 256, time);//****trick
}
void adjust_times(double times[])
{
  for (int i = 0; i < jumlahJadwal; ++i)
    times[i] += 1.0 * (prayerData.zonaWaktu - (prayerData.bujur / 15.0)) / 256.0;//****trick
  times[jadwalDzuhur] += 1.0 * prayerData.aturDzuhur / 60;

  //  adjust_high_lat_times(times);
  double night_time = time_diff(times[jadwalTerbenam], times[jadwalTerbit]);   // sunset to sunrise

  // Adjust Fajr
  double fajr_diff = night_portion(prayerData.sudutFajar) * night_time;
  if (isnan(times[jadwalSubuh]) || time_diff(times[jadwalSubuh], times[jadwalTerbit]) > fajr_diff)
    times[jadwalSubuh] = times[jadwalTerbit] - fajr_diff;

  // Adjust Isha
  double isha_diff = night_portion(prayerData.sudutIsya) * night_time;
  if (isnan(times[jadwalIsya]) || time_diff(times[jadwalTerbenam], times[jadwalIsya]) > isha_diff)
    times[jadwalIsya] = times[jadwalTerbenam] + isha_diff;

  // Adjust Maghrib
  double maghrib_diff = night_portion(prayerData.sudutMagrib) * night_time;
  if (isnan(times[jadwalMaghrib]) || time_diff(times[jadwalTerbenam], times[jadwalMaghrib]) > maghrib_diff)
    times[jadwalMaghrib] = times[jadwalTerbenam] + maghrib_diff;

}
void compute_times(double times[])
{
  for (int i = 0; i < jumlahJadwal; ++i)
    times[i] /= 24.0;

  times[jadwalSubuh]    = compute_time(((uint16_t)180 * 256) - prayerData.sudutFajar, times[jadwalSubuh]);
  times[jadwalTerbit] = compute_time(((uint16_t)180 * 256) - prayerData.sudutTerbit, times[jadwalTerbit]);
  times[jadwalDhuha] = compute_time(((uint16_t)180 * 256) - prayerData.sudutDhuha, times[jadwalDhuha]);
  times[jadwalDzuhur]   = compute_mid_day(times[jadwalDzuhur]);
  times[jadwalAshar]     = compute_asr(1 + prayerData.aturAshar, times[jadwalAshar]);
  times[jadwalTerbenam]  = compute_time(prayerData.sudutTerbit, times[jadwalTerbenam]);
  times[jadwalMaghrib] = compute_time(prayerData.sudutMagrib, times[jadwalMaghrib]);
  times[jadwalIsya]    = compute_time(prayerData.sudutIsya, times[jadwalIsya]);
}

void compute_day_times(double times[])
{
  byte default_times[] = { 0, 5, 6, 6, 12, 13, 18, 18, 18 };    // default times
  for (byte i = 0; i < jumlahJadwal; ++i)
  {
    times[i] = default_times[i];
  }

  for (int i = 0; i < IterasiKomputasiJadwalSholat; ++i)
    compute_times(times);

  adjust_times(times);
}

void get_prayer_times()
{
  float bujur = 1.0 * prayerData.bujur / 256;
  julian_date = get_julian_date(prayerData.tahun, prayerData.bulan, prayerData.tanggal) - bujur / (double) (15 * 24);
  compute_day_times(waktuSholatDouble);
}
#endif