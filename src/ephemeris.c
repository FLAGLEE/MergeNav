#include "ephemeris.h"


#define DTTOL       0.005               /* tolerance of time difference (s) */
#define MAXDTOE     7200.0              /* max time difference to ephem Toe (s) for GPS */
#define MAXDTOE_GLO 1800.0              /* max time difference to GLONASS Toe (s) */


#define DEFURASSR 0.15            /* default accurary of ssr corr (m) */
#define MAXECORSSR 10.0           /* max orbit correction of ssr (m) */
#define MAXCCORSSR (1E-6*CLIGHT)  /* max clock correction of ssr (m) */
#define MAXAGESSR 90.0            /* max age of ssr orbit and clock (s) */
#define MAXAGESSR_HRCLK 10.0      /* max age of ssr high-rate clock (s) */
#define MAXAGESSR_CBIAS 600.0     /* max age of ssr code bias (s) */
#define STD_BRDCCLK 30.0          /* error of broadcast clock (m) */

#define ERREPH_GLO         5.0            /* error of glonass ephemeris (m) */
#define TSTEP              60.0             /* integration step glonass ephemeris (s) */
#define TOL_KEPLER         1E-14
#define MAX_ITER_KEPLER    30

#define SIN_5             -0.0871557427476582 /* sin(-5.0 deg) */
#define COS_5              0.9961946980917456 /* cos(-5.0 deg) */

#define NMAX        10              /* order of polynomial interpolation */
#define MAXDTE     900.0           /* max time difference to ephem time (s) */
#define EXTERR_CLK  1E-3            /* extrapolation error for clock (m/s) */
#define EXTERR_EPH  5E-7            /* extrapolation error for ephem (m/s^2) */



/* variance by ura ephemeris (ref [1] 20.3.3.3.1.1) --------------------------*/
double var_uraeph(int ura)
{
	const double ura_value[] = {
		2.4, 3.4, 4.85, 6.85, 9.65, 13.65, 24.0, 48.0, 96.0, 192.0, 384.0, 768.0, 1536.0,
		3072.0, 6144.0
	};
	return ura<0 || 15<ura ? SQR(6144.0) : SQR(ura_value[ura]);
}
/* variance by ura ssr (ref [4]) ---------------------------------------------*/
double var_urassr(int ura)
{
	double std;
	if (ura <= 0) return SQR(DEFURASSR);
	if (ura >= 63) return SQR(5.4665);
	std = (pow(3.0, (ura >> 3) & 7)*(1.0 + (ura & 7) / 4.0) - 1.0)*1E-3;
	return SQR(std);
}


