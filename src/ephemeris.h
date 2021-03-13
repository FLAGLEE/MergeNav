#ifndef EPHEMERIS_H
#define EPHEMERIS_H

#include "pwjppp.h"
#ifdef __cplusplus
extern "C" {
#endif

	double var_uraeph(int ura);
	double var_urassr(int ura);

	/*GPS GAL BDS SBAS       BRDC*/
	eph_t *selectEphememeris(gtime_t time, const nav_t *nav, int sat, int iode);
	int SatPosition(gtime_t time, const nav_t *nav, int sat, int iode, double *rs, double *dts, double *var, int *svh);
	int BRDC_SatPosition(gtime_t emissionTime, eph_t *eph_t, double *pos, double *clk, double *var);

	/*GLONASS                      BRDC*/
	void derivativeGloOrbitModel(const double *x, const double *vel, double *xdot, const double *acc);
	void gloorbit(double t, double *x, double *v, double *a);
	geph_t *selectGlonassEphememeris(gtime_t time, const nav_t *nav, int sat, int iode);
	int BRDC_GlonassSatPosition(gtime_t emissionTime, geph_t *geph_t, double *pos, double *clk, double *var);

	int SatClockCorrection(gtime_t time, const nav_t *nav, int sat, int iode, double *rs, double *dts);

	int searchSsrDcb(gtime_t time, int sat, int codetype1, int codetype2,ssr_t *ssr, double* cbias);
	int codeBiasFromSsr(gtime_t time, const nav_t* nav, int sat, int codetype, double* cbias, int flag);

	/*precise ephemeris*/
	double interppol(const double *x, const double *y, int n);
	int pephpos(gtime_t time, int sat, const nav_t *nav, double *rs, double *dts, double *vare, double *varc);
	int pephclk(gtime_t time, int sat, const nav_t *nav, double *dts, double *varc);
	int PreSatPosClk(gtime_t time, nav_t *nav, int sat, double *rs, double *dts, double *var);


	//ssr
	int brdcPLUSssr(gtime_t time, const nav_t *nav, int sat, double *rs, double *dts, double *var, int *svh, int opt);


	//ÐÇÀú¼ÆËã£¬opt: ephopt
	int PPPSatPosClk(gtime_t time, nav_t *nav, int sat, double *rs, double *dts, double *var, int *svh, int ephopt);
	int SatPosClk(gtime_t time, nav_t *nav, obsd_t *obs, int n, int ephopt, double *rs, double *dts, double *var, int *svh);

#ifdef __cplusplus
}
#endif

#endif // EPHEMERIS_H
