#include "pwjppp.h"

static void InitialPCV(pcv_t *pcv)
{
	static const pcv_t pcv0 = { { 0 } };
	*pcv = pcv0;
}
/* add antenna parameter -----------------------------------------------------*/
static void addpcv(const pcv_t *pcv, pcvs_t *pcvs)
{
	pcv_t *pcvs_pcv;

	if (pcvs->nmax <= pcvs->n) {
		pcvs->nmax += 256;
		if (!(pcvs_pcv = (pcv_t *)realloc(pcvs->pcv, sizeof(pcv_t)*pcvs->nmax))) {
			printf("addpcv: memory allocation error\n");
			free(pcvs->pcv); pcvs->pcv = NULL; pcvs->n = pcvs->nmax = 0;
			return;
		}
		pcvs->pcv = pcvs_pcv;
	}
	pcvs->pcv[pcvs->n++] = *pcv;
}

static int DecodeAntField(char *buff, char *sep, int i, double *v)
{
	char *p = NULL;
	int n = 0;
	int k;
	for (k = 0; k < i; k++) v[k] = 0.0;
	p = strtok(buff, sep);
	while (p != NULL&&n<i) {
		v[n++] = atof(p) / 1000;
		p = strtok(NULL, sep);
	}
	return n;
}

static int readantex(const char *file, pcvs_t *pcvs)
{
	FILE *fp;
	pcv_t pcv;
	double neu[3] = { 0 };
	int i, f, freq = 0, state = 0, freqs[] = { 1, 2, 5, 6, 7, 8, 0 }/*, ind, j, nazi*/;
	char buff[256]/*, sysstr, *pos*/;


	if (!(fp = fopen(file, "r"))) {
		printf("antex pcv file open error: %s\n", file);
		return 0;
	}
	while (fgets(buff, sizeof(buff), fp)) {

		if (strlen(buff)<60 || strstr(buff + 60, "COMMENT")) continue;

		if (strstr(buff + 60, "START OF ANTENNA")) {
			InitialPCV(&pcv);
			state = 1;
		}
		if (strstr(buff + 60, "END OF ANTENNA")) {
			addpcv(&pcv, pcvs);
			state = 0;
		}
		if (!state) continue;

		if (strstr(buff + 60, "TYPE / SERIAL NO")) {
			strncpy(pcv.type, buff, 20); pcv.type[20] = '\0';
			strncpy(pcv.code, buff + 20, 20); pcv.code[20] = '\0';
			if (!strncmp(pcv.code + 3, "        ", 8)) {
				pcv.sat = satID2satIndex(pcv.code);
			}
		}
		else if (strstr(buff + 60, "VALID FROM")) {
			if (str2time(buff, 0, 43, &pcv.ts)) continue;
		}
		else if (strstr(buff + 60, "VALID UNTIL")) {
			if (str2time(buff, 0, 43, &pcv.te)) continue;
		}
		else if (strstr(buff + 60, "START OF FREQUENCY")) {
			if (sscanf(buff + 4, "%d", &f)<1) continue;
			for (i = 0; i<NFREQ; i++) if (freqs[i] == f) break;
			if (i<NFREQ) freq = i + 1;
			/*			if (pcv.sat) ind = 0;
			else {
			sscanf(buff + 3, "%c", &sysstr);//û�а���QZSS����J��������������������QZSS�����߸���
			pos = strchr(constallationid, sysstr);
			if (pos)
			ind = (int)(pos - constallationid);
			else
			continue;
			//ind=sys2index(satsys(satid2no(freqstr),NULL));
			}*/
		}
		else if (strstr(buff + 60, "END OF FREQUENCY")) {
			freq = 0;
		}
		else if (strstr(buff + 60, "NORTH / EAST / UP")) {
			if (freq<1 || NFREQ<freq) continue;
			if (DecodeAntField(buff, " ", 3, neu)<3) continue;
			pcv.off[freq - 1][0] = neu[pcv.sat ? 0 : 1]; /* x or e */
			pcv.off[freq - 1][1] = neu[pcv.sat ? 1 : 0]; /* y or n */
			pcv.off[freq - 1][2] = neu[2];           /* z or u */
		}
		else if (strstr(buff + 60, "DAZI")) {
			pcv.dazi = str2num(buff, 2, 6);
		}
		else if (strstr(buff, "NOAZI")) {
			if (freq<1 || NFREQ<freq) continue;
			if ((i = DecodeAntField(buff + 8, " ", 19, pcv.var[freq - 1])) <= 0) continue;
			for (; i<19; i++) pcv.var[freq - 1][i] = pcv.var[freq - 1][i - 1];

			/*			if (pcv.dazi != 0.0){
			nazi =(int) (360 / pcv.dazi) + 1;
			for (j = 0; j<nazi; j++){
			fgets(buff, sizeof(buff), fp);
			if ((i = DecodeAntField(buff + 8, " ",19, pcv.azivar[ind][freq - 1][j])) <= 0) continue;
			for (; i<19; i++) pcv.azivar[ind][freq - 1][j][i] = pcv.azivar[ind][freq - 1][j][i - 1];
			}
			}*/
		}
	}
	fclose(fp);

	return 1;
}


