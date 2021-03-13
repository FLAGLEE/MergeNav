#include"configuration.h"

prcopt_t prcopt_;
solopt_t solopt_;
filopt_t filopt_;
int antpostype_;
double elmask_, elmaskar_, elmaskhold_;
double antpos_[3];
char exsats_[1024];
char snrmask_[NFREQ][1024];

opt_t sysopts[] = {
	{ "pos1-posrtmode", 0, (void *)&prcopt_.processRealtime, SWTOPT },
	{ "pos1-posmode", 0, (void *)&prcopt_.processMode, MODOPT },
	//{"pos1-frequency",  3,  (void *)&prcopt_.nf,         FRQOPT },
	{ "pos1-freqnum", 0, (void *)&prcopt_.frequenceNumber, "" },
	{ "pos1-freqopt", 0, (void *)&prcopt_.frequenceMode, FRQOPT },

	{ "pos1-elmask", 1, (void *)&elmask_, "deg" },
	{ "pos1-snrmask_r", 3, (void *)&prcopt_.snrMask.ena[0], SWTOPT },
	{ "pos1-snrmask_L1", 2, (void *)snrmask_[0], "" },
	{ "pos1-snrmask_L2", 2, (void *)snrmask_[1], "" },
	{ "pos1-snrmask_L5", 2, (void *)snrmask_[2], "" },
	{ "pos1-dynamics", 0, (void *)&prcopt_.dynamics, SWTOPT },
	{ "pos1-tidecorr", 0, (void *)&prcopt_.tideCorrect, SWTOPT },
	{ "pos1-ionoopt", 0, (void *)&prcopt_.ionosphereOption, IONOPT },
	{ "pos1-tropopt", 0, (void *)&prcopt_.troposphereOption, TRPOPT },
	{ "pos1-sateph", 0, (void *)&prcopt_.satelliteEphemeris, EPHOPT },
	{ "pos1-posopt1", 0, (void *)&prcopt_.posopt[0], SWTOPT },
	{ "pos1-posopt2", 0, (void *)&prcopt_.posopt[1], SWTOPT },
	{ "pos1-posopt3", 0, (void *)&prcopt_.posopt[2], SWTOPT },
	{ "pos1-posopt4", 0, (void *)&prcopt_.posopt[3], SWTOPT },
	{ "pos1-posopt5", 0, (void *)&prcopt_.posopt[4], SWTOPT },
	{ "pos1-posopt6", 0, (void *)&prcopt_.posopt[5], SWTOPT },
	{ "pos1-posopt7", 0, (void *)&prcopt_.posopt[6], SWTOPT },
	{ "pos1-exclsats", 2, (void *)exsats_, "prn ..." },
	{ "pos1-navsys", 0, (void *)&prcopt_.navigationSystem, SYSOPT },

	{ "pos2-rovpos", 0, (void *)&prcopt_.rovpos, "0:blh,1:xyz" },
	{ "pos2-armode", 0, (void *)&prcopt_.modeAR, ARMOPT },
	{ "pos2-gloarmode", 0, (void *)&prcopt_.glonassModeAR, GAROPT },
	{ "pos2-arthres", 1, (void *)&prcopt_.thresholdAR[0], "" },
	{ "pos2-arlockcnt", 0, (void *)&prcopt_.minLock, "" },
	{ "pos2-arelmask", 1, (void *)&elmaskar_, "deg" },
	{ "pos2-arminfix", 0, (void *)&prcopt_.minFix, "" },
	{ "pos2-elmaskhold", 1, (void *)&elmaskhold_, "deg" },
	{ "pos2-aroutcnt", 0, (void *)&prcopt_.maxOut, "" },
	{ "pos2-maxage", 1, (void *)&prcopt_.maxtdiff, "s" },
	{ "pos2-maxinno", 1, (void *)&prcopt_.maxinno, "m" },
	{ "pos2-maxgdop", 1, (void *)&prcopt_.maxgdop, "" },
	{ "pos2-syncsol", 3, (void *)&prcopt_.syncsol, SWTOPT },
	{ "pos2-slipthres", 1, (void *)&prcopt_.thresholdGFSlip, "m" },
	{ "pos2-maxthreswl", 1, (void *)&prcopt_.thresholdWLSlip, "m" },
	{ "pos2-rejionno", 1, (void *)&prcopt_.maxinno, "m" },
	{ "pos2-rejgdop", 1, (void *)&prcopt_.maxgdop, "" },
	{ "pos2-niter", 0, (void *)&prcopt_.nIteration, "" },
	{ "pos2-robust", 0, (void *)&prcopt_.robust, KALOPT },
	{ "pos2-adapt", 0, (void *)&prcopt_.adapt, ADAOPT },
	{ "pos2-doppler", 0, (void *)&prcopt_.doppler, DOPOPT },
	{ "pos2-trajec", 0, (void *)&prcopt_.trajec, SWTOPT },
	{ "pos2-interval", 1, (void *)&prcopt_.interval, "" },
	{ "pos2-dcbcorr", 0, (void *)&prcopt_.dcbCorrect, DCBOPT },
	{ "pos2-cs_LL", 0, (void *)&prcopt_.csopt[0], SWTOPT },
	{ "pos2-cs_GF", 0, (void *)&prcopt_.csopt[1], SWTOPT },
	{ "pos2-cs_MW", 0, (void *)&prcopt_.csopt[2], SWTOPT },
	{ "pos2-cs_TECR", 0, (void *)&prcopt_.csopt[3], SWTOPT },
	{ "pos2-cs_L1C1", 0, (void *)&prcopt_.csopt[4], SWTOPT },
	{ "pos2-cs_DOPPLER", 0, (void *)&prcopt_.csopt[5], SWTOPT },


	{ "out1-solformat", 0, (void *)&solopt_.posf, SOLOPT },
	{ "out1-outhead", 0, (void *)&solopt_.outhead, SWTOPT },
	{ "out1-outopt", 0, (void *)&solopt_.outopt, SWTOPT },
	{ "out1-timesys", 0, (void *)&solopt_.times, TSYOPT },
	{ "out1-timeform", 0, (void *)&solopt_.timef, TFTOPT },
	{ "out1-timendec", 0, (void *)&solopt_.timeu, "" },
	{ "out1-degform", 0, (void *)&solopt_.degf, DFTOPT },
	{ "out1-fieldsep", 2, (void *)solopt_.sep, "" },
	{ "out1-height", 0, (void *)&solopt_.height, HGTOPT },
	{ "out1-geoid", 0, (void *)&solopt_.geoid, GEOOPT },
	{ "out1-solstatic", 0, (void *)&solopt_.solstatic, STAOPT },
	{ "out1-nmeaintv1", 1, (void *)&solopt_.nmeaintv[0], "s" },
	{ "out1-nmeaintv2", 1, (void *)&solopt_.nmeaintv[1], "s" },
	{ "out1-outstat", 0, (void *)&solopt_.sstat, STSOPT },

	{ "stats-eratio1", 1, (void *)&prcopt_.eratio[0], "" },
	{ "stats-eratio2", 1, (void *)&prcopt_.eratio[1], "" },
	{ "stats-errphase", 1, (void *)&prcopt_.err[1], "m" },
	{ "stats-errphaseel", 1, (void *)&prcopt_.err[2], "m" },
	{ "stats-errphasebl", 1, (void *)&prcopt_.err[3], "m/10km" },
	{ "stats-errdoppler", 1, (void *)&prcopt_.err[4], "Hz" },
	{ "stats-stdbias", 1, (void *)&prcopt_.std[0], "m" },
	{ "stats-stdiono", 1, (void *)&prcopt_.std[1], "m" },
	{ "stats-stdtrop", 1, (void *)&prcopt_.std[2], "m" },
	{ "stats-prnaccelh", 1, (void *)&prcopt_.processNoise[3], "m/s^2" },
	{ "stats-prnaccelv", 1, (void *)&prcopt_.processNoise[4], "m/s^2" },
	{ "stats-prnbias", 1, (void *)&prcopt_.processNoise[0], "m" },
	{ "stats-prniono", 1, (void *)&prcopt_.processNoise[1], "m" },
	{ "stats-prntrop", 1, (void *)&prcopt_.processNoise[2], "m" },
	{ "stats-prndcb", 1, (void*)&prcopt_.processNoise[5], "m" },
	{ "stats-prnisb1", 1, (void *)&prcopt_.prn_isb[0], "m" },
	{ "stats-clkstab", 1, (void *)&prcopt_.sclkstab, "s/s" },

	{ "ant-postype", 3, (void *)&antpostype_, POSOPT },
	{ "ant-pos1", 1, (void *)&antpos_[0], "deg|m" },
	{ "ant-pos2", 1, (void *)&antpos_[1], "deg|m" },
	{ "ant-pos3", 1, (void *)&antpos_[2], "m|m" },
	{ "ant-anttype", 2, (void *)prcopt_.anttype, "" },
	{ "ant-antdele", 1, (void *)&prcopt_.antdel[0], "m" },
	{ "ant-antdeln", 1, (void *)&prcopt_.antdel[1], "m" },
	{ "ant-antdelu", 1, (void *)&prcopt_.antdel[2], "m" },


	{ "file-satantfile", 2, (void *)&filopt_.satantp, "" },
	{ "file-rcvantfile", 2, (void *)&filopt_.rcvantp, "" },
	{ "file-ztdfile", 2, (void *)&filopt_.ztd, "" },
	{ "file-snxfile", 2, (void *)&filopt_.snx, "" },
	{ "file-staposfile", 2, (void *)&filopt_.stapos, "" },
	{ "file-geoidfile", 2, (void *)&filopt_.geoid, "" },
	{ "file-ionofile", 2, (void *)&filopt_.iono, "" },
	{ "file-dcbfile", 2, (void *)&filopt_.dcb, "" },
	{ "file-eopfile", 2, (void *)&filopt_.eop, "" },
	{ "file-blqfile", 2, (void *)&filopt_.blq, "" },
	{ "file-tempdir", 2, (void *)&filopt_.tempdir, "" },
	{ "file-geexefile", 2, (void *)&filopt_.geexe, "" },
	{ "file-solstatfile", 2, (void *)&filopt_.solstat, "" },
	{ "file-tracefile", 2, (void *)&filopt_.trace, "" },
	{ "file-road", 2, (void *)&filopt_.fileroad, "" },
	{ "outdirname", 2, (void *)&filopt_.outdir, "" },
	{ "ofile_bak", 2, (void *)&filopt_.ofile_bak, "" },
	{ "", 0, NULL, "" } /* terminator */
};

