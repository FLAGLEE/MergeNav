/*------------------------------------------------------------------------------
* rtksvr.c : rtk server functions
*
*          Copyright (C) 2007-2017 by T.TAKASU, All rights reserved.
*
* options : -DWIN32    use WIN32 API
*
* version : $Revision:$ $Date:$
* history : 2009/01/07  1.0  new
*           2009/06/02  1.1  support glonass
*           2010/07/25  1.2  support correction input/log stream
*                            supoort online change of output/log streams
*                            supoort monitor stream
*                            added api:
*                                rtksvropenstr(),rtksvrclosestr()
*                            changed api:
*                                rtksvrstart()
*           2010/08/25  1.3  fix problem of ephemeris time inversion (2.4.0_p6)
*           2010/09/08  1.4  fix problem of ephemeris and ssr squence upset
*                            (2.4.0_p8)
*           2011/01/10  1.5  change api: rtksvrstart(),rtksvrostat()
*           2011/06/21  1.6  fix ephemeris handover problem
*           2012/05/14  1.7  fix bugs
*           2013/03/28  1.8  fix problem on lack of glonass freq number in raw
*                            fix problem on ephemeris with inverted toe
*                            add api rtksvrfree()
*           2014/06/28  1.9  fix probram on ephemeris update of beidou
*           2015/04/29  1.10 fix probram on ssr orbit/clock inconsistency
*           2015/07/31  1.11 add phase bias (fcb) correction
*           2015/12/05  1.12 support opt->pppopt=-DIS_FCB
*           2016/07/01  1.13 support averaging single pos as base position
*           2016/07/31  1.14 fix bug on ion/utc parameters input
*           2016/08/20  1.15 support api change of sendnmea()
*           2016/09/18  1.16 fix server-crash with server-cycle > 1000
*           2016/09/20  1.17 change api rtksvrstart()
*           2016/10/01  1.18 change api rtksvrstart()
*           2016/10/04  1.19 fix problem to send nmea of single solution
*           2016/10/09  1.20 add reset-and-single-sol mode for nmea-request
*           2017/04/11  1.21 add rtkfree() in rtksvrfree()
*-----------------------------------------------------------------------------*/
#include "pwjppp.h"

#define MIN_INT_RESET   30000   /* mininum interval of reset command (ms) */