extern int ReadAntennaPCV(const char *file, pcvs_t *pcvs)
{
	char *ext;
	int stat;
	if (!(ext = strrchr(file, '.'))) ext = "";

	if (!strcmp(ext, ".atx") || !strcmp(ext, ".ATX")) {
		stat = readantex(file, pcvs);
	}
	return 1;
}
extern pcv_t* SearchPCV(gtime_t time, int sat, const char *type, pcvs_t *pcvs)
{
	int i, j, n = 0;
	pcv_t *pcv;
	char buff[MAXANT], *p, *types[2];
	if (sat) {
		for (i = 0; i < pcvs->n; i++)
		{
			pcv = pcvs->pcv + i;
			if (sat != pcvs->pcv[i].sat)continue;
			if (pcv->ts.time != 0 && timediff(pcv->ts, time)>0)continue;
			if (pcv->te.time != 0 && timediff(pcv->te, time) < 0)continue;
			return pcv;
		}
	}
	else
	{
		strcpy(buff, type);
		for (p = strtok(buff, " "); p&&n<2; p = strtok(NULL, " ")) types[n++] = p;
		if (n <= 0) return NULL;

		/* search receiver antenna with radome at first */
		for (i = 0; i<pcvs->n; i++) {
			pcv = pcvs->pcv + i;
			for (j = 0; j<n; j++) if (!strstr(pcv->type, types[j])) break;
			if (j >= n) return pcv;
		}
		/* search receiver antenna without radome */
		for (i = 0; i<pcvs->n; i++) {
			pcv = pcvs->pcv + i;
			if (strstr(pcv->type, types[0]) != pcv->type) continue;

			printf("pcv without radome is used type=%s\n", type);
			return pcv;
		}
	}
	return NULL;
}

extern void SetPCV(gtime_t time, sta_t *sta, prcopt_t * popt, pcvs_t *pcvs, nav_t *nav)
{
	int i;
	pcv_t *pcv;
	double enu[3];
	char id[4] = { 0 };
	if (nav) {
		for (i = 0; i < MAXSAT; i++)
		{
			if (!(satIndex2sys(i + 1)&popt->navigationSystem)) continue;
			if (!(pcv = SearchPCV(time, i + 1, "", pcvs))) {
				satIndex2satID(id, i + 1);
				printf("no satellite antenna pcv: %s\n", id);
				continue;
			}
			nav->pcvs[i] = *pcv;
		}
	}

	if (sta) {
		if (!strcmp(popt->anttype, "*")) {
			strcpy(popt->anttype, sta->antType);
			if (sta->antennaDeltaType) {
				if (norm(sta->approxCoor, 3) > 0.0) {
					CoorCar2CoorENU(sta->approxCoor, sta->antennaDelta, enu);
					for (i = 0; i < 3; i++)popt->antdel[i] = enu[i];
				}
			}
			else {
				for (i = 0; i < 3; i++)popt->antdel[i] = sta->antennaDelta[i];
			}
		}
		if (!(pcv = SearchPCV(time, 0, popt->anttype, pcvs))) {
			*popt->anttype = '\0';
			return;
		}
		strcpy(popt->anttype, pcv->type);
		popt->pcvr = *pcv;
	}
}
