#include "ephemeris.h"
#include "input.h"


static const char *obscode[] = {       /* observation code strings */

	"", "1C", "1P", "1W", "1Y", "1M", "1N", "1S", "1L", "1E", /*  0- 9 */
	"1A", "1B", "1X", "1Z", "2C", "2D", "2S", "2L", "2X", "2P", /* 10-19 */
	"2W", "2Y", "2M", "2N", "5I", "5Q", "5X", "7I", "7Q", "7X", /* 20-29 */
	"6A", "6B", "6C", "6X", "6Z", "6S", "6L", "8L", "8Q", "8X", /* 30-39 */
	"2I", "2Q", "6I", "6Q", "3I", "3Q", "3X", "1I", "1Q", ""    /* 40-49 */
};
static const unsigned char obsfreqs[] = { /* 1:L1,2:L2,3:L5,4:L6,5:L7,6:L8,7:L3 */

	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, /*  0- 9 */
	1, 1, 1, 1, 2, 2, 2, 2, 2, 2, /* 10-19 */
	2, 2, 2, 2, 3, 3, 3, 5, 5, 5, /* 20-29 */
	4, 4, 4, 4, 4, 4, 4, 6, 6, 6, /* 30-39 */
	2, 2, 4, 4, 3, 3, 3, 1, 1, 0  /* 40-49 */
};
static const char* codepris[4][MAXFREQ] = {  /* code priority table */

											 /* L1,G1E1a   L2,G2,B1     L5,G3,E5a L6,LEX,B3 E5a,B2    E5a+b */
	{ "PYWCMNSL", "PYWCMNDSLX", "IQX", "", "", "" }, /* GPS */
	{ "PC", "PC", "IQX", "", "", "" }, /* GLO */
	{ "CABXZ", "", "IQX", "ABCXZ", "IQX", "IQX" }, /* GAL */
	{ "IQX", "IQX", "IQX", "IQX", "IQX", "" }  /* BDS */

};

static const char syscodes[] = "GRECSJ";
static const char obstype[] = "CLDS";
static const int navsys[] = {             /* satellite systems */
	SYS_GPS,SYS_GLO,SYS_GAL,SYS_CMP,SYS_SBS,SYS_QZS,0
};

static const char gnssmask[6][33][3] = {
	{
		/* GPS: ref [13] table 3.5-87, ref [14][15] table 3.5-91 */
		""  ,"1C","1P","1W","1Y","1M",""  ,"2C","2P","2W","2Y","2M", /*  1-12 */
		""  ,""  ,"2S","2L","2X",""  ,""  ,""  ,""  ,"5I","5Q","5X", /* 13-24 */
		""  ,""  ,""  ,""  ,""  ,"1S","1L","1X"                      /* 25-32 */
	},
	{
		/* GLONASS: ref [13] table 3.5-93, ref [14][15] table 3.5-97 */
		""  ,"1C","1P",""  ,""  ,""  ,""  ,"2C","2P",""  ,"3I","3Q",
		"3X",""  ,""  ,""  ,""  ,""  ,""  ,""  ,""  ,""  ,""  ,""  ,
		""  ,""  ,""  ,""  ,""  ,""  ,""  ,""
	},
	{
		/* Galileo: ref [15] table 3.5-100 */
		""  ,"1C","1A","1B","1X","1Z",""  ,"6C","6A","6B","6X","6Z",
		""  ,"7I","7Q","7X",""  ,"8I","8Q","8X",""  ,"5I","5Q","5X",
		""  ,""  ,""  ,""  ,""  ,""  ,""  ,""
	},

	{
		/* BeiDou: ref [15] table 3.5-106 */
		""  ,"1I","1Q","1X",""  ,""  ,""  ,"6I","6Q","6X",""  ,""  ,
		""  ,"7I","7Q","7X",""  ,""  ,""  ,""  ,""  ,""  ,""  ,""  ,
		""  ,""  ,""  ,""  ,""  ,""  ,""  ,""
	},

	{
		/* SBAS: ref [13] table 3.5-T+005 */
		""  ,"1C",""  ,""  ,""  ,""  ,""  ,""  ,""  ,""  ,""  ,""  ,
		""  ,""  ,""  ,""  ,""  ,""  ,""  ,""  ,""  ,"5I","5Q","5X",
		""  ,""  ,""  ,""  ,""  ,""  ,""  ,""
	},
	{
		/* QZSS: ref [15] table 3.5-103 */
		""  ,"1C",""  ,""  ,""  ,""  ,""  ,""  ,"6S","6L","6X",""  ,
		""  ,""  ,"2S","2L","2X",""  ,""  ,""  ,""  ,"5I","5Q","5X",
		""  ,""  ,""  ,""  ,""  ,"1S","1L","1X"
	}

};



/* sort codes ----------------------------------------------------------------*/
static void sort_codes(unsigned char *codes, unsigned char *types, int n)
{
	unsigned char tmp;
	char *obs1, *obs2;
	int i, j;

	for (i = 0; i<n - 1; i++) for (j = i + 1; j<n; j++) {
		obs1 = code2obs(codes[i], NULL);
		obs2 = code2obs(codes[j], NULL);
		if (strcmp(obs1, obs2) <= 0) continue;
		tmp = codes[i]; codes[i] = codes[j]; codes[j] = tmp;
		tmp = types[i]; types[i] = types[j]; types[j] = tmp;
	}
}


static int hasType(obsHead *opt, int sys, char *type)
{
	return 1;
}

/* set observation types in rinex option -------------------------------------*/
static void setopt_obstype(const unsigned char *codes,
	const unsigned char *types, int sys, obsHead *opt)
{
	const char type_str[] = "CLDS";
	char type[16], *id;
	int i, j, k, freq;

	//	trace(3, "setopt_obstype: sys=%d\n", sys);

	opt->mulObsNum[sys] = 0;

	if (!(navsys[sys] & opt->navsys)) return;

	for (i = 0; i<NFREQ&&codes[i]; i++) {

		//		if (!(id = code2obs(codes[i], &freq)) || opt->mask[sys][codes[i] - 1] == '0') continue;
		if (!(id = code2obs(codes[i], &freq)) || gnssmask[sys][codes[i] - 1] == '0') continue;

		if (!(opt->freqtype&(1 << (freq - 1)))) {
			continue;
		}
		for (j = 0; j<4; j++) {
			if (!(opt->obstype&(1 << j))) continue;
			if (types && !(types[i] & (1 << j))) continue;

			/* observation type in ver.3 */
			sprintf(type, "%c%s", type_str[j], id);
			if (type[0] == 'C'&&type[2] == 'N') continue; /* codeless */

			strcpy(opt->obsType[sys][opt->mulObsNum[sys]++], type);
		}
	}
}

/* scan observation types ----------------------------------------------------*/
static int scan_obstype(const obsd_t *obs, int nsat, obsHead *opt)
{
	unsigned char codes[6][33] = { { 0 } };
	unsigned char types[6][33] = { { 0 } };
	int i, j, k, l, c = 0, type, sys, abort = 0, n[6] = { 0 };


	for (i = 0; i<nsat; i++) {
		sys = satIndex2sys(obs[i].sat);
		for (l = 0; navsys[l]; l++) if (navsys[l] == sys) break;
		if (!navsys[l]) continue;

		for (j = 0; j<NFREQ; j++) {
			if (!obs[i].code[j]) continue;

			for (k = 0; k<n[l]; k++) {
				if (codes[l][k] == obs[i].code[j]) break;
			}
			if (k >= n[l] && n[l]<32) {
				codes[l][n[l]++] = obs[i].code[j];
			}
			if (k<n[l]) {
				if (obs[i].P[j] != 0.0) types[l][k] |= 1;
				if (obs[i].L[j] != 0.0) types[l][k] |= 2;
				if (obs[i].D[j] != 0.0) types[l][k] |= 4;
				if (obs[i].SNR[j] != 0) types[l][k] |= 8;
			}
		}
	}

	for (i = 0; i<6; i++) {

		/* sort codes */
		sort_codes(codes[i], types[i], n[i]);

		/* set observation types in rinex option */
		setopt_obstype(codes[i], types[i], i, opt);
	}
	return 1;
}