/* write solution header to output stream ------------------------------------*/
static void writesolhead(stream_t *stream, const solopt_t *solopt_t)
{
	/*    unsigned char buff[1024];
	int n;

	n=outsolheads(buff,solopt_t);
	strwrite(stream,buff,n);*/
}
/* save output buffer --------------------------------------------------------*/
static void saveoutbuf(pppsvr_t *svr, unsigned char *buff, int n, int index)
{
	rtksvrlock(svr);

	n = n<svr->buffsize - svr->nsb[index] ? n : svr->buffsize - svr->nsb[index];
	memcpy(svr->sbuf[index] + svr->nsb[index], buff, n);
	svr->nsb[index] += n;

	rtksvrunlock(svr);
}
/* write solution to output stream -------------------------------------------*/
static void writesol(pppsvr_t *svr, int index)
{
}
/* update navigation data ----------------------------------------------------*/
static void updatenav(nav_t *nav)
{
	int i, j;
	for (i = 0; i<MAXSAT; i++) for (j = 0; j<NFREQ; j++) {
		nav->lam[i][j] = GetSatWaveLength(i + 1, j, nav);
	}
}
/* update glonass frequency channel number in raw data struct ----------------*/
static void updatefcn(pppsvr_t *svr)
{
	int i, j, sat, frq;

	for (i = 0; i<MAXPRNGLO; i++) {
		sat = satno2satIndex(SYS_GLO, i + 1);

		for (j = 0, frq = -999; j<1; j++) {
			if (svr->raw[j].nav.geph[i].sat != sat) continue;
			frq = svr->raw[j].nav.geph[i].frq;
		}
		if (frq<-7 || frq>6) continue;

		for (j = 0; j<1; j++) {
			if (svr->raw[j].nav.geph[i].sat == sat) continue;
			svr->raw[j].nav.geph[i].sat = sat;
			svr->raw[j].nav.geph[i].frq = frq;
		}
	}
}
/* update PPP server struct --------------------------------------------------*/
static void updatesvr(pppsvr_t *svr, int ret, obs_t *obs, nav_t *nav, int sat,
	sbsmsg_t *sbsmsg, int index, int iobs)
{
	eph_t *eph1, *eph2, *eph3,*eph4,*eph5;
	geph_t *geph1, *geph2, *geph3, *geph4, *geph5;

	double pos[3], del[3] = { 0 }, dr[3] = {0};
	pcv_t* pcv;
	int i, n = 0, prn, sbssat = svr->sol.opt.sbassatsel, sys, iode;

	tracet(4, "updatesvr: ret=%d sat=%2d index=%d\n", ret, sat, index);

	if (ret == 1) { /* observation data */
		if (iobs<MAXOBSBUF) {
			for (i = 0; i<obs->n; i++) {
				if (svr->sol.opt.exsats[obs->data[i].sat - 1] == 1 ||
					!(satIndex2sys(obs->data[i].sat)&svr->sol.opt.navigationSystem)) continue;
				svr->obs[iobs].data[n++] = obs->data[i];
			}
			svr->obs[iobs].n = n;
			sortobs(&svr->obs[iobs]);
		}
		svr->nmsg[index][0]++;
	}
	else if (ret == 2) { /* ephemeris */
		sys = satIndex2sys(sat);
		if ((sys&svr->sol.opt.navigationSystem)) {
			prn = satIndex2satno(sat);
			if (sys != SYS_GLO) {
//				if (!svr->navsel || svr->navsel == index + 3) {
					eph1 = nav->eph + sat - 1;
					eph2 = svr->nav.eph + sat - 1;
					eph3 = svr->nav.eph + sat - 1 + (MAXSAT);
					eph4 = navppp.eph + sat - 1;
					eph5 = navppp.eph + sat - 1 + (MAXSAT);
					if (eph2->ttr.time == 0 ||
						(eph1->iode != eph3->iode&&eph1->iode != eph2->iode) ||
						(timediff(eph1->toe, eph3->toe) != 0.0&&
							timediff(eph1->toe, eph2->toe) != 0.0)) {
						*eph3 = *eph2;
						*eph2 = *eph1;
						updatenav(&svr->nav);
					}

					if (eph4->ttr.time == 0 ||
						(eph1->iode != eph5->iode && eph1->iode != eph4->iode) ||
						(timediff(eph1->toe, eph5->toe) != 0.0 &&
							timediff(eph1->toe, eph4->toe) != 0.0)) {
						*eph5 = *eph4;
						*eph4 = *eph1;
						updatenav(&navppp);
						OutputBrdc(NULL, &navppp, sat, 1, brdmdir);
					}
//				}
				svr->nmsg[index][1]++;
			}
			else {
//				if (!svr->navsel || svr->navsel == index + 3) {
					geph1 = nav->geph + prn - 1;
					geph2 = svr->nav.geph + prn - 1;
					geph3 = svr->nav.geph + prn - 1 + MAXPRNGLO;
					geph4 = navppp.geph + prn - 1;
					geph5 = navppp.geph + prn - 1 + MAXPRNGLO;
					if (geph2->tof.time == 0 ||
						(geph1->iode != geph3->iode&&geph1->iode != geph2->iode)) {
						*geph3 = *geph2;
						*geph2 = *geph1;
						updatenav(&svr->nav);
						updatefcn(svr);
					}
					if (geph4->tof.time == 0 ||
						(geph1->iode != geph5->iode && geph1->iode != geph4->iode)) {
						*geph5 = *geph4;
						*geph4 = *geph1;
						updatenav(&navppp);
						OutputBrdc(NULL, &navppp, prn, 2, brdmdir);
					}
//				}
				svr->nmsg[index][6]++;
			}
		}
		if (brdcFlag == 0)
		{
			printf("decode brdc nav successfully\n");
			brdcFlag = 1;
		}
	}
	else if (ret == 3) { /* sbas message */
		if (sbsmsg && (sbssat == sbsmsg->prn || sbssat == 0)) {
			if (svr->nsbs<MAXSBSMSG) {
				svr->sbsmsg[svr->nsbs++] = *sbsmsg;
			}
			else {
				for (i = 0; i<MAXSBSMSG - 1; i++) svr->sbsmsg[i] = svr->sbsmsg[i + 1];
				svr->sbsmsg[i] = *sbsmsg;
			}
		}
		svr->nmsg[index][3]++;
	}
	else if (ret == 9) { /* ion/utc parameters */
		if (svr->navsel == 0 || svr->navsel == index + 3) {
			for (i = 0; i<8; i++) navppp.ion_gps[i]=svr->nav.ion_gps[i] = nav->ion_gps[i];
			for (i = 0; i<4; i++) navppp.utc_gps[i] = svr->nav.utc_gps[i] = nav->utc_gps[i];
			for (i = 0; i<4; i++) navppp.ion_gal[i] = svr->nav.ion_gal[i] = nav->ion_gal[i];
			for (i = 0; i<4; i++) navppp.utc_gal[i] = svr->nav.utc_gal[i] = nav->utc_gal[i];
			for (i = 0; i<8; i++) navppp.ion_qzs[i] = svr->nav.ion_qzs[i] = nav->ion_qzs[i];
			for (i = 0; i<4; i++) navppp.utc_qzs[i] = svr->nav.utc_qzs[i] = nav->utc_qzs[i];
			navppp.leaps = svr->nav.leaps = nav->leaps;
		}

		if (svr->rtcm[index].ion.flag) {
			svr->nav.ssrion = svr->rtcm[index].ion;
			navppp.ssrion = svr->rtcm[index].ion;
			//update pppsvr->sol in obs
			for (i = 0; i < extObsNum; i++)
			{
				pppsvr[corNum + i].sol.coefiniflag = 1;
			}
		}
		svr->nmsg[index][2]++;

		printf("get new ion from cnes at time %s \n",time_str(svr->rtcm[index].ion.time,2));
	}
	//else if (ret == 5) { /* antenna postion parameters */
	//	if (index == 0) {
	//		for (i = 0; i < 3; i++) {
	//			svr->sol.rb[i] = svr->rtcm[0].sta.approxCoor[i];
	//		}
	//		/* antenna delta */
	//		CoorCar2CoorGeo(svr->sol.rb, pos);
	//		if (svr->rtcm[0].sta.antennaDeltaType) { /* xyz */
	//			del[2] = svr->rtcm[0].sta.hgt;
	//			CoorENU2CoorCar(pos, del, dr);
	//			for (i = 0; i < 3; i++) {
	//				svr->sol.rb[i] += svr->rtcm[0].sta.antennaDelta[i] + dr[i];
	//				svr->sol.opt.antdel[i] = del[i];
	//			}
	//		}
	//		else { /* enu */
	//			CoorENU2CoorCar(pos, svr->rtcm[0].sta.antennaDelta, dr);
	//			for (i = 0; i < 3; i++) {
	//				svr->sol.rb[i] += dr[i];
	//				svr->sol.opt.antdel[i] = svr->rtcm[0].sta.antennaDelta[i];
	//			}
	//		}
	//		if (strcmp(svr->rtcm[0].sta.antType, "") != 0)
	//		{
	//			strcpy(svr->sol.opt.anttype, svr->rtcm[0].sta.antType);
	//			if (!(pcv = SearchPCV(timeget(), 0, svr->sol.opt.anttype, &Global_PCVs))) {
	//				*svr->sol.opt.anttype = '\0';
	//				return;
	//			}
	//			strcpy(svr->sol.opt.anttype, pcv->type);
	//			svr->sol.opt.pcvr = *pcv;
	//		}
	//	}
	//	svr->nmsg[index][4]++;
	//}

	else if (ret == 7) { /* dgps correction */
		svr->nmsg[index][5]++;
	}
	else if (ret == 10) { /* ssr message */
		for (i = 0; i<MAXSAT; i++) {
			if (!svr->rtcm[index].ssr[i].update) continue;

			/* check consistency between iods of orbit and clock */
			if (svr->rtcm[index].ssr[i].iod[0] !=
				svr->rtcm[index].ssr[i].iod[1]) continue;

			svr->rtcm[index].ssr[i].update = 0;

			iode = svr->rtcm[index].ssr[i].iode;

			sys = satIndex2sys(i + 1);
			prn = satIndex2satno(i + 1);

			/* check corresponding ephemeris exists */
			//if (sys == SYS_GPS || sys == SYS_GAL || sys == SYS_QZS) {
			//	if (svr->nav.eph[i].iode != iode&&
			//		svr->nav.eph[i + MAXSAT].iode != iode) {
			//		continue;
			//	}
			//}
			//else if (sys == SYS_GLO) {
			//	if (svr->nav.geph[prn - 1].iode != iode&&
			//		svr->nav.geph[prn - 1 + MAXPRNGLO].iode != iode) {
			//		continue;
			//	}
			//}
			//svr->nav.ssr[i] = svr->rtcm[index].ssr[i];

			if (sys == SYS_GPS || sys == SYS_GAL || sys == SYS_QZS) {
				if (navppp.eph[i].iode != iode &&
					navppp.eph[i + MAXSAT].iode != iode) {
					continue;
				}
			}
			else if (sys == SYS_GLO) {
				if (navppp.geph[prn - 1].iode != iode &&
					navppp.geph[prn - 1 + MAXPRNGLO].iode != iode) {
					continue;
				}
			}			
			navppp.ssr[i] = svr->rtcm[index].ssr[i];
		}
		svr->nmsg[index][7]++;

		if (ssrFlag == 0)
		{
			printf("decode ssr stream successfully\n");
			ssrFlag = 1;
		}
	}
	else if (ret == -1) { /* error */
		svr->nmsg[index][9]++;
	}
}
/* decode cor raw/rtcm data ---------------------------------------------*/
static int decoderaw_cor(pppsvr_t *svr, int index)
{
	obs_t *obs;
	nav_t *nav;
	sbsmsg_t *sbsmsg = NULL;
	int i, ret, sat, fobs = 0;

	tracet(4, "decoderaw: index=%d\n", index);

	rtksvrlock(svr);

	for (i = 0; i<svr->nb[index]; i++) {

		/* input rtcm/receiver raw data from stream */
		if (svr->format[index] == STRFMT_RTCM3) {
			ret = rtcmDecode(svr->rtcm + index, svr->buff[index][i]);
			obs = &svr->rtcm[index].obs;
			nav = &svr->rtcm[index].nav;
			sat = svr->rtcm[index].ephsat;
		}
		else {
			ret = input_raw(svr->raw + index, svr->format[index], svr->buff[index][i]);
			obs = &svr->raw[index].obs;
			nav = &svr->raw[index].nav;
			sat = svr->raw[index].ephsat;
			sbsmsg = &svr->raw[index].sbsmsg;
		}
#if 0 /* record for receiving tick */
		if (ret == 1) {
			trace(0, "%d %10d T=%s NS=%2d\n", index, tickget(),
				time_str(obs->data[0].time, 0), obs->n);
		}
#endif
		/* update rtk server */
		if (ret>0)updatesvr(svr, ret, obs, nav, sat, sbsmsg, index, fobs);

		/* observation data received */
		if (ret == 1) {
			if (fobs<MAXOBSBUF) fobs++; else svr->prcout++;
		}
	}

	svr->nb[index] = 0;

	rtksvrunlock(svr);

	return fobs;
}