int BRDC_SatPosition(gtime_t emissionTime, eph_t *eph_t, double *pos, double *clk, double *var)
{
	int sat, sys, num = 0, prn = 0;
	double mu, om_e;
	double O;
	double xg, yg, zg;  //BDS GEO satellite

	double tk;
	double n0;             //�ο�ʱ��TOEƽ�����ٶ�
	double A;
	double n;              //�۲�ʱ������ƽ�����ٶ�
	double M;              //�۲�ʱ������ƽ�����	
	double E0;             //ƫ�����
	double Ek;
	double f;              //������
	double uu;             //�����Ǿ�
	double du, dr, di;     //�㶯������
	double u, r, i;
	double x, y;           //���������ϵ�е�λ��
						   //		double L;              //�����㾭��

	char s_sat[4] = { 0 };
	sat = eph_t->sat;
	sys = satIndex2sys(sat);
	prn = satIndex2satno(sat);
	A = eph_t->A;
	if (A <= 0)
	{
		pos[0] = pos[1] = pos[2] = *clk = 0.0;
		return 0;
	}
	tk = timediff(emissionTime, eph_t->toe);

	switch (sys)
	{
	case SYS_GAL:mu = MU_GAL; om_e = OM_E_GAL; break;
	case SYS_CMP:mu = MU_CMP; om_e = OM_E_CMP; break;
	default:	 mu = MU_GPS, om_e = OM_E_GPS; break;
	}

	n0 = sqrt(mu / pow(A, 3));
	n = n0 + eph_t->deln;
	M = eph_t->M0 + n*tk;

	Ek = M;
	//����Ek
	do
	{
		E0 = Ek;
		Ek -= (Ek - eph_t->e*sin(Ek) - M) / (1.0 - eph_t->e*cos(Ek));
		num++;
	} while (fabs(Ek - E0)>TOL_KEPLER&&num<MAX_ITER_KEPLER);
	if (num >= MAX_ITER_KEPLER)
	{
		printf("kepler iteration overflow sat= %s\n", satIndex2satID(s_sat, sat));
		return 0;
	}
	//	if (Ek<0) Ek += 2 * PI;
	//	if (Ek>2 * PI) Ek -= 2 * PI;

	f = atan2(sqrt(1 - pow(eph_t->e, 2))*sin(Ek), cos(Ek) - eph_t->e);
	uu = f + eph_t->omg;
	du = eph_t->cus*sin(2 * uu) + eph_t->cuc*cos(2 * uu);
	dr = eph_t->crs*sin(2 * uu) + eph_t->crc*cos(2 * uu);
	di = eph_t->cis*sin(2 * uu) + eph_t->cic*cos(2 * uu);
	u = uu + du;
	r = eph_t->A*(1.0 - eph_t->e*cos(Ek)) + dr;
	i = eph_t->i0 + eph_t->idot*tk + di;

	x = r*cos(u); y = r*sin(u);

	/* beidou geo satellite (ref [9])rtklib */
	if (sys == SYS_CMP&&prn <= 5) {
		O = eph_t->OMG0 + eph_t->OMGd*tk - om_e*eph_t->toes;
		xg = x*cos(O) - y*cos(i)*sin(O);
		yg = x*sin(O) + y*cos(i)*cos(O);
		zg = y*sin(i);
		pos[0] = xg*cos(om_e*tk) + yg*sin(om_e*tk)*COS_5 + zg*sin(om_e*tk)*SIN_5;
		pos[1] = -xg*sin(om_e*tk) + yg*cos(om_e*tk)*COS_5 + zg*cos(om_e*tk)*SIN_5;
		pos[2] = -yg*SIN_5 + zg*COS_5;
	}
	else
	{
		O = eph_t->OMG0 + (eph_t->OMGd - om_e)*tk - om_e*eph_t->toes;
		pos[0] = x*cos(O) - y*cos(i)*sin(O);
		pos[1] = x*sin(O) + y*cos(i)*cos(O);
		pos[2] = y*sin(i);
	}
	tk = timediff(emissionTime, eph_t->toc);
	*clk = eph_t->f0 + eph_t->f1*tk + eph_t->f2*tk*tk;

	/*�����ЧӦ����*/
	*clk -= 2.0*sqrt(mu*eph_t->A)*eph_t->e*sin(Ek) / SQR(CLIGHT);
	*var = var_uraeph(eph_t->sva);
	return 1;
}

/*************Get Glonass satellite position *****************************************
*give two method
�˴�����GLAB��GPSTK�ķ������Ƚ�PZ-90ת�����ԣ����ԣ�����ϵ������Numerical integration of the dierential equations that describe the
motion of the satellites��Ȼ���ٽ����ת����PZ-90��
��������Ŀǰ��������ײ��ã�rtkliҲ���ô˷�������Ϊ�ϼ򵥣����в�δ���������ܵ�ת����ֱ�����������һ�����ϵ����Ч���ȼ���GPSTK
**********************************************************************************************/
int BRDC_GlonassSatPosition(gtime_t emissionTime, geph_t *geph_t, double *pos, double *clk, double *var)
{
	double t, tt, x[3], v[3], a[3], sid0, s0, numSec, s, coss, sins;
	int sat;
	double ep[6];
	sat = geph_t->sat;

	t = timediff(emissionTime, geph_t->toe);
	*clk = -geph_t->taun + geph_t->gamn*t;

	time2epoch(emissionTime,ep);
	sid0 = getSidTime(emissionTime);
	s0 = sid0*PI / 12;
	numSec = ep[3] * 3600 + ep[4] * 60 + ep[5];
	s = s0 + OM_E_GLO*numSec;
	coss = cos(s);
	sins = sin(s);

	x[0] = geph_t->pos[0] * coss - geph_t->pos[1] * sins;
	x[1] = geph_t->pos[0] * sins + geph_t->pos[1] * coss;
	x[2] = geph_t->pos[2];
	v[0] = geph_t->vel[0] * coss - geph_t->vel[1] * sins - OM_E_GLO*x[1];
	v[1] = geph_t->vel[0] * sins + geph_t->vel[1] * coss + OM_E_GLO*x[0];
	v[2] = geph_t->vel[2];
	for (tt = t<0.0 ? -TSTEP : TSTEP; fabs(t)>1E-9; t -= tt) {
		if (fabs(t)<TSTEP) tt = t;
		numSec += tt;
		s = s0 + OM_E_GLO*numSec;
		coss = cos(s);
		sins = sin(s);
		a[0] = geph_t->acc[0] * coss - geph_t->acc[1] * sins;
		a[1] = geph_t->acc[0] * sins + geph_t->acc[1] * coss;
		a[2] = geph_t->acc[2];
		gloorbit(tt, x, v, a);
	}

	pos[0] = x[0] * coss + x[1] * sins;
	pos[1] = -x[0] * sins + x[1] * coss;
	pos[2] = x[2];
	*var = SQR(ERREPH_GLO);
	return 1;
}