/* set observation types -----------------------------------------------------*/
static void set_obstype(int format, obsHead *opt)
{
	/* supported codes by rtcm2 */
	const unsigned char codes_rtcm2[6][8] = {
		{ CODE_L1C,CODE_L1P,CODE_L2C,CODE_L2P },
		{ CODE_L1C,CODE_L1P,CODE_L2C,CODE_L2P }
	};
	/* supported codes by rtcm3 */
	const unsigned char codes_rtcm3[6][8] = {
		{ CODE_L1C,CODE_L1W,CODE_L2W,CODE_L2X,CODE_L5X },
		{ CODE_L1C,CODE_L1P,CODE_L2C,CODE_L2P },
		{ CODE_L1X,CODE_L5X,CODE_L7X,CODE_L8X },
		{ CODE_L1I,CODE_L7I,CODE_L6I },
		{ CODE_L1C,CODE_L5X },
		{ CODE_L1C,CODE_L2X,CODE_L5X }

	};

	const unsigned char *codes;
	int i;

	trace(3, "set_obstype: format=%d\n", format);

	for (i = 0; i<6; i++) {
		switch (format) {
		case 0: codes = codes_rtcm2[i]; break;
		case 1: codes = codes_rtcm3[i]; break;
		}
		/* set observation types in rinex option */
		setopt_obstype(codes, NULL, i, opt);
	}
}
/* set rtcm antenna and receiver info to options -----------------------------*/
extern void rtcm2ohead(const rtcm_t *rtcm, obsHead *opt)
{
	double pos[3], enu[3];
	int i;


	/* comment */
	//	sprintf(opt->comment[1] + strlen(opt->comment[1]), ", station ID: %d",
	//		rtcm->staid);

	sprintf(opt->comment[1] + strlen(opt->comment[1]), "station ID: %d",
		rtcm->staid);

	/* receiver and antenna info */
	if (!*opt->recserialNum && !*opt->recType && !*opt->recVers) {
		strcpy(opt->recserialNum, rtcm->sta.recserialNum);
		strcpy(opt->recType, rtcm->sta.recType);
		strcpy(opt->recVers, rtcm->sta.recVers);
	}
	if (!*opt->antSerialNum && !*opt->antType && !*opt->antSetup) {
		strcpy(opt->antSerialNum, rtcm->sta.antSerialNum);
		strcpy(opt->antType, rtcm->sta.antType);
		if (rtcm->sta.antsetup) {
			sprintf(opt->antSetup, "%d", rtcm->sta.antsetup);
		}
		else *opt->antSetup = '\0';
	}
	/* antenna approx position */
	if (norm(rtcm->sta.approxCoor, 3)>0.0) {
		for (i = 0; i<3; i++) opt->approxCoor[i] = rtcm->sta.approxCoor[i];
	}
	/* antenna delta */
	if (norm(rtcm->sta.antennaDelta, 3)>0.0) {
		if (!rtcm->sta.antennaDeltaType&&norm(rtcm->sta.antennaDelta, 3)>0.0) { /* enu */
			opt->antennaDeltaHEN[0] = rtcm->sta.antennaDelta[2]; /* h */
			opt->antennaDeltaHEN[1] = rtcm->sta.antennaDelta[0]; /* e */
			opt->antennaDeltaHEN[2] = rtcm->sta.antennaDelta[1]; /* n */
		}
		else if (norm(rtcm->sta.approxCoor, 3)>0.0) { /* xyz */
			CoorCar2CoorGeo(rtcm->sta.approxCoor, pos);
			CoorCar2CoorENU(pos, rtcm->sta.antennaDelta, enu);
			opt->antennaDeltaHEN[0] = enu[2]; /* h */
			opt->antennaDeltaHEN[1] = enu[0]; /* e */
			opt->antennaDeltaHEN[2] = enu[1]; /* n */
		}
	}
	else {
		opt->antennaDeltaHEN[0] = rtcm->sta.hgt;
		opt->antennaDeltaHEN[0] = 0.0;
		opt->antennaDeltaHEN[0] = 0.0;
	}
}


/* time string for ver.3 (yyyymmdd hhmmss UTC) -------------------------------*/
static void timestr_rnx(char *str)
{
	gtime_t time;
	double ep[6];
	time = timeget();
	time.sec = 0.0;
	time2epoch(time, ep);
	sprintf(str, "%04.0f%02.0f%02.0f %02.0f%02.0f%02.0f UTC", ep[0], ep[1], ep[2],
		ep[3], ep[4], ep[5]);
}

