#include"input.h"

#define DTTOL       0.005               /* tolerance of time difference (s) */


static const char *stringVersion = "RINEX VERSION / TYPE";
static const char *stringRunBy = "PGM / RUN BY / DATE";
static const char *stringComment = "COMMENT";
static const char *stringMarkerName = "MARKER NAME";
static const char *stringMarkerNumber = "MARKER NUMBER";
static const char *stringMarkerType = "MARKER TYPE";
static const char *stringObserver = "OBSERVER / AGENCY";
static const char *stringReceiver = "REC # / TYPE / VERS";
static const char *stringAntennaType = "ANT # / TYPE";
static const char *stringAntennaPosition = "APPROX POSITION XYZ";
static const char *stringAntennaDeltaHEN = "ANTENNA: DELTA H/E/N";
static const char *stringAntennaDeltaXYZ = "ANTENNA: DELTA X/Y/Z";
static const char *stringAntennaPhaseCtr = "ANTENNA: PHASECENTER";
static const char *stringAntennaBsightXYZ = "ANTENNA: B.SIGHT XYZ";
static const char *stringAntennaZeroDirAzi = "ANTENNA: ZERODIR AZI";
static const char *stringAntennaZeroDirXYZ = "ANTENNA: ZERODIR XYZ";
static const char *stringCenterOfMass = "CENTER OF MASS: XYZ";
static const char *stringNumObs = "# / TYPES OF OBSERV";      // R2
static const char *stringSystemNumObs = "SYS / # / OBS TYPES";
static const char *stringWaveFact = "WAVELENGTH FACT L1/2";     // R2
static const char *stringSigStrengthUnit = "SIGNAL STRENGTH UNIT";
static const char *stringInterval = "INTERVAL";
static const char *stringFirstTime = "TIME OF FIRST OBS";
static const char *stringLastTime = "TIME OF LAST OBS";
static const char *stringReceiverOffset = "RCV CLOCK OFFS APPL";
static const char *stringSystemDCBSapplied = "SYS / DCBS APPLIED";
static const char *stringSystemPCVSapplied = "SYS / PCVS APPLIED";
static const char *stringSystemScaleFac = "SYS / SCALE FACTOR";
static const char *stringSystemPhaseShift = "SYS / PHASE SHIFTS";
static const char *stringGlonassSlotFreqNo = "GLONASS SLOT / FRQ #";
static const char *stringLeapSeconds = "LEAP SECONDS";
static const char *stringNumSats = "# OF SATELLITES";
static const char *stringPrnObs = "PRN / # OF OBS";
static const char *stringEoH = "END OF HEADER";

static const char *stringIonoCorr = "IONOSPHERIC CORR";
static const char *stringTimeSysCorr = "TIME SYSTEM CORR";
//static const char *stringLeapSeconds = "LEAP SECONDS";         defined in o file 
static const char *stringDeltaUTC = "DELTA-UTC: A0,A1,T,W";
static const char *stringCorrSysTime = "CORR TO SYSTEM TIME";
static const char *stringDUTC = "D-UTC A0,A1,T,W,S,U";
static const char *stringIonAlpha = "ION ALPHA";
static const char *stringIonBeta = "ION BETA";

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
static const char* codepris[6][MAXFREQ] = {  /* code priority table */

											 /* L1,G1E1a   L2,G2,B1     L5,G3,E5a L6,LEX,B3 E5a,B2    E5a+b */
	{ "PYWCMNSL", "PYWCMNDSLX", "IQX", "", "", "" }, /* GPS */
	{ "PC", "PC", "IQX", "", "", "" }, /* GLO */
	{ "CABXZ", "", "IQX", "ABCXZ", "IQX", "IQX" }, /* GAL */
	{ "IQX", "IQX", "IQX", "IQX", "IQX", "" },  /* BDS */
	{ "C"       ,""          ,"IQX"     ,""       ,""       ,"" }, /* SBS */
	{ "CSLXZ"   ,"SLX"       ,"IQX"     ,"SLX"    ,""       ,"" }, /* QZS */

};

static const char syscodes[] = "GRECSJ";
static const char obstype[] = "CLDS";
static const int navsys[] = {             /* satellite systems */
	SYS_GPS,SYS_GLO,SYS_GAL,SYS_CMP,SYS_SBS,SYS_QZS,0
};

static const char frqcodes[] = "125678";  /* frequency codes */

static const double ura_eph[] = {         /* ura values (ref [3] 20.3.3.3.1.1) */
	2.4, 3.4, 4.85, 6.85, 9.65, 13.65, 24.0, 48.0, 96.0, 192.0, 384.0, 768.0, 1536.0,
	3072.0, 6144.0, 0.0
};


/* ura value (m) to ura index ------------------------------------------------*/
int uraindex(double value)
{
	int i;
	for (i = 0; i<15; i++) if (ura_eph[i] >= value) break;
	return i;
}

/* compare ephemeris ---------------------------------------------------------*/
int cmpeph(const void *p1, const void *p2)
{
	eph_t *q1 = (eph_t *)p1, *q2 = (eph_t *)p2;
	return q1->ttr.time != q2->ttr.time ? (int)(q1->ttr.time - q2->ttr.time) :
		(q1->toe.time != q2->toe.time ? (int)(q1->toe.time - q2->toe.time) :
			q1->sat - q2->sat);
}
/* sort and unique ephemeris -------------------------------------------------*/
void uniqeph(nav_t *nav)
{
	eph_t *nav_eph;
	int i, j;


	if (nav->n <= 0) return;

	qsort(nav->eph, nav->n, sizeof(eph_t), cmpeph);

	for (i = 1, j = 0; i<nav->n; i++) {
		if (nav->eph[i].sat != nav->eph[j].sat ||
			nav->eph[i].iode != nav->eph[j].iode) {
			nav->eph[++j] = nav->eph[i];
		}
	}
	nav->n = j + 1;

	if (!(nav_eph = (eph_t *)realloc(nav->eph, sizeof(eph_t)*nav->n))) {
		free(nav->eph); nav->eph = NULL; nav->n = nav->nmax = 0;
		return;
	}
	nav->eph = nav_eph;
	nav->nmax = nav->n;

}
/* compare glonass ephemeris -------------------------------------------------*/
int cmpgeph(const void *p1, const void *p2)
{
	geph_t *q1 = (geph_t *)p1, *q2 = (geph_t *)p2;
	/*	return q1->tof.time != q2->tof.time ? (int)(q1->tof.time - q2->tof.time) :
	(q1->toe.time != q2->toe.time ? (int)(q1->toe.time - q2->toe.time) :
	q1->sat - q2->sat);*/
	return q1->toe.time != q2->toe.time ? (int)(q1->toe.time - q2->toe.time) :
		q1->sat - q2->sat;
}
/* sort and unique glonass ephemeris -----------------------------------------*/
void uniqgeph(nav_t *nav)
{
	geph_t *nav_geph;
	int i, j;


	if (nav->ng <= 0) return;

	qsort(nav->geph, nav->ng, sizeof(geph_t), cmpgeph);

	for (i = j = 0; i<nav->ng; i++) {
		if (nav->geph[i].sat != nav->geph[j].sat ||
			nav->geph[i].toe.time != nav->geph[j].toe.time ||
			nav->geph[i].svh != nav->geph[j].svh) {
			nav->geph[++j] = nav->geph[i];
		}
	}
	nav->ng = j + 1;

	if (!(nav_geph = (geph_t *)realloc(nav->geph, sizeof(geph_t)*nav->ng))) {
		free(nav->geph); nav->geph = NULL; nav->ng = nav->ngmax = 0;
		return;
	}
	nav->geph = nav_geph;
	nav->ngmax = nav->ng;

}
/* compare sbas ephemeris ----------------------------------------------------*/
int cmpseph(const void *p1, const void *p2)
{
	seph_t *q1 = (seph_t *)p1, *q2 = (seph_t *)p2;
	return q1->tof.time != q2->tof.time ? (int)(q1->tof.time - q2->tof.time) :
		(q1->t0.time != q2->t0.time ? (int)(q1->t0.time - q2->t0.time) :
			q1->sat - q2->sat);
}
/* sort and unique sbas ephemeris --------------------------------------------*/
void uniqseph(nav_t *nav)
{
	seph_t *nav_seph;
	int i, j;


	if (nav->ns <= 0) return;

	qsort(nav->seph, nav->ns, sizeof(seph_t), cmpseph);

	for (i = j = 0; i<nav->ns; i++) {
		if (nav->seph[i].sat != nav->seph[j].sat ||
			nav->seph[i].t0.time != nav->seph[j].t0.time) {
			nav->seph[++j] = nav->seph[i];
		}
	}
	nav->ns = j + 1;

	if (!(nav_seph = (seph_t *)realloc(nav->seph, sizeof(seph_t)*nav->ns))) {
		free(nav->seph); nav->seph = NULL; nav->ns = nav->nsmax = 0;
		return;
	}
	nav->seph = nav_seph;
	nav->nsmax = nav->ns;

}
/* unique ephemerides ----------------------------------------------------------
* unique ephemerides in navigation data and update carrier wave length
* args   : nav_t *nav    IO     navigation data
* return : number of epochs
*-----------------------------------------------------------------------------*/
void uniqnav(nav_t *nav)
{
	int i, j;

	/* unique ephemeris */
	uniqeph(nav);
	uniqgeph(nav);
	uniqseph(nav);

	/* update carrier wave length */
	for (i = 0; i<MAXSAT; i++) for (j = 0; j<NFREQ; j++) {
		nav->lam[i][j] = GetSatWaveLength(i + 1, j, nav);
	}
}


void redeph(nav_t *nav)
{
	eph_t *nav_eph;
	int i, j;


	if (nav->n <= 0) return;
	if (timediff(nav->eph[nav->n - 1].toc, nav->eph[0].toc)<10800) return;

	for (i = 1, j = 0; i<nav->n; i++) {
		if (timediff(nav->eph[nav->n - 1].toc, nav->eph[i].toc)<10800) {
			nav->eph[++j] = nav->eph[i];
		}
	}
	nav->n = j + 1;

	if (!(nav_eph = (eph_t *)realloc(nav->eph, sizeof(eph_t)*nav->n))) {
		free(nav->eph); nav->eph = NULL; nav->n = nav->nmax = 0;
		return;
	}
	nav->eph = nav_eph;
	nav->nmax = nav->n;

}