/*Get GLONASS orbit
*t                 difference time
*x                 satellite position
*v                 satellite velocity
*a                 satellite acceleration*/
void gloorbit(double t, double *x, double *v, double *a)
{
	double dxt1[6], dxt2[6], dxt3[6], dxt4[6], tempx[3], tempv[3];
	int i;
	derivativeGloOrbitModel(x, v, dxt1, a);
	for (i = 0; i < 3; i++)
	{
		tempx[i] = x[i] + dxt1[i] * t / 2.0;
		tempv[i] = v[i] + dxt1[i + 3] * t / 2.0;
	}
	derivativeGloOrbitModel(tempx, tempv, dxt2, a);
	for (i = 0; i < 3; i++)
	{
		tempx[i] = x[i] + dxt2[i] * t / 2.0;
		tempv[i] = v[i] + dxt2[i + 3] * t / 2.0;
	}
	derivativeGloOrbitModel(tempx, tempv, dxt3, a);
	for (i = 0; i < 3; i++)
	{
		tempx[i] = x[i] + dxt3[i] * t;
		tempv[i] = v[i] + dxt3[i + 3] * t;
	}
	derivativeGloOrbitModel(tempx, tempv, dxt4, a);
	for (i = 0; i < 3; i++)
	{
		x[i] = x[i] + (dxt1[i] + 2.0*dxt2[i] + 2.0*dxt3[i] + dxt4[i]) * t / 6.0;
		v[i] = v[i] + (dxt1[i + 3] + 2.0*dxt2[i + 3] + 2.0*dxt3[i + 3] + dxt4[i + 3]) * t / 6.0;
	}

}

/*derivative of GLONASS orbital model
*x                   satellite position
*vel                 satellite velocity
*xdot                orbit model
*acc                 satellite acceleration*/
void derivativeGloOrbitModel(const double *x, const double *vel, double *xdot, const double *acc)
{
	double a, b, c, r = norm(x, 3), r2 = pow(r, 2), r3 = pow(r, 3), omg2 = SQR(OM_E_GLO);

	if (r2 <= 0.0) {
		xdot[0] = xdot[1] = xdot[2] = xdot[3] = xdot[4] = xdot[5] = 0.0;
		return;
	}
	a = 1.5*J2_GLO*MU_GLO*SQR(RE_GLO) / r2 / r3; /* 3/2*J2*mu*Ae^2/r^5 */
	b = 5.0*x[2] * x[2] / r2;                    /* 5*z^2/r^2 */
	c = -MU_GLO / r3 - a*(1.0 - b);                /* -mu/r^3-a(1-b) */
	xdot[0] = vel[0]; xdot[1] = vel[1]; xdot[2] = vel[2];
	xdot[3] = (c + omg2)*x[0] + 2.0*OM_E_GLO*vel[1] + acc[0];//��������rtklib�ķ���
	xdot[4] = (c + omg2)*x[1] - 2.0*OM_E_GLO*vel[0] + acc[1];//��������rtklib�ķ���
	xdot[3] = (c)*x[0] + acc[0];
	xdot[4] = (c)*x[1] + acc[1];
	xdot[5] = (c - 2.0*a)*x[2] + acc[2];
}


eph_t *selectEphememeris(gtime_t time, const nav_t *nav, int sat, int iode)
{
	double t, tmax, tmin;
	int sys;
	int i, j = -1;
	sys = satIndex2sys(sat);
	tmax = MAXDTOE + 1.0;
	tmin = tmax + 1.0;
	for (i = 0; i<nav->n; i++) {
		if (nav->eph[i].sat == 0)
			continue;
		if (nav->eph[i].sat != sat) continue;
		if (iode >= 0 && nav->eph[i].iode != iode) continue;
		if ((t = fabs(timediff(nav->eph[i].toe, time)))>tmax) continue;
		if (iode >= 0) return nav->eph + i;
		if (t <= tmin) { j = i; tmin = t; } /* toe closest to time */
	}
	if (iode >= 0 || j<0)
		return NULL;

	return nav->eph + j;
}

geph_t *selectGlonassEphememeris(gtime_t time, const nav_t *nav, int sat, int iode)
{
	double t, tmax, tmin;
	int sys;
	int i, j = -1;
	sys = satIndex2sys(sat);
	tmax = MAXDTOE_GLO + 1;
	tmin = tmax + 1.0;
	for (i = 0; i<nav->ng; i++) {
		if (nav->geph[i].sat == 0)
			continue;
		if (nav->geph[i].sat != sat) continue;
		if (iode >= 0 && nav->geph[i].iode != iode) continue;
		if ((t = fabs(timediff(nav->geph[i].toe, time)))>tmax) continue;
		if (iode >= 0) return nav->geph + i;
		if (t <= tmin) { j = i; tmin = t; } /* toe closest to time */
	}
	if (iode >= 0 || j<0)
		return NULL;

	return nav->geph + j;
}


