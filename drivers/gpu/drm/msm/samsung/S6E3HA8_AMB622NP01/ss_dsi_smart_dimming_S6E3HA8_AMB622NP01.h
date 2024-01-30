/*
 * =================================================================
 *
 *       Filename:  smart_mtp_s6e3.h
 *
 *    Description:  Smart dimming algorithm implementation
 *
 *        Company:  Samsung Electronics
 *
 * ================================================================
 */
/*
<one line to give the program's name and a brief idea of what it does.>
Copyright (C) 2012, Samsung Electronics. All rights reserved.

 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _SS_DSI_SMART_DIMMING_S6E3HA8_AMB622NP01_H_
#define _SS_DSI_SMART_DIMMING_S6E3HA8_AMB622NP01_H_

#include "ss_dsi_panel_common.h"
#include "ss_dsi_smart_dimming_common.h"

static int id1, id2, id3;

#define LUMINANCE_MAX 74
#define HMT_LUMINANCE_MAX 37
#define GAMMA_SET_MAX 34
#define GRAY_SCALE_MAX 256
#define RGB_COMPENSATION 27

/* BIT_SHIFT is used for right bit shfit */
#define BIT_SHIFT 22
#define BIT_SHFIT_MUL 4194304 // pow(2,BIT_SHIFT)

#define VREG0_REF_6P8 28521267	/* 6.8*4194304 */
#define VREG0_REF_6P5 27262976	/* 6.5*4194304 */
#define VREG0_REF_6P4 26843546	/* 6.4*4194304 */
#define VREG0_REF_6P3 26424115	/* 6.3*4194304 */
#define VREG0_REF_6P2 26004685	/* 6.2*4194304 */

#define HBM_INTERPOLATION_STEP 12 /* 443, 467, 490, 513, 537, 560, 583, 606, 630, 653, 676, 700 */
#define HBM_INTERPOLATION_STEP_revG 60

enum {
	GAMMA_CURVE_1P9 = 0,
	GAMMA_CURVE_2P15,
	GAMMA_CURVE_2P2,
	GAMMA_CURVE_2P2_400CD,
};

/* TP */
enum {
	VT = 0,
	V0,
	V1,
	V7,
	V11,
	V23,
	V35,
	V51,
	V87,
	V151,
	V203,
	V255,
	V_MAX,
};

/* RGB */
enum {
	R = 0,
	G = 1,
	B = 2,
	RGB_MAX,
};

static char V_LIST[V_MAX][5] = {
	"VT",
	"V0",
	"V1",
	"V7",
	"V11",
	"V23",
	"V35",
	"V51",
	"V87",
	"V151",
	"V203",
	"V255",
};

struct illuminance_table {
	int lux;
	char gamma_setting[GAMMA_SET_MAX];
} __packed;

struct SMART_DIM {
	/* read C8h 1st ~ 35th */
	char MTP_ORIGN[GAMMA_SET_MAX];

	/* copy MTP_ORIGN -> MTP */
	int MTP[V_MAX][RGB_MAX];

	/* TP's gamma voltage */
	int RGB_OUTPUT[V_MAX][RGB_MAX];

	/* GRAY (0~255) */
	int GRAY[GRAY_SCALE_MAX][RGB_MAX];

	/* Because of AID funtion, below members are added*/
	int lux_table_max;
	int *plux_table;
	struct illuminance_table gen_table[LUMINANCE_MAX];

	int brightness_level;
	int ldi_revision;
	int vregout_voltage;
	char panel_revision;

	/* HBM interpolation */
	struct illuminance_table hbm_interpolation_table[HBM_INTERPOLATION_STEP_revG+1];
	char *hbm_payload;
	int hbm_brightness_level;

	struct illuminance_table hmt_gen_table[HMT_LUMINANCE_MAX];
} __packed;

/* V0,V1,V7,V11,V23,V35,V51,V87,V151,V203,V255 */
static int INFLECTION_VOLTAGE_ARRAY[V_MAX] = {0, 0, 1, 7, 11, 23, 35, 51, 87, 151, 203, 255};

static int vt_coefficient[] = {
	0,  12,  24,  36,
	48,  60,  72,  84,
	96, 108, 138, 148,
	158, 168, 178, 186
};

static int v0_coefficient[] = {
	0, 6, 12, 18,
	24, 30, 36, 42,
	48, 54, 60, 66,
	72, 78, 84, 90
};

/* center (max) gamma value (Hex) */
static int center_gamma[V_MAX][RGB_MAX] = {
	{0x0, 0x0, 0x0},	/* VT */
	{0x0, 0x0, 0x0},	/* V0 */
	{0x80, 0x80, 0x80},	/* V1 */
	{0x80, 0x80, 0x80},	/* V7 */
	{0x80, 0x80, 0x80},	/* V11 */
	{0x80, 0x80, 0x80},	/* V23 */
	{0x80, 0x80, 0x80},	/* V35 */
	{0x80, 0x80, 0x80},	/* V51 */
	{0x80, 0x80, 0x80},	/* V87 */
	{0x80, 0x80, 0x80},	/* V151 */
	{0x80, 0x80, 0x80},	/* V203 */
	{0x100, 0x100, 0x100},	/* V255 */
};

/* fraction for gamma code */
static int fraction[V_MAX][2] = {
/* {numerator, denominator} */
	{0,  860},		/* VT */
	{0,  860},		/* V0 */
	{0,  256},		/* V1 */
	{64, 320},		/* V7 */
	{64, 320},		/* V11 */
	{64, 320},		/* V23 */
	{64, 320},		/* V35 */
	{64, 320},		/* V51 */
	{64, 320},		/* V87 */
	{64, 320},		/* V151 */
	{64, 320},		/* V203 */
	{129, 860},		/* V255 */
};

/* 443 is made but not used */
static int hbm_interpolation_candela_table_revA[HBM_INTERPOLATION_STEP] = {443, 467, 490, 513, 537, 560, 583, 606, 630, 653, 676, 700};
static int hbm_interpolation_candela_table_revG[HBM_INTERPOLATION_STEP_revG] = {405, 410, 415, 420, 425, 430, 435, 440, 445, 450,
										455, 460, 465, 470, 475, 480, 485, 490, 495, 500,
										505, 510, 515, 520, 525, 530, 535, 540, 545, 550,
										555, 560, 565, 570, 575, 580, 585, 590, 595, 600,
										605, 610, 615, 620, 625, 630, 635, 640, 645, 650,
										655, 660, 665, 670, 675, 680, 685, 690, 695, 700};

/* STAR2 rev.A ~ F */
static unsigned int base_luminance_star_revA[LUMINANCE_MAX][2] = {
	{2,	107},
	{3,	107},
	{4,	107},
	{5,	107},
	{6,	107},
	{7,	107},
	{8,	107},
	{9,	107},
	{10,	107},
	{11,	107},
	{12,	107},
	{13,	107},
	{14,	107},
	{15,	107},
	{16,	107},
	{17,	107},
	{19,	107},
	{20,	107},
	{21,	107},
	{22,	107},
	{24,	107},
	{25,	107},
	{27,	107},
	{29,	107},
	{30,	107},
	{32,	107},
	{34,	107},
	{37,	107},
	{39,	107},
	{41,	107},
	{44,	107},
	{47,	107},
	{50,	107},
	{53,	107},
	{56,	107},
	{60,	112},
	{64,	119},
	{68,	125},
	{72,	131},
	{77,	140},
	{82,	149},
	{87,	159},
	{93,	168},
	{98,	177},
	{105,	190},
	{111,	200},
	{119,	212},
	{126,	220},
	{134,	233},
	{143,	249},
	{152,	260},
	{162,	271},
	{172,	280},
	{183,	294},
	{195,	294},
	{207,	294},
	{220,	294},
	{234,	294},
	{249,	300},
	{265,	316},
	{282,	332},
	{300,	348},
	{316,	361},
	{333,	378},
	{350,	388},
	{357,	395},
	{365,	402},
	{372,	402},
	{380,	402},
	{387,	402},
	{395,	402},
	{403,	406},
	{412,	409},
	{420,	420}
};