/* decode receiver raw/rtcm data ---------------------------------------------*/
static int decoderaw_obs(pppsvr_t* svr, int index)
{
	obs_t* obs;
	nav_t* nav;
	sbsmsg_t* sbsmsg = NULL;
	int i, ret, sat, fobs = 0;

	tracet(4, "decoderaw: index=%d\n", index);

	rtksvrlock(svr);

	for (i = 0; i < svr->nb[index]; i++) {

		/* input rtcm/receiver raw data from stream */
		if (svr->format[index] == STRFMT_RTCM3) {
			ret = rtcmDecode(svr->rtcm + index, svr->buff[index][i]);
			obs = &svr->rtcm[index].obs;
			nav = &svr->rtcm[index].nav;
			sat = svr->rtcm[index].ephsat;
		}
		else {
			ret = input_raw(svr->raw + index, svr->format[index], svr->buff[index][i]);
			obs = &svr->raw[index].obs;
			nav = &svr->raw[index].nav;
			sat = svr->raw[index].ephsat;
			sbsmsg = &svr->raw[index].sbsmsg;
		}
#if 0 /* record for receiving tick */
		if (ret == 1) {
			trace(0, "%d %10d T=%s NS=%2d\n", index, tickget(),
				time_str(obs->data[0].time, 0), obs->n);
		}
#endif
		/* update rtk server */
		if (ret > 0)updatesvr(svr, ret, obs, nav, sat, sbsmsg, index, fobs);

		/* observation data received */
		if (ret == 1) {
			if (fobs < MAXOBSBUF) fobs++; else svr->prcout++;
		}
	}

	svr->nb[index] = 0;

	rtksvrunlock(svr);

	return fobs;
}
/* carrier-phase bias (fcb) correction ---------------------------------------*/
static void corr_phase_bias(obsd_t *obs, int n, const nav_t *nav)
{
	double lam;
	int i, j, code;

	for (i = 0; i<n; i++) for (j = 0; j<NFREQ; j++) {

		if (!(code = obs[i].code[j])) continue;
		if ((lam = nav->lam[obs[i].sat - 1][j]) == 0.0) continue;

		/* correct phase bias (cyc) */
		obs[i].L[j] -= nav->ssr[obs[i].sat - 1].pbias[code - 1] / lam;
	}
}
/* periodic command ----------------------------------------------------------*/
static void periodic_cmd(int cycle, const char *cmd, stream_t *stream)
{
	const char *p = cmd, *q;
	char msg[1024], *r;
	int n, period;

	for (p = cmd;; p = q + 1) {
		for (q = p;; q++) if (*q == '\r' || *q == '\n' || *q == '\0') break;
		n = (int)(q - p); strncpy(msg, p, n); msg[n] = '\0';

		period = 0;
		if ((r = strrchr(msg, '#'))) {
			sscanf(r, "# %d", &period);
			*r = '\0';
			while (*--r == ' ') *r = '\0'; /* delete tail spaces */
		}
		if (period <= 0) period = 1000;
		if (*msg&&cycle%period == 0) {
			strsendcmd(stream, msg);
		}
		if (!*q) break;
	}
}
/* baseline length -----------------------------------------------------------*/
static double baseline_len(const sol_t *sol)
{
	double dr[3];
	int i;

	if (norm(sol->rr, 3) <= 0.0 || norm(sol->rb, 3) <= 0.0) return 0.0;

	for (i = 0; i<3; i++) {
		dr[i] = sol->rr[i] - sol->rb[i];
	}
	return norm(dr, 3)*0.001; /* (km) */
}