int SatClockCorrection(gtime_t time, const nav_t *nav, int sat, int iode, double *rs, double *dts)
{
	int sys, k;

	double t;
	char s_sat[4] = { 0 };
	eph_t *eph_t;
	geph_t *geph_t;

	sys = satIndex2sys(sat);

	if (sys == SYS_GPS || sys == SYS_CMP || sys == SYS_GAL)
	{
		if ((eph_t = selectEphememeris(time, nav, sat, iode)) == NULL)
		{
//			printf("Failed to select ephememeris to correct satellite clock for %s at %s\n", satIndex2satID(s_sat, sat),time_str(time,2));
			return 0;
		}
		t = timediff(time, eph_t->toc);
		for (k = 0; k < 2; k++)
			t -= eph_t->f0 + eph_t->f1*t + eph_t->f2*t*t;
		dts[0] = eph_t->f0 + eph_t->f1*t + eph_t->f2*t*t;
		dts[1] = eph_t->f1 + 2 * eph_t->f2*t;
		dts[0] -= 2.0*dot_vector(rs, rs + 3, 3) / CLIGHT / CLIGHT;
	}
	else if (sys == SYS_GLO)
	{
		if ((geph_t = selectGlonassEphememeris(time, nav, sat, iode)) == NULL)
		{
//			printf("Failed to select glonass ephememeris to correct satellite clock for %s\n", satIndex2satID(s_sat, sat));
			return 0;
		}

		t = timediff(time, geph_t->toe);

		for (k = 0; k<2; k++)
			t -= -geph_t->taun + geph_t->gamn*t;
		dts[0] = -geph_t->taun + geph_t->gamn*t;
		dts[1] = geph_t->gamn;
	}
	/* relativity correction */
	//	if (iode==-1)//�˴�����spp

	//move to GPS CMP... not use for GLO
	//	dts[0] -= 2.0*dot_vector(rs, rs + 3, 3) / CLIGHT / CLIGHT;
	return 1;
}


int SatPosition(gtime_t time, const nav_t *nav, int sat, int iode, double *rs, double *dts, double *var, int *svh)
{
	int k, sys;
	eph_t *eph_t;
	geph_t *geph_t;
	gtime_t t;
	double pos1[3], pos2[3], clk1, clk2;

	char s_sat[4] = { 0 };
	sys = satIndex2sys(sat);
	*svh = -1;
	if (sys == SYS_GPS || sys == SYS_CMP || sys == SYS_GAL)
	{
		if ((eph_t = selectEphememeris(time, nav, sat, iode)) == NULL)
		{
//			printf("Failed to select ephememeris to obtain satellite position for %s\n", satIndex2satID(s_sat, sat));
			return 0;
		}
		if (!BRDC_SatPosition(time, eph_t, pos1, &clk1, var))
		{
//			printf("Failed to obtain satellite position by selected ephememeris for %s\n", satIndex2satID(s_sat, sat));
			return 0;
		}
		t = timeadd(time, 0.001);
		if (!BRDC_SatPosition(t, eph_t, pos2, &clk2, var))
		{
//			printf("Failed to obtain satellite position to get sat velocity and clock drift for %s\n", satIndex2satID(s_sat, sat));
			return 0;
		}
		else
		{
			for (k = 0; k < 3; k++)
			{
				rs[k] = pos1[k];
				rs[k + 3] = (pos2[k] - pos1[k]) / 0.001;
			}
			dts[0] = clk1;
			dts[1] = (clk2 - clk1) / 0.001;
		}
		*svh = eph_t->svh;
	}
	else if (sys == SYS_GLO)
	{
		if ((geph_t = selectGlonassEphememeris(time, nav, sat, iode)) == NULL)
		{
//			printf("Failed to select ephememeris to obtain glonass satellite position for %s\n", satIndex2satID(s_sat, sat));
			return 0;
		}
		if (!BRDC_GlonassSatPosition(time, geph_t, pos1, &clk1, var))
		{
//			printf("Failed to obtain glonass satellite position by selected ephememeris for %s\n", satIndex2satID(s_sat, sat));
			return 0;
		}

		t = timeadd(time, 0.001);
		if (!BRDC_GlonassSatPosition(t, geph_t, pos2, &clk2, var))
		{
//			printf("Failed to obtain glonass satellite position by selected ephememeris for %s\n", satIndex2satID(s_sat, sat));
			return 0;
		}
		else
		{
			for (k = 0; k < 3; k++)
			{
				rs[k] = pos1[k];
				rs[k + 3] = (pos2[k] - pos1[k]) / 0.001;
			}
			dts[0] = clk1;
			dts[1] = (clk2 - clk1) / 0.001;
		}
		*svh = geph_t->svh;
	}
	return 1;
}