void redgeph(nav_t *nav)
{
	geph_t *nav_geph;
	int i, j;


	if (nav->ng <= 0) return;
	if (timediff(nav->geph[nav->ng - 1].toe, nav->geph[0].toe)<2700) return;

	for (i = 1, j = 0; i<nav->ng; i++) {
		if (timediff(nav->geph[nav->ng - 1].toe, nav->geph[i].toe)<2700) {
			nav->geph[++j] = nav->geph[i];
		}
	}
	nav->ng = j + 1;

	if (!(nav_geph = (geph_t *)realloc(nav->geph, sizeof(geph_t)*nav->ng))) {
		free(nav->geph); nav->geph = NULL; nav->ng = nav->ngmax = 0;
		return;
	}
	nav->geph = nav_geph;
	nav->ngmax = nav->ng;

}

void redseph(nav_t *nav)
{
	seph_t *nav_seph;
	int i, j;


	if (nav->ns <= 0) return;
	if (timediff(nav->seph[nav->ns - 1].t0, nav->seph[0].t0)<10800)return;


	for (i = 1, j = 0; i<nav->ns; i++) {
		if (timediff(nav->seph[nav->ns - 1].t0, nav->seph[i].t0)<10800) {
			nav->seph[++j] = nav->seph[i];
		}
	}
	nav->ns = j + 1;

	if (!(nav_seph = (seph_t *)realloc(nav->seph, sizeof(seph_t)*nav->ns))) {
		free(nav->seph); nav->seph = NULL; nav->ns = nav->nsmax = 0;
		return;
	}
	nav->seph = nav_seph;
	nav->nsmax = nav->ns;

}
/*reduce ephemerides*/
void rednav(nav_t *nav)
{
	int i, j;

	redeph(nav);
	redgeph(nav);
	redseph(nav);

	/* update carrier wave length */
	for (i = 0; i<MAXSAT; i++) for (j = 0; j<NFREQ; j++) {
		nav->lam[i][j] = GetSatWaveLength(i + 1, j, nav);
	}
}
/* compare observation data -------------------------------------------------*/
static int cmpobs(const void *p1, const void *p2)
{
	obsd_t *q1 = (obsd_t *)p1, *q2 = (obsd_t *)p2;
	double tt = timediff(q1->time, q2->time);
	if (fabs(tt)>DTTOL) return tt<0 ? -1 : 1;
	if (q1->rcv != q2->rcv) return (int)q1->rcv - (int)q2->rcv;
	return (int)q1->sat - (int)q2->sat;
}
/* sort and unique observation data --------------------------------------------
* sort and unique observation data by time, rcv, sat
* args   : obs_t *obs    IO     observation data
* return : number of epochs
*-----------------------------------------------------------------------------*/
extern int sortobs(obs_t *obs)
{
	int i, j, n;

	trace(3, "sortobs: nobs=%d\n", obs->n);

	if (obs->n <= 0) return 0;

	qsort(obs->data, obs->n, sizeof(obsd_t), cmpobs);

	/* delete duplicated data */
	for (i = j = 0; i<obs->n; i++) {
		if (obs->data[i].sat != obs->data[j].sat ||
			obs->data[i].rcv != obs->data[j].rcv ||
			timediff(obs->data[i].time, obs->data[j].time) != 0.0) {
			obs->data[++j] = obs->data[i];
		}
	}
	obs->n = j + 1;

	for (i = n = 0; i<obs->n; i = j, n++) {
		for (j = i + 1; j<obs->n; j++) {
			if (timediff(obs->data[j].time, obs->data[i].time)>DTTOL) break;
		}
	}
	return n;
}

void Rinex2toRinex3(double ver, int sys, char* str, char* type)
{
	strcpy(type, "");

	if (!strcmp(str, "P1")) { /* ver.2.11 GPS L1PY,GLO L2P */
		if (sys == SYS_GPS) sprintf(type, "%c1W", 'C');
		else if (sys == SYS_GLO) sprintf(type, "%c1P", 'C');
	}
	else if (!strcmp(str, "P2")) { /* ver.2.11 GPS L2PY,GLO L2P */
		if (sys == SYS_GPS) sprintf(type, "%c2W", 'C');
		else if (sys == SYS_GLO) sprintf(type, "%c2P", 'C');
	}
	else if (!strcmp(str, "C1")) { /* ver.2.11 GPS L1C,GLO L1C/A */
		if (ver >= 2.12); /* reject C1 for 2.12 */
		else if (sys == SYS_GPS) sprintf(type, "%c1C", 'C');
		else if (sys == SYS_GLO) sprintf(type, "%c1C", 'C');
		else if (sys == SYS_GAL) sprintf(type, "%c1X", 'C'); /* ver.2.12 */
		else if (sys == SYS_SBS) sprintf(type, "%c1C", 'C');
	}
	else if (!strcmp(str, "C2")) { /* ver.2.11/12 GPS L2PY,GLO L2C/A */
		if (sys == SYS_GPS) sprintf(type, "%c2W", 'C');
		else if (sys == SYS_GLO) sprintf(type, "%c2C", 'C');
		else if (sys == SYS_CMP) sprintf(type, "%c1X", 'C'); /* ver.2.12 B1 */
	}
	else if (ver >= 2.12&&str[1] == 'A') { /* ver.2.12 L1C/A */
		if (sys == SYS_GPS) sprintf(type, "%c1C", str[0]);
		else if (sys == SYS_GLO) sprintf(type, "%c1C", str[0]);
		else if (sys == SYS_SBS) sprintf(type, "%c1C", str[0]);
	}
	else if (ver >= 2.12&&str[1] == 'B') { /* ver.2.12 GPS L1C */
		if (sys == SYS_GPS) sprintf(type, "%c1X", str[0]);
	}
	else if (ver >= 2.12&&str[1] == 'C') { /* ver.2.12 GPS L2C */
		if (sys == SYS_GPS) sprintf(type, "%c2X", str[0]);
	}
	else if (ver >= 2.12&&str[1] == 'D') { /* ver.2.12 GLO L2C/A */
		if (sys == SYS_GLO) sprintf(type, "%c2C", str[0]);
	}
	else if (ver >= 2.12&&str[1] == '1') { /* ver.2.12 GPS L1PY,GLO L1P */
		if (sys == SYS_GPS) sprintf(type, "%c1W", str[0]);
		else if (sys == SYS_GLO) sprintf(type, "%c1P", str[0]);
		else if (sys == SYS_GAL) sprintf(type, "%c1X", str[0]); /* tentative */
		else if (sys == SYS_CMP) sprintf(type, "%c1X", str[0]); /* extension */
	}
	else if (ver<2.12&&str[1] == '1') {
		if (sys == SYS_GPS) sprintf(type, "%c1C", str[0]);
		else if (sys == SYS_GLO) sprintf(type, "%c1C", str[0]);
		else if (sys == SYS_GAL) sprintf(type, "%c1X", str[0]); /* tentative */
		else if (sys == SYS_SBS) sprintf(type, "%c1C", str[0]);
	}
	else if (str[1] == '2') {
		if (sys == SYS_GPS) sprintf(type, "%c2W", str[0]);
		else if (sys == SYS_GLO) sprintf(type, "%c2P", str[0]);
		else if (sys == SYS_CMP) sprintf(type, "%c1X", str[0]); /* ver.2.12 B1 */
	}
	else if (str[1] == '5') {
		if (sys == SYS_GPS) sprintf(type, "%c5X", str[0]);
		else if (sys == SYS_GAL) sprintf(type, "%c5X", str[0]);
		else if (sys == SYS_SBS) sprintf(type, "%c5X", str[0]);
	}
	else if (str[1] == '6') {
		if (sys == SYS_GAL) sprintf(type, "%c6X", str[0]);
		else if (sys == SYS_CMP) sprintf(type, "%c6X", str[0]); /* ver.2.12 B3 */
	}
	else if (str[1] == '7') {
		if (sys == SYS_GAL) sprintf(type, "%c7X", str[0]);
		else if (sys == SYS_CMP) sprintf(type, "%c7X", str[0]); /* ver.2.12 B2 */
	}
	else if (str[1] == '8') {
		if (sys == SYS_GAL) sprintf(type, "%c8X", str[0]);
	}
}

/* set string without tail space ---------------------------------------------*/
void setstr(char* dst, const char* src, int n)
{
	char* p = dst;
	const char* q = src;
	while (*q && q < src + n) *p++ = *q++;
	*p-- = '\0';
	while (p >= dst && *p == ' ') *p-- = '\0';
}

