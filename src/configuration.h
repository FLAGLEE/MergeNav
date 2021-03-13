#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "pwjppp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SWTOPT        "0:off,1:on"
#define MODOPT        "1:single,2:ppp-fixed,3:ppp-kinematic,4:ppp-static"
#define FRQOPT        "0:L1/B1(C2),1:L2/B2(C7),2:L5/B3(C6),3:L1L2/B1B2,4:L1L5/B1B3,5:L2L5/B2B3,6:L1L2L5/B1B2B3"
#define SYSOPT        "1:GPS,2:GLONASS,4:GALILEO,8:COMPASS,16:SBAS"
#define IONOPT        "0:off,1:brdc,2:dual-freq,3:est-stec,4:est-stec+constraint5:ionex-tec"
#define TRPOPT        "0:off,1:saas,2:sbas,3:est-ztd,4:est-ztdgrad,5:est-ztd+constraint"
#define EPHOPT        "0:brdc,1:precise,2:brdc+ssrapc,3:brdc+ssrcom"
#define SOLOPT        "0:llh,1:xyz,2:enu,3:nmea"
#define TSYOPT        "0:gpst,1:utc,2:jst"
#define TFTOPT        "0:tow,1:hms"
#define DFTOPT        "0:deg,1:dms"
#define ARMOPT        "0:off,1:fix-and-hold"
#define HGTOPT        "0:ellipsoidal,1:geodetic"
#define GEOOPT        "0:internal,1:egm96,2:egm08_2.5,3:egm08_1,4:gsi2000"
#define STAOPT        "0:all,1:single"
#define STSOPT        "0:off,1:state,2:residual"
#define POSOPT        "0:llh,1:xyz,2:single,3:posfile,4:rinexhead,5:rtcm"
#define TIDEOPT       "0:off,1:solid,2:solid+otl+pole"
#define KALOPT        "0:kalman,1:Rubust-kalman,2:A-kalman"
#define DOPOPT        "0:off,1:state,2:observation"
#define ADAOPT        "0:off,1:two,2:three"
#define DCBOPT        "0:off,1:tgd,2:dcb,3:ssr"
#define GAROPT        "0:off,1:on,2:autocal"

void chop(char *str);
int enum2str(char *s, const char *comment, int val);
int str2enum(const char *str, const char *comment, int *val);
opt_t *searchopt(const char *name, const opt_t *opts);
int str2opt(opt_t *opt, const char *str);
int opt2str(const opt_t *opt, char *str);
int opt2buf(const opt_t *opt, char *buff);
int loadopts(const char *file, opt_t *opts);
int saveopts(const char *file, const char *mode, const char *comment, const opt_t *opts);
void buff2sysopts(void);
void sysopts2buff(void);
void resetsysopts(void);
void getsysopts(prcopt_t *popt, solopt_t *sopt, filopt_t *fopt);
void setsysopts(const prcopt_t *prcopt, const solopt_t *solopt, const filopt_t *filopt);
void readconf(const char *dir, prcopt_t *prcopt, solopt_t *solopt, filopt_t *filopt);
void creatconf(const char *dir);

void getConfiguration(prcopt_t *prcopt, solopt_t *solopt, filopt_t *filopt);

#ifdef __cplusplus
}
#endif
#endif // CONFIGURATION_H
