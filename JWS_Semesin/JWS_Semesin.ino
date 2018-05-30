
#define modeDemo                0    //<<==================================== DEMO prinsip kerja 0 / 1

#define selisihImsak            10

//defenisi pin
#define pinA                    6
#define pinB                    7
#define pinOE                   9
#define pinSCK                  8

#define alarm                   5
#define RTCDetikIRQ             A3

//variabel Setting
#define jumlahKolomPanel        2  //panel
#define jumlahBarisPanel        1  //panel

#define lamaTampilWaktuSholat   10 // * 100 milidetik
#define lamaTampilInformasiJam  100 // * 100 milidetik

#define fontWaktu               angka6x13
#define fontInformasi           SystemFont5x7Ramping
#define fontTanggal             SystemFont5x7Ramping
#define fontJadwalSholat        SystemFont5x7Ramping
#define fontWaktuSholat         SystemFont5x7Ramping
#define fontWaktuMasuk          SystemFont5x7Ramping
#define fontAdzan               SystemFont5x7Ramping
#define fontIqomah              SystemFont5x7Ramping
#define fontMatikanHP           Arial14

#define RTC_ADDRESS             0x68
#define I2CEEPROM_ADDRESS       0x57  //Perhatikan alamat I2CEEPROM 0x50(ds1307 .. 0x57(ds3231)
#define RTC_CONTROL_REG         0x0E  //0x07 ds1307, 0x0E ds3231 


#include <Wire.h>
#include <SPI.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>

#include <DMD2.h>

#include "fungsi.h"
#include "definisi.h"
#include "konstanta.h"
#include "rtc.h"
#include "prayer.h"
#include "i2cUmum.h"
#include "trigonometri.h"

#include <fonts/SystemFont5x7Ramping.h>
#include <fonts/Arial14.h>
#include <fonts/Arial_Black_16.h>
#include <fonts/angka6x13.h>
#include <fonts/AllahMuhammad.h>

const uint16_t alamatFont[] = {&SystemFont5x7Ramping, &Arial14, &Arial_Black_16};

struct Setting
{
  int8_t koreksiHijriah;
  int8_t jumlahAlarm;
  int8_t lamaAdzan;
  int8_t lamaIqomah[5];
  int8_t ulangMatikanHP;
  int8_t lamaSholat[5];
  int8_t lamaSholatJumat;
  int8_t kecepatanJalanInfoText;
  int8_t kecerahanJWS;
  int8_t padamJam;
  int8_t padamMenit;
  int8_t nyalaJam;
  int8_t nyalaMenit;
  int8_t tampilanHijriah;
  int8_t tampilanImsak;
  int8_t tampilanTerbit;
  int8_t tampilanDhuha;
  int8_t tampilanTerbenam;
  int8_t JumlahInfoTextAktif;
};

#if modeDemo
  Setting setting = {0, 5, 1, {5, 5, 5, 5, 5}, 1, {5, 5, 5, 5, 5}, 2, 50, 20, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1};
  char namaMesjid[panjangString] = "Musholla ...\0   ";
#else
  Setting setting;
  char namaMesjid[panjangString];
#endif

//variabel
const char strWaktu[] PROGMEM = "Waktu";
const char strSholat[] PROGMEM = "Sholat";
char strWaktuInformasi[9];
JamDanMenit waktuSholat[9];

EfekMarqueHorizontalkeKiri marqueKiri;
EfekTulis efekTulis;
uint8_t efekScrollOutDanWipeIn;

uint8_t marqueOut;
bool marqueTampilan;
uint8_t wipeInTampilan;
uint8_t wipeOutTampilan;
uint8_t efekTulisTampilan;


uint8_t indexNamaSholat;
uint16_t hitungMundurSholat;
uint16_t hitungMundurPesanSebelumSholat;
uint16_t hitungMundurIqomah;
uint16_t hitungMundurAdzan;
uint8_t hitungMundurAlarm;
uint16_t hitungMudurAlarmIqomah;

byte modeOperasi;
byte modeOperasiSebelumnya;

uint8_t menitSebelumnya = 60;
uint8_t hariSebelumnya = 32;

bool statusAlarm;

bool tampilkanMenit;
bool infoWaktu;
byte infoWaktuTop;
uint16_t lamaTampil = 0;
uint16_t indekTampilan;

char textWaktuJam[6];
char textTanggalHijriah[sizeof(strNamaHari[0]) + 2 + 3 + sizeof(strNamaBulanHijriah[0]) + 3 - 2];

uint8_t bluetoothData[32];
uint16_t indexParseSerial = 0;

byte indexInformasi = 1;
long mulaiMillis;
uint16_t AlamatI2CEEPROM;

int8_t indexSetting = 0;
byte indexSettingInfo;
bool tokenValid = false;
bool RTCValid = true;

byte tanggalHijriah;
byte bulanHijriah;
uint16_t tahunHijriah;

uint16_t AlamatI2CEEPROMSebelumnya;
bool nextEEPROMData = false;
byte EEPROMDataCounter;

bool modeSambungSebelumnya = false;

tanggalDanWaktu now;
tanggalDanWaktu settingWaktu;
byte settingWaktuIndex;

//Class variabel
#define l jumlahKolomPanel * 32
#define t jumlahBarisPanel * 16
SPIDMD dmd(jumlahKolomPanel, jumlahBarisPanel, pinOE, pinA, pinB, pinSCK);
DMDFrame dmdEfek(l, t);