int ReadOBSHead(FILE *fp, obs_t *obs, sta_t *sta, double *rinexVersion, int *tsys, char *obs_Type, char (*obsType)[MAXOBSTYPE][4], int *obsNum, int *mulObsNum)
{
	int i, j, k;
	char str[1024] = { 0 };
	char label[30] = { 0 };
	double ep[6] = { 0 };
	char *p;
	double del[3];

	char fileType, gnssType, satSysTemp, timeType[4];
	int sys;
	while (fgets(str, MAXRNXLEN, fp))
	{
		if (strlen(str) < 60)
			continue;
		strcpy(label, str2nstr(str, 60, 20));
		if (strstr(label, stringVersion))
		{
			*rinexVersion = str2num(str, 0, 20);
			fileType = str2nstr(str, 20, 1)[0];
			if (fileType != 'O'&&fileType != 'o')
			{
				printf("File type is not OBSERVATION : %c\n", fileType);
				return 0;
			}

			gnssType = str2nstr(str, 40, 1)[0];
			switch (gnssType) {
			case ' ':                                       /*Ϊ�ո�Ĭ��ΪG*/
			case 'G': sys = SYS_GPS;  *tsys = TSYS_GPS; break;
			case 'S': sys = SYS_SBS;  *tsys = TSYS_GPS; break;
			case 'R': sys = SYS_GLO;  *tsys = TSYS_UTC; break;
			case 'E': sys = SYS_GAL;  *tsys = TSYS_GAL; break; /* v.2.12 */
			case 'C': sys = SYS_CMP;  *tsys = TSYS_CMP; break; /* v.2.12 */
			case 'M': sys = SYS_NONE; *tsys = TSYS_GPS; break; /* mixed */
			default:
				break;
			}
			continue;
		}
		else if (strstr(label, stringComment));
		else if (strstr(label, stringRunBy));
		else if (strstr(label, stringMarkerName));
		else if (strstr(label, stringMarkerNumber));
		else if (strstr(label, stringMarkerType));//ver3.

		else if (strstr(label, stringObserver));
		else if (strstr(label, stringReceiver))
		{
			if (sta) {
				setstr(sta->recserialNum, str, 20);
				setstr(sta->recType, str + 20, 20);
				setstr(sta->recVers, str + 40, 20);
			}
		}
		else if (strstr(label, stringAntennaType))
		{
			if (sta) {
				setstr(sta->antSerialNum, str, 20);
				setstr(sta->antType, str + 20, 20);
			}
		}
		else if (strstr(label, stringAntennaPosition))
		{
			if (sta) {
				for (i = 0, j = 0; i < 3; i++, j += 14) sta->approxCoor[i] = str2num(str, j, 14);
			}
		}
		else if (strstr(label, stringAntennaDeltaHEN))
		{
			if (sta) {
				for (i = 0, j = 0; i < 3; i++, j += 14) del[i] = str2num(str, j, 14);
				sta->antennaDelta[2] = del[0]; /* h */
				sta->antennaDelta[0] = del[1]; /* e */
				sta->antennaDelta[1] = del[2]; /* n */
			}
		}
		else if (strstr(label, stringAntennaDeltaXYZ));
		else if (strstr(label, stringAntennaPhaseCtr));
		else if (strstr(label, stringAntennaBsightXYZ));
		else if (strstr(label, stringAntennaZeroDirAzi));
		else if (strstr(label, stringAntennaZeroDirXYZ));
		else if (strstr(label, stringCenterOfMass));
		else if (strstr(label, stringSystemNumObs))
		{
			if (*rinexVersion < 3.00)
				return 0;
			satSysTemp = str2nstr(str, 0, 1)[0];
			*obsNum = atoi(str2nstr(str, 3, 3));
			i = (p = strchr(syscodes, satSysTemp)) ? (int)(p - syscodes) : 0;
			mulObsNum[i] = *obsNum;
			for (j = 0, k = 7; j < *obsNum&&j<MAXOBSTYPE; j++, k += 4)
			{
				if (k>58)
				{
					if (!fgets(str, MAXRNXLEN, fp)) break;
					k = 7;
				}
				strcpy(obs_Type, strtok_blank(str2nstr(str, k, 3)));
				strcpy(obsType[i][j], obs_Type);

			}
		}
		else if (strstr(label, stringNumObs))
		{
			if (*rinexVersion > 2.99)
				return 0;
			*obsNum = atoi(str2nstr(str, 0, 6));
			for (i = 0, j = 10; i < *obsNum&&i<MAXOBSTYPE; i++, j += 6)
			{
				if (j>58)
				{
					if (!fgets(str, MAXRNXLEN, fp)) break;
					j = 10;
				}
				strcpy(obs_Type, strtok_blank(str2nstr(str, j, 2)));
				Rinex2toRinex3(*rinexVersion, SYS_GPS, obs_Type, obsType[0][i]);
				Rinex2toRinex3(*rinexVersion, SYS_GLO, obs_Type, obsType[1][i]);
				Rinex2toRinex3(*rinexVersion, SYS_GAL, obs_Type, obsType[2][i]);
				Rinex2toRinex3(*rinexVersion, SYS_CMP, obs_Type, obsType[3][i]);
			}
		}
		else if (strstr(label, stringWaveFact));
		else if (strstr(label, stringSigStrengthUnit));

		else if (strstr(label, stringInterval));
		else if (strstr(label, stringFirstTime))
		{
			strcpy(timeType, str2nstr(str, 48, 3));
			if (strcmp(timeType, "GPS") == 0) *tsys = TSYS_GPS;
			else if (strcmp(timeType, "GLO") == 0) *tsys = TSYS_GLO;
			else if (strcmp(timeType, "GAL") == 0) *tsys = TSYS_GAL;
			else if (strcmp(timeType, "CMP") == 0) *tsys = TSYS_CMP;
		}
		else if (strstr(label, stringLastTime));
		else if (strstr(label, stringReceiverOffset));
		else if (strstr(label, stringSystemDCBSapplied));
		else if (strstr(label, stringSystemPCVSapplied));
		else if (strstr(label, stringSystemScaleFac));
		else if (strstr(label, stringSystemPhaseShift));
		else if (strstr(label, stringGlonassSlotFreqNo));         
		else if (strstr(label, stringLeapSeconds));
		else if (strstr(label, stringNumSats));
		else if (strstr(label, stringPrnObs));
		else if (strstr(label, stringEoH))
		{
			return 1;
		}
	}
	return 1;
}

/* obs type string to obs code -------------------------------------------------
* convert obs code type string to obs code
* args   : char   *str   I      obs code string ("1C","1P","1Y",...)
*          int    *freq  IO     frequency (1:L1,2:L2,3:L5,4:L6,5:L7,6:L8,0:err)
*                               (NULL: no output)
* return : obs code (CODE_???)
* notes  : obs codes are based on reference [6] and qzss extension
*-----------------------------------------------------------------------------*/
unsigned char obs2code(const char *obs, int *freq)
{
	int i;
	if (freq) *freq = 0;
	for (i = 1; *obscode[i]; i++) {
		if (strcmp(obscode[i], obs)) continue;
		if (freq) *freq = obsfreqs[i];
		return (unsigned char)i;
	}
	return CODE_NONE;
}

const char* code2obs(unsigned char code, int *frq)
{
	if (frq) *frq = 0;
	if (code <= CODE_NONE || MAXCODE < code) return "";
	if (frq) *frq = obsfreqs[code];
	return obscode[code];
}

/* search obs data index -----------------------------------------------------*/
int obs2index(double ver, int sys, const unsigned char *code,
	const char *tobs, const char *mask)
{
	char *id;
	int i;

	for (i = 0; i<NFREQ; i++) {

		/* signal mask */
		if (mask[code[i] - 1] == '0') continue;

		if (ver <= 2.99) { /* ver.2 */
			if (!strcmp(tobs, "C1") && (sys == SYS_GPS || sys == SYS_GLO || sys == SYS_QZS ||
				sys == SYS_SBS || sys == SYS_CMP)) {
				if (code[i] == CODE_L1C) return i;
			}
			else if (!strcmp(tobs, "P1")) {
				if (code[i] == CODE_L1P || code[i] == CODE_L1W || code[i] == CODE_L1Y ||
					code[i] == CODE_L1N) return i;
			}
			else if (!strcmp(tobs, "C2") && (sys == SYS_GPS || sys == SYS_QZS)) {
				if (code[i] == CODE_L2S || code[i] == CODE_L2L || code[i] == CODE_L2X)
					return i;
			}
			else if (!strcmp(tobs, "C2") && sys == SYS_GLO) {
				if (code[i] == CODE_L2C) return i;
			}
			else if (!strcmp(tobs, "P2")) {
				if (code[i] == CODE_L2P || code[i] == CODE_L2W || code[i] == CODE_L2Y ||
					code[i] == CODE_L2N || code[i] == CODE_L2D) return i;
			}
			else if (ver >= 2.12&&tobs[1] == 'A') { /* L1C/A */
				if (code[i] == CODE_L1C) return i;
			}
			else if (ver >= 2.12&&tobs[1] == 'B') { /* L1C */
				if (code[i] == CODE_L1S || code[i] == CODE_L1L || code[i] == CODE_L1X)
					return i;
			}
			else if (ver >= 2.12&&tobs[1] == 'C') { /* L2C */
				if (code[i] == CODE_L2S || code[i] == CODE_L2L || code[i] == CODE_L2X)
					return i;
			}
			else if (ver >= 2.12&&tobs[1] == 'D'&&sys == SYS_GLO) { /* GLO L2C/A */
				if (code[i] == CODE_L2C) return i;
			}
			else if (tobs[1] == '2'&&sys == SYS_CMP) { /* BDS B1 */
				if (code[i] == CODE_L1I || code[i] == CODE_L1Q || code[i] == CODE_L1X)
					return i;
			}
			else {
				id = code2obs(code[i], NULL);
				if (id[0] == tobs[1]) return i;
			}
		}
		else { /* ver.3 */
			id = code2obs(code[i], NULL);
			if (!strcmp(id, tobs + 1)) return i;
		}
	}
	return -1;
}



int getcodepri(int system, unsigned char code)
{
	char* p;
	char obs[3];
	int i, j;
	switch (system) {
	case SYS_GPS: i = 0; break;
	case SYS_GLO: i = 1; break;
	case SYS_GAL: i = 2; break;
	case SYS_CMP: i = 3; break;
	case SYS_SBS: i = 4; break;
	default: return 0;
	}
	strcpy(obs, code2obs(code, &j));
	/* search code priority */
	return (p = strchr(codepris[i][j - 1], obs[1])) ? 14 - (int)(p - codepris[i][j - 1]) : 0;
}



int shpData(int base, int i, char *obsType, dataIndex *index)
{
	int n;
	int sys = (int)(pow(base, i));
	char *p;
	if (strcmp(obsType, ""))
	{
		for (n = 1;; n++)
		{
			if (!strcmp(str2nstr(obsType, 1, 2), obscode[n]))
				break;
		}
		index->code = n;
		index->type = (p = strchr(obstype, obsType[0])) ? (int)(p - obstype) : 0;
		index->frq = obsfreqs[n];
		index->pri = getcodepri(sys, index->code);
		index->pos = -1;
		return 1;
	}
	else
		return 0;
}
int allocOBSMemory(obs_t *obs)    
{
	obsd_t *m_odata;
	if (obs->n >= obs->nmax)
	{
		obs->nmax += 72000;
		if (!(m_odata = (obsd_t*)realloc(obs->data, obs->nmax * sizeof(obsd_t))))
		{
			free(m_odata); m_odata = NULL; obs->n = obs->nmax = 0;
			return 0;
		}
		obs->data = m_odata;            
	}
	return 1;
}

