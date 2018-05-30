#ifndef RTC_SEMESIN_H
#define RTC_SEMESIN_H

tanggalDanWaktu ambilRTC(byte alamatI2CRTC)
{
  tanggalDanWaktu tw;
  Wire.beginTransmission(alamatI2CRTC);
  Wire.write((byte)0x00);  
  Wire.endTransmission();
  
  Wire.requestFrom(alamatI2CRTC, (byte)8);
  tw.detik = bcd2bin(Wire.read());
  tw.menit = bcd2bin(Wire.read());
  tw.jam = bcd2bin(Wire.read());
  tw.hari =  Wire.read();
  tw.tanggal = bcd2bin(Wire.read());
  tw.bulan = bcd2bin(Wire.read());
  tw.tahun = bcd2bin(Wire.read()) + 2000;

  return tw;
}
void simpanRTC(byte alamatI2CRTC, tanggalDanWaktu tw) 
{
  Wire.beginTransmission(alamatI2CRTC);
  Wire.write((byte)0x00);  //beginning from SEC Register address

  Wire.write((byte)bin2bcd(tw.detik)); 
  Wire.write((byte)bin2bcd(tw.menit));
  Wire.write((byte)bin2bcd(tw.jam)); //Make sure clock is still 24 Hour
  Wire.write((byte)tw.hari);
  Wire.write((byte)bin2bcd(tw.tanggal));
  Wire.write((byte)bin2bcd(tw.bulan));
  Wire.write(bin2bcd((byte)(tw.tahun - 2000)));  
  Wire.endTransmission();
}

#endif
