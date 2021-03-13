#pragma once
#ifndef INPUT_H
#define INPUT_H
#include "pwjppp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NUMSYS      6                   /* number of systems */
#define DTTOL       0.005               /* tolerance of time difference (s) */
#define MAXDTOE     7200.0              /* max time difference to ephem Toe (s) for GPS */
#define MAXDTOE_GLO 1800.0              /* max time difference to GLONASS Toe (s) */
#define NINCOBS     262144              /* inclimental number of obs data */


	/***************read navigation file***************/
	int cmpeph(const void *p1, const void *p2);
	int cmpgeph(const void *p1, const void *p2);
	int cmpseph(const void *p1, const void *p2);
	void uniqeph(nav_t *nav);
	void uniqgeph(nav_t *nav);
	void uniqseph(nav_t *nav);
	int uraindex(double value);
	double GetSatWaveLength(int sat, int frq, nav_t *nav);
	void uniqnav(nav_t *nav);


	void redeph(nav_t *nav);
	void redgeph(nav_t *nav);
	void redseph(nav_t *nav);
	void rednav(nav_t *nav);

	int addeph(nav_t *nav, eph_t *eph);
	int addgeph(nav_t *nav, geph_t *geph);
	int addseph(nav_t *nav, seph_t *seph);

	int navcpy(nav_t *nav1, nav_t *nav2);

	int decode_gloeph(double ver, int sat, gtime_t toc, double *data, geph_t *gloeph);
	int decode_sbaseph(double ver, int sat, gtime_t toc, double *data, seph_t *seph);
	int decode_eph(double ver, int sat, gtime_t toc, double *data, eph_t *eph_t);

	int ReadNAVHead(FILE *fp, nav_t *nav, double *version, int *sys);
	int ReadNAVData(FILE *fp, nav_t *nav, double version, int sys);
	int ReadNAVFile(char *file, nav_t *nav);
	void releaseNAV(nav_t *nav);
	/***************read navigation file***************/




	/***************read observation file***************/
	int cmpobs(const void *p1, const void *p2);
	int sortobs(obs_t *obs);
	void Rinex2toRinex3(double ver, int sys, char* str, char* type);

	unsigned char obs2code(const char *obs, int *freq);
	const char* code2obs(unsigned char code, int *frq);

	int obs2index(double ver, int sys, const unsigned char *code,
		const char *tobs, const char *mask);

	int getcodepri(int system, unsigned char code);
	int shpData(int base, int n, char *obsType, dataIndex *index);
	int allocOBSMemory(obs_t *obs);
	int satsys2num(int sys);
	int syscodesIndex(char sys);
	int ReadOBSHead(FILE *fp, obs_t *obs, sta_t *sta, double *rinexVersion, int *tsys, char *obs_Type, char(*obsType)[MAXOBSTYPE][4], int *obsNum, int *mulObsNum);
	int ReadOBSData(FILE *fp, obs_t *obs, double *rinexVersion, int tsys, char *obs_Type, char(*obsType)[MAXOBSTYPE][4], int *obsNum, int *mulObsNum);
	int ReadOBSFile(char *file, obs_t *obs, sta_t *sta);
	int addobsdata(obs_t *obs, const obsd_t *data);
	void releaseOBS(obs_t *obs);
	/***************read observation file***************/



	/***************read sp3,clk file***************/
	int ReadSP3File(char *file, nav_t *nav);
	int ReadSP3Head(FILE *fp, nav_t *nav, char *type, double *bfact, char *tsys, int *sats, int *ns);
	int ReadSP3Data(FILE *fp, nav_t *nav, char type, double *bfact, char *tsys, int ns);
	int addsp3(nav_t *nav, peph_t *peph);

	int ReadCLKFile(char *file, nav_t *nav, prcopt_t *opt);
	double ReadCLKHead(FILE *fp);
	int ReadCLKData(FILE *fp, nav_t *nav, prcopt_t *opt, double ver);
	int addclk(nav_t *nav, pclk_t *pclk);
	int cmppclk(const void *p1, const void *p2);
	void combpclk(nav_t *nav);

	void releaseSP3CLK(nav_t *nav);
	/***************read sp3,clk file***************/

	int readDCBFile(const char *file, nav_t *nav);
	int readIFCBFile(const char *fileifcb);
	void setstr(char* dst, const char* src, int n);




	int readGimFile(const char *file, nav_t *nav);

#ifdef __cplusplus
}
#endif

#endif // INPUT_H