int addeph(nav_t *nav, eph_t *eph)    
{
	eph_t *m_eph;
	if (nav->n >= nav->nmax)
	{
		nav->nmax += 256;
		if (!(m_eph = (eph_t*)realloc(nav->eph, nav->nmax * sizeof(eph_t))))
		{
			free(m_eph); m_eph = NULL; nav->n = nav->nmax = 0;
			return 0;
		}
		nav->eph = m_eph;           
	}
	nav->eph[nav->n++] = *eph;
	return 1;
}

int addgeph(nav_t *nav, geph_t *geph)    
{
	geph_t *m_geph;
	if (nav->ng >= nav->ngmax)
	{
		nav->ngmax += 128;
		if (!(m_geph = (geph_t*)realloc(nav->geph, nav->ngmax * sizeof(geph_t))))
		{
			free(m_geph); m_geph = NULL; nav->ng = nav->ngmax = 0;
			return 0;
		}
		nav->geph = m_geph;            //��new��memcpy�ɹ�, �¸���ģ�����
	}
	nav->geph[nav->ng++] = *geph;
	return 1;
}

int addseph(nav_t *nav, seph_t *seph)    //֮ǰ��malloc�����ڴ棬����ֻ�����ڴ棬������ù������������������string����Ķ����ò��ˣ�����new����
{
	seph_t *m_seph;
	if (nav->ns >= nav->nsmax)
	{
		nav->nsmax += 128;
		if (!(m_seph = (seph_t*)realloc(nav->seph, nav->nsmax * sizeof(seph_t))))
		{
			free(m_seph); m_seph = NULL; nav->ns = nav->nsmax = 0;
			return 0;
		}
		nav->seph = m_seph;            //��new��memcpy�ɹ�, �¸���ģ�����
	}
	nav->seph[nav->ns++] = *seph;
	return 1;
}

int navcpy(nav_t *nav, nav_t *nav1)
{
	int i, j;
	eph_t *m_eph;
	geph_t *m_geph;
	seph_t *m_seph;
	pclk_t *nav_pclk;
	peph_t *nav_peph;
	nav_t nav0 = { 0 };
	*nav = nav0;
	if (nav1->n)
	{
		nav->n = nav->nmax = nav1->n;
		if (!(m_eph = (eph_t*)realloc(nav->eph, nav->nmax * sizeof(eph_t))))
		{
			free(m_eph); m_eph = NULL; nav->n = nav->nmax = 0;
			return 0;
		}
		nav->eph = m_eph;
	}
	if (nav1->ng)
	{
		nav->ng = nav->ngmax = nav1->ng;
		if (!(m_geph = (geph_t*)realloc(nav->geph, nav->ngmax * sizeof(geph_t))))
		{
			free(m_geph); m_geph = NULL; nav->ng = nav->ngmax = 0;
			return 0;
		}
		nav->geph = m_geph;            //��new��memcpy�ɹ�, �¸���ģ�����
	}

	if (nav1->ns)
	{
		nav->ns = nav->nsmax = nav1->ns;
		if (!(m_seph = (seph_t*)realloc(nav->seph, nav->nsmax * sizeof(seph_t))))
		{
			free(m_seph); m_seph = NULL; nav->ns = nav->nsmax = 0;
			return 0;
		}
		nav->seph = m_seph;
	}

	if (nav1->nc) {
		nav->nc = nav->ncmax = nav1->nc;
		if (!(nav_pclk = (pclk_t *)realloc(nav->pclk, sizeof(pclk_t)*nav->ncmax))) {
			free(nav->pclk); nav->pclk = NULL; nav->nc = nav->ncmax = 0;
			return 0;
		}
		nav->pclk = nav_pclk;
	}

	if (nav1->ne) {
		nav->ne = nav->nemax = nav1->ne;
		if (!(nav_peph = (peph_t *)realloc(nav->peph, sizeof(peph_t)*nav->nemax))) {
			free(nav->peph); nav->peph = NULL; nav->ne = nav->nemax = 0;
			return 0;
		}
		nav->peph = nav_peph;
	}

	for (i = 0; i < MAXSAT; i++)
	{
		for (j = 0; j < NFREQ; j++)
		{
			nav->lam[i][j] = nav1->lam[i][j];
		}
		nav->pcvs[i] = nav1->pcvs[i];
		nav->ssr[i] = nav1->ssr[i];
		nav->ssrion = nav1->ssrion;
		for (j = 0; j < 3; j++)
		{
			nav->cbias[i][j] = nav1->cbias[i][j];
		}
	}
	for (i = 0; i < nav->n; i++)
		nav->eph[i] = nav1->eph[i];
	for (i = 0; i < nav->ng; i++)
		nav->geph[i] = nav1->geph[i];
	for (i = 0; i < nav->ns; i++)
		nav->seph[i] = nav1->seph[i];
	for (i = 0; i < nav->ne; i++)
		nav->peph[i] = nav1->peph[i];
	for (i = 0; i < nav->nc; i++)
		nav->pclk[i] = nav1->pclk[i];
	return 1;
}

int satsys2num(int sys)
{
	int ind = -1, i;
	for (i = 0;; i++)
	{
		if (sys % 2 != 1)
			sys = sys / 2;
		else
		{
			ind = i;
			break;
		}
	}
	return ind;
}
int syscodesIndex(char sys)
{
	char *p;
	return (p = strchr(syscodes, sys)) ? (int)(p - syscodes) : 0;
}


int ReadOBSData(FILE *fp, obs_t *obs, double *rinexVersion, int tsys, char *obs_Type, char (*obsType)[MAXOBSTYPE][4], int *obsNum, int *mulObsNum)
{
	int n, i = 0, j, k, m = 0, l, nsat = 0, mm, nn, ii, nobs = 0, sats[MAXOBS] = { 0 };
	char str[1024], satID[4];
	double ep[6];
	dataIndex index[NUMSYS][MAXOBS];
	int satsys, sys;
	int flag;
	int index_n[NUMSYS] = { 0 };
	int p[MAXOBSTYPE], kk[16], ll[16];
	double val[MAXOBSTYPE] = { 0 }, lli[MAXOBSTYPE] = { 0 }, ssi[MAXOBSTYPE] = { 0 };
	gtime_t time;
	obsd_t *data;
	for (n = 0; n < NUMSYS; n++)
	{
		for (i = 0; i < (*rinexVersion <= 2.99 ? *obsNum : mulObsNum[n]); i++)
		{
			shpData(2, n, obsType[n][i], index[n] + i);
		}
		index_n[n] = i;

		for (i = 0; i < NFREQ; i++)
		{
			for (j = 0, k = -1; j < index_n[n]; j++)
			{
				if (index[n][j].frq == i + 1 && index[n][j].pri && (k<0 || index[n][j].pri>index[n][k].pri))
					k = j;
			}
			if (k<0) continue;

			for (j = 0; j<index_n[n]; j++) {
				if (index[n][j].code == index[n][k].code) index[n][j].pos = i;
			}
		}
	}

	i = 0;

	if (!(data = (obsd_t *)malloc(sizeof(obsd_t)*MAXOBS))) return 0;

	while (fgets(str, MAXRNXLEN, fp))
	{
		if (m == 0)
		{
			if (*rinexVersion <= 2.99)
			{
				if (!timestr2ep(str2nstr(str, 0, 26), ep))continue;
				if ((nsat = atoi(str2nstr(str, 29, 3))) <= 0)
					continue;
				flag = atoi(str2nstr(str, 28, 1));

				time = epoch2time(ep);
				for (j = 0, k = 32; j < n; j++, k += 3)
				{
					if (k >= 68)
					{
						fgets(str, MAXRNXLEN, fp);
						k = 32;
					}
					if (j < MAXOBS)
					{
						strcpy(satID, str2nstr(str, k, 3));
						sats[i++] = satID2satIndex(satID);
					}
				}
			}
			else
			{
				if (str[0] != '>' || !timestr2ep(str2nstr(str, 1, 28), ep))
				{
//					printf("no rinex 3+ file!\n");
					continue;
				}
				if ((nsat = atoi(str2nstr(str, 32, 3))) <= 0)
					continue;
				time = epoch2time(ep);
				//if (time.time % 30 != 0)continue;
				flag = atoi(str2nstr(str, 31, 1));
			}
		}


		else if (flag <= 2 || flag == 6)
		{
			data[nobs].time = time;
			data[nobs].sat = (unsigned char)sats[m - 1];
			if (*rinexVersion > 2.99)
			{
				strcpy(satID, str2nstr(str, 0, 3));
				data[nobs].sat = satID2satIndex(satID);
			}
			if (data[nobs].sat)
			{

				for (l = 0, k = *rinexVersion <= 2.99 ? 0 : 3; l < (*rinexVersion <= 2.99 ? *obsNum : mulObsNum[syscodesIndex(satID[0])]) && l < MAXOBS; l++, k += 16)
				{
					if (*rinexVersion <= 2.99 && k >= 80)
					{
						fgets(str, MAXRNXLEN, fp);
						k = 0;
					}
					if ((int)strlen(str) > k)
					{
						val[l] = str2num(str, k, 14);
						lli[l] = atoi(str2nstr(str, k + 14, 1));
						ssi[l] = atoi(str2nstr(str, k + 15, 1));
					}
					else
						val[l] = lli[l] = ssi[l] = 0;
				}

				for (ii = 0; ii < NFREQ; ii++) {
					data[nobs].P[ii] = data[nobs].L[ii] = data[nobs].D[ii] = 0.0;
					data[nobs].SNR[ii] = data[nobs].LLI[ii] = data[nobs].code[ii] = 0;
				}
				data[nobs].vsat = 0;
				satsys = satIndex2sys(data[nobs].sat);
				data[nobs].sys = satsys;
				sys = satsys2num(satsys);
				for (mm = nn = ii = 0; ii < index_n[sys]; ii++)
				{
					p[ii] = *rinexVersion <= 2.11 ? index[sys][ii].frq - 1 : index[sys][ii].pos;
					if (index[sys][ii].type == 0 && p[ii] == 0) kk[nn++] = ii;         //C1?                           //���ڴ�����c1/p1,c2/p2�Ķ��ֲ�ͬ���͵����ݣ����������ȼ����ж�
					if (index[sys][ii].type == 0 && p[ii] == 1) ll[mm++] = ii;         //C2?
				}
				if (*rinexVersion <= 2.11)
				{
					if (nn >= 2)
					{
						if (val[kk[0]] == 0 && val[kk[1]] == 0)
						{
							p[kk[0]] = -1;
							p[kk[1]] = -1;
						}
						else if (val[kk[0]] != 0 && val[kk[1]] == 0)
						{
							p[kk[0]] = 0;
							p[kk[1]] = -1;
						}
						else if (val[kk[0]] == 0 && val[kk[1]] != 0)
						{
							p[kk[0]] = -1;
							p[kk[1]] = 0;
						}
						else if (index[sys][kk[0]].pri > index[sys][kk[1]].pri)
						{
							p[kk[0]] = 0;
							p[kk[1]] = -1;
						}
						else
						{
							p[kk[0]] = -1;
							p[kk[1]] = 0;
						}
					}
					if (mm >= 2)
					{
						if (val[ll[0]] == 0 && val[ll[1]] == 0)
						{
							p[ll[0]] = -1;
							p[ll[1]] = -1;
						}
						else if (val[ll[0]] != 0 && val[ll[1]] == 0)
						{
							p[ll[0]] = 1;
							p[ll[1]] = -1;
						}
						else if (val[ll[0]] == 0 && val[ll[1]] != 0)
						{
							p[ll[0]] = -1;
							p[ll[1]] = 1;
						}
						else if (index[sys][ll[0]].pri > index[sys][ll[1]].pri)
						{
							p[ll[0]] = 1;
							p[ll[1]] = -1;
						}
						else
						{
							p[ll[0]] = -1;
							p[ll[1]] = 1;
						}
					}
				}
				for (ii = 0; ii < index_n[sys]; ii++)
				{
					if (p[ii] < 0 || val[ii] == 0)
						continue;
					switch (index[sys][ii].type) {
					case 0: data[nobs].P[p[ii]] = val[ii]; data[nobs].code[p[ii]] = index[sys][ii].code; break;
					case 1: data[nobs].L[p[ii]] = val[ii]; data[nobs].LLI[p[ii]] = (unsigned char)lli[ii]; break;
					case 2: data[nobs].D[p[ii]] = val[ii]; break;
					case 3: data[nobs].SNR[p[ii]] = (unsigned char)(val[ii] * 4.0 + 0.5); break;
					}

				}
				data[nobs].rcv = 1;
				if (nobs < MAXOBS)nobs++;
			}
		}

		if (++m > nsat)
		{
			m = 0;
			for (i = 0; i < nobs; i++)
			{
				/* utc -> gpst */
				if (tsys == TSYS_UTC) data[i].time = timeadd(data[i].time, leapsecond(data[i].time));
			}
			for (i = 0; i < nobs; i++)
			{
				if (addobsdata(obs, data + i) < 0) break;
			}
			nobs = 0;
		}
	}

	free(data);
	if (!sortobs(obs)) return 0;
	return 1;
}