int brdcPLUSssr(gtime_t time, const nav_t *nav, int sat, double *rs, double *dts, double *var, int *svh, int opt)
{
	const ssr_t *ssr;
	//eph_t *eph;
	double t1, t2, t3, er[3], ea[3], ec[3], rc[3], deph[3], dclk, dant[3] = { 0 } /*tk*/;
	int i, sys;
	char s_sat[4] = { 0 };
	ssr = nav->ssr + sat - 1;

	if (ssr->t0[0].time == 0)
	{
		//		printf("no ssr orbit corrections!\n");
		return 0;
	}

	if (ssr->t0[1].time == 0)
	{
		//		printf("no ssr clock corrections!\n");
		return 0;
	}

	if (ssr->iode <= 0)
	{
		//		printf("iode error!\n");
		return 0;
	}

	if (ssr->iod[0] != ssr->iod[1])
	{
		printf("inconsist ssr correction: %s sat=%2d iod=%d %d\n",
			time_str(time, 2), sat, ssr->iod[0], ssr->iod[1]);
		*svh = -1;
		return 0;
	}

	t1 = timediff(time, ssr->t0[0]);
	t2 = timediff(time, ssr->t0[1]);
	t3 = timediff(time, ssr->t0[2]);

	if (fabs(t1)>MAXAGESSR || fabs(t2)>MAXAGESSR) {
		printf("age of ssr error: %s sat=%2d t=%.0f %.0f\n", time_str(time, 2),
			sat, t1, t2);
		*svh = -1;
		return 0;
	}

	if (ssr->udi[0] >= 1.0) t1 -= ssr->udi[0] / 2.0;
	if (ssr->udi[1] >= 1.0) t2 -= ssr->udi[0] / 2.0;

	for (i = 0; i<3; i++) deph[i] = ssr->deph[i] + ssr->ddeph[i] * t1;
	dclk = ssr->dclk[0] + ssr->dclk[1] * t2 + ssr->dclk[2] * t2*t2;

	/* ssr highrate clock correction (ref [4]) */
	if (ssr->iod[0] == ssr->iod[2] && ssr->t0[2].time&&fabs(t3)<MAXAGESSR_HRCLK) {
		dclk += ssr->hrclk;
	}

	if (norm(deph, 3)>MAXECORSSR || fabs(dclk)>MAXCCORSSR) {
		printf("invalid ssr correction: %s %s deph=%.1f dclk=%.1f\n",
			time_str(time, 2), satIndex2satID(s_sat, sat), norm(deph, 3), dclk);
		*svh = -1;
		return 0;
	}
	if (!SatPosition(time, nav, sat, ssr->iode, rs, dts, var, svh)) return 0;
	if (!SatClockCorrection(time, nav, sat, ssr->iode, rs, dts)) return 0;
	sys = satIndex2sys(sat);

	//GLONASS not revise relativity correction
	if (opt >= 9)//com
	{
		if (sys == SYS_GPS || sys == SYS_GAL || sys == SYS_CMP)
			dts[0] += 2.0*dot_vector(rs, rs + 3, 3) / CLIGHT / CLIGHT;
		opt -= 10;
	}
	if (!normv3(rs + 3, ea)) return 0;
	cross3(rs, rs + 3, rc);
	if (!normv3(rc, ec)) {
		*svh = -1;
		return 0;
	}
	cross3(ea, ec, er);

	if (opt)
	{
		if (nav->pcvs[sat - 1].sat == 0)
			return 0;
		/* satellite antenna offset correction */
		satantoff(time, rs, sat, nav, dant);
	}
	if (opt == -1)//����apc->com���
	{
		for (i = 0; i < 3; i++) dant[i] = -dant[i];
	}
	for (i = 0; i<3; i++) {
		rs[i] += -(er[i] * deph[0] + ea[i] * deph[1] + ec[i] * deph[2]) + dant[i];
	}
	/* t_corr = t_sv - (dts(brdc) + dclk(ssr) / CLIGHT) (ref [10] eq.3.12-7) */

	dts[0] += dclk / CLIGHT;
	/* variance by ura ssr (ref [4]) ---------------------------------------------*/
	*var = var_urassr(ssr->ura);
	return 1;
}