/* STAR2 rev.G */
static unsigned int base_luminance_star_revG[LUMINANCE_MAX][2] = {
	{2,	102},
	{3,	102},
	{4,	102},
	{5,	102},
	{6,	102},
	{7,	102},
	{8,	102},
	{9,	102},
	{10,	102},
	{11,	102},
	{12,	102},
	{13,	102},
	{14,	102},
	{15,	102},
	{16,	102},
	{17,	102},
	{18,	102},
	{19,	102},
	{20,	102},
	{21,	102},
	{23,	102},
	{24,	102},
	{26,	102},
	{27,	102},
	{29,	102},
	{31,	102},
	{33,	102},
	{35,	102},
	{37,	102},
	{39,	102},
	{42,	102},
	{45,	102},
	{48,	102},
	{51,	102},
	{54,	102},
	{57,	104},
	{61,	109},
	{65,	116},
	{69,	119},
	{73,	128},
	{78,	136},
	{83,	147},
	{88,	151},
	{94,	162},
	{100,	173},
	{106,	186},
	{113,	193},
	{120,	205},
	{128,	222},
	{136,	232},
	{145,	237},
	{154,	256},
	{164,	264},
	{174,	275},
	{185,	275},
	{197,	275},
	{210,	275},
	{223,	275},
	{237,	283},
	{253,	301},
	{269,	310},
	{286,	322},
	{301,	337},
	{317,	357},
	{333,	366},
	{340,	376},
	{347,	380},
	{354,	380},
	{362,	380},
	{369,	380},
	{376,	380},
	{384,	385},
	{392,	390},
	{400,	400}
};

static int gradation_offset_star_revG[LUMINANCE_MAX][9] = {
/*	V255 V203 V151 V87 V51 V35 V23 V11 V7 */
	{0,	4,	7,	13,	18,	20,	22,	23,	24},
	{0,	3,	5,	10,	14,	16,	18,	21,	22},
	{0,	3,	5,	9,	11,	13,	16,	19,	20},
	{0,	3,	4,	8,	10,	11,	14,	17,	18},
	{0,	2,	4,	6,	8,	10,	13,	16,	17},
	{0,	2,	3,	6,	8,	10,	13,	16,	17},
	{0,	2,	3,	6,	7,	9,	11,	15,	16},
	{0,	2,	2,	5,	6,	8,	10,	15,	16},
	{0,	3,	3,	5,	6,	8,	10,	15,	16},
	{0,	2,	3,	5,	7,	9,	10,	15,	16},
	{0,	2,	3,	5,	7,	9,	11,	16,	17},
	{0,	2,	3,	6,	7,	9,	11,	15,	16},
	{0,	2,	4,	7,	8,	10,	12,	16,	17},
	{0,	2,	4,	7,	8,	10,	12,	16,	17},
	{0,	2,	3,	7,	8,	10,	12,	16,	17},
	{0,	2,	3,	5,	6,	7,	10,	14,	15},
	{0,	2,	3,	5,	6,	7,	10,	14,	15},
	{0,	2,	3,	5,	5,	6,	9,	13,	14},
	{0,	1,	3,	5,	6,	7,	9,	12,	13},
	{0,	2,	3,	4,	7,	7,	9,	12,	13},
	{0,	1,	3,	4,	6,	6,	8,	12,	13},
	{0,	1,	3,	3,	6,	6,	7,	11,	12},
	{0,	1,	2,	3,	5,	5,	7,	11,	12},
	{0,	1,	2,	3,	4,	5,	7,	10,	12},
	{0,	1,	3,	4,	5,	5,	7,	10,	11},
	{0,	1,	3,	3,	4,	4,	6,	8,	9},
	{0,	1,	2,	2,	3,	4,	6,	8,	9},
	{0,	1,	1,	2,	3,	3,	5,	8,	9},
	{0,	1,	1,	2,	3,	3,	5,	7,	8},
	{0,	1,	1,	2,	3,	3,	5,	7,	8},
	{0,	2,	2,	2,	2,	2,	5,	6,	7},
	{0,	2,	2,	2,	3,	2,	4,	5,	6},
	{0,	1,	1,	2,	3,	2,	4,	5,	6},
	{0,	1,	1,	1,	2,	2,	4,	4,	5},
	{0,	1,	1,	1,	2,	2,	4,	4,	5},
	{0,	2,	1,	1,	1,	1,	3,	4,	5},
	{0,	2,	2,	2,	2,	1,	2,	3,	5},
	{0,	2,	1,	2,	2,	1,	2,	3,	4},
	{0,	2,	2,	2,	2,	1,	2,	3,	4},
	{0,	2,	3,	3,	2,	1,	2,	3,	4},
	{0,	2,	3,	2,	2,	1,	2,	3,	4},
	{0,	2,	2,	2,	2,	2,	2,	3,	4},
	{0,	3,	3,	3,	3,	2,	2,	3,	4},
	{0,	2,	3,	3,	2,	1,	2,	3,	4},
	{0,	2,	2,	2,	2,	2,	1,	3,	4},
	{0,	2,	2,	3,	3,	2,	2,	3,	4},
	{0,	2,	2,	3,	3,	2,	2,	3,	4},
	{0,	2,	2,	3,	2,	2,	1,	1,	3},
	{0,	2,	2,	2,	2,	1,	1,	1,	2},
	{0,	2,	3,	2,	2,	1,	1,	2,	3},
	{0,	4,	3,	2,	2,	1,	1,	2,	2},
	{0,	1,	3,	3,	0,	1,	1,	1,	2},
	{0,	2,	3,	3,	0,	1,	1,	1,	2},
	{0,	2,	3,	3,	0,	1,	1,	1,	2},
	{0,	3,	4,	3,	1,	1,	1,	1,	2},
	{0,	2,	3,	2,	0,	0,	0,	0,	1},
	{0,	1,	3,	1,	0,	0,	0,	0,	1},
	{0,	1,	3,	1,	0,	0,	0,	0,	1},
	{0,	1,	3,	1,	0,	0,	-1,	0,	1},
	{0,	1,	3,	1,	0,	0,	0,	0,	0},
	{0,	0,	1,	0,	-1,	-1,	-2,	-1,	0},
	{0,	1,	2,	0,	-1,	-1,	-2,	-1,	0},
	{0,	1,	2,	0,	-1,	-1,	-2,	-1,	0},
	{0,	0,	2,	-1,	-1,	-1,	-2,	-1,	0},
	{0,	0,	2,	-1,	-1,	-1,	-2,	-1,	0},
	{0,	0,	2,	-1,	-1,	-1,	-2,	-1,	0},
	{0,	0,	2,	-1,	-1,	-1,	-3,	-2,	0},
	{0,	1,	0,	-1,	-1,	-1,	-3,	-2,	0},
	{0,	1,	0,	-1,	-1,	-1,	-3,	-2,	0},
	{0,	0,	0,	-1,	-2,	-2,	-3,	-2,	0},
	{0,	-1,	0,	-1,	-1,	-1,	-3,	-2,	0},
	{0,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	0},
	{0,	-1,	-1,	-2,	-1,	-1,	-1,	-1,	0},
	{0,	0,	0,	0,	0,	0,	0,	0,	0},
};