/* add obs data --------------------------------------------------------------*/
int addobsdata(obs_t *obs, const obsd_t *data)
{
	obsd_t *obs_data;

	if (obs->nmax <= obs->n) {
		if (obs->nmax <= 0) obs->nmax = NINCOBS; else obs->nmax *= 2;
		if (!(obs_data = (obsd_t *)realloc(obs->data, sizeof(obsd_t)*obs->nmax))) {
			trace(1, "addobsdata: memalloc error n=%dx%d\n", sizeof(obsd_t), obs->nmax);
			free(obs->data); obs->data = NULL; obs->n = obs->nmax = 0;
			return -1;
		}
		obs->data = obs_data;
	}
	obs->data[obs->n++] = *data;
	return 1;
}

int ReadOBSFile(char *file, obs_t *obs, sta_t *sta)
{
	FILE *fp;

	double rinexVersion;
	char obs_Type[4], obsType[NUMSYS][MAXOBSTYPE][4];
	int obsNum, mulObsNum[NUMSYS],tsys;
	if (!(fp = fopen(file, "r")))
	{
		printf("Obs file open error.\n");
		return 0;
	}
	if (!ReadOBSHead(fp, obs, sta, &rinexVersion,&tsys,obs_Type, obsType,&obsNum,mulObsNum))
	{
		printf("Obs file head read error.\n");
		fclose(fp);
		return 0;
	}
	if (!ReadOBSData(fp, obs, &rinexVersion, tsys, obs_Type, obsType, &obsNum, mulObsNum))
	{
		printf("Obs file body read error.\n");
		fclose(fp);
		return 0;
	}
	fclose(fp);
	return 1;
}

void releaseOBS(obs_t *obs)
{
	if (obs->data != NULL)
	{
		free(obs->data);
		obs->data = NULL;
		obs->n = obs->nmax = 0;
	}
}


int ReadNAVHead(FILE *fp, nav_t *nav,double *version,int *sys)
{
	int i, j;
	char str[1024] = { 0 };
	char label[30] = { 0 };
	char fileType[30], fileSys[30];
	while (fgets(str, MAXRNXLEN, fp))
	{
		if (strlen(str) < 60)
			continue;
		strcpy(label, str2nstr(str, 60, 20));
		if (strstr(label, stringVersion))
		{
			*version = str2num(str, 0, 20);
			strcpy(fileType, str2nstr(str, 20, 20));
			if (*version >= 3) {                        // ver 3
				if (fileType[0] != 'N' && fileType[0] != 'n') {
					printf("File type is not NAVIGATION : %s\n", fileType);
					return 0;
				}
				strcpy(fileSys, str2nstr(str, 40, 20));
				*sys = fileSys[0] == 'G' ? SYS_GPS : (fileSys[0] == 'R' ? SYS_GLO : (fileSys[0] == 'E' ? SYS_GAL : (fileSys[0] == 'C' ? SYS_CMP : (fileSys[0] == 'M' ? SYS_ALL : -1))));
			}
			else
			{
				if (fileType[0] == 'N' || fileType[0] == 'n')
					*sys = SYS_GPS;
				else if (fileType[0] == 'G' || fileType[0] == 'g')
					*sys = SYS_GLO;
				else {
					printf("Version 2 file type is invalid: %s\n", fileType);
					return 0;
				}
			}
		}
		else if (strstr(label, stringRunBy));
		else if (strstr(label, stringComment));
		else if (strstr(label, stringIonAlpha))
		{
			for (i = 0, j = 2; i < 4; i++, j += 12)
			{
				nav->ion_gps[i] = str2num(str, j, 12);
			}
		}
		else if (label == stringIonBeta)
		{
			for (i = 0, j = 2; i < 4; i++, j += 12)
			{
				nav->ion_gps[i + 4] = str2num(str, j, 12);
			}
		}
		else if (strstr(label, stringDeltaUTC))                //rinex 2
		{
			for (i = 0, j = 3; i<2; i++, j += 19)
				nav->utc_gps[i] = str2num(str, j, 19);
			for (; i < 4; i++, j += 9)
				nav->utc_gps[i] = str2num(str, j, 9);
		}
		else if (strstr(label, stringIonoCorr))
		{
			if (strstr(str, "GPSA"))
			{
				for (i = 0, j = 5; i < 4; i++, j += 12)
					nav->ion_gps[i] = str2num(str, j, 12);
			}
			else if (strstr(str, "GPSB"))
			{
				for (i = 0, j = 5; i < 4; i++, j += 12)
					nav->ion_gps[i + 4] = str2num(str, j, 12);
			}
			else if (strstr(str, "GAL"))
			{
				for (i = 0, j = 5; i < 4; i++, j += 12)
					nav->ion_gal[i] = str2num(str, j, 12);
			}
			else if (strstr(str, "BDSA"))
			{
				for (i = 0, j = 5; i < 4; i++, j += 12)
					nav->ion_cmp[i] = str2num(str, j, 12);
			}
			else if (strstr(str, "BDSB"))
			{
				for (i = 0, j = 5; i < 4; i++, j += 12)
					nav->ion_cmp[i + 4] = str2num(str, j, 12);
			}
		}
		else if (strstr(label, stringCorrSysTime)) { // "CORR TO SYSTEM TIME"  R2.10 GLO
			nav->utc_glo[0] = str2num(str, 0, 6);
			nav->utc_glo[0] = str2num(str, 6, 6);
			nav->utc_glo[0] = str2num(str, 12, 6);
			nav->utc_glo[0] = -str2num(str, 21, 19);    // -TauC
		}
		else if (strstr(label, stringTimeSysCorr))
		{
			if (strcmp(str2nstr(str, 0, 4), "GPUT") == 0)
			{
				nav->utc_gps[0] = str2num(str, 5, 17);
				nav->utc_gps[1] = str2num(str, 22, 16);
				nav->utc_gps[2] = str2num(str, 38, 7);
				nav->utc_gps[3] = str2num(str, 45, 5);
			}
			else if (strcmp(str2nstr(str, 0, 4), "GLUT") == 0)
			{
				nav->utc_glo[0] = str2num(str, 5, 17);
				nav->utc_glo[1] = str2num(str, 22, 16);
			}
			else if (strcmp(str2nstr(str, 0, 4), "GAUT") == 0)
			{
				nav->utc_gal[0] = str2num(str, 5, 17);
				nav->utc_gal[1] = str2num(str, 22, 16);
				nav->utc_gal[2] = str2num(str, 38, 7);
				nav->utc_gal[3] = str2num(str, 45, 5);
			}
			else if (strcmp(str2nstr(str, 0, 4), "BDUT") == 0)
			{
				nav->utc_cmp[0] = str2num(str, 5, 17);
				nav->utc_cmp[1] = str2num(str, 22, 16);
				nav->utc_cmp[2] = str2num(str, 38, 7);
				nav->utc_cmp[3] = str2num(str, 45, 5);
			}
		}
		else if (strstr(label, stringLeapSeconds));
		else if (strstr(label, stringEoH))
			return 1;

	}
	return 1;
}