/* output obs types ver.2 ----------------------------------------------------*/
static void outobstype_ver2(FILE *fp, const obsHead *opt)
{
	const char label[] = "# / TYPES OF OBSERV";
	int i;

	//	trace(3, "outobstype_ver2:\n");

	fprintf(fp, "%6d", opt->obsNum);

	for (i = 0; i<opt->obsNum; i++) {
		if (i>0 && i % 9 == 0) fprintf(fp, "      ");

		fprintf(fp, "%6s", opt->obsType[0][i]);

		if (i % 9 == 8) fprintf(fp, "%-20s\n", label);
	}
	if (opt->obsNum == 0 || i % 9>0) {
		fprintf(fp, "%*s%-20s\n", (9 - i % 9) * 6, "", label);
	}
}
/* output obs types ver.3 ----------------------------------------------------*/
static void outobstype_ver3(FILE *fp, const obsHead *opt)
{
	const char label[] = "SYS / # / OBS TYPES";
	int i, j;

	//	trace(3, "outobstype_ver3:\n");

	for (i = 0; navsys[i]; i++) {
		if (!(navsys[i] & opt->navsys) || !opt->mulObsNum[i]) continue;

		fprintf(fp, "%c  %3d", syscodes[i], opt->mulObsNum[i]);

		for (j = 0; j<opt->mulObsNum[i]; j++) {
			if (j>0 && j % 13 == 0) fprintf(fp, "      ");

			fprintf(fp, " %3s", opt->obsType[i][j]);

			if (j % 13 == 12) fprintf(fp, "  %-20s\n", label);
		}
		if (j % 13>0) {
			fprintf(fp, "%*s  %-20s\n", (13 - j % 13) * 4, "", label);
		}
	}
}
/* output rinex obs header -----------------------------------------------------
* output rinex obd file header
* args   : FILE   *fp       I   output file pointer
*          rnxopt_t *opt    I   rinex options
*          nav_t  *nav      I   navigation data
* return : status (1:ok, 0:output error)
*-----------------------------------------------------------------------------*/
static int outrnxobsh(FILE *fp, const obsHead *opt, const nav_t *nav)
{
	const char *glo_codes[] = { "C1C","C1P","C2C","C2P" };
	double ep[6], pos[3] = { 0 }, del[3] = { 0 };
	int i, j, k, n;	//int i,j,k,n,prn[MAXPRNGLO];
	int* prn = (int *)malloc(sizeof(int) * MAXPRNGLO); //deal with disabled glonass
	char date[32], *sys, *tsys = "GPS";

	//	trace(3, "outrnxobsh:\n");

	timestr_rnx(date);

	if (opt->rinexVersion <= 2.99) { /* ver.2 */
		sys = opt->navsys == SYS_GPS ? "G (GPS)" : "M (MIXED)";
	}
	else { /* ver.3 */
		if (opt->navsys == SYS_GPS) sys = "G: GPS";
		else if (opt->navsys == SYS_GLO) sys = "R: GLONASS";
		else if (opt->navsys == SYS_GAL) sys = "E: Galielo";
		else if (opt->navsys == SYS_QZS) sys = "J: QZSS";   /* ver.3.02 */
		else if (opt->navsys == SYS_CMP) sys = "C: BeiDou"; /* ver.3.02 */
		else if (opt->navsys == SYS_SBS) sys = "S: SBAS Payload";
		else sys = "M: Mixed";
	}
	fprintf(fp, "%9.2f%-11s%-20s%-20s%-20s\n", opt->rinexVersion, "", "OBSERVATION DATA",
		sys, "RINEX VERSION / TYPE");
	fprintf(fp, "%-20.20s%-20.20s%-20.20s%-20s\n", opt->program, opt->runBy, date,
		"PGM / RUN BY / DATE");

	for (i = 0; i<MAXCOMMENT; i++) {
		if (!*opt->comment[i]) continue;
		fprintf(fp, "%-60.60s%-20s\n", opt->comment[i], "COMMENT");
	}
	fprintf(fp, "%-60.60s%-20s\n", opt->markerName, "MARKER NAME");
	fprintf(fp, "%-20.20s%-40.40s%-20s\n", opt->markerNumber, "", "MARKER NUMBER");

	if (opt->rinexVersion>2.99) {
		fprintf(fp, "%-20.20s%-40.40s%-20s\n", opt->markerType, "", "MARKER TYPE");
	}
	fprintf(fp, "%-20.20s%-40.40s%-20s\n", opt->observer, opt->agency,
		"OBSERVER / AGENCY");
	fprintf(fp, "%-20.20s%-20.20s%-20.20s%-20s\n", opt->recserialNum, opt->recType,
		opt->recVers, "REC # / TYPE / VERS");
	fprintf(fp, "%-20.20s%-20.20s%-20.20s%-20s\n", opt->antSerialNum, opt->antType,
		" ", "ANT # / TYPE");

	for (i = 0; i<3; i++) if (fabs(opt->approxCoor[i])<1E8) pos[i] = opt->approxCoor[i];
	for (i = 0; i<3; i++) if (fabs(opt->antennaDeltaHEN[i])<1E8) del[i] = opt->antennaDeltaHEN[i];
	fprintf(fp, "%14.4f%14.4f%14.4f%-18s%-20s\n", pos[0], pos[1], pos[2], "",
		"APPROX POSITION XYZ");
	fprintf(fp, "%14.4f%14.4f%14.4f%-18s%-20s\n", del[0], del[1], del[2], "",
		"ANTENNA: DELTA H/E/N");

	if (opt->rinexVersion <= 2.99) { /* ver.2 */
		fprintf(fp, "%6d%6d%-48s%-20s\n", 1, 1, "", "WAVELENGTH FACT L1/2");
		outobstype_ver2(fp, opt);
	}
	else { /* ver.3 */
		outobstype_ver3(fp, opt);
	}
	if (opt->interval>0.0) {
		fprintf(fp, "%10.3f%50s%-20s\n", opt->interval, "", "INTERVAL");
	}

	if (opt->firstEpoch.time != 0)
	{
		time2epoch(opt->firstEpoch, ep);
		fprintf(fp, "  %04.0f%6.0f%6.0f%6.0f%6.0f%13.7f     %-12s%-20s\n", ep[0],
			ep[1], ep[2], ep[3], ep[4], ep[5], tsys, "TIME OF FIRST OBS");
	}
	if (opt->lastEpoch.time != 0)
	{
		time2epoch(opt->lastEpoch, ep);
		fprintf(fp, "  %04.0f%6.0f%6.0f%6.0f%6.0f%13.7f     %-12s%-20s\n", ep[0],
			ep[1], ep[2], ep[3], ep[4], ep[5], tsys, "TIME OF LAST OBS");
	}

	if (opt->rinexVersion >= 3.01) { /* ver.3.01 */
		for (i = 0; navsys[i]; i++) {
			if (!(navsys[i] & opt->navsys) || !opt->mulObsNum[i]) continue;
			fprintf(fp, "%c %-58s%-20s\n", syscodes[i], "", "SYS / PHASE SHIFT");
		}
	}
	if (opt->rinexVersion >= 3.02) { /* ver.3.02 */
		for (i = n = 0; i<MAXPRNGLO; i++) {
			if (nav->glo_fcn[i] >= 1) prn[n++] = i + 1;
		}
		for (i = j = 0; i<(n <= 0 ? 1 : (n - 1) / 8 + 1); i++) {
			if (i == 0) fprintf(fp, "%3d", n); else fprintf(fp, "   ");
			for (k = 0; k<8; k++, j++) {
				if (j<n) {
					fprintf(fp, " R%02d %2d", prn[j], nav->glo_fcn[prn[j] - 1] - 8);
				}
				else {
					fprintf(fp, " %6s", "");
				}
			}
			fprintf(fp, " %-20s\n", "GLONASS SLOT / FRQ #");
		}
	}
	if (opt->rinexVersion >= 3.02) { /* ver.3.02 */
		for (i = 0; i<4; i++) fprintf(fp, " %3s %8.3f", glo_codes[i], 0.0);
		fprintf(fp, "%8s%-20s\n", "", "GLONASS COD/PHS/BIS");
	}
	free(prn);	//free prn
	return fprintf(fp, "%-60.60s%-20s\n", "", "END OF HEADER") != EOF;
}


static void outrnxobsf(FILE *fp, double obs, int lli)
{
	if (obs == 0.0 || obs <= -1E9 || obs >= 1E9) fprintf(fp, "              ");
	else fprintf(fp, "%14.3f", obs);
	if (lli <= 0) fprintf(fp, "  "); else fprintf(fp, "%1.1d ", lli);
}


static void outnavf(FILE *fp, double value)
{
	double e = fabs(value)<1E-99 ? 0.0 : floor(log10(fabs(value)) + 1.0);
	fprintf(fp, " %s.%012.0fE%+03.0f", value<0.0 ? "-" : " ", fabs(value) / pow(10.0, e - 12.0), e);
}
/* output rinex nav header -----------------------------------------------------
* output rinex nav file header
* args   : FILE   *fp       I   output file pointer
*          rnxopt_t *opt    I   rinex options
*          nav_t  nav       I   navigation data (NULL: no input)
* return : status (1:ok, 0:output error)
*-----------------------------------------------------------------------------*/

static int sat2code(int sat, char *code)
{
	int prn;
	prn = satIndex2satno(sat);
	switch (satIndex2sys(sat)) {
	case SYS_GPS: sprintf(code, "G%2d", prn - MINPRNGPS + 1); break;
	case SYS_GLO: sprintf(code, "R%2d", prn - MINPRNGLO + 1); break;
	case SYS_GAL: sprintf(code, "E%2d", prn - MINPRNGAL + 1); break;
	case SYS_SBS: sprintf(code, "S%2d", prn - 100); break;
	case SYS_QZS: sprintf(code, "J%2d", prn - MINPRNQZS + 1); break;
	case SYS_CMP: sprintf(code, "C%2d", prn - MINPRNCMP + 1); break;
	default: return 0;
	}
	return 1;
}