static int gradation_offset_star_revA[LUMINANCE_MAX][9] = {
/*	V255 V203 V151 V87 V51 V35 V23 V11 V7 */
	{0,	7,	10,	16,	21,	24,	28,	30,	32},
	{0,	4,	6,	11,	16,	18,	21,	24,	25},
	{0,	5,	6,	10,	14,	15,	19,	22,	23},
	{0,	4,	4,	8,	12,	13,	16,	19,	20},
	{0,	3,	3,	7,	11,	12,	15,	18,	19},
	{0,	3,	3,	7,	10,	12,	14,	17,	18},
	{0,	3,	3,	6,	10,	11,	13,	17,	18},
	{0,	3,	3,	5,	9,	11,	13,	16,	17},
	{0,	3,	3,	5,	9,	10,	13,	16,	17},
	{0,	3,	3,	5,	9,	11,	13,	16,	17},
	{0,	3,	3,	6,	10,	11,	14,	17,	18},
	{0,	3,	3,	6,	10,	11,	14,	17,	18},
	{0,	3,	3,	7,	10,	11,	14,	17,	18},
	{0,	3,	4,	7,	11,	13,	15,	19,	20},
	{0,	3,	3,	6,	10,	12,	15,	18,	20},
	{0,	3,	3,	6,	10,	11,	14,	17,	19},
	{0,	2,	3,	5,	9,	10,	13,	16,	17},
	{0,	2,	3,	5,	8,	10,	12,	15,	17},
	{0,	2,	3,	4,	8,	9,	12,	15,	16},
	{0,	2,	3,	4,	8,	9,	11,	15,	16},
	{0,	2,	3,	4,	7,	8,	10,	14,	15},
	{0,	2,	2,	3,	7,	8,	10,	13,	15},
	{0,	2,	2,	3,	6,	7,	9,	13,	14},
	{0,	2,	2,	3,	6,	7,	9,	12,	13},
	{0,	2,	2,	3,	6,	6,	8,	12,	13},
	{0,	2,	1,	3,	5,	6,	8,	11,	12},
	{0,	2,	1,	2,	5,	5,	7,	11,	12},
	{0,	1,	1,	2,	5,	5,	7,	10,	11},
	{0,	1,	1,	2,	4,	4,	7,	9,	10},
	{0,	1,	1,	1,	4,	4,	6,	9,	10},
	{0,	1,	1,	1,	4,	4,	6,	8,	9},
	{0,	1,	1,	1,	3,	3,	5,	8,	9},
	{0,	1,	1,	1,	3,	3,	5,	7,	8},
	{0,	1,	1,	1,	3,	2,	4,	7,	8},
	{0,	0,	0,	1,	2,	2,	4,	6,	7},
	{0,	1,	0,	0,	2,	3,	4,	6,	7},
	{0,	1,	0,	1,	2,	2,	3,	5,	7},
	{0,	1,	1,	1,	3,	3,	4,	5,	7},
	{0,	3,	1,	2,	2,	2,	4,	5,	7},
	{0,	2,	1,	1,	2,	3,	3,	6,	6},
	{0,	2,	2,	1,	2,	2,	4,	6,	7},
	{0,	1,	1,	1,	2,	2,	3,	5,	6},
	{0,	1,	2,	2,	2,	3,	3,	5,	7},
	{0,	0,	2,	2,	2,	2,	4,	5,	7},
	{0,	0,	1,	2,	2,	2,	3,	5,	7},
	{0,	-1,	2,	1,	2,	3,	3,	5,	7},
	{0,	-1,	2,	3,	2,	2,	3,	5,	6},
	{0,	0,	3,	3,	2,	3,	3,	4,	6},
	{0,	-1,	2,	3,	2,	2,	2,	4,	6},
	{0,	-2,	2,	2,	1,	2,	2,	4,	6},
	{0,	0,	2,	2,	1,	3,	2,	5,	6},
	{0,	2,	3,	2,	2,	2,	2,	3,	5},
	{0,	2,	4,	3,	2,	3,	2,	3,	5},
	{0,	1,	4,	3,	2,	2,	2,	3,	5},
	{0,	1,	3,	2,	2,	2,	1,	3,	5},
	{0,	1,	3,	2,	1,	2,	1,	2,	4},
	{0,	0,	2,	1,	1,	1,	0,	2,	4},
	{0,	0,	1,	0,	1,	0,	0,	1,	3},
	{0,	-1,	1,	0,	0,	1,	0,	1,	2},
	{0,	0,	2,	1,	0,	0,	0,	1,	2},
	{0,	0,	1,	0,	-1,	0,	0,	0,	1},
	{0,	0,	0,	0,	-1,	-1,	0,	0,	1},
	{0,	2,	1,	0,	0,	0,	-1,	0,	2},
	{0,	0,	0,	-1,	0,	0,	0,	0,	1},
	{0,	1,	1,	0,	-1,	0,	0,	0,	1},
	{0,	2,	1,	-1,	0,	-1,	-1,	0,	1},
	{0,	1,	0,	0,	-1,	-1,	-1,	-1,	1},
	{0,	1,	0,	0,	-1,	-1,	-1,	-1,	1},
	{0,	1,	-1,	0,	-2,	-1,	-1,	-1,	0},
	{0,	1,	-2,	-1,	-2,	-1,	-1,	-1,	0},
	{0,	-1,	-2,	-1,	-2,	-1,	-1,	-1,	1},
	{0,	-1,	-1,	-2,	-2,	-1,	-1,	-1,	1},
	{0,	-1,	-1,	-2,	-1,	-1,	-1,	-1,	1},
	{0,	0,	0,	0,	0,	0,	0,	0,	0}
};