int ReadNAVData(FILE *fp, nav_t *nav, double version,int sys)            //Ҫ��������R2��GPS��GLONASS����R3
{
	int i = 0, j, k, prn, index = 0, sp = 3;//sp��ʾ�ռ����ո�ʼ��
	gtime_t toc;
	double ep[6];
	char str[1024] = { 0 };
	char satID[4] = { 0 };
	int satIndex = 0;
	double data[64] = { 0.0 };
	eph_t eph;
	geph_t geph;
	seph_t seph;
	while (fgets(str, MAXRNXLEN, fp))
	{
		//if (addeph(nav,&eph) == 0)
		//    return 0;
		if (str2nstr(str, 0, 3) == "")
			index++;
		if (i == 0)
		{
			if (version >= 3.0 || sys == SYS_GAL || sys == SYS_CMP)
			{
				strcpy(satID, str2nstr(str, 0, 3));
				satIndex = satID2satIndex(satID);
				sp = 4;
				if (version > 3)
					sys = satIndex2sys(satIndex);
			}
			else
			{
				prn = atoi(str2nstr(str, 0, 2));
				if (sys == SYS_GLO)
				{
					sys = SYS_GLO;
					satIndex = satno2satIndex(sys, prn);
				}
				if (sys == SYS_GPS)
				{
					sys = SYS_GPS;
					satIndex = satno2satIndex(sys, prn);
				}

			}
			timestr2ep(str2nstr(str, sp, 19), ep);
			toc = epoch2time(ep);
			for (j = 0, k = sp + 19; j < 3; j++, k += 19)
				data[i++] = str2num(str, k, 19);
		}
		else
		{
			for (j = 0, k = sp; j < 4; j++, k += 19)
				data[i++] = str2num(str, k, 19);
			if (sys == SYS_GLO&&i >= 15)
			{
				if (!decode_gloeph(version, satIndex, toc, data, &geph))
					return 0;
				addgeph(nav, &geph);
				i = 0;
			}
			else if (sys == SYS_SBS&&i >= 15) {  ////�˴�Ϊ��ȡ��Ϲ㲥����ʱ��ӣ���Ҫ��ȫ����SBAS���ǣ���Ҫ�Ķ���
				if (!decode_sbaseph(version, satIndex, toc, data, &seph))
					return 0;
				addseph(nav, &seph);;
				i = 0;

			}
			else if (i >= 31)
			{
				if (!decode_eph(version, satIndex, toc, data, &eph))
					return 0;
				if (eph.sat != 0)
					addeph(nav, &eph);
				i = 0;
			}
		}
	}
	uniqnav(nav);
	return 1;
}


int decode_gloeph(double ver, int sat, gtime_t toc, double *data, geph_t *gloeph)
{
	geph_t geph0 = { 0 };
	gtime_t tof;
	double tow, tod;
	int week, dow;

	*gloeph = geph0;

	gloeph->sat = sat;

	/* toc rounded by 15 min in utc */
	tow = time2gpst(toc, &week);
	toc = gpst2time(week, floor((tow + 450.0) / 900.0) * 900);
	dow = (int)floor(tow / 86400.0);

	/* time of frame in utc */
	tod = ver <= 2.99 ? data[2] : fmod(data[2], 86400.0); /* tod (v.2), tow (v.3) in utc */
	tof = gpst2time(week, tod + dow*86400.0);
	tof = adjday(tof, toc);

	gloeph->toe = timeadd(toc, leapsecond(toc));   /* toc (gpst) */
	gloeph->tof = timeadd(toc, leapsecond(tof));   /* tof (gpst) */

												   /* iode = tb (7bit), tb =index of UTC+3H within current day */
	gloeph->iode = (int)(fmod(tow + 10800.0, 86400.0) / 900.0 + 0.5);

	gloeph->taun = -data[0];       /* -taun */
	gloeph->gamn = data[1];       /* +gamman */

	gloeph->pos[0] = data[3] * 1E3; gloeph->pos[1] = data[7] * 1E3; gloeph->pos[2] = data[11] * 1E3;
	gloeph->vel[0] = data[4] * 1E3; gloeph->vel[1] = data[8] * 1E3; gloeph->vel[2] = data[12] * 1E3;
	gloeph->acc[0] = data[5] * 1E3; gloeph->acc[1] = data[9] * 1E3; gloeph->acc[2] = data[13] * 1E3;

	gloeph->svh = (int)data[6];
	gloeph->frq = (int)data[10];
	gloeph->age = (int)data[14];

	/* some receiver output >128 for minus frequency number */
	if (gloeph->frq>128) gloeph->frq -= 256;

	//	if (gloeph->frq<MINFREQ_GLO || MAXFREQ_GLO<gloeph->frq) {
	//			trace(2, "rinex gnav invalid freq: sat=%2d fn=%d\n", sat, gloeph->frq);
	//		}
	return 1;
}

int decode_sbaseph(double ver, int sat, gtime_t toc, double *data, seph_t *seph)
{
	seph_t seph0 = { 0 };
	int week;

	*seph = seph0;

	seph->sat = sat;
	seph->t0 = toc;

	time2gpst(toc, &week);
	seph->tof = adjweek(gpst2time(week, data[2]), toc);

	seph->af0 = data[0];
	seph->af1 = data[1];

	seph->pos[0] = data[3] * 1E3; seph->pos[1] = data[7] * 1E3; seph->pos[2] = data[11] * 1E3;
	seph->vel[0] = data[4] * 1E3; seph->vel[1] = data[8] * 1E3; seph->vel[2] = data[12] * 1E3;
	seph->acc[0] = data[5] * 1E3; seph->acc[1] = data[9] * 1E3; seph->acc[2] = data[13] * 1E3;

	seph->svh = (int)data[6];
	seph->sva = uraindex(data[10]);

	return 1;
}

int decode_eph(double ver, int sat, gtime_t toc, double *data, eph_t *eph)
{
	eph_t eph0 = { 0 };
	int sys;
	int week;
	double sec;
	sys = satIndex2sys(sat);

	if (!(sys&(SYS_GPS | SYS_GAL | SYS_CMP))) {
		printf("GPS ephemeris doesn't match!\n");
		return 0;
	}
	*eph = eph0;

	eph->sat = sat;
	eph->toc = toc;

	eph->f0 = data[0];
	eph->f1 = data[1];
	eph->f2 = data[2];

	eph->A = SQR(data[10]); eph->e = data[8]; eph->i0 = data[15]; eph->OMG0 = data[13];
	eph->omg = data[17]; eph->M0 = data[6]; eph->deln = data[5]; eph->OMGd = data[18];
	eph->idot = data[19]; eph->crc = data[16]; eph->crs = data[4]; eph->cuc = data[7];
	eph->cus = data[9]; eph->cic = data[12]; eph->cis = data[14];

	if (sys == SYS_GPS) {
		eph->iode = (int)data[3];      /* IODE */
		eph->iodc = (int)data[26];      /* IODC */
		eph->toes = data[11];      /* toe (s) in gps week */
		eph->week = (int)data[21];      /* gps week */
		week = eph->week;
		sec = data[11];

		eph->toe = adjweek(gpst2time(week, sec), toc);

		sec = data[27];
		eph->ttr = adjweek(gpst2time(week, sec), toc);

		eph->code = (int)data[20];      /* GPS: codes on L2 ch */
		eph->svh = (int)data[24];      /* sv health */
		eph->sva = uraindex(data[23]);  /* ura (m->index) */
		eph->flag = (int)data[22];      /* GPS: L2 P data flag */

		eph->tgd[0] = data[25];      /* TGD */
		eph->fit = data[28];      /* fit interval */
	}
	else if (sys == SYS_GAL) { /* GAL ver.3 */
		eph->iode = (int)data[3];      /* IODnav */
		eph->toes = data[11];      /* toe (s) in galileo week */
		eph->week = (int)data[21];      /* gal week = gps week */
		week = eph->week;
		sec = data[11];
		eph->toe = adjweek(gpst2time(week, sec), toc);
		sec = data[27];
		eph->ttr = adjweek(gpst2time(week, sec), toc);

		eph->code = (int)data[20];      /* data sources */
										/* bit 0 set: I/NAV E1-B */
										/* bit 1 set: F/NAV E5a-I */
										/* bit 2 set: F/NAV E5b-I */
										/* bit 8 set: af0-af2 toc are for E5a.E1 */
										/* bit 9 set: af0-af2 toc are for E5b.E1 */
		eph->svh = (int)data[24];      /* sv health */
									   /* bit     0: E1B DVS */
									   /* bit   1-2: E1B HS */
									   /* bit     3: E5a DVS */
									   /* bit   4-5: E5a HS */
									   /* bit     6: E5b DVS */
									   /* bit   7-8: E5b HS */
		eph->sva = uraindex(data[23]); /* ura (m->index) */

		eph->tgd[0] = data[25];      /* BGD E5a/E1 */
		eph->tgd[1] = data[26];      /* BGD E5b/E1 */
	}
	else if (sys == SYS_CMP) { /* BeiDou v.3.02 */
		eph->toc = bdt2gpst(eph->toc);  /* bdt -> gpst */
		eph->iode = (int)data[3];      /* AODE */
		eph->iodc = (int)data[28];      /* AODC */
		eph->toes = data[11];      /* toe (s) in bdt week */
		eph->week = (int)data[21];      /* bdt week */
										//Add by GF : BDST to GPST +14s
		week = eph->week;
		sec = data[11];
		eph->toe = bdt2gpst(bdt2time(week, sec)); /* bdt -> gpst */
		sec = data[27];
		eph->ttr = bdt2gpst(bdt2time(week, sec)); /* bdt -> gpst */
		eph->toe = adjweek(eph->toe, toc);
		eph->ttr = adjweek(eph->ttr, toc);

		eph->svh = (int)data[24];      /* satH1 */
		eph->sva = uraindex(data[23]);  /* ura (m->index) */

		eph->tgd[0] = data[25];      /* TGD1 B1/B3 */
		eph->tgd[1] = data[26];      /* TGD2 B1/B3 */
	}
	if (eph->iode < 0 || 1023 < eph->iode) {
		;
	}
	if (eph->iodc < 0 || 1023 < eph->iodc) {
		;
	}
	return 1;

}


