#ifndef FUNGSI_H
#define FUNGSI_H

#include "trigonometri.h"

#define swap(value) asm volatile("swap %0" : "=r" (value) : "0" (value))
template <typename type> type sign(type value) {return type((value>0)-(value<0));}

byte bcd2bin(byte val) 
{
	return val - 6 * (val >> 4);
}

byte bin2bcd(byte val)
{
	return val + 6 * (val / 10);
}
uint16_t bin2bcd(uint16_t val)
{
	return val + 6 * (val / 10);
}
uint16_t Convert_IntToBCD16(uint16_t DecimalValue)
{
	uint16_t returnValue = 0;
	//uint16_t LSB_L = DecimalValue;

	while (DecimalValue >= 1000)
	{
		DecimalValue -= 1000;
		returnValue += 0x1000;
	}
	while (DecimalValue >= 100)
	{
		DecimalValue -= 100;
		returnValue += 0x0100;
	}
	while (DecimalValue >= 10)
	{
		DecimalValue -= 10;
		returnValue += 0x0010;
	}
	return returnValue + DecimalValue;
}

void formatWaktu(char *buffer, byte jam, byte menit)
{
	byte jamBCD = bin2bcd(jam);
	byte menitBCD = bin2bcd(menit);
	
	buffer[1] = (jamBCD & 0x0f) + 0x30;
	swap(jamBCD);
	buffer[0] = (jamBCD & 0x0f) + 0x30;
	buffer[2] = ':';
	buffer[4] = (menitBCD & 0x0f) + 0x30;
	swap(menitBCD);
	buffer[3] = (menitBCD & 0x0f) + 0x30;
	buffer[5] = 0;
}
void formatTanggal(char *buffer, byte tanggal, byte bulan, byte tahun)
{
	byte tanggalBCD = bin2bcd(tanggal);
	byte bulanBCD = bin2bcd(bulan);
	byte tahunBCD = bin2bcd(tahun);
	
	buffer[1] = (tanggalBCD & 0x0f) + 0x30;
	swap(tanggalBCD);
	buffer[0] = (tanggalBCD & 0x0f) + 0x30;
	buffer[2] = '/';
	buffer[4] = (bulanBCD & 0x0f) + 0x30;
	swap(bulanBCD);
	buffer[3] = (bulanBCD & 0x0f) + 0x30;
	buffer[5] = '/';
	buffer[6] = '2';
	buffer[7] = '0';
	buffer[9] = (tahunBCD & 0x0f) + 0x30;
	swap(tahunBCD);
	buffer[8] = (tahunBCD & 0x0f) + 0x30;
	buffer[10] = 0;
}

//tanggal mulai dari 1
//bulan mulai dari 1
//tahun dua digit
//minggu = 1, sabtu = 7
//basis 1 Januari 2000
uint16_t jumlahHariPerBulanHijriah[] = {0,30,59,89,118,148,177,207,236,266,295,325,354};
uint16_t jumlahHariPerBulanMasehi[]={0,31,59,90,120,151,181,212,243,273,304,334};
double tahunBulan = 354.367068;

byte hariDariTanggal(byte tanggal, byte bulan, uint16_t tahun)
{
  
  if(tahun >= 2000)
    tahun -= 2000;
  
  uint32_t jumlahHari = tahun * 365;
  uint16_t tahunKabisat = tahun/4;
  for (byte i = 0; i < tahun; i++) 
  {
    if (!(i%4))
    {
      jumlahHari++;
    }
  }
  jumlahHari += jumlahHariPerBulanMasehi[bulan-1];
  if ( (bulan >= 2) && !(tahun % 4)) 
  { 
    jumlahHari++;
  }
  jumlahHari+= tanggal;
  return ((jumlahHari + 6) % 7) + 1;
}
uint32_t jumlahHariDariTanggal(byte tanggal, byte bulan, uint16_t tahun)
{
  uint32_t jumlahHari = (uint32_t)tahun * 365;
  for (uint16_t i = 0; i < tahun; i++) 
  {
    if (!(i%4))
    {
      jumlahHari++;
    }
  }
  jumlahHari += jumlahHariPerBulanMasehi[bulan-1];
  if ( (bulan >= 2) && !(tahun % 4)) 
  { 
    jumlahHari++;
  }
  jumlahHari+= tanggal;
  return jumlahHari + 259;// base 18 April 1999
}
void masehiKeHijriah(uint32_t HariMasehi, byte *tanggal, byte *bulan, uint16_t *tahun)
{
  uint16_t sisaHari;
  *tahun = floor(HariMasehi/tahunBulan);
  sisaHari = HariMasehi - (tahunBulan * *tahun);
 
  for(byte i=0;i<sizeof(jumlahHariPerBulanHijriah);i++)
  {
    if(sisaHari <= jumlahHariPerBulanHijriah[i])
    {
      sisaHari -= jumlahHariPerBulanHijriah[i-1];
      *bulan = i;
      break;
    }
  }
  *tanggal = sisaHari;
  *tahun += 1420;
}
byte copyFlashString(char* buf, const char* alamat)
{
  char c;
	byte l=0;
  while(c = pgm_read_byte(alamat++))
  {
    *buf++ = c;
		l++;
  }
  *buf = 0;
	return l;
	
}
byte copyString(char* buf, char* asal)
{
  char c;
	byte l=0;
  while(c = *asal++)
  {
    *buf++ = c;
		l++;
  }
  *buf = 0;
	return l;
	
}
byte bcdToArray(char *buffer, byte nilai)
{
  swap(nilai);
  *buffer++ = (nilai & 0x0f) + 0x30;
  swap(nilai);
  *buffer = (nilai & 0x0f) + 0x30;
  return 2;
}
byte formatTanggalStrBulan(char *buffer, byte tanggal, char *strBulan, uint16_t Tahun)
{
  byte tanggalBCD = bin2bcd(tanggal);
  uint16_t TahunBCD = Convert_IntToBCD16(Tahun);
  byte tahunHighBCD = highByte(TahunBCD);
  byte tahunLowBCD = lowByte(TahunBCD);

  byte index = 0;
  index = bcdToArray(buffer + index, tanggalBCD);
  buffer[index++] = ' ';
  index += copyFlashString(buffer + index, strBulan);
  buffer[index++] = ' ';
  index += bcdToArray(buffer + index, tahunHighBCD);
  index += bcdToArray(buffer + index, tahunLowBCD);

  return index;
}



#endif
