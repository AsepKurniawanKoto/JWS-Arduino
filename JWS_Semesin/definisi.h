#ifndef DEFINISI_H
#define DEFINISI_H

#define IterasiKomputasiJadwalSholat 1

enum indekJadwal
{
  jadwalImsak,
  jadwalSubuh,
  jadwalTerbit,
  jadwalDhuha,
  jadwalDzuhur,
  jadwalAshar,
  jadwalTerbenam,
  jadwalMaghrib,
  jadwalIsya,
	jumlahJadwal,
};

enum indekInformasi
{
  informasiImsak,
  informasiSubuh,
  informasiTerbit,
  informasiDhuha,
  informasiDzuhur,
  informasiAshar,
  informasiTerbenam,
  informasiMaghrib,
  informasiIsya,
	informasiJam,
	informasiTanggal,
	informasiText,
	informasiGambar1,
	informasiGambar2,
	// informasiGambar3,
	// informasiGambar4,
	jumlahInformasi,
};

#define panjangString						17

#define jumlahToken  						2
#define jumlahByteToken  				(2 * 1)
#define jumlahWaktu  						7
#define jumlahByteWaktu  				(7 * 0)
#define jumlahFloat  						8
#define jumlahByteFloat  				(8 * 2)
#define jumlahSetting  					28
#define jumlahByteSetting				(28 * 1)
#define jumlahNamaMesjid				1
#define jumlahByteNamaMesjid  	(1 * panjangString)
#define jumlahJumlahInfo				1
#define jumlahByteJumlahInfo  	(1 * 1)
#define jumlahTextInfo					11
#define jumlahByteTextInfo 	 		(11 * 4)//+ padding 2 byte

enum _alamatDataEEPROM
{
  alamatTokenRTC,
  alamatTokenBluetooth,
	
  alamatDetik,
  alamatMenit,
  alamatJam,
  alamatHari,
  alamatTanggal,
  alamatBulan,
  alamatTahun,
	
  alamatLintang,
  alamatBujur,
  alamatZonaWaktu,
	alamatSudutFajar,
	alamatSudutTerbit,
	alamatSudutDhuha,
	alamatSudutMagrib,
	alamatSudutIsya,
	
	alamatAturDzuhur,
	alamatAturAshar,
  alamatKoreksiHijriah,
  alamatJumlahAlarm,
  alamatLamaAdzan,
  alamatLamaIqomahSubuh,
  alamatLamaIqomahDzuhur,
  alamatLamaIqomahAsar,
  alamatLamaIqomahMagrib,
  alamatLamaIqomahIsya,
  alamatUlangiPesanSebelumSholat,
  alamatLamaSholatSubuh,
  alamatLamaSholatDzuhur,
  alamatLamaSholatAsar,
  alamatLamaSholatMagrib,
  alamatLamaSholatIsya,
  alamatLamaSholatJumat,
  alamatKecepatanJalanInfoText,
  alamatKecerahanJWS,
  alamatPadamJam,
  alamatPadamMenit,
  alamatNyalaJam,
  alamatNyalaMenit,
  alamatTampilanHijriah,
  alamatTampilanImsak,
  alamatTampilanTerbit,
  alamatTampilanDhuha,
  alamatTampilanTerbenam,
	
  alamatNamaMesjid,
	
  alamatJumlahInfoTextAktif,
	
  alamatInfoText_SebelumSholat,
  alamatPanjangText_SebelumSholat,
  alamatSambungText_SebelumSholat,
  alamatFontText_SebelumSholat,
  alamatInfoText_0,
  alamatPanjangText_0,
  alamatSambungText_0,
  alamatFontText_0,
  alamatInfoText_1,
  alamatPanjangText_1,
  alamatSambungText_1,
  alamatFontText_1,
  alamatInfoText_2,
  alamatPanjangText_2,
  alamatSambungText_2,
  alamatFontText_2,
  alamatInfoText_3,
  alamatPanjangText_3,
  alamatSambungText_3,
  alamatFontText_3,
  alamatInfoText_4,
  alamatPanjangText_4,
  alamatSambungText_4,
  alamatFontText_4,
  alamatInfoText_5,
  alamatPanjangText_5,
  alamatSambungText_5,
  alamatFontText_5,
  alamatInfoText_6,
  alamatPanjangText_6,
  alamatSambungText_6,
  alamatFontText_6,
  alamatInfoText_7,
  alamatPanjangText_7,
  alamatSambungText_7,
  alamatFontText_7,
  alamatInfoText_8,
  alamatPanjangText_8,
  alamatSambungText_8,
  alamatFontText_8,
  alamatInfoText_9,
  alamatPanjangText_9,
  alamatSambungText_9,
  alamatFontText_9,
  // StrNamaMesjid,
};

enum _modeOperasi
{
  tampilWaktuSholat,
  tampilInformasiJam,
  tampilInformasiTanggal,
  tampilInformasiSebelumSholat,
  tampilInformasiText,
  modeWaktuSholat,
	settingBluetooth,
	settingTombol,
  padam,
};

enum _sumberInformasi
{
	sumberMemory,
	sumberFlash,
	sumberI2CEEPROM,
};
	
struct EfekMarqueHorizontalkeKiri
{
	uint8_t sumber;
	uint16_t alamat;
	uint8_t top;
	uint8_t tinggi;
	uint8_t left;
	uint8_t lebar;
	uint8_t step;
	char data;
	uint16_t posisi;
	uint16_t panjang;
};

struct EfekTulis
{
	uint8_t sumber;
	uint16_t alamat;
	uint16_t index;
	uint8_t efekTulis;
	uint8_t efekTulisDataIndex;
	int8_t posisiX;
	int8_t posisiY;

};
	

struct JamDanMenit
{
  uint8_t jam;
  uint8_t menit;
};

struct tanggalDanWaktu
{
  uint8_t detik;
  uint8_t menit;
  uint8_t jam;
  uint8_t hari;
  uint8_t tanggal;
  uint8_t bulan;
  uint16_t tahun;
};


// set_high_lats_adjust_method
// set_fajr_angle
// set_dhuhr_minutes
// set_asr_method
// set_maghrib_minutes
// set_isha_minutes

// set_maghrib_angle
// set_isha_angle

// Abaikan,Tengah malam,Satu ketujuh,Sudut




#endif