/* rtk server thread ---------------------------------------------------------*/
#ifdef WIN32
static DWORD WINAPI pppcorsvrthread(void* arg)
#else
static void* pppcorsvrthread(void* arg)
#endif
{
	pppsvr_t* svr = (pppsvr_t*)arg;
	obs_t obs;
	obsd_t data[MAXOBS];
	double tt;
	unsigned int tick, ticknmea, tick1hz, tickreset;
	unsigned char* p, * q;
	gtime_t ts, te;
	FILE* fp=NULL;

	char ssrPath[200] = { 0 };

	double tow;
	int week;
	char fileName[30] = { '\0' };

	gtime_t pretime = {0}, time;
	int ret=0;

	int i, j, n, cycle, cputime, stat;
	tracet(3, "pppcorsvrthread:\n");
	svr->state = 1;
	obs.data = data;
	svr->tick = tickget();
	ticknmea = tick1hz = svr->tick - 1000;
	tickreset = svr->tick - MIN_INT_RESET;


	//printf("pppcorsvrthread:\n");
	for (cycle = 0; svr->state; cycle++) 
	{
		//printf("cycle is %d\n", cycle);
		tick = tickget();
		for (i = 0; i < MAXSTRPPP; i++) {
			//printf("i is %d\ti < %d\n", i, MAXSTRPPP);
			p = svr->buff[i] + svr->nb[i]; q = svr->buff[i] + svr->buffsize;
			/* read receiver raw/rtcm data from input stream */
		
			/*if ((n = strread(svr->stream + i, p, q - p)) <= 0) {
				printf("n=%d\n", n);
				continue;
			}*/

			if (typeflag == 2) {//stream
				if ((n = strread(svr->stream + i, p, q - p)) <= 0) {
					continue;

				}
			}
			else {//file
				n = strread(svr->stream + i, p, q - p);
				if (n == -1) {
					//strlock(svr->stream + i);//
					printf("end of file %s\n", svr->stream->path);
					//strunlock(svr->stream + i);//


					strclose(svr->stream + i);

					svr->nb[i] = svr->npb[i] = 0;
					free(svr->buff[i]); svr->buff[i] = NULL;
					free(svr->pbuf[i]); svr->pbuf[i] = NULL;
					free_raw(svr->raw + i);
					free_rtcm(svr->rtcm + i);


					svr->nsb[i] = 0;
					free(svr->sbuf[i]); svr->sbuf[i] = NULL;

					return 0;

				}
				else if (n == 0) {
					continue;
				}
			}
			
//			if (i == 0) {
//				fwrite(svr->buff[i] + svr->nb[i], n, 1, fp);
//			}
			/* write receiver raw/rtcm data to log stream */
//			strwrite(svr->stream + i + 4, p, n);
			svr->nb[i] += n;
			/* save peek buffer */
			rtksvrlock(svr);
			n = n < svr->buffsize - svr->npb[i] ? n : svr->buffsize - svr->npb[i];
			memcpy(svr->pbuf[i] + svr->npb[i], p, n);
			svr->npb[i] += n;
			rtksvrunlock(svr);
		}
		lock(&global_lock);
		for (i = 0; i < MAXSTRPPP; i++)
		{
			/* decode receiver raw/rtcm data */
			decoderaw_cor(svr, i);
		}
		unlock(&global_lock);

		if ((cputime = (int)(tickget() - tick)) > 0) svr->cputime = cputime;

		/* sleep until next cycle */
		sleepms(svr->cycle - cputime);
	}

	/////////////////////////////////
	//filefinish:
	////////////////////////////////
	for (i = 0; i < MAXSTRPPP; i++) strclose(svr->stream + i);
	for (i = 0; i < MAXSTRPPP; i++) {
		svr->nb[i] = svr->npb[i] = 0;
		free(svr->buff[i]); svr->buff[i] = NULL;
		free(svr->pbuf[i]); svr->pbuf[i] = NULL;
		free_raw(svr->raw + i);
		free_rtcm(svr->rtcm + i);
	}
	for (i = 0; i < 2; i++) {
		svr->nsb[i] = 0;
		free(svr->sbuf[i]); svr->sbuf[i] = NULL;
	}
	return 0;
}