/* discard space characters at tail ------------------------------------------*/
void chop(char *str)
{
	char *p;
	if ((p = strchr(str, '#'))) *p = '\0'; /* comment */
	for (p = str + strlen(str) - 1; p >= str && !isgraph((int)*p); p--)
		*p = '\0';
}
/* enum to string ------------------------------------------------------------*/
int enum2str(char *s, const char *comment, int val)
{
	char str[32], *p, *q;
	int n;

	n = sprintf(str, "%d:", val);
	if (!(p = strstr(comment, str))) {
		return sprintf(s, "%d", val);
	}
	if (!(q = strchr(p + n, ',')) && !(q = strchr(p + n, ')'))) {
		strcpy(s, p + n);
		return (int)strlen(p + n);
	}
	strncpy(s, p + n, q - p - n); s[q - p - n] = '\0';
	return (int)(q - p - n);
}
/* string to enum ------------------------------------------------------------*/
int str2enum(const char *str, const char *comment, int *val)
{
	const char *p;
	char s[32];

	for (p = comment;; p++) {
		if (!(p = strstr(p, str))) break;
		if (*(p - 1) != ':') continue;
		for (p -= 2; '0' <= *p&&*p <= '9'; p--);
		return sscanf(p + 1, "%d", val) == 1;
	}
	sprintf(s, "%30.30s:", str);
	if ((p = strstr(comment, s))) { /* number */
		return sscanf(p, "%d", val) == 1;
	}
	return 0;
}
/* search option ---------------------------------------------------------------
* search option record
* args   : char   *name     I  option name
*          opt_t  *opts     I  options table
*                              (terminated with table[i].name="")
* return : option record (NULL: not found)
*-----------------------------------------------------------------------------*/
opt_t *searchopt(const char *name, const opt_t *opts)
{
	int i;


	for (i = 0; *opts[i].name; i++) {
		if (strstr(opts[i].name, name)) return (opt_t *)(opts + i);
	}
	return NULL;
}
/* string to option value ------------------------------------------------------
* convert string to option value
* args   : opt_t  *opt      O  option
*          char   *str      I  option value string
* return : status (1:ok,0:error)
*-----------------------------------------------------------------------------*/
int str2opt(opt_t *opt, const char *str)
{
	int s[6] = { 0 };
	int i;
	switch (opt->format) {
	case 0: *(int    *)opt->var = atoi(str); break;
	case 1: *(double *)opt->var = atof(str); break;
	case 2: strcpy((char *)opt->var, str);  break;
	case 3: return str2enum(str, opt->comment, (int *)opt->var);
	case 4:
		sscanf(str, "%d,%d,%d,%d,%d,%d", s, s + 1, s + 2, s + 3, s + 4, s + 5);
		for (i = 0; i < 6; i++)
		{
			*((double    *)opt->var + i) = s[i];
		}
		break;

	default: return 0;
	}
	return 1;
}
/* option value to string ------------------------------------------------------
* convert option value to string
* args   : opt_t  *opt      I  option
*          char   *str      O  option value string
* return : length of output string
*-----------------------------------------------------------------------------*/
int opt2str(const opt_t *opt, char *str)
{
	char *p = str;
	int s[6] = { 0 };
	int i;

	switch (opt->format) {
	case 0: p += sprintf(p, "%d", *(int   *)opt->var); break;
	case 1: p += sprintf(p, "%.15g", *(double*)opt->var); break;
	case 2: p += sprintf(p, "%s", (char  *)opt->var); break;
	case 3: p += enum2str(p, opt->comment, *(int *)opt->var); break;
	case 4:
		sscanf(str, "%d,%d,%d,%d,%d,%d", s, s + 1, s + 2, s + 3, s + 4, s + 5);
		for (i = 0; i < 6; i++)
		{
			*((double    *)opt->var + i) = s[i];
		}
		break;
	}
	return (int)(p - str);
}
/* option to string -------------------------------------------------------------
* convert option to string (keyword=value # comment)
* args   : opt_t  *opt      I  option
*          char   *buff     O  option string
* return : length of output string
*-----------------------------------------------------------------------------*/
int opt2buf(const opt_t *opt, char *buff)
{
	char *p = buff;
	int n;

	//	trace(3, "opt2buf : name=%s\n", opt->name);

	p += sprintf(p, "%-18s =", opt->name);
	p += opt2str(opt, p);
	if (*opt->comment) {
		if ((n = (int)(buff + 30 - p))>0) p += sprintf(p, "%*s", n, "");
		p += sprintf(p, " # (%s)", opt->comment);
	}
	return (int)(p - buff);
}
/* load options ----------------------------------------------------------------
* load options from file
* args   : char   *file     I  options file
*          opt_t  *opts     IO options table
*                              (terminated with table[i].name="")
* return : status (1:ok,0:error)
*-----------------------------------------------------------------------------*/
int loadopts(const char *file, opt_t *opts)
{
	FILE *fp;
	opt_t *opt;
	char buff[2048], *p;
	int n = 0;

	//	trace(3, "loadopts: file=%s\n", file);

	if (!(fp = fopen(file, "r"))) {
		//		trace(1, "loadopts: options file open error (%s)\n", file);
		return 0;
	}
	while (fgets(buff, sizeof(buff), fp)) {
		n++;
		chop(buff);

		if (buff[0] == '\0') continue;

		if (!(p = strstr(buff, "="))) {
			fprintf(stderr, "invalid option %s (%s:%d)\n", buff, file, n);
			continue;
		}
		*p++ = '\0';
		chop(buff);
		if (!(opt = searchopt(buff, opts))) continue;

		if (!str2opt(opt, p)) {
			fprintf(stderr, "invalid option value %s (%s:%d)\n", buff, file, n);
			continue;
		}
	}
	fclose(fp);

	return 1;
}
/* save options to file --------------------------------------------------------
* save options to file
* args   : char   *file     I  options file
*          char   *mode     I  write mode ("w":overwrite,"a":append);
*          char   *comment  I  header comment (NULL: no comment)
*          opt_t  *opts     I  options table
*                              (terminated with table[i].name="")
* return : status (1:ok,0:error)
*-----------------------------------------------------------------------------*/
int saveopts(const char *file, const char *mode, const char *comment,
	const opt_t *opts)
{
	FILE *fp;
	char buff[2048];
	int i;

	//	trace(3, "saveopts: file=%s mode=%s\n", file, mode);

	if (!(fp = fopen(file, mode))) {
		//		trace(1, "saveopts: options file open error (%s)\n", file);
		return 0;
	}
	if (comment) fprintf(fp, "# %s\n\n", comment);

	for (i = 0; *opts[i].name; i++) {
		opt2buf(opts + i, buff);
		fprintf(fp, "%s\n", buff);
	}
	fclose(fp);
	return 1;
}
/* system options buffer to options ------------------------------------------*/
void buff2sysopts(void)
{
	double pos[3], *rr;
	char buff[1024], *p, *id;
	int i, j, sat, *ps;

	prcopt_.elevation = elmask_;
	prcopt_.elevationAR = elmaskar_;
	prcopt_.elevationHoldAR = elmaskhold_;

	ps = &prcopt_.rovpos;
	rr = prcopt_.fixPosition;

	if (antpostype_ == 0) { /* lat/lon/hgt */
		*ps = 0;
		pos[0] = antpos_[0];
		pos[1] = antpos_[1];
		pos[2] = antpos_[2];
		CoorGeo2CoorCar(pos, rr);
	}
	else if (antpostype_ == 1) { /* xyz-ecef */
		*ps = 0;
		rr[0] = antpos_[0];
		rr[1] = antpos_[1];
		rr[2] = antpos_[2];
	}
	else *ps = antpostype_ - 1;
	/* excluded satellites */
	for (i = 0; i<MAXSAT; i++) prcopt_.exsats[i] = 0;
	if (exsats_[0] != '\0') {
		strcpy(buff, exsats_);
		for (p = strtok(buff, " "); p; p = strtok(NULL, " ")) {
			if (*p == '+') id = p + 1; else id = p;
			if (!(sat = satID2satIndex(id))) continue;
			prcopt_.exsats[sat - 1] = *p == '+' ? 2 : 1;
		}
	}
	/* snrmask */
	for (i = 0; i<NFREQ; i++) {
		for (j = 0; j<9; j++) prcopt_.snrMask.mask[i][j] = 0.0;
		strcpy(buff, snrmask_[i]);
		for (p = strtok(buff, ","), j = 0; p&&j<9; p = strtok(NULL, ",")) {
			prcopt_.snrMask.mask[i][j++] = atof(p);
		}
	}
}
/* options to system options buffer ------------------------------------------*/
void sysopts2buff(void)
{
	double pos[3], *rr;
	char id[32], *p;
	int i, j, sat, *ps;

	elmask_ = prcopt_.elevation;
	elmaskar_ = prcopt_.elevationAR;
	elmaskhold_ = prcopt_.elevationHoldAR;

	for (i = 0; i<2; i++) {
		ps = &prcopt_.rovpos;
		rr = prcopt_.fixPosition;

		if (*ps == 0) {
			antpostype_ = 0;
			CoorCar2CoorGeo(rr, pos);
			antpos_[0] = pos[0];
			antpos_[1] = pos[1];
			antpos_[2] = pos[2];
		}
		else antpostype_ = *ps + 1;
	}
	/* excluded satellites */
	exsats_[0] = '\0';
	for (sat = 1, p = exsats_; sat <= MAXSAT&&p - exsats_<(int)sizeof(exsats_) - 32; sat++) {
		if (prcopt_.exsats[sat - 1]) {
			satIndex2satID(id, sat);
			p += sprintf(p, "%s%s%s", p == exsats_ ? "" : " ",
				prcopt_.exsats[sat - 1] == 2 ? "+" : "", id);
		}
	}
	/* snrmask */
	for (i = 0; i<NFREQ; i++) {
		snrmask_[i][0] = '\0';
		p = snrmask_[i];
		for (j = 0; j<9; j++) {
			p += sprintf(p, "%s%.0f", j>0 ? "," : "", prcopt_.snrMask.mask[i][j]);
		}
	}
}
/* reset system options to default ---------------------------------------------
* reset system options to default
* args   : none
* return : none
*-----------------------------------------------------------------------------*/
void resetsysopts(void)
{
	int i;

	//	trace(3, "resetsysopts:\n");
	prcopt_t prcopt_t0 = { 0 };
	solopt_t solopt_t0 = { 0 };
	prcopt_ = prcopt_t0;
	solopt_ = solopt_t0;
	filopt_.satantp[0] = '\0';
	filopt_.rcvantp[0] = '\0';
	filopt_.stapos[0] = '\0';
	filopt_.geoid[0] = '\0';
	filopt_.dcb[0] = '\0';
	filopt_.blq[0] = '\0';
	filopt_.solstat[0] = '\0';
	filopt_.trace[0] = '\0';
	antpostype_ = 0;
	elmask_ = 15.0;
	elmaskar_ = 0.0;
	elmaskhold_ = 0.0;
	for (i = 0; i<3; i++) {
		antpos_[i] = 0.0;
	}
	exsats_[0] = '\0';
}
/* get system options ----------------------------------------------------------
* get system options
* args   : prcopt_t *popt   IO processing options (NULL: no output)
*          solopt_t *sopt   IO solution options   (NULL: no output)
*          folopt_t *fopt   IO file options       (NULL: no output)
* return : none
* notes  : to load system options, use loadopts() before calling the function
*-----------------------------------------------------------------------------*/
void getsysopts(prcopt_t *popt, solopt_t *sopt, filopt_t *fopt)
{

	buff2sysopts();
	if (popt) *popt = prcopt_;
	if (sopt) *sopt = solopt_;
	if (fopt) *fopt = filopt_;
}
/* set system options ----------------------------------------------------------
* set system options
* args   : prcopt_t *prcopt_t I  processing options (NULL: default)
*          solopt_t *solopt_t I  solution options   (NULL: default)
*          filopt_t *filopt I  file options       (NULL: default)
* return : none
* notes  : to save system options, use saveopts() after calling the function
*-----------------------------------------------------------------------------*/
void setsysopts(const prcopt_t *prcopt, const solopt_t *solopt,
	const filopt_t *filopt)
{

	resetsysopts();
	if (prcopt) prcopt_ = *prcopt;
	if (solopt) solopt_ = *solopt;
	if (filopt) filopt_ = *filopt;
	sysopts2buff();
}