double GetSatWaveLength(int sat, int frq, nav_t *nav)
{
	const double freq_glo[] = { FREQ1_GLO, FREQ2_GLO, FREQ3_GLO };
	const double dfrq_glo[] = { DFRQ1_GLO, DFRQ2_GLO, 0.0 };
	int i, sys = satIndex2sys(sat);

	if (sys == SYS_GLO) {
		if (0 <= frq&&frq <= 2) {
			for (i = 0; i<nav->ng; i++) {
				if (nav->geph[i].sat != sat) continue;
				return CLIGHT / (freq_glo[frq] + dfrq_glo[frq] * nav->geph[i].frq);
			}
		}
	}
	else if (sys == SYS_CMP) {
		if (frq == 0) return CLIGHT / FREQ1_CMP; /* B1 */
		else if (frq == 1) return CLIGHT / FREQ2_CMP; /* B2 */
		else if (frq == 2) return CLIGHT / FREQ3_CMP; /* B3 */
	}
	else {
		if (frq == 0) return CLIGHT / FREQ1; /* L1/E1 */
		else if (frq == 1) return CLIGHT / FREQ2; /* L2 */
		else if (frq == 2) return CLIGHT / FREQ5; /* L5/E5a */
		else if (frq == 3) return CLIGHT / FREQ6; /* L6/LEX */
		else if (frq == 4) return CLIGHT / FREQ7; /* E5b */
		else if (frq == 5) return CLIGHT / FREQ8; /* E5a+b */
	}
	return 0.0;
}

int ReadNAVFile(char *file, nav_t *nav)
{
	FILE *fp;
	double version;
	int sys;
	if (!(fp = fopen(file, "r")))
	{
		printf("Nav file open error!\n");
		return 0;
	}
	if (!ReadNAVHead(fp, nav,&version,&sys))
	{
		printf("Nav file head read error!\n");
		fclose(fp);
		return 0;
	}
	if (!ReadNAVData(fp, nav,version,sys))
	{
		printf("Nav file body read error!\n");
		fclose(fp);
		return 0;
	}
	fclose(fp);
	return 1;
}

void releaseNAV(nav_t *nav)
{
	if (nav->eph != NULL)
	{
		free(nav->eph);
		nav->eph = NULL;
		nav->n = nav->nmax = 0;
	}
	if (nav->geph != NULL)
	{
		free(nav->geph);
		nav->geph = NULL;
		nav->ng = nav->ngmax = 0;
	}
	if (nav->seph != NULL)
	{
		free(nav->seph);
		nav->seph = NULL;
		nav->ns = nav->nsmax = 0;
	}

	if (nav->tec != NULL)
	{
		free(nav->tec);
		nav->tec = NULL;
		nav->nt = nav->ntmax = 0;
	}
}

int addsp3(nav_t *nav, peph_t *peph)
{
	peph_t *nav_peph;

	if (nav->ne >= nav->nemax) {
		nav->nemax += 256;
		if (!(nav_peph = (peph_t *)realloc(nav->peph, sizeof(peph_t)*nav->nemax))) {
			free(nav->peph); nav->peph = NULL; nav->ne = nav->nemax = 0;
			return 0;
		}
		nav->peph = nav_peph;
	}
	nav->peph[nav->ne++] = *peph;
	return 1;
}
int ReadSP3Head(FILE*fp, nav_t *nav,char *type,double *bfact,char *tsys,int *sats,int *ns)
{
	int i, j, k = 0;
	char satID[4] = { 0 };
	char str[1024] = { 0 };
	char label[30] = { 0 };
	double ep[6] = { 0 };
	gtime_t time;

	for (i = 0; i < 22; i++)
	{
		if (!fgets(str, MAXRNXLEN, fp))
			break;
		if (i == 0) {
			type[0] = str[2];

			if (!str2time(str, 3, 28, &time)) return 0;
		}
		else if (2 <= i&&i <= 6) {
			if (i == 2) {
				*ns = atoi(str2nstr(str, 4, 2));
			}
			for (j = 0; j<17 && k<*ns; j++) {
				strcpy(satID, str2nstr(str, 9 + 3 * j, 3));
				if (k<MAXSAT) sats[k++] = satID2satIndex(satID);
			}
		}
		else if (i == 12) {
			strcpy(tsys, str2nstr(str, 9, 3));
		}
		else if (i == 14) {
			bfact[0] = str2num(str, 3, 10);
			bfact[1] = str2num(str, 14, 12);
		}
	}
	return 1;
}
int ReadSP3Data(FILE*fp, nav_t *nav,char type,double *bfact, char *tsys, int ns)
{
	peph_t peph;
	gtime_t time;
	double val, std, base;
	int i, j, sat, n = ns*(type == 'P' ? 1 : 2), v;
	char str[1024], satID[4];
	while (fgets(str, MAXRNXLEN, fp)) {

		if (!strncmp(str, "EOF", 3)) break;
		if (str[0] != '*' || !str2time(str, 3, 28, &time))
			continue;
		if (!strcmp(tsys, "UTC")) time = timeadd(time, leapsecond(time)); /* utc->gpst */
		peph.time = time;
		peph.index = 0;
		for (i = 0; i<MAXSAT; i++) {
			for (j = 0; j<4; j++) {
				peph.pos[i][j] = 0.0;
				peph.std[i][j] = 0.0f;
				peph.vel[i][j] = 0.0;
				peph.vst[i][j] = 0.0f;
			}
			for (j = 0; j<3; j++) {
				peph.cov[i][j] = 0.0f;
				peph.vco[i][j] = 0.0f;
			}
		}

		for (i = v = 0; i < n&&fgets(str, MAXRNXLEN, fp); i++)
		{
			if (str[0] == '*')
			{
				fseek(fp, -(strlen(str)+1), SEEK_CUR);
				break;
			}
			if (strlen(str)<4 || (str[0] != 'P'&&str[0] != 'V')) continue;
			strcpy(satID, str2nstr(str, 1, 3));
			sat = satID2satIndex(satID);
			if (sat <= 0)continue;
			for (j = 0; j < 4; j++)
			{
				val = str2num(str, 4 + j * 14, 14);
				std = str2num(str, 61 + j * 3, j<3 ? 2 : 3);
				if (str[0] == 'P')
				{
					if (val != 0 && fabs(val - 999999.999999) >= 1E-6)
					{
						peph.pos[sat - 1][j] = val*(j < 3 ? 1000.0 : 1E-6);
						v = 1;
					}
					if ((base = bfact[j<3 ? 0 : 1])>0.0&&std>0.0)
					{
						peph.std[sat - 1][j] = (float)(pow(base, std)*(j < 3 ? 1E-3 : 1E-12));
					}
				}
				else if (v) { /* velocity */
					if (val != 0.0&&fabs(val - 999999.999999) >= 1E-6) {
						peph.vel[sat - 1][j] = val*(j<3 ? 0.1 : 1E-10);
					}
					if ((base = bfact[j<3 ? 0 : 1])>0.0&&std>0.0) {
						peph.vst[sat - 1][j] = (float)(pow(base, std)*(j<3 ? 1E-7 : 1E-16));
					}
				}
			}

		}

		if (v)
		{
			if (!addsp3(nav, &peph))
				return 0;
		}
	}

	return 1;
}

int ReadSP3File(char *file, nav_t *nav)
{
	FILE *fp;
	char *ext;
	double bfact[2];
	int ns;
	char type;
	int sats[MAXSAT];
	char tsys[4];
	if (!(ext = strrchr(file, '.')))
		return 0;
	if (!strstr(ext + 1, "sp3") && !strstr(ext + 1, "SP3") &&
		!strstr(ext + 1, "eph") && !strstr(ext + 1, "EPH")) return 0;


	if (!(fp = fopen(file, "r")))
	{
		printf("SP3 file open error.\n");
		return 0;
	}
	if (!ReadSP3Head(fp, nav,&type,bfact,tsys,sats,&ns))
	{
		printf("SP3 file head read error.\n");
		fclose(fp);
		return 0;
	}
	if (!ReadSP3Data(fp, nav,type,bfact,tsys,ns))
	{
		printf("SP3 file body read error.\n");
		fclose(fp);
		return 0;
	}
	fclose(fp);
	return 1;
}

double ReadCLKHead(FILE *fp)
{
	char str[1024] = { 0 };
	char label[30] = { 0 };
	char type = '\0';
	double version = 0.0;
	int p = 0;
	while (fgets(str, MAXRNXLEN, fp))
	{
		if (strlen(str) < 60)
			continue;
		if (strstr(str, stringVersion))
		{
			version = str2num(str, 0, 20);
			if (version > 3.03)
				p = 1;
			type = str2nstr(str, 20+p, 1)[0];
			if (type != 'C'&&type != 'c')
			{
				printf("File type is not CLOCK : %c\n", type);
				return 0;
			}
		}
		else if (strstr(str, stringEoH))
		{
			return version;
		}
	}
	return version;
}


int addclk(nav_t *nav, pclk_t *pclk)
{
	pclk_t *nav_pclk;

	if (nav->nc >= nav->ncmax) {
		nav->ncmax += 1024;
		if (!(nav_pclk = (pclk_t *)realloc(nav->pclk, sizeof(pclk_t)*nav->ncmax))) {
			free(nav->pclk); nav->pclk = NULL; nav->nc = nav->ncmax = 0;
			return 0;
		}
		nav->pclk = nav_pclk;
	}
	nav->pclk[nav->nc++] = *pclk;
	return 1;
}