static int rgb_offset_star_revA[LUMINANCE_MAX][RGB_COMPENSATION] = {
/*	R255 G255 B255 R203 G203 B203 R151 G151 B151
	R87 G87 B87 R51 G51 B51 R35 G35 B35
	R23 G23 B23 R11 G11 B11
*/
	{-4,	1,	-3,	-2,	0,	-2,	-1,	0,	-2,	-5,	0,	-6,	-9,	-1,	-11,	-6,	1,	-7,	-5,	-2,	-9,	3,	1,	-5,	-1,	-1,	-1},
	{-3,	1,	-2,	-1,	0,	-2,	-3,	0,	-3,	-2,	1,	-4,	-8,	-1,	-9,	-8,	0,	-8,	-5,	0,	-8,	1,	1,	-6,	2,	-2,	0},
	{-2,	1,	-2,	-1,	0,	-1,	-3,	0,	-3,	-2,	1,	-4,	-7,	-1,	-9,	-4,	2,	-7,	-7,	-3,	-11,	-1,	1,	-10,	0,	1,	-1},
	{-2,	1,	-2,	-1,	0,	-1,	-1,	0,	-1,	-4,	-1,	-5,	-5,	0,	-7,	-5,	1,	-7,	-6,	0,	-8,	-2,	0,	-10,	-3,	1,	-3},
	{-1,	1,	-1,	-1,	0,	-1,	-1,	1,	-1,	-4,	-1,	-5,	-5,	1,	-7,	-4,	1,	-7,	-6,	0,	-9,	-3,	1,	-11,	1,	0,	-2},
	{-1,	1,	-1,	-1,	0,	-1,	-1,	1,	-1,	-4,	-1,	-5,	-4,	2,	-6,	-6,	-2,	-9,	-2,	3,	-5,	-2,	-1,	-13,	1,	1,	-2},
	{-1,	1,	-1,	-1,	0,	-1,	-1,	1,	-1,	-2,	-1,	-4,	-7,	-1,	-9,	-2,	1,	-7,	-3,	2,	-6,	-4,	-4,	-18,	2,	1,	-1},
	{-1,	1,	-1,	-1,	0,	-1,	-1,	0,	-1,	-2,	1,	-3,	-5,	0,	-8,	-5,	-2,	-8,	-3,	2,	-7,	1,	0,	-13,	2,	1,	-1},
	{-1,	1,	-1,	-1,	0,	-1,	0,	1,	0,	-2,	0,	-3,	-6,	-1,	-9,	-2,	2,	-6,	-5,	-2,	-11,	1,	0,	-13,	3,	1,	-1},
	{0,	1,	0,	-1,	0,	-1,	-1,	1,	-1,	-2,	0,	-3,	-5,	0,	-8,	-5,	-2,	-9,	-1,	3,	-7,	2,	1,	-12,	3,	0,	-2},
	{0,	1,	0,	-1,	0,	-1,	0,	1,	0,	-3,	-1,	-5,	-5,	-1,	-9,	-2,	1,	-8,	-3,	-1,	-10,	1,	0,	-14,	3,	1,	-1},
	{1,	1,	1,	-1,	0,	-1,	-1,	1,	-1,	-2,	-1,	-4,	-5,	0,	-9,	-2,	1,	-8,	-2,	-1,	-10,	2,	0,	-14,	4,	1,	-1},
	{1,	1,	1,	-1,	0,	-1,	0,	1,	0,	-3,	-1,	-5,	-4,	2,	-7,	-1,	1,	-8,	-1,	0,	-10,	2,	0,	-14,	3,	1,	-2},
	{3,	1,	3,	-1,	0,	-1,	-1,	0,	-1,	-1,	-1,	-4,	-3,	1,	-8,	-4,	-2,	-11,	2,	4,	-8,	2,	-3,	-16,	2,	3,	2},
	{3,	1,	3,	-1,	0,	-1,	0,	1,	0,	-2,	-1,	-5,	-4,	1,	-8,	-3,	-2,	-10,	-2,	0,	-12,	4,	1,	-13,	-1,	-1,	-1},
	{3,	1,	3,	-1,	0,	-1,	0,	1,	0,	-2,	-1,	-4,	-5,	-1,	-10,	0,	2,	-7,	-1,	-1,	-12,	4,	0,	-14,	-1,	-1,	-3},
	{3,	1,	3,	-1,	0,	-1,	-1,	0,	-1,	-1,	0,	-2,	-5,	0,	-9,	0,	1,	-7,	-2,	-1,	-12,	5,	0,	-14,	3,	1,	-2},
	{3,	1,	3,	-1,	0,	-1,	-1,	0,	-1,	-1,	0,	-2,	-3,	1,	-7,	-4,	-2,	-9,	2,	2,	-9,	6,	1,	-12,	-2,	-4,	-8},
	{3,	1,	3,	-1,	0,	-1,	-1,	0,	-1,	0,	1,	-2,	-4,	-1,	-8,	-1,	1,	-7,	-3,	-2,	-12,	6,	1,	-12,	3,	1,	-1},
	{3,	1,	3,	-1,	0,	-1,	-1,	0,	-1,	0,	1,	-2,	-5,	-1,	-8,	0,	1,	-6,	-1,	1,	-12,	2,	-3,	-14,	3,	0,	-3},
	{3,	1,	3,	0,	0,	0,	-2,	0,	-2,	0,	0,	-2,	-2,	1,	-6,	-2,	1,	-6,	-2,	0,	-13,	2,	-2,	-14,	3,	1,	-2},
	{3,	1,	3,	0,	0,	0,	0,	0,	-1,	0,	0,	-2,	-5,	-1,	-8,	-3,	0,	-7,	-1,	0,	-11,	6,	3,	-11,	-1,	-4,	-7},
	{3,	1,	3,	0,	0,	0,	0,	0,	-1,	-1,	0,	-2,	-3,	0,	-6,	-1,	0,	-6,	-1,	1,	-10,	0,	-4,	-18,	3,	0,	-2},
	{3,	1,	3,	0,	0,	0,	0,	0,	-1,	-2,	-1,	-3,	-2,	0,	-5,	-3,	-1,	-7,	-1,	0,	-10,	4,	1,	-14,	4,	1,	-2},
	{3,	1,	3,	0,	0,	0,	0,	0,	-1,	-2,	-1,	-3,	-3,	-1,	-6,	1,	3,	-2,	-1,	1,	-9,	-1,	-4,	-18,	3,	1,	-3},
	{3,	1,	3,	0,	0,	0,	0,	1,	0,	-2,	-1,	-3,	-1,	2,	-3,	-2,	-1,	-7,	-3,	0,	-10,	5,	0,	-14,	3,	2,	-2},
	{3,	1,	3,	0,	0,	0,	0,	1,	1,	0,	0,	-2,	-4,	-1,	-6,	1,	2,	-3,	-1,	1,	-8,	-3,	-5,	-20,	4,	0,	-4},
	{3,	1,	3,	0,	0,	0,	0,	1,	1,	-1,	-1,	-3,	-3,	-1,	-5,	-1,	0,	-5,	-1,	1,	-7,	1,	0,	-16,	3,	0,	-5},
	{3,	1,	3,	0,	0,	0,	0,	1,	0,	0,	-1,	-1,	-2,	1,	-4,	-1,	1,	-4,	-5,	-5,	-11,	5,	5,	-11,	3,	1,	-4},
	{3,	1,	3,	0,	0,	0,	0,	1,	1,	1,	0,	-1,	-3,	0,	-4,	-2,	-1,	-6,	0,	2,	-5,	0,	-1,	-17,	3,	0,	-4},
	{3,	1,	3,	0,	0,	1,	-1,	0,	-1,	2,	1,	0,	-4,	-1,	-5,	-2,	-1,	-5,	-2,	-1,	-7,	4,	4,	-13,	3,	0,	-5},
	{3,	1,	3,	0,	0,	0,	-1,	0,	0,	1,	0,	0,	-1,	1,	-3,	-2,	0,	-4,	-1,	0,	-5,	-3,	-3,	-20,	2,	0,	-5},
	{3,	1,	3,	0,	0,	1,	-1,	0,	-1,	1,	0,	0,	-1,	0,	-3,	-3,	-1,	-5,	-2,	0,	-5,	3,	2,	-14,	2,	0,	-6},
	{3,	1,	3,	0,	0,	0,	-1,	0,	0,	1,	0,	0,	-2,	0,	-3,	3,	2,	-1,	-2,	-1,	-4,	-5,	-4,	-21,	3,	0,	-5},
	{3,	1,	3,	-1,	0,	0,	1,	0,	0,	0,	-1,	-1,	0,	3,	-1,	-1,	0,	-3,	-3,	-2,	-5,	0,	2,	-14,	1,	-1,	-7},
	{3,	1,	3,	-1,	0,	-1,	1,	0,	0,	1,	1,	0,	-2,	0,	-3,	-4,	-3,	-5,	2,	3,	0,	-2,	0,	-17,	2,	0,	-4},
	{3,	0,	3,	-1,	0,	0,	1,	0,	0,	0,	-1,	-1,	-1,	0,	-2,	0,	0,	-3,	2,	3,	1,	-4,	-1,	-18,	1,	-1,	-6},
	{3,	0,	3,	0,	0,	0,	-1,	0,	-1,	2,	0,	1,	-3,	-1,	-4,	-1,	0,	-4,	-1,	0,	-2,	2,	4,	-12,	0,	-1,	-7},
	{3,	1,	3,	0,	0,	0,	0,	0,	0,	0,	-1,	-1,	0,	2,	-1,	-2,	-1,	-4,	-1,	0,	-3,	1,	3,	-12,	1,	-1,	-6},
	{3,	1,	3,	0,	0,	0,	0,	0,	0,	0,	0,	0,	-1,	0,	-2,	-2,	-2,	-4,	2,	3,	0,	-6,	-5,	-20,	7,	4,	-1},
	{2,	0,	3,	0,	0,	0,	0,	0,	0,	0,	0,	0,	-1,	0,	-3,	1,	1,	-1,	-4,	-2,	-5,	-2,	-1,	-15,	0,	-1,	-8},
	{3,	0,	3,	0,	0,	0,	1,	1,	0,	1,	1,	1,	-3,	-1,	-3,	0,	0,	-2,	0,	1,	-3,	-3,	-2,	-16,	5,	4,	-2},
	{3,	0,	3,	-1,	0,	0,	0,	0,	0,	0,	0,	0,	-2,	0,	-3,	0,	0,	-2,	2,	3,	0,	-4,	-3,	-16,	-1,	-2,	-9},
	{2,	0,	3,	1,	0,	1,	-1,	0,	-1,	1,	0,	1,	-1,	0,	-2,	1,	2,	-1,	-3,	-2,	-5,	1,	2,	-11,	-2,	-2,	-9},
	{3,	0,	3,	1,	0,	0,	0,	0,	-1,	0,	0,	0,	-1,	-1,	-2,	-1,	0,	-2,	0,	1,	-3,	0,	1,	-11,	-2,	-2,	-9},
	{3,	0,	3,	0,	0,	1,	-1,	0,	-1,	2,	1,	2,	-2,	-2,	-3,	-3,	-1,	-4,	4,	4,	2,	0,	0,	-11,	-3,	-3,	-11},
	{2,	0,	3,	0,	0,	0,	0,	1,	1,	0,	-1,	0,	-1,	0,	-1,	2,	2,	-1,	-3,	-2,	-5,	-3,	-2,	-13,	2,	3,	-4},
	{4,	1,	3,	0,	-1,	0,	0,	0,	-1,	0,	-1,	0,	1,	2,	0,	-2,	-3,	-4,	-1,	1,	-2,	1,	2,	-9,	-3,	-3,	-11},
	{4,	1,	3,	0,	0,	0,	0,	1,	0,	-1,	-1,	0,	0,	1,	0,	0,	0,	-2,	0,	0,	-2,	0,	2,	-9,	-5,	-4,	-13},
	{3,	0,	3,	1,	1,	1,	-1,	0,	-1,	0,	0,	0,	0,	1,	0,	-1,	-2,	-3,	3,	4,	1,	-2,	0,	-10,	-6,	-5,	-14},
	{3,	1,	3,	-1,	0,	0,	1,	0,	1,	-1,	-1,	0,	1,	1,	0,	-3,	-2,	-5,	1,	1,	-1,	-4,	-2,	-11,	-1,	1,	-8},
	{3,	1,	3,	0,	0,	0,	0,	0,	0,	1,	0,	1,	-1,	0,	0,	-1,	0,	-2,	1,	1,	-1,	0,	1,	-7,	-1,	1,	-7},
	{3,	1,	3,	-1,	0,	0,	0,	0,	-1,	0,	0,	0,	0,	0,	0,	-2,	-1,	-3,	2,	3,	1,	0,	0,	-7,	-2,	0,	-8},
	{3,	1,	3,	0,	0,	0,	0,	0,	0,	-1,	0,	0,	0,	0,	-1,	1,	0,	-1,	-2,	-2,	-3,	3,	4,	-3,	-3,	-1,	-9},
	{3,	1,	3,	0,	0,	0,	0,	0,	0,	0,	0,	1,	0,	-1,	-1,	0,	0,	-1,	3,	3,	1,	-4,	-3,	-9,	-3,	-1,	-9},
	{3,	1,	3,	0,	0,	-1,	0,	0,	1,	0,	-1,	0,	1,	1,	0,	-2,	-2,	-2,	1,	2,	0,	1,	3,	-4,	1,	-1,	-5},
	{3,	1,	2,	0,	-1,	0,	-1,	0,	0,	0,	0,	0,	1,	-1,	0,	0,	1,	0,	3,	3,	3,	-4,	-4,	-9,	-2,	-2,	-7},
	{3,	1,	2,	0,	-1,	0,	-1,	0,	-1,	1,	1,	1,	-1,	-1,	-1,	3,	3,	2,	1,	-1,	0,	-1,	1,	-5,	0,	-2,	-6},
	{2,	1,	2,	0,	0,	0,	-1,	0,	0,	0,	0,	0,	2,	0,	1,	-2,	-2,	-2,	3,	3,	3,	-1,	0,	-5,	0,	-2,	-5},
	{2,	1,	2,	0,	-1,	0,	-1,	0,	0,	0,	0,	0,	2,	2,	2,	-1,	-1,	-1,	2,	2,	2,	-1,	-1,	-5,	-2,	-3,	-6},
	{1,	0,	1,	1,	0,	0,	0,	0,	1,	-1,	0,	0,	1,	1,	1,	-1,	-2,	-2,	0,	0,	0,	2,	2,	-1,	6,	2,	3},
	{0,	0,	0,	2,	0,	1,	-1,	0,	0,	0,	0,	0,	0,	0,	0,	2,	1,	1,	-1,	0,	-1,	1,	1,	-1,	5,	1,	3},
	{1,	0,	1,	0,	0,	0,	0,	0,	0,	0,	1,	1,	1,	-1,	0,	0,	1,	0,	2,	2,	1,	0,	1,	-1,	-4,	-6,	-8},
	{1,	0,	1,	0,	0,	0,	0,	-1,	0,	0,	1,	1,	-1,	-2,	-2,	0,	0,	0,	-1,	0,	-1,	4,	4,	3,	4,	0,	2},
	{1,	0,	1,	0,	0,	0,	1,	0,	0,	-1,	0,	0,	2,	1,	1,	-2,	-1,	-2,	1,	0,	1,	2,	2,	1,	3,	-1,	1},
	{1,	0,	1,	-1,	0,	-1,	1,	0,	0,	-1,	1,	0,	0,	-1,	0,	1,	1,	0,	1,	0,	1,	3,	3,	2,	2,	-2,	0},
	{1,	0,	1,	-1,	0,	-1,	2,	1,	1,	-1,	0,	0,	0,	-1,	0,	1,	1,	0,	1,	0,	1,	1,	1,	1,	2,	-2,	0},
	{1,	0,	1,	-1,	0,	-1,	2,	1,	1,	-2,	-1,	-1,	1,	0,	1,	1,	1,	0,	0,	-1,	0,	2,	2,	2,	2,	-2,	1},
	{1,	0,	1,	-1,	0,	-1,	1,	0,	0,	-1,	-1,	0,	2,	1,	2,	-1,	-1,	-2,	0,	-1,	0,	1,	0,	1,	9,	4,	9},
	{0,	0,	0,	0,	0,	0,	1,	0,	0,	-1,	0,	0,	2,	1,	2,	-2,	-2,	-3,	-1,	-1,	-1,	2,	1,	2,	8,	4,	8},
	{0,	0,	0,	0,	0,	0,	1,	0,	0,	-1,	0,	0,	2,	1,	1,	-2,	-1,	-2,	0,	0,	0,	2,	3,	2,	2,	-1,	1},
	{0,	0,	0,	0,	0,	0,	1,	0,	0,	0,	1,	1,	1,	0,	1,	-1,	0,	-2,	0,	-1,	0,	1,	2,	1,	0,	-1,	0},
	{1,	0,	0,	0,	0,	0,	0,	0,	0,	-1,	1,	0,	1,	-1,	0,	-1,	1,	-1,	0,	-1,	0,	1,	1,	1,	0,	-1,	0},
	{0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0}
};

