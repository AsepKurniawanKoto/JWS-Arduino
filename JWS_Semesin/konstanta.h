#ifndef KONSTANTA_H
#define KONSTANTA_H

#define xmym	0x5
#define xmy0	0x4
#define xmyp	0x7
#define x0ym	0x1
#define x0y0	0x0
#define x0yp	0x3
#define xpym	0xD
#define xpy0	0xC
#define xpyp	0xF

const byte siluetMesjid[] PROGMEM =  {
	xpyp,xpy0,xpyp,xpy0,
	xpyp,xpy0,xpy0,xpyp,
	x0yp,x0ym,xpym,xpy0,
	xpy0,xpym,xpy0,xpym,
	xpy0,xpym,xpy0,xpy0,xpyp,x0yp,
	xmyp,xmyp,x0yp,xpyp,
	xpyp,xpyp,x0yp,x0ym,
	xpym,xpym,xpym,x0ym,xmym,xmym,x0ym,
	xpym,xpy0,xpy0,xpy0,xpy0,xpy0,
	xpyp,x0yp,xmyp,xmyp,xmyp,
	x0yp,x0yp,x0yp,xpyp,
	xpyp,xpyp,xpyp,xpyp,
	xpyp,x0yp,xpy0,x0ym,
	xpym,xpym,xpym,xpym,
	xpym,xpym,x0ym,x0ym,
	x0ym,xmym,xmym,
	xmym,x0ym,xpym,xpy0,
	xpy0,xpy0,xpy0,xpy0,
	xpyp,x0yp,
	xmyp,xmyp,x0yp,xpyp,
	xpyp,xpyp,x0yp,x0ym,
	xpym,xpym,xpym,x0ym,xmym,xmym,x0ym,
	xpym,xpy0,xpy0,
	xpyp,xpy0,xpyp,xpy0,
	xpyp,xpy0,xpy0,xpyp,
	x0yp,x0ym,xpym,xpy0,
	xpy0,xpym,xpy0,xpym,
	xpy0,xpym,x0y0,
};

//konstanta
const char strWaktuSholat[][9] PROGMEM = {
																				{"Imsak\0  "}, 
                                        {"Subuh\0  "}, 
                                        {"Terbit\0 "}, 
                                        {"Dhuha\0  "}, 
                                        {"Dzuhur\0 "}, 
                                        {"Ashar\0  "}, 
                                        {"Terbenam"}, 
                                        {"Magrib\0 "}, 
                                        {"Isya\0   "}, 
                                        {"Adzan\0  "}, 
                                        {"Iqomah\0 "}, 
                                        {"Jum'at\0 "}
																				};
#define indexMagrib 7
#define indexAdzan 9
#define indexIqomah 10
#define indexJumat 11

const char strNamaBulan[][4]  PROGMEM = {
																				{"Jan"}, 
																				{"Feb"},
																				{"Mar"},
																				{"Apr "},
																				{"Mei"},
																				{"Jun"},
																				{"Jul"},
																				{"Agt"},
																				{"Sep"},
																				{"Okt"},
																				{"Nov"},
																				{"Des"},
																				};																 
const char strNamaHari[][7]  PROGMEM = {
																				{"Minggu"}, 
																				{"Senin\0"},
																				{"Selasa"},
																				{"Rabu\0 "},
																				{"Kamis\0"},
																				{"Jum'at"},
																				{"Sabtu\0"},
																				};																 
const char strNamaBulanHijriah[][14] PROGMEM = { 
																				{"Muharram\0    "},
                                        {"Shafar\0      "},    
                                        {"Rabiul Awal\0 "},
                                        {"Rabiul Akhir\0"}, 
                                        {"Jumadil Awal\0"},
                                        {"Jumadil Akhir"},
                                        {"Rajab\0       "},
                                        {"Sya'ban\0     "},
                                        {"Ramadhan\0    "},
                                        {"Syawal\0      "},
                                        {"Dzulqa'dah\0  "},
                                        {"Dzulhijah\0   "}
																			  };
																						 
#endif																						 