static const double ura_eph[] = {         /* ura values (ref [3] 20.3.3.3.1.1) */
	2.4, 3.4, 4.85, 6.85, 9.65, 13.65, 24.0, 48.0, 96.0, 192.0, 384.0, 768.0, 1536.0,
	3072.0, 6144.0, 0.0
};

/* ura index to ura value (m) ------------------------------------------------*/
static double uravalue(int sva)
{
	return 0 <= sva&&sva<15 ? ura_eph[sva] : 32767.0;
}

static int outrnxnavh(FILE *fp, const ephh_t *opt, const nav_t *nav)
{
	int i;
	char date[64], *sys;


	timestr_rnx(date);

	if (opt->version <= 2.99) { /* ver.2 */
		fprintf(fp, "%9.2f           %-20s%-20s%-20s\n", opt->version,
			"N: GPS NAV DATA", "", "RINEX VERSION / TYPE");
	}
	else { /* ver.3 */
		if (opt->sys == SYS_GPS) sys = "G: GPS";
		else if (opt->sys == SYS_GLO) sys = "R: GLONASS";
		else if (opt->sys == SYS_GAL) sys = "E: Galileo";
		else if (opt->sys == SYS_QZS) sys = "J: QZSS";   /* v.3.02 */
		else if (opt->sys == SYS_CMP) sys = "C: BeiDou"; /* v.3.02 */
		else if (opt->sys == SYS_SBS) sys = "S: SBAS Payload";
		else sys = "M: Mixed";

		fprintf(fp, "%9.2f           %-20s%-20s%-20s\n", opt->version,
			"N: GNSS NAV DATA", sys, "RINEX VERSION / TYPE");
	}
	fprintf(fp, "%-20.20s%-20.20s%-20.20s%-20s\n", opt->program, opt->runBy, date,
		"PGM / RUN BY / DATE");

	for (i = 0; i<MAXCOMMENT; i++) {
		if (!*opt->comment[i]) continue;
		fprintf(fp, "%-60.60s%-20s\n", opt->comment[i], "COMMENT");
	}
	if (opt->version <= 2.99) { /* ver.2 */
		if (opt->outiono) {
			fprintf(fp, "  %12.4E%12.4E%12.4E%12.4E%10s%-20s\n", opt->ion_gps[0],
				opt->ion_gps[1], opt->ion_gps[2], opt->ion_gps[3], "", "ION ALPHA");
			fprintf(fp, "  %12.4E%12.4E%12.4E%12.4E%10s%-20s\n", opt->ion_gps[4],
				opt->ion_gps[5], opt->ion_gps[6], opt->ion_gps[7], "", "ION BETA");
		}
		if (opt->outtime) {
			fprintf(fp, "   ");
			outnavf(fp, opt->utc_gps[0]);
			outnavf(fp, opt->utc_gps[1]);
			fprintf(fp, "%9.0f%9.0f %-20s\n", opt->utc_gps[2], opt->utc_gps[3],
				"DELTA-UTC: A0,A1,T,W");
		}
	}
	else { /* ver.3 */
		if (opt->sys&SYS_GPS) {
			if (opt->outiono) {
				fprintf(fp, "GPSA %12.4E%12.4E%12.4E%12.4E%7s%-20s\n",
					opt->ion_gps[0], opt->ion_gps[1], opt->ion_gps[2],
					opt->ion_gps[3], "", "IONOSPHERIC CORR");
				fprintf(fp, "GPSB %12.4E%12.4E%12.4E%12.4E%7s%-20s\n",
					opt->ion_gps[4], opt->ion_gps[5], opt->ion_gps[6],
					opt->ion_gps[7], "", "IONOSPHERIC CORR");
			}
		}
		if (opt->sys&SYS_GAL) {
			if (opt->outiono) {
				fprintf(fp, "GAL  %12.4E%12.4E%12.4E%12.4E%7s%-20s\n",
					opt->ion_gal[0], opt->ion_gal[1], opt->ion_gal[2], 0.0, "",
					"IONOSPHERIC CORR");
			}
		}
		if (opt->sys&SYS_QZS) {
			if (opt->outiono) {
				fprintf(fp, "QZSA %12.4E%12.4E%12.4E%12.4E%7s%-20s\n",
					opt->ion_qzs[0], opt->ion_qzs[1], opt->ion_qzs[2],
					opt->ion_qzs[3], "", "IONOSPHERIC CORR");
				fprintf(fp, "QZSB %12.4E%12.4E%12.4E%12.4E%7s%-20s\n",
					opt->ion_qzs[4], opt->ion_qzs[5], opt->ion_qzs[6],
					opt->ion_qzs[7], "", "IONOSPHERIC CORR");
			}
		}
		if (opt->sys&SYS_GPS) {
			if (opt->outtime) {
				fprintf(fp, "GPUT %17.10E%16.9E%7.0f%5.0f %-5s %-2s %-20s\n",
					opt->utc_gps[0], opt->utc_gps[1], opt->utc_gps[2],
					opt->utc_gps[3], "", "", "TIME SYSTEM CORR");
			}
		}
		if (opt->sys&SYS_GAL) {
			if (opt->outtime) {
				fprintf(fp, "GAUT %17.10E%16.9E%7.0f%5.0f %-5s %-2s %-20s\n",
					opt->utc_gal[0], opt->utc_gal[1], opt->utc_gal[2],
					opt->utc_gal[3], "", "", "TIME SYSTEM CORR");
			}
		}
		if (opt->sys&SYS_QZS) { /* ver.3.02 */
			if (opt->outtime) {
				fprintf(fp, "QZUT %17.10E%16.9E%7.0f%5.0f %-5s %-2s %-20s\n",
					opt->utc_qzs[0], opt->utc_qzs[1], opt->utc_qzs[2],
					opt->utc_qzs[3], "", "", "TIME SYSTEM CORR");
			}
		}
		if (opt->sys&SYS_CMP) { /* ver.3.02 */
			if (opt->outtime) {
				fprintf(fp, "BDUT %17.10E%16.9E%7.0f%5.0f %-5s %-2s %-20s\n",
					opt->utc_cmp[0], opt->utc_cmp[1], opt->utc_cmp[2],
					opt->utc_cmp[3], "", "", "TIME SYSTEM CORR");
			}
		}
	}
	if (opt->outleaps) {
		fprintf(fp, "%6d%54s%-20s\n", opt->leapSeconds, "", "LEAP SECONDS");
	}
	return fprintf(fp, "%60s%-20s\n", "", "END OF HEADER") != EOF;
}
/* output rinex nav body -------------------------------------------------------
* output rinex nav file body record
* args   : FILE   *fp       I   output file pointer
*          rnxopt_t *opt    I   rinex options
*          eph_t  *eph      I   ephemeris
* return : status (1:ok, 0:output error)
*-----------------------------------------------------------------------------*/
static int outrnxnavb(FILE *fp, const ephh_t *opt, const eph_t *eph)
{
	double ep[6], ttr;
	int week, sys, prn;
	char code[32], *sep;


	prn = satIndex2satno(eph->sat);
	if (!(sys = satIndex2sys(eph->sat)) || !(sys&opt->sys)) return 0;

	if (sys != SYS_CMP) {
		time2epoch(eph->toc, ep);
	}
	else {
		time2epoch(gpst2bdt(eph->toc), ep); /* gpst -> bdt */
	}
	if (opt->version>2.99 || sys == SYS_GAL || sys == SYS_CMP) { /* ver.3 or ver.2 GAL */
		if (!sat2code(eph->sat, code)) return 0;
		fprintf(fp, "%-3s %04.0f %2.0f %2.0f %2.0f %2.0f %2.0f", code, ep[0], ep[1],
			ep[2], ep[3], ep[4], ep[5]);
		sep = "    ";
	}
	else if (sys == SYS_QZS) { /* ver.2 or ver.3.02 QZS */
		if (!sat2code(eph->sat, code)) return 0;
		fprintf(fp, "%-3s %02d %2.0f %2.0f %2.0f %2.0f %4.1f", code,
			(int)ep[0] % 100, ep[1], ep[2], ep[3], ep[4], ep[5]);
		sep = "    ";
	}
	else {
		fprintf(fp, "%2d %02d %2.0f %2.0f %2.0f %2.0f %4.1f", prn,
			(int)ep[0] % 100, ep[1], ep[2], ep[3], ep[4], ep[5]);
		sep = "   ";
	}
	outnavf(fp, eph->f0);
	outnavf(fp, eph->f1);
	outnavf(fp, eph->f2);
	fprintf(fp, "\n%s", sep);

	outnavf(fp, eph->iode); /* GPS/QZS: IODE, GAL: IODnav, BDS: AODE */
	outnavf(fp, eph->crs);
	outnavf(fp, eph->deln);
	outnavf(fp, eph->M0);
	fprintf(fp, "\n%s", sep);

	outnavf(fp, eph->cuc);
	outnavf(fp, eph->e);
	outnavf(fp, eph->cus);
	outnavf(fp, sqrt(eph->A));
	fprintf(fp, "\n%s", sep);

	outnavf(fp, eph->toes);
	outnavf(fp, eph->cic);
	outnavf(fp, eph->OMG0);
	outnavf(fp, eph->cis);
	fprintf(fp, "\n%s", sep);

	outnavf(fp, eph->i0);
	outnavf(fp, eph->crc);
	outnavf(fp, eph->omg);
	outnavf(fp, eph->OMGd);
	fprintf(fp, "\n%s", sep);

	outnavf(fp, eph->idot);
	outnavf(fp, eph->code);
	outnavf(fp, eph->week); /* GPS/QZS: GPS week, GAL: GAL week, BDS: BDT week */
	outnavf(fp, eph->flag);
	fprintf(fp, "\n%s", sep);

	outnavf(fp, uravalue(eph->sva));
	outnavf(fp, eph->svh);
	outnavf(fp, eph->tgd[0]); /* GPS/QZS:TGD, GAL:BGD E5a/E1, BDS: TGD1 B1/B3 */
	if (sys == SYS_GAL || sys == SYS_CMP) {
		outnavf(fp, eph->tgd[1]); /* GAL:BGD E5b/E1, BDS: TGD2 B2/B3 */
	}
	else {
		outnavf(fp, eph->iodc);   /* GPS/QZS:IODC */
	}
	fprintf(fp, "\n%s", sep);

	if (sys != SYS_CMP) {
		ttr = time2gpst(eph->ttr,&week);
	}
	else {
		ttr = time2gpst(gpst2bdt(eph->ttr),&week); /* gpst -> bdt */
	}
	outnavf(fp, ttr + (week - eph->week)*604800.0);

	if (sys == SYS_GPS || sys == SYS_QZS) {
		outnavf(fp, eph->fit);
	}
	else if (sys == SYS_CMP) {
		outnavf(fp, eph->iodc); /* AODC */
	}
	else {
		outnavf(fp, 0.0); /* spare */
	}
	return fprintf(fp, "\n") != EOF;
}