/* creat and read options ----------------------------------------------------------
* creat and read options
* args   : const char *dir  I  directory name (NULL: default)
prcopt_t *prcopt_t I  processing options (NULL: default)
*          solopt_t *solopt_t I  solution options   (NULL: default)
*          filopt_t *filopt I  file options       (NULL: default)
* return : none
* notes  : to save system options, use saveopts() after calling the function
*-----------------------------------------------------------------------------*/
void readconf(const char *dir, prcopt_t *prcopt, solopt_t *solopt,
	filopt_t *filopt)
{
	resetsysopts();
	loadopts(dir, sysopts);
	buff2sysopts();
	if (prcopt) *prcopt = prcopt_;
	if (solopt) *solopt = solopt_;
	if (filopt) *filopt = filopt_;

}

void creatconf(const char *dir)
{
	gtime_t t;
	t = timeget();
	struct tm *starttime;
	starttime = localtime(&(t.time));
	starttime->tm_year = starttime->tm_year + 1900;
	starttime->tm_mon = starttime->tm_mon + 1;
	FILE *fp = stdout;
	fp = fopen(dir, "w");
	fprintf(fp, "#ppp options created at %d/%d/%d %d:%d:%d by pwj\n\n", starttime->tm_year, starttime->tm_mon, starttime->tm_mday, starttime->tm_hour, starttime->tm_min, starttime->tm_sec);
	fprintf(fp, "%-24s%-10s%s", "file-road", "=F:\\253\\", "\n");
	fprintf(fp, "%-24s%-10s%s", "pos1-posrtmode", "=0", "# (0:final process,1:real time process)\n");
	fprintf(fp, "%-24s%-10s%s", "pos1-posmode", "=4", "# (1:single,2:ppp-fixed,3:ppp-kinematic,4:ppp-static)\n");
	fprintf(fp, "%-24s%-10s%s", "pos1-freqnum", "=2", "# (number of frequences)\n");
	fprintf(fp, "%-24s%-10s%s", "pos1-freqopt", "=3", "# (0:L1/B1(C2),1:L2/B2(C7),2:L5/B3(C6),3:L1L2/B1B2,4:L1L5/B1B3,5:L2L5/B2B3,6:L1L2L5/B1B2B3)\n");
	fprintf(fp, "%-24s%-10s%s", "pos1-soltype", "=0", "# (0:forward,1:backward,2:combined)\n");
	fprintf(fp, "%-24s%-10s%s", "pos1-navsys", "=1", "# (1:GPS,2:GLONASS,4:GALILEO,8:COMPASS,16:SBAS)\n");
	fprintf(fp, "%-24s%-10s%s", "pos1-elmask", "=10", "# (deg)\n");
	fprintf(fp, "%-24s%-10s%s", "pos1-sateph", "=1", "# (0:brdc,1:precise,2:brdc+ssrapc,3:brdc+ssrcom)\n");
	fprintf(fp, "%-24s%-10s%s", "pos1-ionoopt", "=3", "# (0:off,1:brdc,2:dual-freq,3:est-stec,4:est-stec+constraint,5:ionex-tec,6:tec_ssr)\n");
	fprintf(fp, "%-24s%-10s%s", "pos1-tropopt", "=3", "# (0:off,1:saas,2:sbas,3:est-ztd,4:est-ztdgrad,5:est-ztd+constraint)\n");
	fprintf(fp, "%-24s%-10s%s", "pos1-dynamics", "=0", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "pos1-tidecorr", "=1", "# (0:off,1:solid,2:solid+otl+pole)\n");
	fprintf(fp, "%-24s%-10s%s", "pos1-posopt1", "=1", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "pos1-posopt2", "=1", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "pos1-posopt3", "=1", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "pos1-posopt4", "=1", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "pos1-posopt5", "=1", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "pos1-posopt6", "=0", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "pos1-posopt7", "=0", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "pos1-exclsats", "=", "# (prn ...)\n\n");


	fprintf(fp, "%-24s%-10s%s", "pos2-rovpos", "=1", "# (0:blh,1:xyz)\n");
	fprintf(fp, "%-24s%-10s%s", "pos2-armode", "=1", "# (0:off,1:fix-and-hold)\n");
	fprintf(fp, "%-24s%-10s%s", "pos2-gloarmode", "=0", "# (0:off,1:on,2:autocal)\n");
	fprintf(fp, "%-24s%-10s%s", "pos2-arthres", "=3", "# (AR validation threshold)\n");
	fprintf(fp, "%-24s%-10s%s", "pos2-elmaskhold", "=0", "# (deg\n");
	fprintf(fp, "%-24s%-10s%s", "pos2-aroutcnt", "=5", "# (maxout)\n");
	fprintf(fp, "%-24s%-10s%s", "pos2-maxage", "=30.0", "# (max difference of time (s))\n");
	fprintf(fp, "%-24s%-10s%s", "pos2-maxinno", "=50.0", "# (reject threshold of innovation(m))\n");
	fprintf(fp, "%-24s%-10s%s", "pos2-maxgdop", "=30.0", "# (reject threshold of gdop)\n");
	fprintf(fp, "%-24s%-10s%s", "pos2-slipthres", "=0.05", "# (m)\n");
	fprintf(fp, "%-24s%-10s%s", "pos2-maxthreswl", "=2.0", "# (m)\n");
	fprintf(fp, "%-24s%-10s%s", "pos2-rejionno", "=30", "# (m)\n");
	fprintf(fp, "%-24s%-10s%s", "pos2-rejgdop", "=30", "\n");
	fprintf(fp, "%-24s%-10s%s", "pos2-niter", "=1", "\n");
	fprintf(fp, "%-24s%-10s%s", "pos2-robust", "=0", "# (0:kalman,1:Rubust-kalman,2:A-kalman)\n");
	fprintf(fp, "%-24s%-10s%s", "pos2-adapt", "=0", "# (0:no,1:two,2:three)\n");
	fprintf(fp, "%-24s%-10s%s", "pos2-doppler", "=0", "# (0:no,1:state,2:observation)\n");
	fprintf(fp, "%-24s%-10s%s", "pos2-interval ", "=0", "# (process interval)\n");
	fprintf(fp, "%-24s%-10s%s", "pos2-dcbcorr", "=1", "# (0:no,1:TGD,2:DCB)\n");

	fprintf(fp, "%-24s%-10s%s", "pos2-cs_LL", "=1", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "pos2-cs_GF", "=1", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "pos2-cs_MW", "=1", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "pos2-cs_TECR", "=1", "# (0:off,1:on)\n\n");


	fprintf(fp, "%-24s%-10s%s", "out1-solformat", "=1", "# (0:llh,1:xyz,2:enu,3:nmea)\n");
	fprintf(fp, "%-24s%-10s%s", "out1-outhead", "=0", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "out1-outopt", "=1", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "out1-timesys", "=0", "# (0:gpst,1:utc,2:jst)\n");
	fprintf(fp, "%-24s%-10s%s", "out1-timeform", "=1", "# (0:tow,1:hms)\n");
	fprintf(fp, "%-24s%-10s%s", "out1-timendec", "=3", "\n");
	fprintf(fp, "%-24s%-10s%s", "out1-degform", "=1", "# (0:deg,1:dms)\n");
	fprintf(fp, "%-24s%-10s%s", "out1-fieldsep", "=", "\n");
	fprintf(fp, "%-24s%-10s%s", "out1-height", "=0", "# (0:ellipsoidal,1:geodetic)\n");
	fprintf(fp, "%-24s%-10s%s", "out1-geoid", "=0", "# (0:internal,1:egm96,2:egm08_2.5,3:egm08_1,4:gsi2000)\n");
	fprintf(fp, "%-24s%-10s%s", "out1-solstatic", "=0", "# (0:all,1:single)\n");
	fprintf(fp, "%-24s%-10s%s", "out1-nmeaintv1", "=0", "# (s)\n");
	fprintf(fp, "%-24s%-10s%s", "out1-nmeaintv2", "=0", "# (s)\n");
	fprintf(fp, "%-24s%-10s%s", "out1-outstat", "=2", "# (0:off,1:state,2:residual)\n\n");


	fprintf(fp, "%-24s%-10s%s", "out2-log", "=0", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "out2-elev", "=0", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "out2-trop", "=0", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "out2-iono", "=0", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "out2-fixion", "=0", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "out2-gim", "=0", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "out2-resc", "=0", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "out2-resp", "=0", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "out2-corr", "=0", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "out2-resi", "=0", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "out2-dcb", "=0", "# (0:off,1:on)\n");
	fprintf(fp, "%-24s%-10s%s", "out2-isb", "=0", "# (0:off,1:on)\n\n");




	fprintf(fp, "%-24s%-10s%s", "stats-eratio1", "=1000", "\n");
	fprintf(fp, "%-24s%-10s%s", "stats-eratio2", "=1000", "\n");
	fprintf(fp, "%-24s%-10s%s", "stats-errphase", "=0.003", "# (m)\n");
	fprintf(fp, "%-24s%-10s%s", "stats-errphaseel", "=0.003", "# (m)\n");
	fprintf(fp, "%-24s%-10s%s", "stats-errphasebl", "=0", "# (m/10km)\n");
	fprintf(fp, "%-24s%-10s%s", "stats-errdoppler", "=10", "# (Hz)\n");
	fprintf(fp, "%-24s%-10s%s", "stats-stdbias", "=30", "# (m)\n");
	fprintf(fp, "%-24s%-10s%s", "stats-stdiono", "=0.03 ", "# (m)\n");
	fprintf(fp, "%-24s%-10s%s", "stats-stdtrop", "=0.3", "# (m)\n");
	fprintf(fp, "%-24s%-10s%s", "stats-prnaccelh", "=10", "# (m/s^2)\n");
	fprintf(fp, "%-24s%-10s%s", "stats-prnaccelv", "=10", "# (m/s^2)\n");
	fprintf(fp, "%-24s%-10s%s", "stats-prnbias", "=1e-7", "# (m)\n");
	fprintf(fp, "%-24s%-10s%s", "stats-prniono", "=1e-2", "# (m)\n");
	fprintf(fp, "%-24s%-10s%s", "stats-prntrop", "=1e-4", "# (m)\n");
	fprintf(fp, "%-24s%-10s%s", "stats-clkstab", "=5e-12", "# (s/s)\n");
	fprintf(fp, "%-24s%-10s%s", "ant-anttype", "=*", "# (*:select from atx file)\n");
	fprintf(fp, "%-24s%-10s%s", "ant-pos1", "=", "# (b|x)\n");
	fprintf(fp, "%-24s%-10s%s", "ant-pos2", "=", "# (l|y)\n");
	fprintf(fp, "%-24s%-10s%s", "ant-pos3", "=", "# (h|z)\n");
	fprintf(fp, "%-24s%-10s%s", "ant-antdele", "=0", "# (m)\n");
	fprintf(fp, "%-24s%-10s%s", "ant-antdeln", "=0", "# (m)\n");
	fprintf(fp, "%-24s%-10s%s", "ant-antdelu", "=0", "# (m)\n\n");

	fprintf(fp, "%-24s%-10s%s", "file-satantfile", "=F:\\253\\igs08_1840.atx", "\n");
	fprintf(fp, "%-24s%-10s%s", "file-rcvantfile", "=F:\\253\\igs08_1840.atx", "\n");
	fprintf(fp, "%-24s%-10s%s", "file-ztdfile", "=", "\n");
	fprintf(fp, "%-24s%-10s%s", "file-snxfile", "=", "\n");
	fprintf(fp, "%-24s%-10s%s", "file-staposfile", "=", "\n");
	fprintf(fp, "%-24s%-10s%s", "file-geoidfile", "=", "\n");
	fprintf(fp, "%-24s%-10s%s", "file-ionofile", "=", "\n");
	fprintf(fp, "%-24s%-10s%s", "file-dcbfile", "=", "\n");
	fprintf(fp, "%-24s%-10s%s", "file-eopfile", "=", "\n");
	fprintf(fp, "%-24s%-10s%s", "file-blqfile", "=", "\n");
	fprintf(fp, "%-24s%-10s%s", "file-tempdir", "=", "\n");
	fprintf(fp, "%-24s%-10s%s", "file-geexefile", "=", "\n");
	fprintf(fp, "%-24s%-10s%s", "file-solstatfile", "=", "\n");
	fprintf(fp, "%-24s%-10s%s", "file-tracefile", "=", "\n");
	fprintf(fp, "\n\n");


	fclose(fp);
}


void getConfiguration(prcopt_t *prcopt, solopt_t *solopt, filopt_t *filopt)
{
	char conf[300] = "";
	strcpy(conf, "D:\\data\\ZhuhaiIONserver\\0804\\config.whu");
	readconf(conf, prcopt, solopt, filopt);
}