/* initialize rtk server -------------------------------------------------------
* initialize rtk server
* args   : pppsvr_t *svr    IO rtk server
* return : status (0:error,1:ok)
*-----------------------------------------------------------------------------*/
extern int rtksvrinit(pppsvr_t* svr)
{
	gtime_t time0 = { 0 };
	eph_t  eph0 = { 0,-1,-1 };
	geph_t geph0 = { 0,-1 };
	seph_t seph0 = { 0 };
	int i, j;

	tracet(3, "rtksvrinit:\n");

	svr->state = svr->cycle = svr->nmeacycle = svr->nmeareq = 0;
	for (i = 0; i < 3; i++) svr->nmeapos[i] = 0.0;
	svr->buffsize = 0;
	for (i = 0; i < MAXSTRPPP; i++) svr->format[i] = 0;
	for (i = 0; i < 2; i++) svr->solopt[i] = solopt_default;
	svr->navsel = svr->nsbs = svr->nsol = 0;
	InitialSolution(&svr->sol, &prcopt_default);
	for (i = 0; i < MAXSTRPPP; i++) svr->nb[i] = 0;
	for (i = 0; i < 2; i++) svr->nsb[i] = 0;
	for (i = 0; i < MAXSTRPPP; i++) svr->npb[i] = 0;
	for (i = 0; i < MAXSTRPPP; i++) svr->buff[i] = NULL;
	for (i = 0; i < 2; i++) svr->sbuf[i] = NULL;
	for (i = 0; i < MAXSTRPPP; i++) svr->pbuf[i] = NULL;
	for (i = 0; i < MAXSTRPPP; i++) for (j = 0; j < 10; j++) svr->nmsg[i][j] = 0;
	for (i = 0; i < MAXSTRPPP; i++) svr->ftime[i] = time0;
	for (i = 0; i < MAXSTRPPP; i++) svr->files[i][0] = '\0';
	svr->moni = NULL;
	svr->tick = 0;
	svr->thread = 0;
	svr->cputime = svr->prcout = svr->nave = 0;
	for (i = 0; i < 3; i++) svr->rb_ave[i] = 0.0;

	if (!(svr->nav.eph = (eph_t*)malloc(sizeof(eph_t) * (MAXSAT) * 2)) ||
		!(svr->nav.geph = (geph_t*)malloc(sizeof(geph_t) * NSATGLO * 2)) ||
		!(svr->nav.seph = (seph_t*)malloc(sizeof(seph_t) * NSATSBS * 2))) {
		tracet(1, "rtksvrinit: malloc error\n");
		return 0;
	}
	for (i = 0; i < (MAXSAT) * 2; i++) svr->nav.eph[i] = eph0;
	for (i = 0; i < NSATGLO * 2; i++) svr->nav.geph[i] = geph0;
	//	for (i = 0; i<NSATSBS * 2; i++) svr->nav.seph[i] = seph0;
	svr->nav.n = (MAXSAT) * 2;
	svr->nav.ng = NSATGLO * 2;
	svr->nav.ns = NSATSBS * 2;



	for (i = 0; i < MAXOBSBUF; i++)
	{
		if (!(svr->obs[i].data = (obsd_t*)malloc(sizeof(obsd_t) * MAXOBS)))
		{
			tracet(1, "rtksvrinit: malloc error\n");
			return 0;
		}
	}

	//for (i = 0; i<MAXSTRINPPP; i++) {
	//	memset(svr->raw + i, 0, sizeof(raw_t));
	//	memset(svr->rtcm + i, 0, sizeof(rtcm_t));
	//}

	for (i = 0; i < MAXSTRPPP; i++) strinit(svr->stream + i);

	for (i = 0; i < MAXSTRPPP; i++)* svr->cmds_periodic[i] = '\0';
	*svr->cmd_reset = '\0';
	svr->bl_reset = 10.0;
	initlock(&svr->lock);
	svr->staname[0] = '\0';
	svr->flag = 1;
	svr->prctick = 0;
	svr->obstick = 0;

	return 1;
}
/* free rtk server -------------------------------------------------------------
* free rtk server
* args   : pppsvr_t *svr    IO rtk server
* return : none
*-----------------------------------------------------------------------------*/
extern void rtksvrfree(pppsvr_t *svr)
{
	int i;

	free(svr->nav.eph);
	free(svr->nav.geph);
	free(svr->nav.seph);
	for (i = 0; i<MAXOBSBUF; i++)
	{
		free(svr->obs[i].data);
	}
	releaseSolution(&svr->sol);
}