/* output rinex gnav header ----------------------------------------------------
* output rinex gnav (glonass navigation) file header
* args   : FILE   *fp       I   output file pointer
*          rnxopt_t *opt    I   rinex options
*          nav_t  nav       I   navigation data (NULL: no input)
* return : status (1:ok, 0:output error)
*-----------------------------------------------------------------------------*/
static int outrnxgnavh(FILE *fp, const ephh_t *opt, const nav_t *nav)
{
	int i;
	char date[64];


	timestr_rnx(date);

	if (opt->version <= 2.99) { /* ver.2 */
		fprintf(fp, "%9.2f           %-20s%-20s%-20s\n", opt->version,
			"GLONASS NAV DATA", "", "RINEX VERSION / TYPE");
	}
	else { /* ver.3 */
		fprintf(fp, "%9.2f           %-20s%-20s%-20s\n", opt->version,
			"N: GNSS NAV DATA", "R: GLONASS", "RINEX VERSION / TYPE");
	}
	fprintf(fp, "%-20.20s%-20.20s%-20.20s%-20s\n", opt->program, opt->runBy, date,
		"PGM / RUN BY / DATE");

	for (i = 0; i<MAXCOMMENT; i++) {
		if (!*opt->comment[i]) continue;
		fprintf(fp, "%-60.60s%-20s\n", opt->comment[i], "COMMENT");
	}
	return fprintf(fp, "%60s%-20s\n", "", "END OF HEADER") != EOF;
}
/* output rinex gnav body ------------------------------------------------------
* output rinex gnav (glonass navigation) file body record
* args   : FILE   *fp       I   output file pointer
*          rnxopt_t *opt    I   rinex options
*          geph_t  *geph    I   glonass ephemeris
* return : status (1:ok, 0:output error)
*-----------------------------------------------------------------------------*/
static int outrnxgnavb(FILE *fp, const ephh_t *opt, const geph_t *geph)
{
	gtime_t toe;
	double ep[6], tof;
	int prn;
	char code[32], *sep;

	prn = satIndex2satno(geph->sat);
	if ((satIndex2sys(geph->sat)&opt->sys) != SYS_GLO) return 0;

	tof = time2gpst(gpst2utc(geph->tof),NULL);      /* v.3: tow in utc */
	if (opt->version <= 2.99) tof = fmod(tof, 86400.0); /* v.2: tod in utc */

	toe = gpst2utc(geph->toe); /* gpst -> utc */
	time2epoch(toe, ep);

	if (opt->version <= 2.99) { /* ver.2 */
		fprintf(fp, "%2d %02d %2.0f %2.0f %2.0f %2.0f %4.1f", prn, (int)ep[0] % 100,
			ep[1], ep[2], ep[3], ep[4], ep[5]);
		sep = "   ";
	}
	else { /* ver.3 */
		if (!sat2code(geph->sat, code)) return 0;
		fprintf(fp, "%-3s %04.0f %2.0f %2.0f %2.0f %2.0f %2.0f", code, ep[0], ep[1],
			ep[2], ep[3], ep[4], ep[5]);
		sep = "    ";
	}
	outnavf(fp, -geph->taun);
	outnavf(fp, geph->gamn);
	outnavf(fp, tof);
	fprintf(fp, "\n%s", sep);

	outnavf(fp, geph->pos[0] / 1E3);
	outnavf(fp, geph->vel[0] / 1E3);
	outnavf(fp, geph->acc[0] / 1E3);
	outnavf(fp, geph->svh);
	fprintf(fp, "\n%s", sep);

	outnavf(fp, geph->pos[1] / 1E3);
	outnavf(fp, geph->vel[1] / 1E3);
	outnavf(fp, geph->acc[1] / 1E3);
	outnavf(fp, geph->frq);
	fprintf(fp, "\n%s", sep);

	outnavf(fp, geph->pos[2] / 1E3);
	outnavf(fp, geph->vel[2] / 1E3);
	outnavf(fp, geph->acc[2] / 1E3);
	outnavf(fp, geph->age);

	return fprintf(fp, "\n") != EOF;
}