//int codeBiasFromSsr(gtime_t time, const nav_t* nav, int sat, int codetype, double* cbias)
//{
//	const ssr_t* ssr;
//	double dt;
//	int i, udi;
//	ssr = nav->ssr + sat - 1;
//
//	if (ssr->t0[4].time == 0)
//	{
//		return 0;
//	}
//
//	dt = timediff(time, ssr->t0[4]);
//	udi = ssr->udi[4];
//
//	if (fabs(dt - udi) > MAXAGESSR_CBIAS)
//	{
//		printf("age of ssr cbias error: %s sat=%2d t=%.0f\n", time_str(time, 2),
//			sat, dt);
//		return 0;
//	}
//
//	for (i = 0; i < MAXCODE; i++)
//	{
//		if (ssr->codes[i] == codetype) break;
//	}
//
//	if (i >= MAXCODE) return 0;
//
//	*cbias = ssr->cbias[i];
//	return 1;
//}

int searchSsrDcb(gtime_t time, int sat, int codetype1, int codetype2, ssr_t *ssr,double* cbias)
{
	double dt;
	int i, udi, sys;
	double bias1, bias2;

	if (ssr->t0[4].time == 0)
	{
		return 0;
	}

	dt = timediff(time, ssr->t0[4]);
	udi = ssr->udi[4];
	
	/*if (fabs(dt - udi) > MAXAGESSR_CBIAS)
	{
		printf("age of ssr cbias error: %s sat=%2d t=%.0f\n", time_str(time, 2),
			sat, dt);
		return 0;
	}*/

	for (i = 0; i < MAXCODE; i++)
	{
		if (ssr->codes[i] == codetype1) break;
	}
	if (i >= MAXCODE) return 0;
	bias1 = ssr->cbias[i];

	for (i = 0; i < MAXCODE; i++)
	{
		if (ssr->codes[i] == codetype2) break;
	}
	if (i >= MAXCODE) return 0;
	bias2 = ssr->cbias[i];

	*cbias = bias2 - bias1;
	return 1;
}

//flag=0: DCB for codetype and same frequency (e.g. L1C-L1W, L2C-L2W, L2X-L2W)
//flag=1: L1W-L2W DCB (if GPS) L1P-L2P (if GLONASS) L1X-L5X (if GALILEO)
int codeBiasFromSsr(gtime_t time, const nav_t* nav, int sat, int codetype, double* cbias, int flag)
{
	const ssr_t* ssr;
	int sys;
	ssr = nav->ssr + sat - 1;

	sys = satIndex2sys(sat);

	switch (sys)
	{
	case SYS_GPS:
	{
		if (flag==1)//P1P2
		{
			return searchSsrDcb(time, sat, CODE_L1W, CODE_L2W, ssr, cbias);
		}
		else
		{
			switch (codetype)
			{
			case CODE_L1C:
				return searchSsrDcb(time, sat, CODE_L1W,codetype, ssr, cbias);
			case CODE_L1P:
			case CODE_L1W:
				return 1;
			case CODE_L2C:
			case CODE_L2D:
			case CODE_L2L:
			case CODE_L2S:
			case CODE_L2X:
			case CODE_L2W:
				return searchSsrDcb(time, sat, CODE_L2W, codetype, ssr, cbias);
			}
		}
	}

	case SYS_GLO:
	{
		if (flag == 1)//P1P2
		{
			return searchSsrDcb(time, sat, CODE_L1P, CODE_L2P, ssr, cbias);
		}
		else
		{
			switch (codetype)
			{
			case CODE_L1C:
				return searchSsrDcb(time, sat,  CODE_L1P, codetype, ssr, cbias);
			case CODE_L2C:
				return searchSsrDcb(time, sat,  CODE_L2P, codetype, ssr, cbias);
			}
		}
	}

	case SYS_GAL:
	{
		if (flag == 1)//P1P2
		{
			return searchSsrDcb(time, sat, CODE_L1X, CODE_L5X, ssr, cbias);
		}
		else
		{//保留
			return 0;
		}
	}
	default:
		return 0;

	}
	return 1;
}


double interppol(const double *x, const double *y, int n) {//  polynomial interpolation change to Lagrange method
	int			i, j;
	double		yEst = 0, prod;

	// Perform the interpolation
	for (i = 0; i<n; i++) {
		prod = 1;
		for (j = 0; j<n; j++) {
			if (i != j) {
				prod *= (x[j] / (x[j] - x[i]));
			}
		}
		prod *= y[i];
		yEst += prod;
	}

	return yEst;
}