/* lock/unlock rtk server ------------------------------------------------------
* lock/unlock rtk server
* args   : pppsvr_t *svr    IO rtk server
* return : status (1:ok 0:error)
*-----------------------------------------------------------------------------*/
extern void rtksvrlock(pppsvr_t* svr) { lock(&svr->lock); }
extern void rtksvrunlock(pppsvr_t* svr) { unlock(&svr->lock); }


static int pppsvrconf(int ind, pppsvr_t* svr, int cycle, int buffsize, int* strs,
	char** paths, int* formats, int navsel, char** cmds,
	char** cmds_periodic, char** rcvopts, int nmeacycle,
	int nmeareq, const double* nmeapos, prcopt_t* prcopt,
	solopt_t* solopt, stream_t* moni, char* errmsg)
{
	gtime_t time, time0 = { 0 };
	int i, j, rw;
	svr->cycle = cycle > 1 ? cycle : 1;
	svr->nmeacycle = nmeacycle > 1000 ? nmeacycle : 1000;
	svr->nmeareq = nmeareq;
	for (i = 0; i < 3; i++) svr->nmeapos[i] = nmeapos[i];
	svr->buffsize = buffsize > 4096 ? buffsize : 4096;

	svr->navsel = navsel;
	svr->nsbs = 0;
	svr->nsol = 0;
	svr->prcout = 0;
	//releaseSolution(&svr->sol);
	InitialSolution(&svr->sol, prcopt);
	int onefileflag = 0;

	if (corNum == 1) {
		onefileflag = corNum;
	}
	else {
		onefileflag = MAXSTRPPP;
	}

	for (i = 0; i < onefileflag; i++) { /* input/log streams */
		svr->nb[i] = svr->npb[i] = 0;
		if (!(svr->buff[i] = (unsigned char*)malloc(buffsize)) ||
			!(svr->pbuf[i] = (unsigned char*)malloc(buffsize))) {
			tracet(1, "rtksvrstart: malloc error\n");
			sprintf(errmsg, "rtk server malloc error");
			return 0;
		}
		for (j = 0; j < 10; j++) svr->nmsg[i][j] = 0;
		strcpy(svr->cmds_periodic[i], !cmds_periodic[i] ? "" : cmds_periodic[i]);

		/* initialize receiver raw and rtcm control */
		init_rtcm(svr->rtcm + i);
		/* set receiver and rtcm option */
		strcpy(svr->rtcm[i].opt, rcvopts[i]);
		if (i == 0)
		{
			init_raw(svr->raw);
			strcpy(svr->raw[i].opt, rcvopts[i]);
		}
	}


	for (i = 0; i < MAXOBSBUF; i++) svr->obs[i].n = 0;
	for (i = 0; i < 2; i++) { /* output peek buffer */
		if (!(svr->sbuf[i] = (unsigned char*)malloc(buffsize))) {
			tracet(1, "rtksvrstart: malloc error\n");
			sprintf(errmsg, "ppp server malloc error");
			return 0;
		}
	}
	/* set solution options */
	for (i = 0; i < 2; i++) {
		svr->solopt[i] = solopt[i];
	}

	/* update navigation data */
	for (i = 0; i < (MAXSAT) * 2; i++) svr->nav.eph[i].ttr = time0;
	for (i = 0; i < NSATGLO * 2; i++) svr->nav.geph[i].tof = time0;
	for (i = 0; i < NSATSBS * 2; i++) svr->nav.seph[i].tof = time0;
	updatenav(&svr->nav);

	/* set monitor stream */
	svr->moni = moni;

	/* open input streams */

	svr->format[0] = formats[ind];
	rw = STR_MODE_R|STR_MODE_W;
	if (!stropen(svr->stream, strs[ind], rw, paths[ind])) {
		sprintf(errmsg, "str%d open error path=%s\n", ind + 1, paths[ind]);
		strclose(svr->stream);
		printf(errmsg);
		return 0;
	}
	/* set initial time for rtcm and raw */
	time = utc2gpst(timeget());
	svr->raw[0].time = strs[ind] == STR_FILE ? strgettime(svr->stream) : time;
	svr->rtcm[0].time = strs[ind] == STR_FILE ? strgettime(svr->stream) : time;

	/* write start commands to input streams */
	if (cmds[ind])
	{
		strwrite(svr->stream, (unsigned char*)"", 0); /* for connect */
		sleepms(100);
		strsendcmd(svr->stream, cmds[ind]);
	}
	return 1;
}