static int rgb_offset_star_revG[LUMINANCE_MAX][RGB_COMPENSATION] = {
/*	R255 G255 B255 R203 G203 B203 R151 G151 B151
	R87 G87 B87 R51 G51 B51 R35 G35 B35
	R23 G23 B23 R11 G11 B11
*/
	{-2,	1,	-1,	-1,	0,	-2,	-1,	0,	-2,	-6,	1,	-4,	0,	-2,	-5,	-7,	0,	-5,	-9,	-1,	-9,	-11,	-1,	-5,	-7,	-1,	-3},
	{-1,	1,	-1,	0,	0,	-1,	-1,	0,	-1,	-6,	0,	-4,	-10,	0,	-7,	-10,	-1,	-7,	-9,	1,	-8,	-13,	-1,	-5,	-7,	-1,	-4},
	{-1,	1,	-1,	0,	0,	0,	0,	-1,	-2,	-5,	2,	-3,	-7,	0,	-6,	-8,	1,	-9,	-9,	1,	-11,	-9,	0,	-10,	-7,	-1,	-5},
	{-1,	1,	-1,	0,	0,	0,	0,	0,	0,	-5,	1,	-3,	-8,	0,	-7,	-9,	-1,	-9,	-9,	1,	-9,	-8,	-2,	-12,	-7,	-1,	-6},
	{0,	1,	0,	0,	0,	0,	-1,	0,	-1,	-4,	1,	-3,	-7,	-2,	-7,	-6,	0,	-7,	-10,	0,	-12,	-7,	-2,	-11,	-7,	-1,	-4},
	{1,	0,	1,	-1,	0,	-2,	0,	0,	0,	-5,	0,	-4,	-7,	-2,	-7,	-6,	0,	-8,	-11,	-1,	-12,	-7,	2,	-11,	-7,	-1,	-5},
	{0,	1,	0,	1,	0,	0,	-1,	0,	0,	-4,	0,	-4,	-5,	0,	-6,	-6,	0,	-6,	-7,	-1,	-11,	-7,	-1,	-13,	-7,	-1,	-4},
	{0,	1,	0,	1,	0,	0,	0,	1,	0,	-3,	0,	-3,	-6,	0,	-7,	-3,	2,	-5,	-7,	-2,	-12,	-5,	-3,	-15,	-6,	-1,	-4},
	{1,	1,	1,	0,	0,	-1,	0,	0,	1,	-3,	0,	-3,	-6,	-1,	-7,	-3,	1,	-6,	-6,	-2,	-12,	-3,	0,	-14,	-6,	-1,	-4},
	{2,	1,	1,	0,	0,	0,	0,	0,	0,	-4,	0,	-3,	-5,	-1,	-7,	-5,	-2,	-8,	-2,	3,	-9,	-4,	-1,	-15,	-4,	0,	-5},
	{2,	1,	1,	0,	0,	0,	0,	0,	0,	-4,	0,	-3,	-4,	-1,	-7,	-5,	-2,	-8,	-3,	-1,	-12,	-3,	-3,	-15,	-4,	-1,	-7},
	{2,	1,	1,	0,	0,	0,	0,	0,	0,	-3,	0,	-3,	-5,	-1,	-7,	-2,	-1,	-6,	-3,	0,	-12,	2,	4,	-12,	-5,	-1,	-5},
	{2,	1,	1,	0,	0,	0,	-1,	0,	0,	-3,	0,	-3,	-4,	-1,	-7,	-4,	-1,	-9,	-2,	1,	-10,	0,	1,	-13,	0,	-1,	-1},
	{3,	1,	2,	0,	0,	0,	0,	0,	0,	-4,	0,	-3,	-4,	0,	-7,	-4,	-2,	-9,	0,	1,	-10,	-3,	-4,	-17,	-1,	-1,	-1},
	{3,	1,	2,	0,	0,	0,	-1,	0,	0,	-3,	0,	-3,	-4,	-2,	-8,	-4,	-1,	-8,	-2,	-1,	-12,	2,	0,	-14,	-1,	-1,	-1},
	{3,	1,	2,	0,	0,	0,	-1,	0,	0,	-2,	1,	-1,	-3,	0,	-6,	-2,	-1,	-8,	-2,	-1,	-11,	6,	2,	-14,	-1,	-1,	-1},
	{3,	1,	2,	0,	0,	0,	-1,	0,	0,	-2,	0,	-2,	-4,	-1,	-7,	-1,	0,	-7,	-3,	-2,	-13,	4,	2,	-13,	-3,	-2,	-3},
	{3,	1,	2,	0,	0,	0,	-1,	-1,	-1,	-3,	0,	-3,	-1,	1,	-4,	-3,	-1,	-7,	-1,	1,	-11,	7,	3,	-12,	-1,	-1,	-5},
	{3,	1,	3,	0,	0,	0,	0,	-1,	0,	-2,	1,	-2,	0,	2,	-4,	-3,	0,	-6,	-2,	-2,	-12,	5,	1,	-14,	-1,	-1,	-5},
	{3,	1,	2,	0,	0,	0,	0,	0,	0,	-2,	0,	-2,	-4,	-1,	-6,	-3,	0,	-5,	-1,	0,	-10,	3,	2,	-13,	-2,	0,	-5},
	{3,	1,	2,	0,	0,	0,	-1,	-1,	-1,	-1,	1,	-1,	-2,	-1,	-5,	-1,	-1,	-4,	0,	0,	-11,	2,	-2,	-17,	-2,	0,	-5},
	{3,	1,	2,	0,	0,	0,	0,	-1,	0,	-2,	1,	-1,	-2,	-1,	-4,	-2,	-1,	-5,	-2,	0,	-10,	4,	-2,	-16,	-1,	-1,	-4},
	{3,	1,	2,	0,	0,	0,	0,	-1,	0,	-3,	0,	-2,	-1,	1,	-3,	-3,	-1,	-6,	-1,	2,	-8,	4,	-3,	-16,	-1,	-1,	-4},
	{3,	1,	2,	0,	0,	0,	0,	-1,	0,	-3,	0,	-2,	0,	1,	-2,	-5,	-1,	-6,	-2,	0,	-8,	4,	-3,	-16,	-1,	-1,	-4},
	{3,	1,	2,	0,	0,	0,	0,	-1,	0,	-2,	1,	0,	-2,	-1,	-4,	-3,	-1,	-6,	0,	-1,	-8,	5,	1,	-16,	-1,	-1,	-5},
	{3,	1,	2,	0,	0,	0,	0,	-1,	0,	-2,	1,	0,	1,	0,	-3,	-1,	0,	-3,	0,	0,	-6,	3,	0,	-14,	-1,	-2,	-5},
	{3,	1,	2,	0,	0,	0,	0,	-1,	0,	-2,	1,	-1,	-1,	0,	-2,	-1,	0,	-3,	0,	0,	-7,	5,	4,	-14,	-1,	-1,	-5},
	{3,	1,	2,	0,	0,	0,	0,	0,	1,	-3,	0,	-1,	0,	1,	-1,	0,	0,	-2,	2,	0,	-6,	2,	-1,	-18,	-1,	-2,	-5},
	{3,	1,	2,	0,	0,	0,	0,	0,	1,	-3,	0,	-1,	0,	-1,	-2,	-1,	1,	-2,	1,	1,	-5,	6,	3,	-16,	-1,	-1,	-6},
	{3,	1,	2,	0,	0,	0,	1,	0,	1,	-4,	0,	-1,	1,	-1,	-1,	-1,	0,	-3,	2,	1,	-3,	5,	-1,	-16,	-1,	0,	-5},
	{3,	1,	3,	1,	0,	1,	0,	0,	0,	-2,	-1,	-2,	1,	2,	0,	-1,	0,	-4,	0,	-1,	-6,	8,	2,	-15,	-2,	0,	-5},
	{3,	1,	3,	-1,	0,	1,	-1,	-1,	-2,	0,	1,	0,	-1,	-1,	-1,	-1,	0,	-3,	4,	3,	-1,	7,	2,	-15,	-1,	-1,	-5},
	{3,	1,	3,	-2,	0,	0,	0,	0,	0,	0,	0,	0,	0,	-1,	0,	0,	0,	-2,	2,	2,	-5,	5,	1,	-14,	-1,	-1,	-5},
	{3,	1,	3,	-2,	0,	0,	1,	0,	1,	0,	1,	0,	0,	-1,	0,	-4,	-3,	-5,	1,	-1,	-4,	4,	3,	-14,	-1,	-1,	-5},
	{3,	1,	3,	-2,	0,	0,	1,	0,	1,	0,	0,	-1,	-1,	-2,	-1,	2,	-1,	0,	-2,	-1,	-6,	1,	1,	-15,	-2,	-3,	-5},
	{2,	1,	2,	-1,	-1,	-1,	2,	0,	1,	1,	2,	1,	-1,	-2,	0,	0,	-2,	-3,	-1,	-4,	-7,	-1,	0,	-20,	-1,	-3,	-8},
	{3,	1,	3,	-1,	-1,	-1,	-1,	0,	0,	1,	0,	1,	0,	-1,	0,	1,	0,	-1,	4,	1,	-2,	-1,	0,	-18,	-1,	-1,	-9},
	{2,	0,	2,	1,	1,	1,	-1,	-1,	0,	-1,	0,	-2,	0,	-1,	0,	-1,	0,	-2,	4,	-2,	-2,	0,	-1,	-18,	-3,	-2,	-5},
	{3,	0,	3,	-1,	1,	0,	0,	0,	1,	-1,	0,	-1,	2,	1,	1,	0,	-2,	-2,	3,	-1,	-2,	-2,	-1,	-19,	-3,	-3,	-6},
	{2,	1,	2,	0,	0,	0,	-1,	-1,	-1,	0,	0,	0,	0,	1,	0,	-1,	-3,	-3,	6,	3,	1,	-1,	-3,	-20,	-4,	-3,	-5},
	{3,	0,	2,	0,	0,	0,	-1,	0,	0,	0,	0,	1,	0,	-1,	-2,	1,	0,	1,	-1,	-4,	-4,	0,	1,	-17,	-4,	-2,	-8},
	{3,	0,	3,	-1,	0,	0,	0,	0,	0,	0,	0,	-1,	0,	-1,	0,	0,	-1,	0,	3,	0,	-2,	-1,	-2,	-17,	2,	2,	-7},
	{3,	1,	3,	-1,	0,	0,	1,	1,	0,	-1,	-1,	-2,	-1,	-1,	-1,	-1,	-4,	-2,	4,	3,	0,	1,	-2,	-17,	2,	0,	-6},
	{3,	0,	3,	-1,	0,	0,	0,	0,	0,	0,	1,	0,	1,	1,	0,	0,	-1,	-1,	-1,	-3,	-5,	3,	0,	-15,	1,	0,	-6},
	{3,	0,	3,	-2,	0,	-1,	1,	1,	1,	-1,	-1,	-1,	-1,	0,	-2,	1,	0,	-1,	2,	0,	-1,	3,	0,	-14,	0,	-1,	-8},
	{3,	0,	2,	0,	1,	1,	0,	-1,	-1,	-1,	0,	-1,	0,	-1,	-1,	2,	3,	4,	0,	0,	0,	-7,	-1,	-10,	0,	1,	-8},
	{4,	1,	3,	0,	0,	0,	0,	0,	1,	-1,	0,	-1,	1,	1,	0,	-2,	0,	-1,	-2,	-3,	-1,	-3,	1,	-8,	-1,	1,	-8},
	{4,	1,	4,	-1,	0,	-1,	0,	0,	0,	0,	1,	0,	0,	-1,	-2,	0,	0,	0,	1,	2,	3,	0,	0,	-6,	-1,	0,	-8},
	{3,	1,	3,	-1,	0,	0,	-1,	0,	0,	0,	0,	-1,	1,	2,	1,	0,	0,	-2,	0,	0,	4,	3,	-1,	-5,	-3,	0,	-4},
	{4,	0,	3,	-1,	0,	0,	-1,	0,	0,	1,	1,	0,	1,	1,	0,	-1,	-1,	-1,	-3,	-2,	-3,	-7,	0,	-8,	3,	0,	-4},
	{5,	1,	4,	-2,	0,	-1,	0,	0,	0,	0,	0,	-1,	1,	1,	1,	-1,	1,	-1,	-2,	-3,	-2,	-3,	-2,	-3,	0,	-1,	-10},
	{3,	0,	3,	0,	0,	-1,	0,	0,	0,	-1,	-1,	-1,	2,	2,	1,	0,	0,	-1,	0,	1,	1,	-3,	1,	-2,	-6,	0,	-8},
	{3,	1,	3,	0,	0,	0,	-1,	0,	0,	0,	0,	0,	1,	1,	0,	-3,	-2,	-1,	-1,	-1,	-2,	0,	0,	-2,	-8,	0,	-10},
	{4,	2,	5,	0,	0,	-1,	0,	0,	0,	0,	0,	0,	1,	1,	0,	-1,	0,	-1,	-2,	-2,	-2,	0,	1,	0,	-8,	-3,	-10},
	{3,	1,	4,	-1,	-1,	-1,	0,	0,	1,	0,	1,	0,	-1,	-1,	-2,	0,	1,	1,	-2,	-3,	-2,	-3,	0,	-4,	-1,	3,	-7},
	{3,	1,	3,	-1,	0,	-1,	0,	0,	0,	0,	0,	1,	1,	1,	0,	1,	0,	0,	-1,	-1,	0,	-4,	0,	-2,	-1,	0,	-4},
	{3,	1,	3,	-1,	0,	-1,	0,	0,	0,	1,	0,	1,	0,	0,	-1,	1,	0,	0,	-1,	-2,	0,	-4,	-2,	-5,	5,	0,	-3},
	{3,	1,	3,	-1,	0,	-1,	1,	0,	1,	-1,	-1,	-1,	1,	0,	0,	0,	-1,	-1,	4,	1,	3,	-4,	-1,	-4,	-3,	0,	-8},
	{2,	0,	2,	-1,	0,	0,	0,	0,	0,	0,	0,	1,	1,	0,	-1,	0,	-1,	0,	4,	0,	3,	-4,	-3,	-7,	4,	0,	0},
	{2,	0,	2,	-1,	0,	-1,	1,	0,	0,	0,	0,	1,	0,	0,	-1,	2,	1,	1,	-2,	-3,	-2,	1,	2,	-1,	2,	0,	-2},
	{2,	1,	2,	-1,	0,	0,	1,	0,	0,	0,	0,	1,	1,	1,	0,	-1,	-2,	-1,	1,	0,	1,	2,	0,	0,	0,	-1,	-3},
	{2,	1,	2,	0,	-1,	0,	0,	0,	0,	0,	1,	1,	-1,	-1,	-2,	1,	0,	0,	3,	0,	3,	1,	-1,	-1,	-1,	-2,	-3},
	{2,	0,	2,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	2,	0,	0,	0,	0,	-1,	-3,	0,	5,	0,	3,	0,	-3,	-3},
	{2,	1,	2,	-1,	0,	0,	-1,	0,	-1,	2,	1,	2,	1,	1,	0,	-2,	-3,	-1,	2,	0,	1,	2,	0,	1,	-2,	-3,	-4},
	{2,	1,	2,	-1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	-1,	0,	0,	2,	3,	0,	3,	5,	-2,	1,	2,	-1,	-8},
	{0,	0,	0,	1,	1,	1,	-1,	-1,	0,	0,	0,	0,	-1,	-1,	-1,	2,	0,	3,	1,	0,	1,	4,	0,	2,	-1,	-1,	-4},
	{0,	0,	0,	0,	0,	0,	1,	1,	1,	-1,	-1,	0,	1,	1,	0,	0,	0,	1,	1,	0,	1,	0,	-1,	0,	3,	0,	2},
	{0,	0,	0,	0,	0,	0,	1,	1,	1,	-1,	-1,	0,	1,	1,	1,	0,	0,	0,	0,	0,	2,	4,	0,	3,	2,	-1,	1},
	{0,	0,	0,	0,	0,	0,	0,	0,	0,	-1,	-1,	0,	0,	0,	0,	0,	0,	0,	2,	0,	2,	4,	-2,	1,	2,	-1,	1},
	{0,	0,	0,	0,	1,	1,	0,	0,	0,	0,	0,	0,	1,	1,	1,	-2,	-3,	-2,	2,	0,	2,	8,	0,	6,	-5,	0,	-6},
	{0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	-1,	0,	-1,	1,	0,	1,	2,	0,	2,	3,	0,	3,	-7,	0,	-6},
	{0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	1,	0,	0,	1,	1},
	{0,	0,	0,	0,	0,	0,	-1,	0,	0,	0,	1,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},
	{0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0},
};