extern int OutputObs(const pppsvr_t *svr, obsHead *opt, const obsd_t *obs, int n, const nav_t *nav, const char *dir)
{

	char timestr[30] = { 0 };

	char sats[MAXOBS][4] = { "" };
	int i, j, k, m, sys, ns, ind[MAXOBS], s[MAXOBS] = { 0 }, doy, year;
	const char **mask;

	FILE *fp = NULL;
	char path[1024] = { 0 };
	char fileName[100] = { 0 };
	char staName[5] = { 0 };
	strcpy(staName, svr->staname);
	doy = time2doy(obs->time,&year);
	strcpy(path, dir);
	sprintf(fileName, "%4s%03d0.%2do",
		staName, doy, year - 2000);
	strcat(path, fileName);
	if (access(path, 0))
	{
		fp = fopen(path, "a+");

		opt->rinexVersion = 3.00;
		for (i = 0; i < n; i++)
		{
			opt->navsys |= satIndex2sys(obs[i].sat);
		}
//		opt->navsys = SYS_ALL;
		/*strcpy(opt->program, "DreamPPP");
		strcpy(opt->runBy, "WHU-PWJ");
		strcpy(opt->markerName, staName);
		strcpy(opt->agency, "WHU");*/
		strcpy(opt->program, "NavFusion");
		strcpy(opt->runBy, "XCBD");
		strcpy(opt->markerName, staName);
		strcpy(opt->agency, "WHUFLAG");

		opt->firstEpoch = obs->time;

		opt->obstype |= OBSTYPE_ALL;
		opt->freqtype |= FREQTYPE_ALL;
		scan_obstype(obs, n, opt);
		set_obstype(1, opt);
		outrnxobsh(fp, opt, nav);
	}

	if (fp == NULL)
	{
		fp = fopen(path, "a+");
		opt->rinexVersion = 3.00;
		for (i = 0; i < n; i++)
		{
			opt->navsys |= satIndex2sys(obs[i].sat);
		}
		opt->navsys = SYS_ALL;
		opt->obstype |= OBSTYPE_ALL;
		opt->freqtype |= FREQTYPE_ALL;
//		scan_obstype(obs, n, opt);
		set_obstype(1, opt);

	}


	for (i = 0; i < n; i++)
	{
		opt->navsys |= satIndex2sys(obs[i].sat);
	}
	opt->navsys = SYS_ALL;

	for (i = ns = 0; i<n&&ns<MAXOBS; i++) {
		sys = satIndex2sys(obs[i].sat);
		if (!(sys&opt->navsys)/* || opt->exsats[obs[i].sat - 1]*/) continue;
		if (!satIndex2satID(sats[ns], obs[i].sat)) continue;
		switch (sys) {
		case SYS_GPS: s[ns] = 0; break;
		case SYS_GLO: s[ns] = 1; break;
		case SYS_GAL: s[ns] = 2; break;
		case SYS_CMP: s[ns] = 3; break;
		case SYS_SBS: s[ns] = 4; break;
		case SYS_QZS: s[ns] = 5; break;
		}
		if (!opt->mulObsNum[opt->rinexVersion <= 2.99 ? 0 : s[ns]]) continue;
		ind[ns++] = i;
	}


	fprintf(fp, ">%s%3d%3d\n", time2ObsTimeStr(obs->time, timestr), 0, n);


	for (i = 0; i<ns; i++) {
		sys = satIndex2sys(obs[ind[i]].sat);

		if (opt->rinexVersion <= 2.99) { /* ver.2 */
			m = 0;
			mask = gnssmask[s[i]];
		}
		else { /* ver.3 */
			fprintf(fp, "%-3s", sats[i]);
			m = s[i];
			mask = gnssmask[s[i]];
		}
		for (j = 0; j<opt->mulObsNum[m]; j++) {

			if (opt->rinexVersion <= 2.99) { /* ver.2 */
				if (j % 5 == 0) fprintf(fp, "\n");
			}
			/* search obs data index */
			if ((k = obs2index(opt->rinexVersion, sys, obs[ind[i]].code, opt->obsType[m][j],
				mask))<0) {
				outrnxobsf(fp, 0.0, -1);
				continue;
			}
			/* output field */
			switch (opt->obsType[m][j][0]) {
			case 'C':
			case 'P': outrnxobsf(fp, obs[ind[i]].P[k], -1); break;
			case 'L': outrnxobsf(fp, obs[ind[i]].L[k], obs[ind[i]].LLI[k]); break;
			case 'D': outrnxobsf(fp, obs[ind[i]].D[k], -1); break;
			case 'S': outrnxobsf(fp, obs[ind[i]].SNR[k] * 0.25, -1); break;
			}
		}
		if (opt->rinexVersion > 2.99&&fprintf(fp, "\n") == EOF) {
			fclose(fp);
			fp = NULL;
			return 0;
		}
	}
	if (opt->rinexVersion > 2.99)
	{
		fclose(fp);
		fp = NULL;

		return 1;
	}


	/*   for (int i = 0; i < obs->satNum; i++)
	{
	fprintf(fp, "%-3s%14.3f%16.3f%16.3f%16.3f%16.3f%16.3f\n",
	satIndex2satID(s_sat, obs->ssat[i].sat),
	obs->ssat[i].P[0], obs->ssat[i].L[0], (double)obs->ssat[i].SNR[0] * 0.25,
	obs->ssat[i].P[1], obs->ssat[i].L[1], (double)obs->ssat[i].SNR[1] * 0.25);
	}*/

}