/* compare precise clock -----------------------------------------------------*/
int cmppclk(const void *p1, const void *p2)
{
	pclk_t *q1 = (pclk_t *)p1, *q2 = (pclk_t *)p2;
	double tt = timediff(q1->time, q2->time);
	return tt<-1E-9 ? -1 : 1;
}
/* combine precise clock -----------------------------------------------------*/
void combpclk(nav_t *nav)
{
	pclk_t *nav_pclk;
	int i, j, k;

	if (nav->nc <= 0) return;

	qsort(nav->pclk, nav->nc, sizeof(pclk_t), cmppclk);

	for (i = 0, j = 1; j<nav->nc; j++) {
		if (fabs(timediff(nav->pclk[i].time, nav->pclk[j].time))<1E-9) {
			for (k = 0; k<MAXSAT; k++) {
				if (nav->pclk[j].clk[k][0] == 0.0) continue;
				nav->pclk[i].clk[k][0] = nav->pclk[j].clk[k][0];
				nav->pclk[i].std[k][0] = nav->pclk[j].std[k][0];
			}
		}
		else if (++i<j) nav->pclk[i] = nav->pclk[j];
	}
	nav->nc = i + 1;

	if (!(nav_pclk = (pclk_t *)realloc(nav->pclk, sizeof(pclk_t)*nav->nc))) {
		free(nav->pclk); nav->pclk = NULL; nav->nc = nav->ncmax = 0;
		return;
	}
	nav->pclk = nav_pclk;
	nav->ncmax = nav->nc;

}

int ReadCLKData(FILE *fp, nav_t *nav, prcopt_t *opt, double ver)
{
	pclk_t pclk = { 0 };
	gtime_t time;
	double data[2];
	int i, j, sat, p=0;
	char str[MAXRNXLEN], satid[8] = "";

	if (!nav) return 0;
	if (ver > 3.03)p = 5;

	while (fgets(str, MAXRNXLEN, fp)) {

		if (!str2time(str, 8+p, 26, &time)) {
			continue;
		}
		strcpy(satid, str2nstr(str, 3, 4));

		/* only read AS (satellite clock) record */
		if (strncmp(str, "AS", 2) || !(sat = satID2satIndex(satid))) continue;

		if (!(satIndex2sys(sat)&opt->navigationSystem)) continue;//��ʱֻ��GPS

		for (i = 0, j = 40+p; i<2; i++, j += 20)
			data[i] = str2num(str, j, 19);

		if (fabs(timediff(time, pclk.time))>1E-9) {
			pclk.time = time;
			pclk.index = 0;
			for (i = 0; i<MAXSAT; i++) {
				pclk.clk[i][0] = 0.0;
				pclk.std[i][0] = 0.0f;
			}
		}
		pclk.clk[sat - 1][0] = data[0];
		pclk.std[sat - 1][0] = (float)data[1];
		if (!addclk(nav, &pclk)) return 0;
	}
	combpclk(nav);
	return nav->nc>0;
}

int ReadCLKFile(char *file, nav_t *nav, prcopt_t *opt)
{
	FILE *fp;
	char *ext;
	double verc;
	if (!(ext = strrchr(file, '.')))
		return 0;
	if (strcmp(ext, ".clk") && strcmp(ext, ".clk_30s") && strcmp(ext, ".clk_5s")
		&& strcmp(ext, ".CLK") && strcmp(ext, ".CLK_30S") && strcmp(ext, ".CLK_5S")) return 0;

	if (!(fp = fopen(file, "r")))
	{
		printf("CLK file open error.\n");
		return 0;
	}
	if (!(verc=ReadCLKHead(fp)))
	{
		printf("CLK file head read error.\n");
		fclose(fp);
		return 0;
	}
	if (!ReadCLKData(fp, nav, opt,verc))
	{
		printf("CLK file body read error.\n");
		fclose(fp);
		return 0;
	}
	fclose(fp);
	return 1;
}

void releaseSP3CLK(nav_t *nav)
{
	if (nav->peph != NULL)
	{
		free(nav->peph);
		nav->peph = NULL;
		nav->ne = nav->nemax = 0;
	}
	if (nav->pclk != NULL)
	{
		free(nav->pclk);
		nav->pclk = NULL;
		nav->nc = nav->ncmax = 0;
	}
}


int readDCBFile(const char *file, nav_t *nav)
{
	FILE *fp;
	double cbias;
	int sat, type = 0;
	char buff[256], *ext;

	//	trace(3, "readdcbf: file=%s\n", file);

	if (!(fp = fopen(file, "r"))) {
		printf("dcb parameters file open error: %s\n", file);
		return 0;
	}

	ext = strrchr(file, '.');
	if (!stricmp(ext, ".dcb"))
	{
		while (fgets(buff, sizeof(buff), fp)) {

			if (strstr(buff, "DIFFERENTIAL (P1-P2) CODE BIASES")) type = 1;
			else if (strstr(buff, "DIFFERENTIAL (P1-C1) CODE BIASES")) type = 2;
			else if (strstr(buff, "DIFFERENTIAL (P2-C2) CODE BIASES")) type = 3;

			if (!type) continue;

			if (!(sat = satID2satIndex(buff)) || (cbias = str2num(buff, 26, 9)) == 0.0) continue;

			nav->cbias[sat - 1][type - 1] = cbias * 1E-9 * CLIGHT; /* ns -> m */
		}
	}

	if (!stricmp(ext, ".bsx"))
	{
		while (fgets(buff, sizeof(buff), fp)) {//需要进一步扩展其他系统的频点信息

			if (strncmp(buff + 1, "DSB", 3))continue;
			if (!satID2satIndex(buff + 11))continue;

			if (strstr(buff, "C1W  C2W")) type = 1;
			else if (strstr(buff, "C1C  C1W")) type = 2;
			else if (strstr(buff, "C2C  C2W")) type = 3;
			else if (strstr(buff, "C2W  C2X")) type = 4;
			else type = 0;

			if (!type) continue;

			if (!(sat = satID2satIndex(buff + 11)) || (cbias = str2num(buff, 70, 21)) == 0.0) continue;


			if (type == 2)cbias *= -1;
			if (type == 3)cbias *= -1;

			nav->cbias[sat - 1][type - 1] = cbias * 1E-9 * CLIGHT; /* ns -> m */
		}
	}

	fclose(fp);

	return 1;
}


/* read dcb parameters file --------------------------------------------------*/
int readRcvDCBFile(const char* file, sta_t* sta)
{
	FILE* fp;
	double cbias;
	int sat, type = 0, nsys = 0;
	char buff[256];
	char sitename[5] = { '\0' }, name[5] = { '\0' }, * label, * ext;

	strncpy(sitename, sta->stationName, 4);//
	//uppercase(sitename);// 

	sta->dcb[0] = sta->dcbrms[0] = 0;

	if (!(fp = fopen(file, "r"))) {
		return 0;
	}

	if (!(ext = strrchr(file, '.'))) return 0;

	if (stricmp(ext, ".txt") == 0) {
		while (fgets(buff, sizeof(buff), fp)) {
			if (strlen(buff) < 47) continue;
			strncpy(name, buff + 6, 4);//uppercase(name);
			if (buff[0] == 'G' && stricmp(name, sitename) == 0) {
				cbias = str2num(buff, 26, 34);
				sta->dcb[0] = cbias/**1E-9*CLIGHT*/; /* ns -> m */
				sta->dcbrms[0] = str2num(buff, 38, 46)/**1E-9*CLIGHT*/;
				nsys++;
				if (nsys == NSYS) break;
			}
			else if (buff[0] == 'R' && stricmp(name, sitename) == 0) {
				cbias = str2num(buff, 26, 34);
				sta->dcb[1] = cbias/**1E-9*CLIGHT*/; /* ns -> m */
				sta->dcbrms[1] = str2num(buff, 38, 46)/**1E-9*CLIGHT*/;
				nsys++;
				if (nsys == NSYS) break;
			}
			else if (buff[0] == 'E' && stricmp(name, sitename) == 0) {
				cbias = str2num(buff, 26, 34);
				sta->dcb[2] = cbias/**1E-9*CLIGHT*/; /* ns -> m */
				sta->dcbrms[2] = str2num(buff, 38, 46)/**1E-9*CLIGHT*/;
				nsys++;
				if (nsys == NSYS) break;
			}
			else if (buff[0] == 'C' && stricmp(name, sitename) == 0) {
				cbias = str2num(buff, 26, 34);
				sta->dcb[3] = cbias/**1E-9*CLIGHT*/; /* ns -> m */
				sta->dcbrms[3] = str2num(buff, 38, 46)/**1E-9*CLIGHT*/;
				nsys++;
				if (nsys == NSYS) break;
			}

		}

	}
	else {//rcv dcb from GIM(COD)
		while (fgets(buff, sizeof(buff), fp)) {
			if (strlen(buff) < 60) continue;
			label = buff + 60;

			if (strstr(label, "STATION / BIAS / RMS") == label) {
				strncpy(name, buff + 6, 4);
				if (buff[3] == 'G' && !stricmp(name, sitename)) {
					cbias = str2num(buff, 26, 35);
					sta->dcb[0] = cbias/**1E-9*CLIGHT*/; /* ns -> m */
					sta->dcbrms[0] = str2num(buff, 36, 45)/**1E-9*CLIGHT*/;
					nsys++;
					if (nsys == NSYS) break;
				}
				else if (buff[3] == 'R' && !stricmp(name, sitename)) {
					cbias = str2num(buff, 26, 35);
					sta->dcb[1] = cbias/**1E-9*CLIGHT*/; /* ns -> m */
					sta->dcbrms[1] = str2num(buff, 36, 45)/**1E-9*CLIGHT*/;
					nsys++;
					if (nsys == NSYS) break;
				}
				else if (buff[3] == 'E' && !stricmp(name, sitename)) {
					cbias = str2num(buff, 26, 35);
					sta->dcb[2] = cbias/**1E-9*CLIGHT*/; /* ns -> m */
					sta->dcbrms[2] = str2num(buff, 36, 45)/**1E-9*CLIGHT*/;
					nsys++;
					if (nsys == NSYS) break;
				}
				else if (buff[3] == 'C' && !stricmp(name, sitename)) {
					cbias = str2num(buff, 26, 35);
					sta->dcb[3] = cbias/**1E-9*CLIGHT*/; /* ns -> m */
					sta->dcbrms[3] = str2num(buff, 36, 45)/**1E-9*CLIGHT*/;
					nsys++;
					if (nsys == NSYS) break;
				}
			}
			else if (strstr(label, "END OF AUX DATA") == label) break;
		}

	}

	fclose(fp);

	return 1;
}