/************
 *	  HMT   *
 ************/
static unsigned int base_luminance_reverse_hmt_single[][2] = {
	{10,	42},
	{11,	47},
	{12,	51},
	{13,	56},
	{14,	60},
	{15,	64},
	{16,	68},
	{17,	74},
	{19,	82},
	{20,	86},
	{21,	89},
	{22,	94},
	{23,	97},
	{25,	106},
	{27,	114},
	{29,	121},
	{31,	130},
	{33,	137},
	{35,	145},
	{37,	154},
	{39,	162},
	{41,	168},
	{44,	180},
	{47,	191},
	{50,	201},
	{53,	213},
	{56,	224},
	{60,	239},
	{64,	251},
	{68,	265},
	{72,	277},
	{77,	212},
	{82,	223},
	{87,	236},
	{93,	249},
	{99,	263},
	{105,	279},
};

static int gradation_offset_reverse_hmt_single[][9] = {
/*	V255 V203 V151 V87 V51 V35 V23 V11 V7 */
	{0,	2,	2,	3,	5,	6,	6,	9,	8},
	{0,	1,	2,	2,	5,	6,	7,	9,	8},
	{0,	1,	2,	2,	4,	6,	6,	8,	8},
	{0,	1,	1,	3,	4,	5,	6,	9,	9},
	{0,	0,	2,	2,	5,	6,	6,	8,	9},
	{0,	0,	2,	2,	4,	5,	6,	9,	9},
	{0,	0,	2,	2,	4,	5,	6,	9,	9},
	{0,	0,	2,	2,	3,	5,	6,	9,	9},
	{0,	1,	2,	2,	4,	5,	5,	8,	8},
	{0,	2,	2,	2,	4,	5,	6,	8,	8},
	{0,	2,	2,	2,	3,	5,	6,	8,	8},
	{0,	2,	1,	2,	4,	4,	5,	8,	8},
	{0,	2,	2,	2,	4,	5,	5,	8,	8},
	{0,	1,	0,	1,	3,	5,	6,	8,	8},
	{0,	3,	1,	2,	3,	4,	5,	8,	9},
	{0,	3,	2,	2,	3,	5,	5,	8,	9},
	{0,	4,	2,	3,	3,	5,	5,	7,	8},
	{0,	4,	3,	3,	4,	5,	5,	8,	8},
	{0,	3,	4,	3,	4,	5,	5,	8,	8},
	{0,	3,	3,	2,	3,	4,	4,	7,	8},
	{0,	3,	4,	3,	3,	5,	5,	7,	8},
	{0,	3,	4,	3,	3,	5,	5,	8,	9},
	{0,	2,	4,	3,	3,	5,	5,	8,	8},
	{0,	2,	5,	3,	3,	5,	5,	8,	9},
	{0,	3,	5,	4,	3,	4,	4,	7,	8},
	{0,	3,	5,	4,	3,	5,	5,	7,	8},
	{0,	4,	7,	5,	4,	5,	4,	7,	8},
	{0,	5,	6,	4,	4,	4,	5,	8,	8},
	{0,	4,	6,	5,	3,	4,	4,	7,	8},
	{0,	5,	7,	5,	4,	5,	5,	7,	8},
	{0,	6,	9,	6,	4,	4,	4,	7,	9},
	{0,	2,	4,	2,	1,	3,	3,	4,	4},
	{0,	3,	4,	3,	1,	2,	2,	4,	4},
	{0,	3,	5,	3,	1,	2,	2,	4,	4},
	{0,	4,	5,	3,	1,	1,	1,	3,	4},
	{0,	5,	6,	3,	2,	2,	2,	4,	5},
	{0,	4,	5,	3,	1,	1,	1,	4,	5},
};