extern int OutputBrdc(const Ntrip_base *information, const nav_t *nav, const int sat, const int index, const char *dir)
{
	int i;
	FILE *fpG = NULL;
	char pathG[1024] = { 0 };
	char fileNameG[100] = { 0 };
	FILE* fpR = NULL;
	char pathR[1024] = { 0 };
	char fileNameR[100] = { 0 };

	FILE* fpE = NULL;
	char pathE[1024] = { 0 };
	char fileNameE[100] = { 0 };

	FILE* fpC = NULL;
	char pathC[1024] = { 0 };
	char fileNameC[100] = { 0 };

	FILE* fpN = NULL;
	char pathN[1024] = { 0 };
	char fileNameN[100] = { 0 };

	int year, doy;
	ephh_t nhead = nav->nhead;

	int sys;

	doy = time2doy(timeadd(timeget(), leapsecond(timeget())),&year);
	strcpy(pathG, dir);
	strcpy(pathR, dir);
	strcpy(pathE, dir);
	strcpy(pathC, dir);
	strcpy(pathN, dir);

	if (prefixflag ==1) {
		sprintf(fileNameG, "%s.%2dp",
			ephprefix, year - 2000);
		strcat(pathG, fileNameG);

		sprintf(fileNameR, "%s.%2dg",
			ephprefix, year - 2000);
		strcat(pathR, fileNameR);

		sprintf(fileNameE, "%s.%2de",
			ephprefix, year - 2000);
		strcat(pathE, fileNameE);

		sprintf(fileNameC, "%s.%2dc",
			ephprefix, year - 2000);
		strcat(pathC, fileNameC);

		sprintf(fileNameN, "%s.%2dn",
			ephprefix, year - 2000);
		strcat(pathN, fileNameN);

	}
	else {
		sprintf(fileNameG, "%s%03d0.%2dp",
			ephprefix, doy, year - 2000);
		strcat(pathG, fileNameG);

		sprintf(fileNameR, "%s%03d0.%2dg",
			ephprefix, doy, year - 2000);
		strcat(pathR, fileNameR);

		sprintf(fileNameE, "%s%03d0.%2de",
			ephprefix, doy, year - 2000);
		strcat(pathE, fileNameE);

		sprintf(fileNameC, "%s%03d0.%2dc",
			ephprefix, doy, year - 2000);
		strcat(pathC, fileNameC);

		sprintf(fileNameN, "%s%03d0.%2dn",
			ephprefix, doy, year - 2000);
		strcat(pathN, fileNameN);
	}

//	if (strstr(information->mountPoint, "RTCM3EPH")) {
	




	nhead.version = verbrdc;

	strcpy(nhead.program, "NavFusion");
	strcpy(nhead.runBy, "XCBD");

	for (i = 0; i < nav->n; i++)
	{
		nhead.sys |= satIndex2sys(nav->eph[i].sat);
		if (nhead.leapSeconds == 0)
			nhead.leapSeconds = leapsecond(nav->eph[i].toe);
	}
	for (i = 0; i < nav->ng; i++)
	{
		nhead.sys |= satIndex2sys(nav->geph[i].sat);
		if (nhead.leapSeconds == 0)
			nhead.leapSeconds = leapsecond(nav->geph[i].toe);
	}
	if (nhead.leapSeconds != 0)
		nhead.outleaps = 1;



	nhead.version = 3.0;
	if (access(pathG, 0))
	{
		fpG = fopen(pathG, "a");
		outrnxnavh(fpG, &nhead, nav);
	}
	else
	{
		fpG = fopen(pathG, "a");
	}
	nhead.version = verbrdc;
	
	if (access(pathR, 0))
	{
		fpR = fopen(pathR, "a");
		nhead.sys = SYS_GLO;
		outrnxnavh(fpR, &nhead, nav);
	}
	else
	{
		fpR = fopen(pathR, "a");
	}

	
	if (access(pathE, 0))
	{
		fpE = fopen(pathE, "a");
		nhead.sys = SYS_GAL;
		outrnxnavh(fpE, &nhead, nav);
	}
	else
	{
		fpE = fopen(pathE, "a");
	}

	
	if (access(pathC, 0))
	{
		fpC = fopen(pathC, "a");
		nhead.sys = SYS_CMP;
		outrnxnavh(fpC, &nhead, nav);
	}
	else
	{
		fpC = fopen(pathC, "a");
	}

	
	if (access(pathN, 0))
	{
		fpN = fopen(pathN, "a");
		nhead.sys = SYS_GPS;
		outrnxnavh(fpN, &nhead, nav);
	}
	else
	{
		fpN = fopen(pathN, "a");
	}



	if (index == 1)
	{
		sys = satIndex2sys(sat);
		
		nhead.version = 3.0;
		outrnxnavb(fpG, &nhead, nav->eph + sat - 1);

		nhead.version = verbrdc;
		if (sys == SYS_GPS)
		{
			outrnxnavb(fpN, &nhead, nav->eph + sat - 1);
		}
		if (sys == SYS_GAL)
		{
			outrnxnavb(fpE, &nhead, nav->eph + sat - 1);
		}
		if (sys == SYS_CMP)
		{
			outrnxnavb(fpC, &nhead, nav->eph + sat - 1);
		}
	}

	if (index == 2) 
	{
		nhead.version = 3.0;
		outrnxgnavb(fpG, &nhead, nav->geph + sat - 1);
		nhead.version = 2.0;
		outrnxgnavb(fpR, &nhead, nav->geph + sat - 1);
	}
	fclose(fpG);
	fpG = NULL;
	fclose(fpR);
	fpR = NULL;
	fclose(fpE);
	fpE = NULL;
	fclose(fpC);
	fpC = NULL;
	fclose(fpN);
	fpN = NULL;
	return 1;
}

extern int OutputSsr(const Ntrip_base *information, const ssr_t *ssr, const char *dir)
{
	int i;
	FILE *fp = NULL;
	char path[1024] = { 0 };
	char fileName[100] = { 0 };
	int week;
	double tow;
	tow = time2gpst(timeadd(timeget(), leapsecond(timeget())), &week);
	strcpy(path, dir);
	sprintf(fileName, "%s%4d%d.ssr",
		information->mountPoint, week, (int)floor(tow/86400.0));
	strcat(path, fileName);
	if (fp == NULL)
	{
		fp = fopen(path, "a+");
	}
	char timestr[30] = { 0 };
	char s_sat[5] = { 0 };
	for (i = 0; i < MAXSAT; i++)
	{
		if (ssr[i].iode == 0)
			continue;
		fprintf(fp, "%4s%4s%s%8d%8d%19.7f%19.7f%19.7f%19.7f%19.7f%19.7f\n",
			satIndex2satID(s_sat, i + 1), "EPH", time2ObsTimeStr(ssr[i].t0[0], timestr),
			ssr[i].iode, ssr[i].iod[0],
			ssr[i].deph[0], ssr[i].deph[1], ssr[i].deph[2],
			ssr[i].ddeph[0], ssr[i].ddeph[1], ssr[i].ddeph[2]);
		fprintf(fp, "%4s%4s%s%8d%8d%19.7f%19.7f%19.7f\n",
			satIndex2satID(s_sat, i + 1), "CLK", time2ObsTimeStr(ssr[i].t0[1], timestr),
			ssr[i].iode, ssr[i].iod[1],
			ssr[i].dclk[0], ssr[i].dclk[1], ssr[i].dclk[2]);

		//		Global_RTCM.ssr[i] = para->rtcm->ssr[i];//����ssr��ȫ�ֱ���
	}
	fclose(fp);
	fp = NULL;
	return 1;
}

extern int Outputpeph(const Ntrip_base *information, const ssr_t *ssr, const nav_t *nav, const char *dir)
{
	FILE *eph_fp = NULL;
	FILE *clk_fp = NULL;
	char path[1024] = { 0 };
	char fileName[100] = { 0 };
	int week;
	double tow;
	tow = time2gpst(timeadd(timeget(), leapsecond(timeget())), &week);
	strcpy(path, dir);
	sprintf(fileName, "%s%4d%d.sp3",
		information->mountPoint, week, (int)floor(tow/86400.0));
	strcat(path, fileName);
	if (eph_fp == NULL)
	{
		eph_fp = fopen(path, "a+");
	}
	memset(fileName, 0, 100);
	strcpy(path, dir);
	sprintf(fileName, "%s%4d%d.clk",
		information->mountPoint, week, (int)floor(tow/86400.0));
	strcat(path, fileName);
	if (clk_fp == NULL)
	{
		clk_fp = fopen(path, "a+");
	}


	double rs[6] = { 0.0 }, dts[6] = { 0.0 }, var;
	char timestr[30] = { 0 };
	char s_sat[5] = { 0 };
	char s_clk[20] = { 0 };
	int i, opt = 0, svh[MAXOBS] = { 0 };
	gtime_t time;
	int timeFlag = 0;
	if (!strcmp(information->ID, "BRDC_APC_ITRF"))
		opt = -1;
	//	if (para->prcopt_t.satelliteEphemeris == 3)
	//		opt = 0;
	opt += 10;
	for (i = 0; i < MAXSAT; i++)
	{
		if (ssr[i].iode <= 0)
			continue;
		time = ssr[i].t0[1];
		if (time.time == 0)
			continue;
		//		if (time.time % 60 != 0)
		//			continue;
		if (!brdcPLUSssr(time, nav, i + 1, rs, dts, &var, svh, opt))continue;
		if (timeFlag == 0)
		{
			fprintf(eph_fp, "*  %s\n", time2peph_tTimeStr(time, timestr));
			timeFlag = 1;
		}
		fprintf(eph_fp, "P%s%14.6f%14.6f%14.6f%14.6f\n",
			satIndex2satID(s_sat, i + 1), rs[0] / 1000, rs[1] / 1000, rs[2] / 1000, dts[0] * 1E6);
		fprintf(clk_fp, "AS %s  %s  1   %19s\n", satIndex2satID(s_sat, i + 1), time2pclk_tTimeStr(time, timestr), num2CLKScientific(dts[0], s_clk));
		//	para->rtcm->ssr[i].iode = -1;
	}
	if (eph_fp != NULL) {
		fclose(eph_fp);
		eph_fp = NULL;
	}
	if (clk_fp != NULL) {
		fclose(clk_fp);
		clk_fp = NULL;
	}
	return 1;
}