int pephpos(gtime_t time, int sat, const nav_t *nav, double *rs,
	double *dts, double *vare, double *varc)
{
	double t[NMAX + 1], p[3][NMAX + 1], c[2], *pos, std = 0.0, s[3], sinl, cosl;
	int i, j, k, index;
	char id[16];

	satIndex2satID(id, sat);

	rs[0] = rs[1] = rs[2] = dts[0] = 0.0;

	if (nav->ne<NMAX + 1 ||
		timediff(time, nav->peph[0].time)<-MAXDTE ||
		timediff(time, nav->peph[nav->ne - 1].time)>MAXDTE) {
		return 0;
	}
	/* binary search */
	for (i = 0, j = nav->ne - 1; i<j;) {
		k = (i + j) / 2;
		if (timediff(nav->peph[k].time, time)<0.0) i = k + 1; else j = k;
	}
	index = i <= 0 ? 0 : i - 1;

	/* polynomial interpolation for orbit */
	i = index - (NMAX + 1) / 2;
	if (i<0) i = 0; else if (i + NMAX >= nav->ne) i = nav->ne - NMAX - 1;

	for (j = 0; j <= NMAX; j++) {
		t[j] = timediff(nav->peph[i + j].time, time);
		if (norm(nav->peph[i + j].pos[sat - 1], 3) <= 0.0) {
			return 0;
		}
	}
	for (j = 0; j <= NMAX; j++) {
		pos = nav->peph[i + j].pos[sat - 1];
#if 0
		p[0][j] = pos[0];
		p[1][j] = pos[1];
#else
		/* correciton for earh rotation ver.2.4.0 */
		sinl = sin(OM_E_GPS*t[j]);
		cosl = cos(OM_E_GPS*t[j]);
		p[0][j] = cosl*pos[0] - sinl*pos[1];//sat pos at signal emission time
		p[1][j] = sinl*pos[0] + cosl*pos[1];
#endif
		p[2][j] = pos[2];
	}
	for (i = 0; i<3; i++) {
		rs[i] = interppol(t, p[i], NMAX + 1);
	}
	if (vare) {
		for (i = 0; i<3; i++) s[i] = nav->peph[index].std[sat - 1][i];
		std = norm(s, 3);

		/* extrapolation error for orbit */
		if (t[0]>0.0) std += EXTERR_EPH*SQR(t[0]) / 2.0;
		else if (t[NMAX]<0.0) std += EXTERR_EPH*SQR(t[NMAX]) / 2.0;
		*vare = SQR(std);
	}
	/* linear interpolation for clock */
	t[0] = timediff(time, nav->peph[index].time);
	t[1] = timediff(time, nav->peph[index + 1].time);
	c[0] = nav->peph[index].pos[sat - 1][3];
	c[1] = nav->peph[index + 1].pos[sat - 1][3];

	if (t[0] <= 0.0) {
		if ((dts[0] = c[0]) != 0.0) {
			std = nav->peph[index].std[sat - 1][3] * CLIGHT - EXTERR_CLK*t[0];
		}
	}
	else if (t[1] >= 0.0) {
		if ((dts[0] = c[1]) != 0.0) {
			std = nav->peph[index + 1].std[sat - 1][3] * CLIGHT + EXTERR_CLK*t[1];
		}
	}
	else if (c[0] != 0.0&&c[1] != 0.0) {
		dts[0] = (c[1] * t[0] - c[0] * t[1]) / (t[0] - t[1]);
		i = t[0]<-t[1] ? 0 : 1;
		std = nav->peph[index + i].std[sat - 1][3] + EXTERR_CLK*fabs(t[i]);
	}
	else {
		dts[0] = 0.0;
	}
	if (varc) *varc = SQR(std);
	return 1;
}
/* satellite clock by precise clock ------------------------------------------*/
int pephclk(gtime_t time, int sat, const nav_t *nav, double *dts,
	double *varc)
{
	double t[2], c[2], std;
	int i, j, k, index;


	if (nav->nc<2 ||
		timediff(time, nav->pclk[0].time)<-MAXDTE ||
		timediff(time, nav->pclk[nav->nc - 1].time)>MAXDTE) {
		return 1;
	}
	/* binary search */
	for (i = 0, j = nav->nc - 1; i<j;) {
		k = (i + j) / 2;
		if (timediff(nav->pclk[k].time, time)<0.0) i = k + 1; else j = k;
	}
	index = i <= 0 ? 0 : i - 1;

	/* linear interpolation for clock */
	t[0] = timediff(time, nav->pclk[index].time);
	t[1] = timediff(time, nav->pclk[index + 1].time);
	c[0] = nav->pclk[index].clk[sat - 1][0];
	c[1] = nav->pclk[index + 1].clk[sat - 1][0];

	if (t[0] <= 0.0) {
		if ((dts[0] = c[0]) == 0.0) return 0;
		std = nav->pclk[index].std[sat - 1][0] * CLIGHT - EXTERR_CLK*t[0];
	}
	else if (t[1] >= 0.0) {
		if ((dts[0] = c[1]) == 0.0) return 0;
		std = nav->pclk[index + 1].std[sat - 1][0] * CLIGHT + EXTERR_CLK*t[1];
	}
	else if (c[0] != 0.0&&c[1] != 0.0) {
		dts[0] = (c[1] * t[0] - c[0] * t[1]) / (t[0] - t[1]);
		i = t[0]<-t[1] ? 0 : 1;
		std = nav->pclk[index + i].std[sat - 1][0] * CLIGHT + EXTERR_CLK*fabs(t[i]);
	}
	else {
		return 0;
	}
	if (varc) *varc = SQR(std);
	return 1;
}