static int rgb_offset_reverse_hmt_single[][RGB_COMPENSATION] = {
/*	R255 G255 B255 R203 G203 B203 R151 G151 B151
	R87 G87 B87 R51 G51 B51 R35 G35 B35
	R23 G23 B23 R11 G11 B11
*/
	{0,	0,	1,	0,	0,	0,	-1,	0,	-1,	-2,	2,	-4,	-4,	3,	-7,	-1,	3,	-6,	-4,	4,	-8,	0,	3,	-8,	-7,	1,	-3},
	{0,	0,	2,	-1,	0,	0,	-1,	0,	-1,	-2,	2,	-4,	-4,	2,	-6,	-2,	2,	-6,	-3,	3,	-6,	0,	4,	-10,	-12,	0,	-2},
	{0,	0,	2,	0,	0,	0,	0,	0,	-1,	-2,	2,	-4,	-3,	3,	-6,	-3,	2,	-6,	-3,	2,	-6,	0,	4,	-9,	0,	0,	-2},
	{0,	0,	1,	0,	0,	0,	-1,	0,	0,	-3,	1,	-4,	-4,	3,	-7,	-3,	3,	-7,	-3,	2,	-6,	1,	3,	-7,	-7,	1,	-4},
	{0,	0,	2,	0,	0,	0,	-1,	0,	-1,	-2,	1,	-3,	-3,	2,	-6,	-4,	3,	-7,	-2,	3,	-8,	1,	4,	-9,	-3,	0,	0},
	{0,	0,	1,	0,	0,	0,	0,	0,	0,	-2,	1,	-4,	-4,	2,	-6,	-2,	3,	-6,	-2,	3,	-7,	0,	3,	-8,	-3,	1,	-4},
	{1,	0,	2,	0,	0,	0,	0,	0,	0,	-3,	1,	-4,	-4,	2,	-6,	-2,	3,	-6,	-2,	3,	-6,	0,	4,	-9,	-3,	1,	-3},
	{0,	0,	1,	-1,	0,	0,	0,	0,	0,	-1,	1,	-2,	-4,	2,	-6,	-2,	3,	-6,	-2,	3,	-7,	0,	3,	-6,	-1,	2,	-4},
	{0,	0,	1,	-1,	0,	0,	0,	0,	0,	-2,	1,	-3,	-3,	2,	-5,	-4,	2,	-6,	-4,	3,	-8,	0,	3,	-8,	-3,	2,	-5},
	{1,	0,	1,	0,	0,	0,	1,	0,	0,	-2,	1,	-3,	-4,	2,	-5,	-3,	3,	-6,	-1,	3,	-6,	-2,	3,	-8,	-2,	2,	-4},
	{0,	0,	1,	0,	0,	0,	0,	0,	0,	-2,	1,	-2,	-2,	2,	-4,	-3,	3,	-6,	-2,	3,	-6,	-3,	4,	-9,	-1,	2,	-4},
	{1,	0,	2,	0,	0,	0,	0,	0,	0,	-2,	1,	-2,	-3,	2,	-4,	-4,	3,	-7,	-1,	3,	-6,	-2,	4,	-8,	-2,	2,	-4},
	{1,	0,	1,	0,	0,	0,	0,	0,	0,	-2,	1,	-3,	-3,	1,	-4,	-3,	3,	-6,	-4,	3,	-8,	-2,	4,	-10,	-5,	2,	-5},
	{1,	0,	1,	0,	0,	0,	0,	0,	-1,	-1,	1,	-2,	-3,	1,	-4,	-4,	2,	-6,	-3,	3,	-7,	-1,	4,	-10,	0,	2,	-4},
	{0,	0,	2,	0,	0,	0,	0,	0,	0,	-2,	0,	-2,	-3,	1,	-4,	-4,	2,	-6,	-2,	2,	-6,	-4,	4,	-10,	0,	1,	-2},
	{1,	0,	1,	0,	0,	0,	0,	0,	0,	-2,	0,	-2,	-3,	1,	-4,	-4,	3,	-6,	-3,	3,	-6,	-1,	4,	-8,	-1,	1,	-4},
	{0,	0,	1,	0,	0,	0,	0,	0,	0,	-2,	1,	-2,	-2,	1,	-3,	-2,	2,	-4,	-3,	3,	-6,	-5,	5,	-12,	0,	0,	-1},
	{0,	0,	1,	0,	0,	0,	0,	0,	0,	-2,	0,	-2,	-3,	1,	-4,	-3,	2,	-4,	-3,	3,	-6,	-4,	5,	-10,	-2,	1,	-4},
	{1,	0,	2,	1,	0,	0,	0,	0,	0,	-2,	0,	-2,	-3,	1,	-4,	-3,	2,	-5,	-4,	3,	-8,	-6,	5,	-10,	-1,	2,	-4},
	{1,	0,	2,	0,	0,	0,	0,	0,	0,	-1,	0,	-1,	-1,	1,	-3,	-4,	2,	-5,	-4,	3,	-7,	-5,	5,	-10,	-2,	2,	-5},
	{0,	0,	1,	0,	0,	1,	0,	0,	0,	0,	0,	-2,	-4,	1,	-4,	-2,	2,	-4,	-3,	3,	-6,	-4,	5,	-10,	-2,	2,	-4},
	{1,	0,	2,	0,	0,	0,	0,	0,	0,	-2,	0,	-2,	-3,	1,	-3,	-3,	2,	-4,	-5,	3,	-8,	-5,	4,	-9,	-1,	1,	-3},
	{2,	0,	2,	0,	0,	0,	0,	0,	0,	-2,	0,	-2,	-2,	1,	-3,	-3,	1,	-4,	-4,	3,	-6,	-5,	4,	-9,	-1,	2,	-5},
	{2,	0,	3,	0,	0,	0,	-1,	0,	-1,	-1,	0,	-1,	-2,	1,	-3,	-2,	1,	-3,	-5,	3,	-7,	-4,	4,	-10,	-2,	1,	-4},
	{3,	0,	4,	0,	0,	0,	0,	0,	0,	-1,	0,	-1,	-2,	1,	-3,	-3,	2,	-4,	-4,	3,	-7,	-4,	4,	-9,	-2,	2,	-4},
	{3,	0,	3,	0,	0,	0,	-1,	0,	0,	0,	0,	0,	-2,	1,	-4,	-3,	1,	-4,	-4,	2,	-6,	-5,	5,	-10,	0,	1,	-3},
	{3,	0,	3,	0,	0,	0,	-2,	0,	-1,	-1,	0,	0,	-3,	1,	-3,	-1,	2,	-4,	-4,	3,	-6,	-4,	4,	-9,	-1,	1,	-4},
	{3,	0,	3,	0,	0,	0,	-1,	0,	0,	-1,	0,	-1,	-2,	1,	-3,	-3,	1,	-4,	-5,	2,	-6,	-4,	4,	-10,	-2,	2,	-5},
	{4,	0,	4,	0,	0,	0,	-1,	0,	-1,	-1,	0,	-1,	-2,	1,	-3,	-2,	1,	-4,	-4,	3,	-6,	-5,	4,	-10,	0,	2,	-5},
	{2,	0,	4,	0,	0,	0,	0,	0,	0,	0,	0,	-1,	-2,	1,	-3,	-3,	1,	-4,	-4,	2,	-6,	-5,	5,	-10,	-1,	2,	-5},
	{3,	0,	4,	0,	0,	0,	0,	0,	0,	-2,	0,	-2,	-2,	1,	-2,	-3,	1,	-4,	-4,	2,	-6,	-5,	4,	-10,	-1,	1,	-4},
	{3,	0,	3,	0,	0,	0,	0,	0,	0,	-1,	0,	-1,	-1,	0,	-2,	-2,	0,	-2,	-4,	2,	-5,	-4,	4,	-8,	0,	2,	-5},
	{3,	0,	3,	0,	0,	0,	-1,	0,	0,	0,	0,	0,	-1,	0,	-1,	-2,	0,	-2,	-2,	2,	-4,	-4,	4,	-8,	0,	2,	-4},
	{3,	0,	3,	0,	0,	0,	0,	0,	0,	-1,	0,	-1,	-2,	0,	-2,	-1,	0,	-2,	-2,	2,	-4,	-4,	4,	-8,	-1,	2,	-5},
	{4,	0,	4,	0,	0,	0,	0,	0,	0,	-1,	0,	-1,	-2,	1,	-2,	-1,	1,	-2,	-3,	2,	-4,	-3,	4,	-8,	-1,	2,	-4},
	{2,	0,	4,	0,	0,	0,	0,	0,	0,	-1,	0,	0,	-1,	0,	-2,	-1,	0,	-2,	-2,	1,	-4,	-5,	3,	-8,	-3,	2,	-6},
	{3,	0,	4,	0,	0,	0,	0,	0,	0,	0,	0,	0,	-1,	1,	-2,	-1,	0,	-2,	-2,	1,	-3,	-4,	3,	-8,	-2,	2,	-5},
};
#endif
