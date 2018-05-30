#ifndef I2C_SEMESIN_H
#define I2C_SEMESIN_H

byte bacaByteI2C(byte alamatI2CRTC, uint16_t address)
{
  Wire.beginTransmission(alamatI2CRTC);
  Wire.write(address >> 8);
  Wire.write(address & 0xFF);
  Wire.endTransmission();
  Wire.requestFrom(alamatI2CRTC, (byte)1);
  byte data = 0;
  if (Wire.available())
  {
      data = Wire.read();
  }
  return data;
}

#endif