extern void OutputResult(sol_t *sol)
{
	//	return 0;
}




static void writeSp3Head(FILE* fp, gtime_t time, int intervel)
{
	int i;
	int epoch = 86400 / intervel;
	int week, mjd;
	double ep[6], tow;
	char satID[4];
	int orbitAccExp[85];
	time2epoch(time, ep);
	tow = time2gpst(time, &week);
	mjd = time2mjd(time);
	fprintf(fp, "%s%4d %2d %2d %2d %2d %11.8f %7d %5s %5s %3s %4s\n",
		"#cP", (int)ep[0], (int)ep[1], (int)ep[2], (int)ep[3], (int)ep[4], ep[5], epoch, "ORBIT", "IGS14", "FIT", "RTS");
	fprintf(fp, "%s %4d %15.8f %14.8f %5d %15.13f\n",
		"##", week, tow, (double)intervel, mjd, 0);
	fprintf(fp, "%2s %3d   ", "+ ", MAXSAT);
	for (i = 0; i < /*MAXSAT*/85; i++)
	{
		satIndex2satID(satID, i + 1);
		if (satID[0] == '\0')
		{
			strcpy(satID, "0");
		}
		if (i < 17)
		{
			fprintf(fp, "%3s", satID);
		}
		else
		{
			if (i % 17 == 0)
			{
				fprintf(fp, "\n+%8s", "");
			}
			fprintf(fp, "%3s", satID);
		}

	}
	fprintf(fp, "\n");
	for (i = 0; i < 85; i++)
	{
		if (i < MAXSAT)
		{
			orbitAccExp[i] = 2;
		}
		else
		{
			orbitAccExp[i] = 0;
		}
	}
	for (i = 0; i < 85; i++)
	{
		if (i % 17 == 0)
		{
			fprintf(fp, "++%7s", "");
		}
		fprintf(fp, "%3d", orbitAccExp[i]);
		if (i % 17 == 16)
		{
			fprintf(fp, "\n");
		}
	}

	fprintf(fp, "%s\n", "%c G  cc GPS ccc cccc cccc cccc cccc ccccc ccccc ccccc ccccc");
	fprintf(fp, "%s\n", "%c cc cc ccc ccc cccc cccc cccc cccc ccccc ccccc ccccc ccccc");
	fprintf(fp, "%s\n", "%f  1.2500000  1.025000000  0.00000000000  0.000000000000000");
	fprintf(fp, "%s\n", "%f  0.0000000  0.000000000  0.00000000000  0.000000000000000");
	fprintf(fp, "%s\n", "%i    0    0    0    0      0      0      0      0         0");
	fprintf(fp, "%s\n", "%i    0    0    0    0      0      0      0      0         0");

	fprintf(fp, "%-61s\n", "/* CLK93");

}

static void writeClkHead(FILE* fp)
{
	fprintf(fp, "%9.2f%11s%-40s%-20s\n", 2.0, "", "CLOCK DATA", "RINEX VERSION / TYPE");
	fprintf(fp, "%60s%-20s\n", "", "END OF HEADER");
}

extern int writeSp3Clk(gtime_t time, const ssr_t* ssr, const nav_t* nav, const char* dir, const char* clkName, int itype, int otype, int intervel)
{

	if (time.time % intervel != 0)
		return 0;

	int ephFlag = 0, clkFlag = 0;
	FILE* eph_fp = NULL;
	FILE* clk_fp = NULL;
	char path[1024] = { 0 };
	char fileName[100] = { 0 };
	int week;
	double tow;
	double value[4];

	time.sec = 0;
	//	tow = time2gpst(timeadd(timeget(), leapsecond(timeget())), &week);
	tow = time2gpst(time, &week);
	strcpy(path, dir);
	sprintf(fileName, "%s%4d%d.sp3",
		clkName, week, (int)floor(tow / 86400.0));
	strcat(path, fileName);
	if (!_access(path, 0))
	{
		ephFlag = 1;
	}
	if (eph_fp == NULL)
	{
		eph_fp = fopen(path, "a+");
		if (!ephFlag)
		{
			writeSp3Head(eph_fp, time, intervel);
		}
	}
	memset(fileName, 0, 100);
	strcpy(path, dir);
	sprintf(fileName, "%s%4d%d.clk",
		clkName, week, (int)floor(tow / 86400.0));
	strcat(path, fileName);
	if (!_access(path, 0))
	{
		clkFlag = 1;
	}
	if (clk_fp == NULL)
	{
		clk_fp = fopen(path, "a+");
		if (!clkFlag)
		{
			writeClkHead(clk_fp);
		}
	}


	double rs[6] = { 0.0 }, dts[6] = { 0.0 }, var;
	char timestr[30] = { 0 };
	char s_sat[5] = { 0 };
	char s_clk[20] = { 0 };
	int i, j, opt = 0, svh[MAXOBS] = { 0 };
	int timeFlag = 0;
	if (itype == 2)//SSR_APC
		opt = -1;
	if (itype == 3)//SSR_COM
		opt = 0;
	opt += 10;
	for (i = 0; i < MAXSAT; i++)
	{
		if (timeFlag == 0)
		{
			fprintf(eph_fp, "*  %s\n", time2peph_tTimeStr(time, timestr));
			timeFlag = 1;
		}
		if (ssr[i].iode <= 0)
		{
			fprintf(eph_fp, "P%s%14.6f%14.6f%14.6f%14.6f\n",
				satIndex2satID(s_sat, i + 1), 999999.999999, 999999.999999, 999999.999999, 999999.999999);
			continue;
		}
		if (ssr[i].t0[1].time == 0)
		{
			fprintf(eph_fp, "P%s%14.6f%14.6f%14.6f%14.6f\n",
				satIndex2satID(s_sat, i + 1), 999999.999999, 999999.999999, 999999.999999, 999999.999999);
			continue;
		}
		//		if (time.time % intervel != 0)
		//			continue;
		if (!brdcPLUSssr(time, nav, i + 1, rs, dts, &var, svh, opt))
		{
			fprintf(eph_fp, "P%s%14.6f%14.6f%14.6f%14.6f\n",
				satIndex2satID(s_sat, i + 1), 999999.999999, 999999.999999, 999999.999999, 999999.999999);
			continue;
		}
		fprintf(eph_fp, "P%s%14.6f%14.6f%14.6f%14.6f\n",
			satIndex2satID(s_sat, i + 1), rs[0] / 1000, rs[1] / 1000, rs[2] / 1000, dts[0] * 1E6);
		fprintf(clk_fp, "AS %s  %s  1   %19s\n", satIndex2satID(s_sat, i + 1), time2pclk_tTimeStr(time, timestr), num2CLKScientific(dts[0], s_clk));
		//	para->rtcm->ssr[i].iode = -1;
	}
	if (eph_fp != NULL) {
		fclose(eph_fp);
		eph_fp = NULL;
	}
	if (clk_fp != NULL) {
		fclose(clk_fp);
		clk_fp = NULL;
	}
	return 1;
}