int PreSatPosClk(gtime_t time, nav_t *nav, int sat, double *rs, double *dts, double *var)
{

	double rss[3], rst[3], dtss[1], dtst[1], dant[3] = { 0 }, vare = 0.0, varc = 0.0, tt = 1E-3;
	int i;

	if (sat <= 0 || MAXSAT<sat) return 0;

	/* satellite position and clock bias */
	if (!pephpos(time, sat, nav, rss, dtss, &vare, &varc) ||
		!pephclk(time, sat, nav, dtss, &varc)) return 0;

	time = timeadd(time, tt);
	if (!pephpos(time, sat, nav, rst, dtst, NULL, NULL) ||
		!pephclk(time, sat, nav, dtst, NULL)) return 0;

	/* satellite antenna offset correction */
	satantoff(time, rss, sat, nav, dant);

	for (i = 0; i<3; i++) {
		rs[i] = rss[i] + dant[i];
		rs[i + 3] = (rst[i] - rss[i]) / tt;
	}
	/* relativistic effect correction */
	if (dtss[0] != 0.0) {
		dts[0] = dtss[0] - 2.0*dot_vector(rs, rs + 3, 3) / CLIGHT / CLIGHT;
		dts[1] = (dtst[0] - dtss[0]) / tt;
	}
	else { /* no precise clock */
		dts[0] = dts[1] = 0.0;
	}
	if (var) *var = vare + varc;


	return 1;
}


int PPPSatPosClk(gtime_t time, nav_t *nav, int sat, double *rs, double *dts, double *var, int *svh, int ephopt)
{
	*svh = 0;
	switch (ephopt) {
	case EPHOPT_BRDC:return SatPosition(time, nav, sat, -1, rs, dts, var, svh);
	case EPHOPT_PREC:return PreSatPosClk(time, nav, sat, rs, dts, var);
	case EPHOPT_SSRAPC: return brdcPLUSssr(time, nav, sat, rs, dts, var, svh, 0);
	case EPHOPT_SSRCOM: return brdcPLUSssr(time, nav, sat, rs, dts, var, svh, 1);
	}
	*svh = -1;
	return 0;
}

/*******************��������λ��**********************/
//gtime_t time                    gps time
//nav_t *nav                        ��������
//obsd_t *obs       �۲�����
//int ephopt                        ��������
//double *rs                        rs��6*n����λ�ú��ٶ�
//double *drs                      drs��2*n�����Ӳ���ٶ�
//double *var                      var,1*n�������
int SatPosClk(gtime_t time, nav_t *nav, obsd_t *obs, int n, int ephopt, double *rs, double *dts, double *var, int *svh)
{
	int i, j, sat, info = 0;
	double pseudoRange;
	gtime_t *EmissionTime = (gtime_t *)malloc(sizeof(gtime_t)*n);

	for (i = 0; i < n&&i < MAXOBS; i++) {
		sat = obs[i].sat;
		for (j = 0; j < 6; j++) rs[j + i * 6] = 0.0;
		for (j = 0; j < 2; j++) dts[j + i * 2] = 0.0;
		var[i] = 0.0; svh[i] = 0;

		for (j = 0, pseudoRange = 0.0; j < NFREQ; j++)//i++ error,revise by pwj 2018.3.28
		{
			if ((pseudoRange = obs[i].P[j]) != 0)
				break;
		}
		EmissionTime[i] = timeadd(time, -pseudoRange / CLIGHT);
		SatClockCorrection(EmissionTime[i], nav, sat, -1, rs + i * 6, dts + i * 2);
		EmissionTime[i] = timeadd(EmissionTime[i], -dts[i * 2]);
		info += PPPSatPosClk(EmissionTime[i], nav, sat, rs + i * 6, dts + i * 2, var + i, svh + i, ephopt);
	}
	free(EmissionTime);
	return info >= 4 ? 1 : 0;
}