DMD_TextBox boxKolom1(dmd, 0, 0, l / 2, t);
DMD_TextBox boxKolom2(dmd, l / 2, 0, l / 2, t);
DMD_TextBox boxBaris1(dmd, 0, 0, l, (t / 2) - 1);
DMD_TextBox boxBaris2(dmd, 0, t / 2, l, t / 2);

DMD_TextBox boxKolom2Baris1(dmd, l / 2, 0, l / 2, t / 2);
DMD_TextBox boxKolom2Baris2(dmd, l / 2, t / 2, l / 2, t / 2);

//Eeprom24C32_64 I2CEEPROM(EEPROM_ADDRESS);
HardwareSerial *bluetooth;

#define tokenRTC 0xA9
#define tokenBluetooth 0x4C
#define tokenSetInformasi "225654"
#define tokenSetSetting "963215"

void setup() {
  Serial.begin(9600);
  Serial.println("JWS Bluetooth");
  Serial.println("http://www.semesin.com/project/");
  Serial.println();

  bluetooth = &Serial;

  pinMode(alarm, OUTPUT);
  pinMode(RTCDetikIRQ, INPUT_PULLUP);

  //Interupsi setiap 1 detik
  Wire.begin();
  Wire.beginTransmission(RTC_ADDRESS);
  Wire.write(RTC_CONTROL_REG);
  Wire.write(0x00);//SQW freq, 1Hz
  Wire.endTransmission();

#if modeDemo
  prayerData = { -1576, 27343, 1792, 5120, 213, -1152, 1024, 4608, 0, 0, 0,0,0};
  settingWaktu = {50, 29, 4, 5, 7, 5, 2018};
  simpanRTC(RTC_ADDRESS, settingWaktu);
#endif

  if (EEPROM.read(alamatTokenRTC) != tokenRTC)
  {
    //Waktu compiler
    char Month[12];
    int Day, Year;
    uint8_t monthIndex;
    int Hour, Min, Sec;
    char *monthName[12] = {
      "Jan", "Feb", "Mar", "Apr", "May", "Jun",
      "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    sscanf(__TIME__, "%d:%d:%d", &Hour, &Min, &Sec);
    sscanf(__DATE__, "%s %d %d", Month, &Day, &Year);
    for (monthIndex = 0; monthIndex < 12; monthIndex++) {
      if (strcmp(Month, monthName[monthIndex]) == 0) break;
    }
    uint8_t wday = hariDariTanggal(Day, monthIndex + 1, Year);
    settingWaktu = {Sec, Min, Hour, wday, Day, monthIndex + 1, Year};
    simpanRTC(RTC_ADDRESS, settingWaktu);
    EEPROM.write(alamatTokenRTC, tokenRTC);
  }

#if !modeDemo
  if (EEPROM.read(alamatTokenBluetooth) == tokenBluetooth)
  {
    ambilSetting();
  }
#endif
  ambilHijriah();
  dmd.begin();

  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();

  mulaiMillis = millis();
  modeOperasi = tampilWaktuSholat;

  while (!digitalRead(RTCDetikIRQ));
}

void loop() {
  if (modeOperasi != settingBluetooth)
  {
    if (digitalRead(RTCDetikIRQ))
    {
      if (RTCValid)
      {
        RTCValid = false;
        now = ambilRTC(RTC_ADDRESS);

        //dikerjakan bila Jam berubah
        if (hariSebelumnya != now.tanggal)
        {
          prayerData.tanggal = now.tanggal;
          prayerData.bulan = now.bulan;
          prayerData.tahun = now.tahun;
          get_prayer_times();
          
          for (byte i = 0; i < jumlahJadwal; i++) //7 waktu (Imsak dihitung terpisah)
          {
            int menit;
            int jam;
            get_float_time_parts(waktuSholatDouble[i], jam, menit);
            waktuSholat[i].jam = jam;
            waktuSholat[i].menit = menit;
          }
          uint16_t waktuStamp = waktuSholat[jadwalSubuh].jam * 60 + waktuSholat[jadwalSubuh].menit - selisihImsak;
          waktuSholat[jadwalImsak].jam = waktuStamp / 60;
          waktuSholat[jadwalImsak].menit = waktuStamp % 60;
          hariSebelumnya = now.tanggal;
        }

        //dikerjakan bila menit berubah
        if (menitSebelumnya != now.menit)
        {
          if (modeOperasi == tampilWaktuSholat)
          {
            ambilWaktu(now.jam, now.menit);
          }
          if (modeOperasi != padam)
          {
            //cek waktu-waktu sholat
            for (int i = 0; i < sizeof(waktuSholat) / sizeof(waktuSholat[0]); i++)
            {
              if ((now.jam == waktuSholat[i].jam) && (now.menit == waktuSholat[i].menit))
              {
                if (!(
                  (i == jadwalImsak) || 
                  (i == jadwalTerbit) || 
                  (i == jadwalDhuha) || 
                  (i == jadwalTerbenam)
                  )) //kecuali terbit dan terbenam
                {
#if modeDemo
                  hitungMundurAlarm = 2 * 2;
#else
                  hitungMundurAlarm = setting.jumlahAlarm * 2;
#endif
                  statusAlarm = HIGH;
                  digitalWrite(alarm, statusAlarm);

                  if ((now.hari == 6) && (i == 3)) //Hari jumat
                  {
                    hitungMundurAdzan = 0;
                    hitungMundurIqomah = 0;
                    hitungMudurAlarmIqomah = 0;
                    hitungMundurPesanSebelumSholat = 0;
                    hitungMundurSholat = setting.lamaSholatJumat * 60;//menit
                    indexNamaSholat = indexJumat;
                  }
                  else if (i == jadwalImsak)
                  {
                    hitungMundurAdzan = 0;
                    hitungMundurIqomah = 0;
                    hitungMudurAlarmIqomah = 0;
                    hitungMundurPesanSebelumSholat = 0;
                    hitungMundurSholat = 1;
                    indexNamaSholat = i;
                  }
                  else
                  {
#if modeDemo
                    hitungMundurAdzan = 5;//detik
                    hitungMundurIqomah = 5;//detik
                    hitungMudurAlarmIqomah = 3;
                    hitungMundurPesanSebelumSholat = 2;
                    hitungMundurSholat = 5;
#else
                    hitungMundurAdzan = setting.lamaAdzan * 60; //menit
                    hitungMundurIqomah = setting.lamaIqomah[i] * 60; //menit
                    hitungMudurAlarmIqomah = hitungMundurAlarm;
                    hitungMundurPesanSebelumSholat = setting.ulangMatikanHP + 1;
                    hitungMundurSholat = setting.lamaSholat[i] * 60; //menit
#endif
                    indexNamaSholat = i;
                  }

                  indekTampilan = 0;
                  wipeOutTampilan = 0;
                  wipeInTampilan = 0;
                  marqueTampilan = false;
                  marqueOut = 0;
                  lamaTampil = 0;
                  tampilkanMenit = false;
                  infoWaktu = false;
                  modeOperasi = modeWaktuSholat;

                  if (i == jadwalMaghrib)
                  {
                    ambilHijriah();
                  }

                  dmd.clearScreen();
                  dmd.selectFont(fontWaktuMasuk);

                  char buf[sizeof(strWaktu[0])];
                  byte index = copyFlashString(buf, strWaktu);
                  buf[index] = 0;
                  uint8_t lebarText = dmd.stringWidth(buf);
                  dmd.drawString((l - lebarText) / 2, 0, buf);

                  char buf2[sizeof(strSholat) + 1 + sizeof(strWaktuSholat[0]) + 1 - 2];
                  index = copyFlashString(buf2, strSholat);
                  buf2[index++] = ' ';
                  index += copyFlashString(&buf2[index], strWaktuSholat[indexNamaSholat]);
                  buf2[index] = 0;
                  lebarText = dmd.stringWidth(buf2);
                  Serial.println(buf2);
                  dmd.drawString((l - lebarText) / 2, t / 2, buf2);

                  break;
                }
              }
            }
            if ((now.jam == setting.padamJam) && (now.menit == setting.padamMenit))
            {
              modeOperasi = padam;
              dmd.clearScreen();
              dmd.end();
              Serial.println("Mode Padam");
            }
          }
          if ((now.jam == setting.nyalaJam) && (now.menit == setting.nyalaMenit))
          {
            modeOperasi = tampilWaktuSholat;
            Serial.println("Mode Normal");
            dmd.begin();
          }
          menitSebelumnya =  now.menit;
        }

        //Timing
        if (modeOperasi == modeWaktuSholat)
        {
          prosedurSholat();
        }
        if (infoWaktu)
        {
          tampilInformasiWaktu();
        }
        else if (modeOperasi == tampilWaktuSholat)
        {
          if (now.detik & 0x01)
          {
            dmd.drawBox((l / 4) - 1, 4, (l / 4), 5);
            dmd.drawBox((l / 4) - 1, 10, (l / 4), 11);
          }
          else
          {
            dmd.drawBox((l / 4) - 1, 4, (l / 4), 5, GRAPHICS_OFF);
            dmd.drawBox((l / 4) - 1, 10, (l / 4), 11, GRAPHICS_OFF);
          }
        }
      }
    }
    else
    {
      RTCValid = true;
    }

    if (!lamaTampil && (modeOperasi != modeWaktuSholat) && (modeOperasi != padam) && (modeOperasi != settingBluetooth))
    {
      prosedurInformasi();
    }
    if (tampilkanMenit && (modeOperasi == tampilWaktuSholat))
    {
      boxKolom1.clear();
      dmd.selectFont(fontWaktu);
      uint8_t lebarText = dmd.stringWidth(textWaktuJam);
      uint8_t tinggiText = dmd.fontHeader.height;
      dmd.drawString(((l / 4) - lebarText) - 2, (t - tinggiText) / 2 , textWaktuJam);
      dmd.drawString((l / 4) + 2, (t - tinggiText) / 2 , &textWaktuJam[3]);

      tampilkanMenit = false;
    }
    efekTampilan();
  }
  cekBluetooth();
  sleep_mode();
}

//=====================Fungsi
//===================================================================Tampilan
void prosedurInformasi()
{
  if (indekTampilan == 0)
  {
    boxKolom1.clear();
    ambilWaktu(now.jam, now.menit);
  }
  if (indekTampilan <= informasiIsya)//tampilkan waktu-waktu sholat
  {
    if (
      ((indekTampilan == informasiImsak) && setting.tampilanImsak)  ||
      ((indekTampilan == informasiTerbit) && setting.tampilanTerbit)  ||
      ((indekTampilan == informasiDhuha) && setting.tampilanDhuha)  ||
      ((indekTampilan == informasiTerbenam) && setting.tampilanTerbenam) ||
      ((indekTampilan != informasiImsak) &&
       (indekTampilan != informasiTerbit) &&
       (indekTampilan != informasiDhuha) &&
       (indekTampilan != informasiTerbenam))
    )
    {
      dmdEfek.clearScreen();
      dmdEfek.selectFont(fontJadwalSholat);
        
      char buf[sizeof(strWaktuSholat[0])];
      byte index = copyFlashString(buf, strWaktuSholat[indekTampilan]);
      buf[index] = 0;
      uint8_t lebarText = dmdEfek.stringWidth(buf);
      dmdEfek.drawString((l / 2) + (((l / 2) - lebarText) / 2), 0, buf);

      formatWaktu(buf, waktuSholat[indekTampilan].jam, waktuSholat[indekTampilan].menit);
      lebarText = dmdEfek.stringWidth(buf);
      dmdEfek.drawString((l / 2) + (((l / 2) - lebarText) / 2), t / 2, buf);
      
      wipeInTampilan = l / 2;
      if (indekTampilan != jadwalImsak)
      {
        wipeOutTampilan = l / 2;
      }

      infoWaktu = false;
      lamaTampil = lamaTampilWaktuSholat;
      modeOperasi = tampilWaktuSholat;
      marqueTampilan = false;
    }
  }
  else if (indekTampilan == informasiJam)
  {
    dmd.clearScreen();
    dmd.selectFont(fontTanggal);
    byte index = formatTanggalStrBulan(strWaktuInformasi, now.tanggal, (char*)&strNamaBulan[now.bulan - 1], now.tahun);
    strWaktuInformasi[index] = 0;
    uint8_t lebarText = dmd.stringWidth(strWaktuInformasi);
    dmd.drawString((l - lebarText) / 2, 0, strWaktuInformasi);


    infoWaktu = true;
    infoWaktuTop = 8;
    tampilInformasiWaktu();
    lamaTampil = lamaTampilInformasiJam;
    modeOperasi = tampilInformasiJam;
    marqueTampilan = false;
  }
  else if ((indekTampilan == informasiTanggal) && setting.tampilanHijriah)//tampilkan tanggal
  {
    boxBaris2.clear();
    dmd.selectFont(fontTanggal);
    byte index = copyFlashString(textTanggalHijriah, strNamaHari[now.hari-1]);
    textTanggalHijriah[index++] = ',';
    textTanggalHijriah[index++] = ' ';
    index += formatTanggalStrBulan(&textTanggalHijriah[index], tanggalHijriah, (char*)&strNamaBulanHijriah[bulanHijriah - 1], tahunHijriah);
    textTanggalHijriah[index++] = ' ';
    textTanggalHijriah[index++] = 'H';
    textTanggalHijriah[index] = 0;

    infoWaktu = false;
    modeOperasi = tampilInformasiTanggal;
    marqueTampilan = true;
    marqueOut = l;
    lamaTampil = 1;

    dmdEfek.clearScreen();
    marqueKiri.sumber = sumberMemory;
    marqueKiri.alamat = &textTanggalHijriah;
    marqueKiri.top = 8;
    marqueKiri.tinggi = 8;
    marqueKiri.left = 0;
    marqueKiri.lebar = l;
    marqueKiri.step = 1;
    marqueKiri.data = textTanggalHijriah[0];
    marqueKiri.posisi = l;
    marqueKiri.panjang = 0;
  }
  else if (indekTampilan == informasiText) //tampilkan text
  {
    uint16_t panjang;
    uint8_t sambung;
    uint8_t font;

    if (indexInformasi > setting.JumlahInfoTextAktif)
    {
      indexInformasi = 1;//0 = informasi sebelum sholat
    }
    uint16_t basisAlamat = jumlahByteToken + jumlahByteWaktu + jumlahByteFloat + jumlahByteSetting + jumlahByteNamaMesjid + jumlahByteJumlahInfo;
    EEPROM.get(basisAlamat + 0 + (indexInformasi << 3), marqueKiri.alamat);
    EEPROM.get(basisAlamat + 2 + (indexInformasi << 3), panjang);
    sambung = EEPROM.read(basisAlamat + 4 + (indexInformasi << 3));
    font = EEPROM.read(basisAlamat + 5 + (indexInformasi << 3));

#if modeDemo
    sambung = 0;
#endif
    if ((panjang > 1))
    {
      if (sambung)
      {
        if (!modeSambungSebelumnya)
        {
          dmd.clearScreen();
        }
        indekTampilan--;
        marqueOut = dmd.fontHeader.fixedWidth;
        marqueKiri.top = (t - dmd.fontHeader.height) / 2;
        marqueKiri.tinggi = t;
      }
      else if (modeSambungSebelumnya)
      {
        marqueOut = l;
        infoWaktu = false;
        marqueKiri.top = (t - dmd.fontHeader.height) / 2;
        marqueKiri.tinggi = t;
      }
      else
      {
        marqueOut = l;
        dmd.clearScreen();
        if (dmd.fontHeader.height <= t / 2)
        {
          infoWaktu = true;
          infoWaktuTop = 0;
          tampilInformasiWaktu();
          marqueKiri.top = (((t/2) - dmd.fontHeader.height) / 2) + (t/2);
          marqueKiri.tinggi = 8;
        }
        else
        {
          marqueKiri.top = (t - dmd.fontHeader.height) / 2;
          marqueKiri.tinggi = t;
        }
      }

      dmd.selectFont(alamatFont[(uint16_t)font]);
      modeOperasi = tampilInformasiText;
      infoWaktu = false;

      marqueKiri.sumber = sumberI2CEEPROM;
      marqueKiri.left = 0;
      marqueKiri.lebar = l;
      marqueKiri.step = 1;
      marqueKiri.data = bacaByteI2C(I2CEEPROM_ADDRESS, marqueKiri.alamat);
      marqueKiri.posisi = l;
      marqueKiri.panjang = 0;

      marqueTampilan = true;
      lamaTampil = 1;
    }
    indexInformasi++;
    modeSambungSebelumnya = sambung;
  }
  else if (indekTampilan == informasiGambar1) //tampilkan gambar1
  {
    efekTulisTampilan = sizeof(siluetMesjid);

    efekTulis.alamat = &siluetMesjid;
    efekTulis.sumber = sumberFlash;
    efekTulis.index = 0;
    efekTulis.posisiX = 0;
    efekTulis.posisiY = 0;

    dmd.clearScreen();
    lamaTampil = 1;
  }
  else if (indekTampilan == informasiGambar2) //tampilkan gambar2
  {
    
//    dmd.clearScreen();
//    dmd.selectFont(AllahMuhammad);
//    dmd.drawString(0, 0, "\"");
//    dmd.drawString(32, 0, "!");

    dmdEfek.clearScreen();
    dmdEfek.selectFont(AllahMuhammad);
    dmdEfek.drawString(0, 0, "\"");
    dmdEfek.drawString(32, 0, "!");
    efekScrollOutDanWipeIn = 32;
    lamaTampil = 100;

//    Serial.println('\1',HEX);
//    Serial.println('\2',HEX);
  }
  if (++indekTampilan >= jumlahInformasi)
  {
    indekTampilan = 0;
  }
}
//===================================================================Efek
void efekTampilan()
{
  //efek tampilan
  if(efekScrollOutDanWipeIn)
  {
    if ((millis() - mulaiMillis) > 10)
    {
      mulaiMillis = millis();
      efekScrollOutDanWipeIn--;
      DMDFrame ScrollOut1 = dmd.subFrame(1, 0, efekScrollOutDanWipeIn, t);
      dmd.copyFrame(ScrollOut1, 0, 0);
      DMDFrame ScrollOut2 = dmd.subFrame(l - efekScrollOutDanWipeIn - 1, 0, efekScrollOutDanWipeIn, t);
      dmd.copyFrame(ScrollOut2, l - efekScrollOutDanWipeIn, 0);

      DMDFrame frameWipeIn1 = dmdEfek.subFrame(efekScrollOutDanWipeIn, 0, 1, t);
      dmd.copyFrame(frameWipeIn1, efekScrollOutDanWipeIn, 0);
      DMDFrame frameWipeIn2 = dmdEfek.subFrame(l - efekScrollOutDanWipeIn - 1, 0, 1, t);
      dmd.copyFrame(frameWipeIn2, l - efekScrollOutDanWipeIn - 1, 0);
    }
  }
  else if(efekTulisTampilan)
  {
    if ((millis() - mulaiMillis) > 20)
    {
      mulaiMillis = millis();

      int8_t x,y;
      uint8_t dataTulis;

      dmd.setPixel(efekTulis.posisiX, t - efekTulis.posisiY -1);
      
      //for (byte datatTulis=0;dataTulis < 16;dataTulis++)
      if(efekTulis.sumber == sumberFlash)
      {
        dataTulis= pgm_read_byte(efekTulis.alamat + efekTulis.index++);
      }
      x = (dataTulis & 0x08)?1:-1 * ((dataTulis>>2) & 0x01);
      y = (dataTulis & 0x02)?1:-1 * ((dataTulis>>0) & 0x01);
      efekTulis.posisiX += x;
      efekTulis.posisiY += y;
      efekTulisTampilan--;
   }
  }
  else if (wipeOutTampilan)
  {
    if ((millis() - mulaiMillis) > 10)
    {
      mulaiMillis = millis();
      wipeOutTampilan--;
      dmd.drawLine(((l / 2) + wipeOutTampilan), 0, ((l / 2) + wipeOutTampilan), t - 1, GRAPHICS_OFF);
    }
  }
  else if (wipeInTampilan)
  {
    if ((millis() - mulaiMillis) > 10)
    {
      mulaiMillis = millis();
      wipeInTampilan--;
      DMDFrame frameWipe = dmdEfek.subFrame(l - wipeInTampilan, 0, 1, t);
      dmd.copyFrame(frameWipe, l - wipeInTampilan, 0);
    }
  }
  else if (marqueTampilan)
  {
    if ((millis() - mulaiMillis) > (1000 / setting.kecepatanJalanInfoText))
    {
      mulaiMillis = millis();
      
      DMDFrame to_move = dmd.subFrame(marqueKiri.step, marqueKiri.top, marqueKiri.lebar, marqueKiri.tinggi);
      dmd.copyFrame(to_move, marqueKiri.left, marqueKiri.top);
      int8_t lebarChar = dmd.charWidth(marqueKiri.data);
      if ((marqueKiri.panjang + lebarChar + marqueKiri.posisi) < marqueKiri.lebar)
      {
        marqueKiri.alamat++;
        if(marqueKiri.sumber == sumberI2CEEPROM)
        {
          marqueKiri.data = bacaByteI2C(I2CEEPROM_ADDRESS, marqueKiri.alamat);
        }
        else if(marqueKiri.sumber == sumberMemory)
        {
          marqueKiri.data = *(uint8_t*)marqueKiri.alamat;
        }
        else if(marqueKiri.sumber == sumberFlash)
        {
           marqueKiri.data = pgm_read_byte(marqueKiri.alamat);
        }
        
        if(marqueKiri.data == 0)
        {
          marqueTampilan = false;
        }
        marqueKiri.panjang += lebarChar + 1;
      }
      dmd.drawChar(marqueKiri.panjang + marqueKiri.posisi - 1, marqueKiri.top, marqueKiri.data);
      marqueKiri.posisi--;
    }
  }
  else if(marqueOut)
  {
    if ((millis() - mulaiMillis) > (1000 / setting.kecepatanJalanInfoText))
    {
      mulaiMillis = millis();
      DMDFrame to_move = dmd.subFrame(marqueKiri.step, marqueKiri.top, marqueKiri.lebar, marqueKiri.tinggi);
      dmd.copyFrame(to_move, marqueKiri.left, marqueKiri.top);
      marqueOut--;
      if(!marqueOut)
      {
        lamaTampil = 0;
      }
    }
  }
  else if ((millis() - mulaiMillis) > 100)
  {
    mulaiMillis = millis();
    if (lamaTampil)
    {
      lamaTampil--;
    }
  }
}
//===================================================================masuk waktu sholat
void prosedurSholat()//dalam loop detik
{
  if (hitungMundurAlarm)
  {
    hitungMundurAlarm--;
    digitalWrite(alarm, statusAlarm);
    statusAlarm = !statusAlarm;
    if (!hitungMundurAlarm)
    {
      dmd.clearScreen();
      if (hitungMundurAdzan)
      {
        dmd.selectFont(fontAdzan);
        uint8_t lebarText = dmd.stringWidth(namaMesjid);
        dmd.drawString((l - lebarText) / 2, 0, namaMesjid);
        
        byte buf[sizeof(strWaktuSholat[0])];
        byte index = copyFlashString(buf, strWaktuSholat[indexAdzan]);
        buf[index++] = ' ';
        index += copyFlashString(&buf[index], strWaktuSholat[indexNamaSholat]);
        buf[index] = 0;
        lebarText = dmd.stringWidth(buf);
        dmd.drawString((l - lebarText) / 2, t / 2, buf);
      }
      digitalWrite(alarm, LOW);
    }
  }
  else if (hitungMundurAdzan)
  {
    hitungMundurAdzan--;
    if (!hitungMundurAdzan)
    {
      dmd.clearScreen();
      if (hitungMundurIqomah)
      {
        dmd.selectFont(fontIqomah);
        byte buf[sizeof(strWaktuSholat[0])];
        byte index = copyFlashString(buf, strWaktuSholat[indexIqomah]);
        buf[index] = 0;
        uint8_t lebarText = dmd.stringWidth(buf);
        dmd.drawString((l - lebarText) / 2, 0, buf);
        RTCValid = true;
      }
    }
  }
  else if (hitungMundurIqomah)
  {
    byte index = bcdToArray(textWaktuJam, bin2bcd(hitungMundurIqomah / 60));
    textWaktuJam[index++] = ':';
    index += bcdToArray(&textWaktuJam[index], bin2bcd(hitungMundurIqomah % 60));
    textWaktuJam[index] = 0;
    dmd.selectFont(fontIqomah);
    uint8_t lebarText = dmd.stringWidth(textWaktuJam);
    dmd.drawString((l - lebarText) / 2, t / 2, textWaktuJam);

    if (hitungMudurAlarmIqomah >= hitungMundurIqomah)
    {
      digitalWrite(alarm, statusAlarm);
      statusAlarm = !statusAlarm;
    }
    hitungMundurIqomah--;
    if (!hitungMundurIqomah)
    {
      digitalWrite(alarm, LOW);
    }
  }
  else if (hitungMundurPesanSebelumSholat)
  {
    if (!lamaTampil)
    {
      hitungMundurPesanSebelumSholat--;
      if (hitungMundurPesanSebelumSholat)
      {
        uint16_t panjang;
        uint8_t sambung;
        uint8_t font;
    
        uint16_t basisAlamat = jumlahByteToken + jumlahByteWaktu + jumlahByteFloat + jumlahByteSetting + jumlahByteNamaMesjid + jumlahByteJumlahInfo;
        EEPROM.get(basisAlamat + 0, marqueKiri.alamat);
        EEPROM.get(basisAlamat + 2, panjang);
        sambung = EEPROM.read(basisAlamat + 4);
        font = EEPROM.read(basisAlamat + 5);

        if ((panjang > 1))
        {
          dmd.clearScreen();
          dmd.selectFont(alamatFont[font]);

          marqueKiri.sumber = sumberI2CEEPROM;
          marqueKiri.top = (t - dmd.fontHeader.height) / 2;
          marqueKiri.tinggi = dmd.fontHeader.height;
          marqueKiri.left = 0;
          marqueKiri.lebar = l;
          marqueKiri.step = 1;
          marqueKiri.data = bacaByteI2C(I2CEEPROM_ADDRESS, marqueKiri.alamat);
          marqueKiri.posisi = l;
          marqueKiri.panjang = 0;

          marqueTampilan = true;
          lamaTampil = 1;
          marqueOut = l;
        }
      }
      else
      {
        marqueTampilan = false;
        dmd.clearScreen();
        dmd.end();
      }
    }
  }
  else if (hitungMundurSholat)
  {
    hitungMundurSholat--;
    if (!hitungMundurSholat)
    {
      ambilWaktu(now.jam, now.menit);
      indekTampilan = 0;
      modeOperasi = tampilWaktuSholat;
      dmd.begin();
    }
  }
}
//===================================================================Setting bluetooth
void cekBluetooth()
{
  while (bluetooth->available())
  {
    char c = bluetooth->read();
    if (c != '\r')
    {
      if (nextEEPROMData)
      {
        safeWriteEEPROM(c);
      }
      else
      {
        bluetoothData[indexParseSerial++] = c;
      }
    }
    else
    {
      bluetoothData[indexParseSerial] = 0;
      if (!tokenValid)
      {
        if (strcmp(bluetoothData, tokenSetInformasi) == 0)
        {
          tokenValid = true;
          indexSetting = alamatJumlahInfoTextAktif;
          indexSettingInfo = 0;
          AlamatI2CEEPROM = 0;
          modeOperasiSebelumnya = modeOperasi;
          modeOperasi = settingBluetooth;
        }
        if (strcmp(bluetoothData, tokenSetSetting) == 0)
        {
          tokenValid = true;
          indexSetting = alamatDetik;
          settingWaktuIndex = 0;
          modeOperasiSebelumnya = modeOperasi;
          modeOperasi = settingBluetooth;
        }
      }
      else
      {
        bluetoothData[indexParseSerial] = 0;
//                Serial.println(bluetoothData);
        if (indexSetting <= alamatTahun)
        {
          uint8_t nilai = atoi(bluetoothData);
           *(&settingWaktu.detik + settingWaktuIndex++) = nilai;
        }
        else if (indexSetting == alamatNamaMesjid)
        {
          uint16_t alamatNama = jumlahByteToken + jumlahByteWaktu + jumlahByteFloat + jumlahByteSetting;

          for (byte i = 0; i < sizeof(namaMesjid), bluetoothData[i]; i++)
          {
            EEPROM.write(alamatNama++, bluetoothData[i]);
          }


          EEPROM.write(alamatTokenBluetooth, tokenBluetooth);
          tokenValid = false;
          modeOperasi = modeOperasiSebelumnya;

          settingWaktu.hari = hariDariTanggal(settingWaktu.tanggal, settingWaktu.bulan, settingWaktu.tahun);
          simpanRTC(RTC_ADDRESS, settingWaktu);

          ambilSetting();

          menitSebelumnya = 60;
          hariSebelumnya = 32;
          lamaTampil = 0;
          ambilWaktu(settingWaktu.jam, settingWaktu.menit);
          ambilHijriah();
          Serial.println("Setting diterima");
        }
        else if (indexSetting <= alamatSudutIsya)
        {
          int16_t nilai = atoi(bluetoothData);
          EEPROM.put(((indexSetting - jumlahToken - jumlahWaktu) << 1) + jumlahByteToken + jumlahByteWaktu, (int16_t)nilai);
        }
        else if (indexSetting <= alamatTampilanTerbenam)
        {
          int8_t nilai = atoi(bluetoothData);
          EEPROM.put(((indexSetting - jumlahToken - jumlahWaktu - jumlahFloat)) + jumlahByteToken + jumlahByteWaktu + jumlahByteFloat, (int8_t)nilai);
        }
        else if (indexSetting == alamatJumlahInfoTextAktif)
        {
          int8_t nilai = atoi(bluetoothData);
          EEPROM.put(jumlahByteToken + jumlahByteWaktu + jumlahByteFloat + jumlahByteSetting + jumlahByteNamaMesjid, (int8_t)nilai);
          if (indexSetting == alamatJumlahInfoTextAktif)
          {
            setting.JumlahInfoTextAktif = nilai;
            if (setting.JumlahInfoTextAktif == 0)
            {
              tokenValid = false;
              nextEEPROMData = false;
            }
            nextEEPROMData = true;
            EEPROMDataCounter = 0;
            Wire.beginTransmission(I2CEEPROM_ADDRESS);
            Wire.write((uint8_t)0);
            Wire.write((uint8_t)0);
            AlamatI2CEEPROMSebelumnya = 0;
          }
        }
        else
        {
          uint8_t index = (indexSetting - alamatInfoText_SebelumSholat) & 0x03;
          uint16_t basisAlamat = jumlahByteToken + jumlahByteWaktu + jumlahByteFloat + jumlahByteSetting + jumlahByteNamaMesjid + jumlahByteJumlahInfo;
          switch (index)
          {
            case 0:
              safeWriteEEPROM(0);
              EEPROM.put(basisAlamat + 0 + (indexSettingInfo), (int16_t)AlamatI2CEEPROMSebelumnya);
              AlamatI2CEEPROMSebelumnya = AlamatI2CEEPROM;
              nextEEPROMData = false;
              break;
            case 1:
              EEPROM.put(basisAlamat + 2 + (indexSettingInfo), (int16_t)(atoi(bluetoothData)));
              break;
            case 2:
              EEPROM.put(basisAlamat + 4 + (indexSettingInfo), (int8_t)(atoi(bluetoothData)));
              break;
            case 3:
              EEPROM.put(basisAlamat + 5 + (indexSettingInfo), (int8_t)(atoi(bluetoothData)));
              nextEEPROMData = true;
              indexSettingInfo += 8;
              break;
          }
          if ((indexSettingInfo>>3) == setting.JumlahInfoTextAktif + 1)//+info sebelum sholat
          {
            if (AlamatI2CEEPROM & 0x0F)
            {
              Wire.endTransmission();
            }
            lamaTampil = 0;
            modeOperasi = modeOperasiSebelumnya;
            tokenValid = false;
            nextEEPROMData = false;
            Serial.println("Informasi diterima");
          }
        }
        indexSetting++;
      }
      indexParseSerial = 0;
    }
  }
}
//===================================================================Fungsi pendukung
void safeWriteEEPROM(byte b)
{
  delayMicroseconds(100);
  Wire.write((char)b);
  EEPROMDataCounter++;
  AlamatI2CEEPROM++;
  if (EEPROMDataCounter == 16)
  {
    EEPROMDataCounter = 0;
    Wire.endTransmission();
    delay(10);
    Wire.beginTransmission(I2CEEPROM_ADDRESS);
    Wire.write((uint8_t)(AlamatI2CEEPROM >> 8));
    Wire.write((uint8_t)(AlamatI2CEEPROM & 0xFF));
  }
}
void ambilSetting()
{
//  prayerData
  int16_t *alamat16 = &prayerData.lintang;
  for (byte i = 0; i <= alamatSudutIsya - alamatLintang; i++)
  {
    EEPROM.get(jumlahByteToken + jumlahByteWaktu + (i << 1), *alamat16);
    alamat16++;
  }
  prayerData.aturDzuhur = EEPROM.read(jumlahByteToken + jumlahByteWaktu + jumlahByteFloat + 0);
  prayerData.aturAshar = EEPROM.read(jumlahByteToken + jumlahByteWaktu + jumlahByteFloat + 1);
  
  int8_t *alamat8 = &setting.koreksiHijriah;
  for (byte i = 0; i <= alamatTampilanTerbenam - alamatKoreksiHijriah; i++)
  {
    *alamat8 = EEPROM.read(jumlahByteToken + jumlahByteWaktu + jumlahByteFloat + i + 2);
    alamat8++;
  }
  uint16_t alamatNama = jumlahByteToken + jumlahByteWaktu + jumlahByteFloat + jumlahByteSetting;
  for (byte i = 0; i < panjangString; i++)
  {
    char c = EEPROM.read(alamatNama++);
    namaMesjid[i] = c;
    if (c == 0)
    {
      break;
    }
  }
  setting.JumlahInfoTextAktif =  EEPROM.read(jumlahByteToken + jumlahByteWaktu + jumlahByteFloat + jumlahByteSetting + jumlahByteNamaMesjid);
  if (setting.kecerahanJWS == 64)
  {
    dmd.setBrightness(255);
  }
  else
  {
    dmd.setBrightness(setting.kecerahanJWS << 2);
  }
}
void ambilHijriah()
{
  uint32_t jumlahHari;
  uint16_t menitMagrib;
  uint16_t menitSekarang;

  now = ambilRTC(RTC_ADDRESS);

  jumlahHari = jumlahHariDariTanggal(now.tanggal, now.bulan, now.tahun - 2000) + setting.koreksiHijriah;
  menitMagrib = (waktuSholat[indexMagrib].jam * 60) + waktuSholat[indexMagrib].menit;
  menitSekarang = (now.jam * 60) + now.menit;
  if (menitSekarang >= menitMagrib)
  {
    jumlahHari++;//Pergantian hari hijrah pada magrib
  }
  masehiKeHijriah(jumlahHari, &tanggalHijriah, &bulanHijriah, &tahunHijriah);
}
void ambilWaktu(byte jam, byte menit)
{
  formatWaktu(textWaktuJam, jam, menit);
  textWaktuJam[2] = 0;
  tampilkanMenit = true;
}
void tampilInformasiWaktu()
{
  dmd.selectFont(fontInformasi);

  byte jamBCD = bin2bcd(now.jam);
  byte menitBCD = bin2bcd(now.menit);
  byte detikBCD = bin2bcd(now.detik);

  byte index = 0;
  index = bcdToArray(strWaktuInformasi + index, jamBCD);
  strWaktuInformasi[index++] = ':';
  index += bcdToArray(strWaktuInformasi + index, menitBCD);
  strWaktuInformasi[index++] = ':';
  index += bcdToArray(strWaktuInformasi + index, detikBCD);
  strWaktuInformasi[index] = 0;

  uint8_t lebarText = dmd.stringWidth(strWaktuInformasi);
  dmd.drawString((l - lebarText) / 2, infoWaktuTop, strWaktuInformasi);
}
//30964
//30898
//30874
//30868
//30710
//30080