/* start rtk server ------------------------------------------------------------
* start rtk server thread
* args   : pppsvr_t *svr    IO rtk server
*          int     cycle    I  server cycle (ms)
*          int     buffsize I  input buffer size (bytes)
*          int     *strs    I  stream types (STR_???)
*                              types[0]=input stream rover
*                              types[1]=input stream base station
*                              types[2]=input stream correction
*                              types[3]=output stream solution 1
*                              types[4]=output stream solution 2
*                              types[5]=log stream rover
*                              types[6]=log stream base station
*                              types[7]=log stream correction
*          char    *paths   I  input stream paths
*          int     *format  I  input stream formats (STRFMT_???)
*                              format[0]=input stream rover
*                              format[1]=input stream base station
*                              format[2]=input stream correction
*          int     navsel   I  navigation message select
*                              (0:rover,1:base,2:ephem,3:all)
*          char    **cmds   I  input stream start commands
*                              cmds[0]=input stream rover (NULL: no command)
*                              cmds[1]=input stream base (NULL: no command)
*                              cmds[2]=input stream corr (NULL: no command)
*          char    **cmds_periodic I input stream periodic commands
*                              cmds[0]=input stream rover (NULL: no command)
*                              cmds[1]=input stream base (NULL: no command)
*                              cmds[2]=input stream corr (NULL: no command)
*          char    **rcvopts I receiver options
*                              rcvopt[0]=receiver option rover
*                              rcvopt[1]=receiver option base
*                              rcvopt[2]=receiver option corr
*          int     nmeacycle I nmea request cycle (ms) (0:no request)
*          int     nmeareq  I  nmea request type
*                              (0:no,1:base pos,2:single sol,3:reset and single)
*          double *nmeapos  I  transmitted nmea position (ecef) (m)
*          solopt_t *prcopt_t I  rtk processing options
*          solopt_t *solopt_t I  solution options
*                              solopt_t[0]=solution 1 options
*                              solopt_t[1]=solution 2 options
*          stream_t *moni   I  monitor stream (NULL: not used)
*          char   *errmsg   O  error message
* return : status (1:ok 0:error)
*-----------------------------------------------------------------------------*/
extern int rtksvrstart(pppsvr_t* svr, int cycle, int buffsize, int* strs, char** paths, int* formats, int navsel, char** cmds, char** cmds_periodic, char** rcvopts, int nmeacycle, int nmeareq, const double* nmeapos, prcopt_t* prcopt, solopt_t* solopt, stream_t* moni, char* errmsg)
{
	int i,j;
	int ret;

	strinitcom();
	


	for (i = 0; i < corNum; i++)
	{
		ret = pppsvrconf(i, svr + i, cycle, buffsize, strs, paths, formats, navsel, cmds, cmds_periodic, rcvopts, nmeacycle,
			nmeareq, nmeapos, prcopt, solopt, moni, errmsg);

		if (!ret) continue;


		/* create correction server thread*/
#ifdef WIN32
		if (!(svr[i].thread = CreateThread(NULL, 0, pppcorsvrthread, svr + i, 0, NULL))) {
#else
		if ((ret = pthread_create(&svr[i].thread, &attr, pppcorsvrthread, svr + i))) {
#endif
			strclose(svr[i].stream);
			sprintf(errmsg, "correction thread create error\n");
			printf("correction thread create error\n");
			return 0;
		}
	}

	return 1;
}

/* stop rtk server -------------------------------------------------------------
* start rtk server thread
* args   : pppsvr_t *svr    IO rtk server
*          char    **cmds   I  input stream stop commands
*                              cmds[0]=input stream rover (NULL: no command)
*                              cmds[1]=input stream base  (NULL: no command)
*                              cmds[2]=input stream ephem (NULL: no command)
* return : none
*-----------------------------------------------------------------------------*/
extern void rtksvrstop(pppsvr_t *svr, char **cmds)
{
	int i;

	tracet(3, "rtksvrstop:\n");

	/* write stop commands to input streams */
	//rtksvrlock(svr);
	//for (i = 0; i<MAXSTRPPP; i++) {
	//	if (cmds[i]) strsendcmd(svr->stream + i, cmds[i]);
	//}
	//rtksvrunlock(svr);

	/* stop rtk server */
	svr->state = 0;

	/* free rtk server thread */
#ifdef WIN32
	WaitForSingleObject(svr->thread, 10000);
	CloseHandle(svr->thread);
#else
	pthread_join(svr->thread, NULL);
#endif
}
/* open output/log stream ------------------------------------------------------
* open output/log stream
* args   : pppsvr_t *svr    IO rtk server
*          int     index    I  output/log stream index
*                              (3:solution 1,4:solution 2,5:log rover,
*                               6:log base station,7:log correction)
*          int     str      I  output/log stream types (STR_???)
*          char    *path    I  output/log stream path
*          solopt_t *solopt_t I  solution options
* return : status (1:ok 0:error)
*-----------------------------------------------------------------------------*/
extern int rtksvropenstr(pppsvr_t *svr, int index, int str, const char *path,
	const solopt_t *solopt_t)
{
	tracet(3, "rtksvropenstr: index=%d str=%d path=%s\n", index, str, path);

	if (index<3 || index>7 || !svr->state) return 0;

	rtksvrlock(svr);

	if (svr->stream[index].state>0) {
		rtksvrunlock(svr);
		return 0;
	}
	if (!stropen(svr->stream + index, str, STR_MODE_W, path)) {
		tracet(2, "stream open error: index=%d\n", index);
		rtksvrunlock(svr);
		return 0;
	}
	if (index <= MAXSTRPPP) {
		svr->solopt[index - 3] = *solopt_t;

		/* write solution header to solution stream */
		writesolhead(svr->stream + index, svr->solopt + index - 3);
	}
	rtksvrunlock(svr);
	return 1;
}
/* close output/log stream -----------------------------------------------------
* close output/log stream
* args   : pppsvr_t *svr    IO rtk server
*          int     index    I  output/log stream index
*                              (3:solution 1,4:solution 2,5:log rover,
*                               6:log base station,7:log correction)
* return : none
*-----------------------------------------------------------------------------*/
extern void rtksvrclosestr(pppsvr_t *svr, int index)
{
	tracet(3, "rtksvrclosestr: index=%d\n", index);

	if (index<3 || index>7 || !svr->state) return;

	rtksvrlock(svr);

	strclose(svr->stream + index);

	rtksvrunlock(svr);
}
/* get observation data status -------------------------------------------------
* get current observation data status
* args   : pppsvr_t *svr    I  rtk server
*          int     rcv      I  receiver (0:rover,1:base,2:ephem)
*          gtime_t *time    O  time of observation data
*          int     *sat     O  satellite prn numbers
*          double  *az      O  satellite azimuth angles (rad)
*          double  *el      O  satellite elevation angles (rad)
*          int     **snr    O  satellite snr for each freq (dBHz)
*                              snr[i][j] = sat i freq j snr
*          int     *vsat    O  valid satellite flag
* return : number of satellites
*-----------------------------------------------------------------------------*/
extern int rtksvrostat(pppsvr_t *svr, int rcv, gtime_t *time, int *sat,
	double *az, double *el, int **snr, int *vsat)
{
	int i, j, ns;

	tracet(4, "rtksvrostat: rcv=%d\n", rcv);

	if (!svr->state) return 0;
	rtksvrlock(svr);
	ns = svr->obs[0].n;
	if (ns>0) {
		*time = svr->obs[0].data[0].time;
	}
	for (i = 0; i<ns; i++) {
		sat[i] = svr->obs[0].data[i].sat;
		az[i] = svr->sol.ssat[sat[i] - 1].azimuth;
		el[i] = svr->sol.ssat[sat[i] - 1].elevation;
		for (j = 0; j<NFREQ; j++) {
			snr[i][j] = (int)(svr->obs[0].data[i].SNR[j] * 0.25);
		}
		if (svr->sol.stat == SOLQ_NONE || svr->sol.stat == SOLQ_SINGLE) {
			vsat[i] = svr->sol.ssat[sat[i] - 1].available;
		}
		else {
			vsat[i] = svr->sol.ssat[sat[i] - 1].vsat[0];
		}
	}
	rtksvrunlock(svr);
	return ns;
}
/* get stream status -----------------------------------------------------------
* get current stream status
* args   : pppsvr_t *svr    I  rtk server
*          int     *sstat   O  status of streams
*          char    *msg     O  status messages
* return : none
*-----------------------------------------------------------------------------*/
extern void rtksvrsstat(pppsvr_t *svr, int *sstat, char *msg)
{
	int i;
	char s[MAXSTRMSG], *p = msg;

	tracet(4, "rtksvrsstat:\n");

	rtksvrlock(svr);
	for (i = 0; i<MAXSTRPPP; i++) {
		sstat[i] = strstat(svr->stream + i, s);
		if (*s) p += sprintf(p, "(%d) %s ", i + 1, s);
	}
	rtksvrunlock(svr);
}
/* mark current position -------------------------------------------------------
* open output/log stream
* args   : pppsvr_t *svr    IO rtk server
*          char    *name    I  marker name
*          char    *comment I  comment string
* return : status (1:ok 0:error)
*-----------------------------------------------------------------------------*/
extern int rtksvrmark(pppsvr_t *svr, const char *name, const char *comment)
{
	char buff[MAXSOLMSG + 1], tstr[32], *p, *q;
	double tow, pos[3];
	int i, sum, week;

	tracet(4, "rtksvrmark:name=%s comment=%s\n", name, comment);

	if (!svr->state) return 0;

	rtksvrlock(svr);

	time2str(svr->sol.time, tstr, 2);

	tow = time2gpst(svr->sol.time,&week);
	CoorCar2CoorGeo(svr->sol.rr, pos);

	for (i = 0; i<2; i++) {
		p = buff;
		if (svr->solopt[i].posf == SOLF_STAT) {
			p += sprintf(p, "$MARK,%d,%.3f,%d,%.4f,%.4f,%.4f,%s,%s\n", week, tow,
				svr->sol.stat, svr->sol.rr[0], svr->sol.rr[1],
				svr->sol.rr[2], name, comment);
		}
		else if (svr->solopt[i].posf == SOLF_NMEA) {
			p += sprintf(p, "$GPTXT,01,01,02,MARK:%s,%s,%.9f,%.9f,%.4f,%d,%s",
				name, tstr, pos[0] * R2D, pos[1] * R2D, pos[2], svr->sol.stat,
				comment);
			for (q = (char *)buff + 1, sum = 0; *q; q++) sum ^= *q; /* check-sum */
			p += sprintf(p, "*%02X%c%c", sum, 0x0D, 0x0A);
		}
		else {
			p += sprintf(p, "%s MARK: %s,%s,%.9f,%.9f,%.4f,%d,%s\n", COMMENTH,
				name, tstr, pos[0] * R2D, pos[1] * R2D, pos[2], svr->sol.stat,
				comment);
		}
		strwrite(svr->stream + i + 3, (unsigned char *)buff, p - buff);
		saveoutbuf(svr, (unsigned char *)buff, p - buff, i);
	}
	if (svr->moni) {
		p = buff;
		p += sprintf(p, "%s MARK: %s,%s,%.9f,%.9f,%.4f,%d,%s\n", COMMENTH,
			name, tstr, pos[0] * R2D, pos[1] * R2D, pos[2], svr->sol.stat,
			comment);
		strwrite(svr->moni, (unsigned char *)buff, p - buff);
	}
	rtksvrunlock(svr);
	return 1;
}
