#ifndef PWJPPP_H
#define PWJPPP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <io.h>
#ifdef WIN32
#include<WinSock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#else
#include <pthread.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif

#define ENAGPS   1
#define ENAGLO   1
#define ENACMP   1
#define ENAGAL   1
#define ENAQZS   0
#define ENASBS   0

#define versionNum  "1.0"
#define SQR(x)      ((x)*(x))
#define ROUND(x)  (int)floor((x)+0.5)
#define MAXCHARNUM 1024

#define	NLINE_LENGTH   82
#define	OLINE_LENGTH   82
#define	GM_EARTH       3.986005000e14	  /*������������*/
#define	GM_SOLAR       1.32712440e20	  /*̫����������*/
#define GM_LUNA        4.9027993e12		  /*������������*/
#define MU_GPS         3.986005000e14     /*ֻ�ǻ��˸����֣���GM_EARTHһ�� Gravitational constant*/
#define MU_GLO         3.986004400e14
#define MU_GAL         3.986004418e14
#define MU_CMP         3.986004418e14
#define OM_E_GPS       7.2921151467e-5    /*Earth's angular velocity*/
#define OM_E_GLO       7.2921150000e-5
#define OM_E_GAL       7.2921151467e-5
#define OM_E_CMP       7.2921150000e-5
#define J2_GLO         1.0826257e-3       /*Second zonal coefficient of spherical harmonic expression*/



#define RE_GLO         6378136.00               /*GLONASS��������a*/
#define	A_WGS84        6378137.00		        /*WGS-84��������a (m)*/
#define	B_WGS84        6356752.3142		        /*WGS-84��������b (m)*/
#define	C_WGS84        6399593.6258		        /*WGS-84��������c (m)*/
#define	IB_WGS84       (1 / 298.257223563)	    /*WGS-84��������ib (m)*/
#define	EE_WGS84       (IB_WGS84*(2-IB_WGS84))	/*WGS-84��������eƽ�� (m)*/
#define EEP_WGS84      0.00673949674227         /*WGS-84��������ep ƽ�� (m)*/


#define HION           350000.0              /* �������߶�(m) */

	/* constants connectNtrip -----------------------------------------------------------------*/

#define AU             149597870691.0     
#define PI             3.14159265358979324    
#define D2R            (PI / 180.0)           /*degree2radian*/
#define R2D            (180.0 / PI)           /*radian2degree*/
#define CLIGHT         299792458.0          /*m/s*/
#define SC2RAD         3.1415926535898     /* semi-circle to radian (IS-GPS) */
#define OMGE           7.2921151467E-5     /* earth angular velocity (IS-GPS) (rad/s) */

#define MAXSTRPATH  1024
#define MAXSTRMSG   1024                /* max length of stream message */
#define MAXCOMMENT  10                  /* max number of RINEX comments */
#define MAXSBSAGEF  30.0                /* max age of SBAS fast correction (s) */
#define MAXSBSAGEL  1800.0              /* max age of SBAS long term corr (s) */
#define MAXSBSURA   8                   /* max URA of SBAS satellite */
#define MAXBAND     10                  /* max SBAS band of IGP */
#define MAXNIGP     201                 /* max number of IGP in SBAS band */
#define MAXSOLMSG   4096                /* max length of solution message */
#define MAXRAWLEN   4096                /* max length of receiver raw message *

#define TINTACT             200         /* period for stream active (ms) */
#define SERIBUFFSIZE        4096        /* serial buffer size (bytes) */
#define TIMETAGH_LEN        64          /* time tag file header length */
#define MAXCLI              32          /* max client connection for tcp svr */
#define MAXSTATMSG          32          /* max length of status message */

#define FREQ1          1.57542e9            /* L1/E1  frequency (Hz) */
#define FREQ2          1.22760e9            /* L2     frequency (Hz) */
#define FREQ5          1.17645e9            /* L5/E5a frequency (Hz) */
#define FREQ6          1.27875e9            /* E6/LEX frequency (Hz) */
#define FREQ7          1.20714e9            /* E5b    frequency (Hz) */
#define FREQ8          1.191795e9           /* E5a+b  frequency (Hz) */
#define FREQ1_GLO      1.60200e9        /* GLONASS G1 base frequency (Hz) */
#define DFRQ1_GLO      0.56250e6        /* GLONASS G1 bias frequency (Hz/n) */
#define FREQ2_GLO      1.24600e9        /* GLONASS G2 base frequency (Hz) */
#define DFRQ2_GLO      0.43750e6        /* GLONASS G2 bias frequency (Hz/n) */
#define FREQ3_GLO      1.202025e9       /* GLONASS G3 frequency (Hz) */
#define FREQ1_CMP      1.561098e9       /* BeiDou B1(_2) frequency (Hz) */
#define FREQ2_CMP      1.20714e9        /* BeiDou B2(_7)frequency (Hz) */
#define FREQ3_CMP      1.26852e9        /* BeiDou B3(_6) frequency (Hz) */


#define MAXFREQ        7//7
#define NFREQ          3//6
#define MAXOBSTYPE     64
#define MAXOBS         64

#if ENAGPS
#define MINPRNGPS    1
#define MAXPRNGPS    32
#define NSATGPS      32
#define NSYSGPS      1
#else
#define MINPRNGPS    0
#define MAXPRNGPS    0
#define NSATGPS      0
#define NSYSGPS      0
#endif

#if ENAGLO
#define MINPRNGLO    1
#define MAXPRNGLO    27
#define NSATGLO      27
#define NSYSGLO      1
#else
#define MINPRNGLO    0
#define MAXPRNGLO    0
#define NSATGLO      0
#define NSYSGLO      0
#endif

#if ENACMP
#define MINPRNCMP    1
#define MAXPRNCMP    61
#define NSATCMP      61
#define NSYSCMP      1
#else
#define MINPRNCMP    0
#define MAXPRNCMP    0
#define NSATCMP      0
#define NSYSCMP      0
#endif

#if ENAGAL
#define MINPRNGAL    1
#define MAXPRNGAL    36   //27->30 edit by pwj 2018.4.24
#define NSATGAL      36   //30->36 edit by pwj 2020.2.20
#define NSYSGAL      1
#else
#define MINPRNGAL    0
#define MAXPRNGAL    0   //27->30 edit by pwj 2018.4.24
#define NSATGAL      0
#define NSYSGAL      0
#endif

#if ENAQZS
#define MINPRNQZS    193                 /* min satellite PRN number of QZSS */
#define MAXPRNQZS    199                 /* max satellite PRN number of QZSS */
#define MINPRNQZS_S  183                 /* min satellite PRN number of QZSS SAIF */
#define MAXPRNQZS_S  189                 /* max satellite PRN number of QZSS SAIF */
#define NSATQZS      (MAXPRNQZS-MINPRNQZS+1) /* number of QZSS satellites */
#define NSYSQZS      1
#else
#define MINPRNQZS    0
#define MAXPRNQZS    0
#define MINPRNQZS_S  0
#define MAXPRNQZS_S  0
#define NSATQZS      0
#define NSYSQZS      0
#endif

#if ENASBS
#define MINPRNSBS    120
#define MAXPRNSBS    158
#define NSATSBS      (MAXPRNSBS-MINPRNSBS+1)
#define NSYSSBS      1
#else
#define MINPRNSBS    0
#define MAXPRNSBS    0
#define NSATSBS      0
#define NSYSSBS      0
#endif


#define NSYS         (NSYSGPS+NSYSGLO+NSYSCMP+NSYSGAL+NSYSSBS+NSYSQZS)
#define MAXSAT       (NSATGPS+NSATGLO+NSATCMP+NSATGAL+NSATSBS)


#define EFACT_GPS   1.0                 /* error factor: GPS */
#define EFACT_GLO   1.5                 /* error factor: GLONASS */
#define EFACT_GAL   1.0                 /* error factor: Galileo */
#define EFACT_QZS   1.0                 /* error factor: QZSS */
#define EFACT_CMP   2.0                 /* error factor: BeiDou */
#define EFACT_SBS   3.0                 /* error factor: SBAS */

#define SYS_NONE       0x00
#define SYS_GPS        0x01
#define SYS_GLO        0x02
#define SYS_GAL        0x04
#define SYS_CMP        0x08
#define SYS_SBS        0x10
#define SYS_QZS        0x20
#define SYS_ALL        0xFF

#define TSYS_GPS       0
#define TSYS_UTC       1
#define TSYS_GLO       2
#define TSYS_GAL       3
#define TSYS_CMP       4

#define MAXANT      64                  /* max length of station name/antenna type */
#define MAXRNXLEN   (16*MAXOBSTYPE+4)   /* max rinex record length */
#define DTTOL       0.005               /* tolerance of time difference (s) */
#define MAXTECLAYER 4                   /* max SSR VTEC layer*/
#define MAXTECCOEFF 289
#define MAXRCVCMD   4096                /* max length of receiver commands */


#define OBSTYPE_PR  0x01                /* observation type: pseudorange */
#define OBSTYPE_CP  0x02                /* observation type: carrier-phase */
#define OBSTYPE_DOP 0x04                /* observation type: doppler-freq */
#define OBSTYPE_SNR 0x08                /* observation type: SNR */
#define OBSTYPE_ALL 0xFF                /* observation type: all */

#define FREQTYPE_L1 0x01                /* frequency type: L1/E1 */
#define FREQTYPE_L2 0x02                /* frequency type: L2/B1 */
#define FREQTYPE_L5 0x04                /* frequency type: L5/E5a/L3 */
#define FREQTYPE_L6 0x08                /* frequency type: E6/LEX/B3 */
#define FREQTYPE_L7 0x10                /* frequency type: E5b/B2 */
#define FREQTYPE_L8 0x20                /* frequency type: E5(a+b) */
#define FREQTYPE_ALL 0xFF               /* frequency type: all */

#define CODE_NONE   0                   /* obs code: none or unknown */
#define CODE_L1C    1                   /* obs code: L1C/A,G1C/A,E1C (GPS,GLO,GAL,QZS,SBS) */
#define CODE_L1P    2                   /* obs code: L1P,G1P    (GPS,GLO) */
#define CODE_L1W    3                   /* obs code: L1 Z-track (GPS) */
#define CODE_L1Y    4                   /* obs code: L1Y        (GPS) */
#define CODE_L1M    5                   /* obs code: L1M        (GPS) */
#define CODE_L1N    6                   /* obs code: L1codeless (GPS) */
#define CODE_L1S    7                   /* obs code: L1C(D)     (GPS,QZS) */
#define CODE_L1L    8                   /* obs code: L1C(P)     (GPS,QZS) */
#define CODE_L1E    9                   /* obs code: L1-SAIF    (QZS) */
#define CODE_L1A    10                  /* obs code: E1A        (GAL) */
#define CODE_L1B    11                  /* obs code: E1B        (GAL) */
#define CODE_L1X    12                  /* obs code: E1B+C,L1C(D+P) (GAL,QZS) */
#define CODE_L1Z    13                  /* obs code: E1A+B+C,L1SAIF (GAL,QZS) */
#define CODE_L2C    14                  /* obs code: L2C/A,G1C/A (GPS,GLO) */
#define CODE_L2D    15                  /* obs code: L2 L1C/A-(P2-P1) (GPS) */
#define CODE_L2S    16                  /* obs code: L2C(M)     (GPS,QZS) */
#define CODE_L2L    17                  /* obs code: L2C(L)     (GPS,QZS) */
#define CODE_L2X    18                  /* obs code: L2C(M+L),B1I+Q (GPS,QZS,CMP) */
#define CODE_L2P    19                  /* obs code: L2P,G2P    (GPS,GLO) */
#define CODE_L2W    20                  /* obs code: L2 Z-track (GPS) */
#define CODE_L2Y    21                  /* obs code: L2Y        (GPS) */
#define CODE_L2M    22                  /* obs code: L2M        (GPS) */
#define CODE_L2N    23                  /* obs code: L2codeless (GPS) */
#define CODE_L5I    24                  /* obs code: L5/E5aI    (GPS,GAL,QZS,SBS) */
#define CODE_L5Q    25                  /* obs code: L5/E5aQ    (GPS,GAL,QZS,SBS) */
#define CODE_L5X    26                  /* obs code: L5/E5aI+Q  (GPS,GAL,QZS,SBS) */
#define CODE_L7I    27                  /* obs code: E5bI,B2I   (GAL,CMP) */
#define CODE_L7Q    28                  /* obs code: E5bQ,B2Q   (GAL,CMP) */
#define CODE_L7X    29                  /* obs code: E5bI+Q,B2I+Q (GAL,CMP) */
#define CODE_L6A    30                  /* obs code: E6A        (GAL) */
#define CODE_L6B    31                  /* obs code: E6B        (GAL) */
#define CODE_L6C    32                  /* obs code: E6C        (GAL) */
#define CODE_L6X    33                  /* obs code: E6B+C,LEXS+L,B3I+Q (GAL,QZS,CMP) */
#define CODE_L6Z    34                  /* obs code: E6A+B+C    (GAL) */
#define CODE_L6S    35                  /* obs code: LEXS       (QZS) */
#define CODE_L6L    36                  /* obs code: LEXL       (QZS) */
#define CODE_L8I    37                  /* obs code: E5(a+b)I   (GAL) */
#define CODE_L8Q    38                  /* obs code: E5(a+b)Q   (GAL) */
#define CODE_L8X    39                  /* obs code: E5(a+b)I+Q (GAL) */
#define CODE_L2I    40                  /* obs code: B1I        (CMP) */
#define CODE_L2Q    41                  /* obs code: B1Q        (CMP) */
#define CODE_L6I    42                  /* obs code: B3I        (CMP) */
#define CODE_L6Q    43                  /* obs code: B3Q        (CMP) */
#define CODE_L3I    44                  /* obs code: G3I        (GLO) */
#define CODE_L3Q    45                  /* obs code: G3Q        (GLO) */
#define CODE_L3X    46                  /* obs code: G3I+Q      (GLO) */
#define CODE_L1I    47                  /* obs code: B1I        (BDS) */
#define CODE_L1Q    48                  /* obs code: B1Q        (BDS) */
#define MAXCODE     48                  /* max number of obs code */

#define SD_NONE   0
#define SD_OBS      1
#define SD_BRDC    2
#define SD_SSR       3
#define SD_PRE       4

#define P2_5        0.03125             /* 2^-5 */
#define P2_6        0.015625            /* 2^-6 */
#define P2_11       4.882812500000000E-04 /* 2^-11 */
#define P2_15       3.051757812500000E-05 /* 2^-15 */
#define P2_17       7.629394531250000E-06 /* 2^-17 */
#define P2_19       1.907348632812500E-06 /* 2^-19 */
#define P2_20       9.536743164062500E-07 /* 2^-20 */
#define P2_21       4.768371582031250E-07 /* 2^-21 */
#define P2_23       1.192092895507810E-07 /* 2^-23 */
#define P2_24       5.960464477539063E-08 /* 2^-24 */
#define P2_27       7.450580596923828E-09 /* 2^-27 */
#define P2_29       1.862645149230957E-09 /* 2^-29 */
#define P2_30       9.313225746154785E-10 /* 2^-30 */
#define P2_31       4.656612873077393E-10 /* 2^-31 */
#define P2_32       2.328306436538696E-10 /* 2^-32 */
#define P2_33       1.164153218269348E-10 /* 2^-33 */
#define P2_35       2.910383045673370E-11 /* 2^-35 */
#define P2_38       3.637978807091710E-12 /* 2^-38 */
#define P2_39       1.818989403545856E-12 /* 2^-39 */
#define P2_40       9.094947017729280E-13 /* 2^-40 */
#define P2_43       1.136868377216160E-13 /* 2^-43 */
#define P2_48       3.552713678800501E-15 /* 2^-48 */
#define P2_50       8.881784197001252E-16 /* 2^-50 */
#define P2_55       2.775557561562891E-17 /* 2^-55 */

#define IONOOPT_OFF 0                   /* ionosphere option: correction off */
#define IONOOPT_BRDC 1                  /* ionosphere option: broadcast model */
#define IONOOPT_IFLC 2                  /* ionosphere option: L1/L2 or L1/L5 iono-free LC */
#define IONOOPT_SSR  3
#define IONOOPT_EST 4                   /* ionosphere option: estimation */
#define IONOOPT_CONS 5                   
#define IONOOPT_STEC 6                  /* ionosphere option: SLANT TEC model */


#define TROPOPT_OFF 0                   /* troposphere option: correction off */
#define TROPOPT_SAAS 1                  /* troposphere option: Saastamoinen model */
#define TROPOPT_SBAS 2                  /* troposphere option: SBAS model */
#define TROPOPT_EST 3                   /* troposphere option: ZTD estimation */
#define TROPOPT_ESTG 4                  /* troposphere option: ZTD+grad estimation */
#define TROPOPT_COR 5                   /* troposphere option: ZTD correction */
#define TROPOPT_CORG 6                  /* troposphere option: ZTD+grad correction */

#define EPHOPT_BRDC 0                   /* ephemeris option: broadcast ephemeris */
#define EPHOPT_PREC 1                   /* ephemeris option: precise ephemeris */
#define EPHOPT_SSRAPC 2                 /* ephemeris option: broadcast + SSR_APC */
#define EPHOPT_SSRCOM 3                 /* ephemeris option: broadcast + SSR_COM */
#define EPHOPT_SBAS   4

#define PMODE_SINGLE 1                  /* positioning mode: single */
#define PMODE_PPP_FIXED 2               /* positioning mode: PPP-coordinate fixed */
#define PMODE_PPP_KINEMA 3              /* positioning mode: PPP-kinemaric */
#define PMODE_PPP_STATIC 4              /* positioning mode: PPP-static */

#define SOLF_LLH    0                   /* solution format: lat/lon/height */
#define SOLF_XYZ    1                   /* solution format: x/y/z-ecef */
#define SOLF_ENU    2                   /* solution format: e/n/u-baseline */
#define SOLF_NMEA   3                   /* solution format: NMEA-183 */
#define SOLF_STAT   4                   /* solution format: solution status */
#define SOLF_GSIF   5                   /* solution format: GSI-F1/2/3 */

#define SOLQ_NONE   0                   /* solution status: no solution */
#define SOLQ_UBX    1                   /* solution status: fix */
#define SOLQ_SBAS   2                   /* solution status: SBAS */
#define SOLQ_SINGLE 3                   /* solution status: single */
#define SOLQ_PPP    4                   /* solution status: PPP */
#define MAXSOLQ     4                   /* max number of solution status */

#define STR_NONE     0                  /* stream type: none */
#define STR_SERIAL   1                  /* stream type: serial */
#define STR_FILE     2                  /* stream type: file */
#define STR_TCPSVR   3                  /* stream type: TCP server */
#define STR_TCPCLI   4                  /* stream type: TCP client */
#define STR_NTRIPSVR 6                  /* stream type: NTRIP server */
#define STR_NTRIPCLI 7                  /* stream type: NTRIP client */
#define STR_FTP      8                  /* stream type: ftp */
#define STR_HTTP     9                  /* stream type: http */
#define STR_NTRIPC_S 10                 /* stream type: NTRIP caster server */
#define STR_NTRIPC_C 11                 /* stream type: NTRIP caster client */
#define STR_UDPSVR   12                 /* stream type: UDP server */
#define STR_UDPCLI   13                 /* stream type: UDP server */
#define STR_MEMBUF   14                 /* stream type: memory buffer */

#define STRFMT_RTCM2 0                  /* stream format: RTCM 2 */
#define STRFMT_RTCM3 1                  /* stream format: RTCM 3 */
#define STRFMT_OEM4  2                  /* stream format: NovAtel OEMV/4 */
#define STRFMT_OEM3  3                  /* stream format: NovAtel OEM3 */
#define STRFMT_UBX   4                  /* stream format: u-blox LEA-*T */
#define STRFMT_SS2   5                  /* stream format: NovAtel Superstar II */
#define STRFMT_CRES  6                  /* stream format: Hemisphere */
#define STRFMT_STQ   7                  /* stream format: SkyTraq S1315F */
#define STRFMT_GW10  8                  /* stream format: Furuno GW10 */
#define STRFMT_JAVAD 9                  /* stream format: JAVAD GRIL/GREIS */
#define STRFMT_NVS   10                 /* stream format: NVS NVC08C */
#define STRFMT_BINEX 11                 /* stream format: BINEX */
#define STRFMT_RT17  12                 /* stream format: Trimble RT17 */
#define STRFMT_SEPT  13                 /* stream format: Septentrio */
#define STRFMT_CMR   14                 /* stream format: CMR/CMR+ */
#define STRFMT_TERSUS 15                /* stream format: TERSUS */
#define STRFMT_LEXR  16                 /* stream format: Furuno LPY-10000 */
#define STRFMT_RINEX 17                 /* stream format: RINEX */
#define STRFMT_SP3   18                 /* stream format: SP3 */
#define STRFMT_RNXCLK 19                /* stream format: RINEX CLK */
#define STRFMT_SBAS  20                 /* stream format: SBAS messages */
#define STRFMT_NMEA  21                 /* stream format: NMEA 0183 */
#ifndef EXTLEX
#define MAXRCVFMT    15                 /* max number of receiver format */
#else
#define MAXRCVFMT    16
#endif

#define STR_MODE_R  0x1                 /* stream mode: read */
#define STR_MODE_W  0x2                 /* stream mode: write */
#define STR_MODE_RW 0x3                 /* stream mode: read/write */

#define COMMENTH    "%"                 /* comment line indicator for solution */

#define GEOID_EMBEDDED    0             /* geoid model: embedded geoid */
#define GEOID_EGM96_M150  1             /* geoid model: EGM96 15x15" */
#define GEOID_EGM2008_M25 2             /* geoid model: EGM2008 2.5x2.5" */
#define GEOID_EGM2008_M10 3             /* geoid model: EGM2008 1.0x1.0" */
#define GEOID_GSI2000_M15 4             /* geoid model: GSI geoid 2000 1.0x1.5" */
#define GEOID_RAF09       5             /* geoid model: IGN RAF09 for France 1.5"x2" */


	/*ITRF2014->ITRF97-2010.0*/
#define ITRF_T1     0.0074
#define ITRF_T2    -0.0005
#define ITRF_T3    -0.0628
#define ITRF_D      0.0038
#define ITRF_R1     0.0000
#define ITRF_R2     0.0000
#define ITRF_R3     0.00026
#define ITRF_dT1    0.0001
#define ITRF_dT2   -0.0005
#define ITRF_dT3   -0.0033
#define ITRF_dD     0.00012
#define ITRF_dR1    0.0000
#define ITRF_dR2    0.0000
#define ITRF_dR3    0.00002

#define LLI_SLIP    0x01                /* LLI: cycle-slip */
#define LLI_HALFC   0x02                /* LLI: half-cycle not resovled */
#define LLI_BOCTRK  0x04                /* LLI: boc tracking of mboc signal */
#define LLI_HALFA   0x40                /* LLI: half-cycle added */
#define LLI_HALFS   0x80                /* LLI: half-cycle subtracted */

#define NTRIP_AGENT         "WHU/" versionNum
#define NTRIP_CLI_PORT      2101        /* default ntrip-client connection port */
#define NTRIP_SVR_PORT      80          /* default ntrip-server connection port */
#define NTRIP_MAXRSP        32768       /* max size of ntrip response */
#define NTRIP_MAXSTR        512         /* max length of mountpoint string */
#define NTRIP_RSP_OK_CLI    "ICY 200 OK\r\n" /* ntrip response: client */
#define NTRIP_RSP_OK_SVR    "OK\r\n"    /* ntrip response: server */
#define NTRIP_ENDL               "\r\n"    /* ntrip response: server */
#define NTRIP_RSP_SRCTBL    "SOURCETABLE 200 OK\r\n" /* ntrip response: source table */
#define NTRIP_RSP_TBLEND    "ENDSOURCETABLE"
#define NTRIP_RSP_HTTP      "HTTP/"     /* ntrip response: http */
#define NTRIP_RSP_ERROR     "ERROR"     /* ntrip response: error */

#define MAXSOLBUF   256                 /* max number of solution buffer */
#define MAXOBSBUF   10                 /* max number of observation data buffer */
#define MAXSBSMSG   32                  /* max number of SBAS msg in RTK server */
#define MAXSTRPPP   2                /* max number of stream in PPP server 8->102 edited by pwj 2020.08.03*/
#define MAXSTRINPPP 200                 /* max number of stream in PPP user,3->100 edited by pwj 2020.08.10 */

#define TIMES_GPST  0                   /* time system: gps time */
#define TIMES_UTC   1                   /* time system: utc */
#define TIMES_JST   2                   /* time system: jst */

//# define min(x,y) ((x)<(y)?(x):(y))
//# define max(x,y) ((x)<(y)?(y):(x))

#ifdef WIN32
#define thread_t    HANDLE
#define lock_t      CRITICAL_SECTION
#define initlock(f) InitializeCriticalSection(f)
#define lock(f)     EnterCriticalSection(f)
#define unlock(f)   LeaveCriticalSection(f)
#define FILEPATHSEP '\\'
#else
#define thread_t    pthread_t
#define lock_t      pthread_mutex_t
#define initlock(f) pthread_mutex_init(f,NULL)
#define lock(f)     pthread_mutex_lock(f)
#define unlock(f)   pthread_mutex_unlock(f)
#define FILEPATHSEP '/'
#endif

/* constants Time -----------------------------------------------------------------*/
typedef struct {
	time_t time;
	double sec;
}gtime_t;


typedef struct {        /* earth rotation parameter data type */
	double mjd;         /* mjd (days) */
	double xp, yp;       /* pole offset (rad) */
	double xpr, ypr;     /* pole offset rate (rad/day) */
	double ut1_utc;     /* ut1-utc (s) */
	double lod;         /* length of day (s/day) */
} erpd_t;

typedef struct {        /* earth rotation parameter type */
	int n, nmax;         /* number and max number of data */
	erpd_t *data;       /* earth rotation parameter data */
} erp_t;

typedef struct {        /* antenna parameter type */
	int sat;            /* satellite number (0:receiver) */
	char type[MAXANT];  /* antenna type */
	char code[MAXANT];  /* serial number or satellite code */
	gtime_t ts, te;      /* valid time start and end */
							//	double off[2][NFREQ][3]; /* phase center offset e/n/u or x/y/z (m) */
							//	double var[2][NFREQ][19]; /* phase center variation (m) */
	double off[NFREQ][3]; /* phase center offset e/n/u or x/y/z (m) */
	double var[NFREQ][19]; /* phase center variation (m) */
	double dazi;                  /* el=90,85,...,0 or nadir=0,1,2,3,... (deg) */
									//	double azivar[2][NFREQ][73][19];
} pcv_t;

typedef struct {
	int n, nmax;
	pcv_t *pcv;
}pcvs_t;

typedef struct {        /* option type */
	char *name;         /* option name */
	int format;         /* option format (0:int,1:double,2:string,3:enum) */
	void *var;          /* pointer to option variable */
	char *comment;      /* option comment/enum labels/unit */
} opt_t;

typedef struct {        /* SNR mask type */
	int ena[2];         /* enable flag {rover,base} */
	double mask[NFREQ][9]; /* mask (dBHz) at 5,10,...85 deg */
} snrmask_t;

typedef struct {
	int processRealtime;
	int processMode;
	int frequenceMode;
	int frequenceNumber;
	int navigationSystem;
	double elevation;
	snrmask_t snrMask;
	int satelliteEphemeris;
	int ionosphereOption;
	int troposphereOption;
	int modeAR;         /* AR mode (0:off,1:continuous,2:instantaneous,3:fix and hold,4:ppp-ar) */
	int glonassModeAR;      /* GLONASS AR mode (0:off,1:on,2:auto cal,3:ext cal) */
	int maxOut;         /* obs outage count to reset bias */
	int minLock;        /* min lock count to fix ambiguity */
	int minFix;         /* min fix count to hold ambiguity */
	int dynamics;       /* dynamics model (0:none,1:velociy,2:accel) */
	int tideCorrect;       /* earth tide correction (0:off,1:solid,2:solid+otl+pole) */
	int nIteration;          /* number of filter iteration */
	int codeSmooth;     /* code smoothing window size (0:none) */
	double err[5];
	double eratio[NFREQ]; /* code/phase error ratio */
	double std[3];      /* initial-state std [0]bias,[1]iono [2]trop */
	double processNoise[6];      /* process-noise std [0]bias,[1]iono [2]trop [3]acch [4]accv [5]DCB*/
	double prn_isb[4];	/* inter system bias process-noise std [0]glo-gps */
	double sclkstab;    /* satellite clock stability (sec/sec) */
	double thresholdAR[5];  /* AR validation threshold */
	double elevationAR;    /* elevation mask of AR for rising satellite (deg) */
	double elevationHoldAR;  /* elevation mask to hold ambiguity (deg) */
	double thresholdGFSlip;   /* slip threshold of geometry-free phase (m) */

	double thresholdWLSlip;   /* slip threshold of MW phase (m) default:2cycle*/

	double maxtdiff;    /* max difference of time (sec) */
	double maxinno;     /* reject threshold of innovation (m) */
	double maxgdop;     /* reject threshold of gdop */
	int rovpos;                  /*pos fixed mode blh,xyz..*/
	double fixPosition[3];       /* rover position for fixed mode {x,y,z} (ecef) (m) */
	char anttype[MAXANT]; /* antenna types {rover,base} */
	double antdel[3]; /* antenna delta {{rov_e,rov_n,rov_u},{ref_e,ref_n,ref_u}} */
	pcv_t pcvr;      /* receiver antenna parameters {rov,base} */
	unsigned char exsats[MAXSAT]; /* excluded satellites (1:excluded,2:included) */

	int  posopt[7];     /* positioning options */
	int  syncsol;       /* solution sync mode (0:off,1:on) */
	double odisp[6 * 11]; /* ocean tide loading parameters {rov} */
	int robust;
	int adapt;
	int doppler;
	int trajec;
	int  csopt[6];         // cycle slip options([0]cycle_LL,[1]cycle_GF,[2]cycle_MW,[3]cycle_TECR,[4]cycle_L1C1,[5]cycle_doppler)
	double prc_time[2][6]; // processing time {start} {end } (0: no limit)
	double interval;
	int dcbCorrect;     /*0:no,1:TGD,2:DCB file,3:ssr cbias*/
	int useglo;
	char pppopt[256];   /* ppp option */
	int sbassatsel;     /* SBAS satellite selection (0:all) */
}prcopt_t;

typedef struct {        /* solution options type */
	int posf;           /* solution format (SOLF_???) */
	int times;          /* time system (TIMES_???) */
	int timef;          /* time format (0:sssss.s,1:yyyy/mm/dd hh:mm:ss.s) */
	int timeu;          /* time digits under decimal point */
	int degf;           /* latitude/longitude format (0:ddd.ddd,1:ddd mm ss) */
	int outhead;        /* output header (0:no,1:yes) */
	int outopt;         /* output processing options (0:no,1:yes) */
	int outvel;
	int datum;          /* datum (0:WGS84,1:Tokyo) */
	int height;         /* height (0:ellipsoidal,1:geodetic) */
	int geoid;          /* geoid model (0:EGM96,1:JGD2000) */
	int solstatic;      /* solution of static mode (0:all,1:single) */
	int sstat;          /* solution statistics level (0:off,1:states,2:residuals) */
	int trace;          /* debug trace level (0:off,1-5:debug) */
	double nmeaintv[2]; /* nmea output interval (s) (<0:no,0:all) */
						/* nmeaintv[0]:gprmc,gpgga,nmeaintv[1]:gpgsv */
	char sep[64];       /* field separator */
	char prog[64];      /* program name */
	double maxsolstd;
} solopt_t;

typedef struct {        /* file options type */
	char satantp[MAXSTRPATH]; /* satellite antenna parameters file */
	char rcvantp[MAXSTRPATH]; /* receiver antenna parameters file */
	char ztd[MAXSTRPATH];
	char snx[MAXSTRPATH];
	char stapos[MAXSTRPATH]; /* station positions file */
	char geoid[MAXSTRPATH]; /* external geoid data file */
	char iono[MAXSTRPATH]; /* ionosphere data file */
	char dcb[MAXSTRPATH]; /* dcb data file */
	char eop[MAXSTRPATH]; /* eop data file */
	char blq[MAXSTRPATH]; /* ocean tide loading blq file */
	char tempdir[MAXSTRPATH]; /* ftp/http temporaly directory */
	char geexe[MAXSTRPATH]; /* google earth exec file */
	char solstat[MAXSTRPATH]; /* solution statistics file */
	char trace[MAXSTRPATH]; /* debug trace file */

	char rnxobs[MAXSTRPATH]; /* input obs file*/
	char rnxnav_gps[MAXSTRPATH]; /* input GPS nav file*/
	char peph_t_gps[MAXSTRPATH]; /* input GPS preise ephemeris file*/
	char pclk_t_gps[MAXSTRPATH]; /* input GPS preise satellite clock file*/
	char rnxnav_cmp[MAXSTRPATH]; /* input BDS nav file*/
	char peph_t_cmp[MAXSTRPATH]; /* input CMP preise ephemeris file*/
	char pclk_t_cmp[MAXSTRPATH]; /* input CMP preise satellite clock file*/
	char antpos[MAXSTRPATH]; /* output solution file*/
	char residu[MAXSTRPATH]; /* output phase and code residuals file*/
	char ambigu[MAXSTRPATH]; /* output phase ambiguitys file*/
	char fileroad[MAXSTRPATH];/*file road*/
	char outdir[MAXSTRPATH];  /*out dir*/
	char ofile_bak[MAXSTRPATH]; /*ofile bak*/
} filopt_t;

typedef struct {
	gtime_t time;
	int sys;
	char satID[4];
	unsigned char sat, rcv;
	unsigned char SNR[NFREQ];
	unsigned char LLI[NFREQ];
	unsigned char vsat;
	unsigned char code[NFREQ];
	double L[NFREQ];
	double P[NFREQ];
	double D[NFREQ];
	double smoothP[NFREQ];
}obsd_t;

typedef struct {
	int n, nmax;
	obsd_t *data;
}obs_t;

typedef struct {        /* almanac type */
	int sat;            /* satellite number */
	int svh;            /* sv health (0:ok) */
	int svconf;         /* as and sv config */
	int week;           /* GPS/QZS: gps week, GAL: galileo week */
	gtime_t toa;        /* Toa */
						/* SV orbit parameters */
	double A, e, i0, OMG0, omg, M0, OMGd;
	double toas;        /* Toa (s) in week */
	double f0, f1;       /* SV clock parameters (af0,af1) */
} alm_t;

typedef struct {
	double version;                 ///< RINEX Version
	char  fileType[21];           ///< File type "N...."
	char fileSys[21];            ///< File system string
	char   type;
	int   sys;
	char program[21];        ///< Program string
	char runBy[21];         ///< Agency string
	char runDate[4];               ///< Date string; includes "UTC" at the end
	char commentList[61];  ///< Comment list
	double utc_gps[4];  /* GPS delta-UTC parameters {A0,A1,T,W} */
	double utc_glo[4];  /* GLONASS UTC GPS time parameters */
	double utc_gal[4];  /* Galileo UTC GPS time parameters */
	double utc_cmp[4];  /* BeiDou UTC parameters */
	double utc_qzs[4];
	//		double utc_sbs[4];  /* SBAS UTC parameters */
	double ion_gps[8];  /* GPS iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3} */
	double ion_gal[4];  /* Galileo iono model parameters {ai0,ai1,ai2,0} */
	double ion_cmp[8];  /* BeiDou iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3} */
	double ion_qzs[8];
	int leapSeconds;               ///< Leap seconds
	int leapDelta;                 ///< Change in Leap seconds at ref time
	int leapWeek;                  ///< Week number of ref time
	int leapDay;
	int outiono;
	int outtime;
	int outleaps;
	char comment[MAXCOMMENT][64]; /* comments */
}ephh_t;

typedef struct {
	int sat;            /* satellite number */
	int iode, iodc;      /* IODE,IODC */
	int sva;            /* SV accuracy (URA index) */
	int svh;            /* SV health (0:ok) */
	int week;           /* GPS/QZS: gps week, GAL: galileo week */
	int code;           /* GPS/QZS: code on L2, GAL/CMP: data sources */
	int flag;           /* GPS/QZS: L2 P data flag, CMP: nav type */
	gtime_t toe, toc, ttr; /* Toe,Toc,T_trans */
							/* SV orbit parameters */
	double A, e, i0, OMG0, omg, M0, deln, OMGd, idot;
	double crc, crs, cuc, cus, cic, cis;
	double toes;        /* Toe (s) in week */
	double fit;         /* fit interval (h) */
	double f0, f1, f2;    /* SV clock parameters (af0,af1,af2) */
	double tgd[4];      /* group delay parameters */
}eph_t;

typedef struct {        /* GLONASS broadcast ephemeris type */
	int sat;            /* satellite number */
	int iode;           /* IODE (0-6 bit of tb field) */
	int frq;            /* satellite frequency number */
	int svh, sva, age;    /* satellite health, accuracy, age of operation */
	gtime_t toe;        /* epoch of epherides (gpst) */
	gtime_t tof;        /* message frame time (gpst) */
	double pos[3];      /* satellite position (ecef) (m) */
	double vel[3];      /* satellite velocity (ecef) (m/s) */
	double acc[3];      /* satellite acceleration (ecef) (m/s^2) */
	double taun, gamn;   /* SV clock bias (s)/relative freq bias */
	double dtaun;       /* delay between L1 and L2 (s) */
} geph_t;

typedef struct {        /* SBAS ephemeris type */
	int sat;            /* satellite number */
	gtime_t t0;         /* reference epoch time (GPST) */
	gtime_t tof;        /* time of message frame (GPST) */
	int sva;            /* SV accuracy (URA index) */
	int svh;            /* SV health (0:ok) */
	double pos[3];      /* satellite position (m) (ecef) */
	double vel[3];      /* satellite velocity (m/s) (ecef) */
	double acc[3];      /* satellite acceleration (m/s^2) (ecef) */
	double af0, af1;     /* satellite clock-offset/drift (s,s/s) */
}seph_t;


typedef struct {        /* precise ephemeris type */
	gtime_t time;       /* time (GPST) */
	int index;          /* ephemeris index for multiple files */
	double pos[MAXSAT][4]; /* satellite position/clock (ecef) (m|s) */
	float  std[MAXSAT][4]; /* satellite position/clock std (m|s) */
	double vel[MAXSAT][4]; /* satellite velocity/clk-rate (m/s|s/s) */
	float  vst[MAXSAT][4]; /* satellite velocity/clk-rate std (m/s|s/s) */
	float  cov[MAXSAT][3]; /* satellite position covariance (m^2) */
	float  vco[MAXSAT][3]; /* satellite velocity covariance (m^2) */
} peph_t;

typedef struct {        /* precise clock type */
	gtime_t time;       /* time (GPST) */
	int index;          /* clock index for multiple files */
	double clk[MAXSAT][1]; /* satellite clock (s) */
	float  std[MAXSAT][1]; /* satellite clock std (s) */
} pclk_t;



typedef struct {        /* station parameter type */
	char stationName[10];
	char markerName[MAXANT]; /* marker name */
	char markerNumber[MAXANT]; /* marker number */
	char markerType[MAXANT]; /* marker type */
	char antType[MAXANT]; /* antenna descriptor */
	char antSerialNum[MAXANT]; /* antenna serial number */
	char recType[MAXANT]; /* receiver type descriptor */
	char recVers[MAXANT]; /* receiver firmware version */
	char recserialNum[MAXANT]; /* receiver serial number */
	int antsetup;       /* antenna setup id */
	int itrf;           /* ITRF realization year */
	int antennaDeltaType;        /* antenna delta type (0:enu,1:xyz) */
	double approxCoor[3];      /* station position (ecef) (m) */
	double antennaDelta[3];      /* antenna position delta (e/n/u or x/y/z) (m) */
	double hgt;         /* antenna height (m) */
	double interval;    // data sampling rate
	double dcb[4];			/* receiver dcb, GPS, GLONASS, GALILEO, BDS*/
	double dcbrms[4];	/* receiver dcb rms, GPS, GLONASS, GALILEO, BDS*/
} sta_t;

typedef struct {        /* DGPS/GNSS correction type */
	gtime_t t0;         /* correction time */
	double prc;         /* pseudorange correction (PRC) (m) */
	double rrc;         /* range rate correction (RRC) (m/s) */
	int iod;            /* issue of data (IOD) */
	double udre;        /* UDRE */
} dgps_t;

typedef struct {        /* SSR correction type */
	gtime_t t0;      /* epoch time (GPST) {eph,clk,hrclk,ura,bias} */
	double udi;      /* SSR update interval (s) */
	int iod;         /* iod ssr {eph,clk,hrclk,ura,bias} */
	int iode;           /* issue of data */
	int refd;           /* sat ref datum (0:ITRF,1:regional) */
	double deph[3];    /* delta orbit {radial,along,cross} (m) */
	double ddeph[3];    /* dot delta orbit {radial,along,cross} (m/s) */
	unsigned char update; /* update flag (0:no update,1:update) */
} ssr_t_EPH;

typedef struct {        /* SSR correction type */
	gtime_t t0;      /* epoch time (GPST) {eph,clk,hrclk,ura,bias} */
	double udi;      /* SSR update interval (s) */
	int iod;         /* iod ssr {eph,clk,hrclk,ura,bias} */
	int iode;           /* issue of data */
	int refd;           /* sat ref datum (0:ITRF,1:regional) */
	double dclk[3];    /* delta clock {c0,c1,c2} (m,m/s,m/s^2) */
	unsigned char update; /* update flag (0:no update,1:update) */
} ssr_t_CLK;

typedef struct {        /* SSR correction type */
	gtime_t t0;      /* epoch time (GPST) {eph,clk,hrclk,ura,bias} */
	double udi;      /* SSR update interval (s) */
	int iod;         /* iod ssr {eph,clk,hrclk,ura,bias} */
	int iode;           /* issue of data */
	int refd;           /* sat ref datum (0:ITRF,1:regional) */
	double hrclk;       /* high-rate clock corection (m) */
	unsigned char update; /* update flag (0:no update,1:update) */
} ssr_t_HRCLK;

typedef struct {        /* SSR correction type */
	gtime_t t0;      /* epoch time (GPST) {eph,clk,hrclk,ura,bias} */
	double udi;      /* SSR update interval (s) */
	int iod;         /* iod ssr {eph,clk,hrclk,ura,bias} */
	int iode;           /* issue of data */
	int ura;            /* URA indicator */
	int refd;           /* sat ref datum (0:ITRF,1:regional) */
	unsigned char update; /* update flag (0:no update,1:update) */
} ssr_t_URA;

typedef struct {        /* SSR correction type */
	gtime_t t0;      /* epoch time (GPST) {eph,clk,hrclk,ura,bias} */
	double udi;      /* SSR update interval (s) */
	int iod;         /* iod ssr {eph,clk,hrclk,ura,bias} */
	int iode;           /* issue of data */
	int refd;           /* sat ref datum (0:ITRF,1:regional) */
	float cbias[MAXCODE]; /* code biases (m) */
	unsigned char update; /* update flag (0:no update,1:update) */
} ssr_t_CBIAS;

typedef struct {
	gtime_t time;
	int flag;   //flag=1:get ssr ion from cnes; flag=2：update ion 
	int iod;
	double udint;
	int provid;
	int solid;
	int vtecqi;
	int nlayer;
	double vtec[MAXTECLAYER][3 + MAXTECCOEFF];/*VTEC:Height,Degree,Order,cosine coefficients,sine*/
}ssrion_t;

typedef struct {        /* SSR correction type */
	gtime_t t0[5];      /* epoch time (GPST) {eph,clk,hrclk,ura,bias} */
	double udi[5];      /* SSR update interval (s) */
	int iod[5];         /* iod ssr {eph,clk,hrclk,ura,bias} */
	int iode;           /* issue of data */
	int ura;            /* URA indicator */
	int refd;           /* sat ref datum (0:ITRF,1:regional) */
	double deph[3];    /* delta orbit {radial,along,cross} (m) */
	double ddeph[3];    /* dot delta orbit {radial,along,cross} (m/s) */
	double dclk[3];    /* delta clock {c0,c1,c2} (m,m/s,m/s^2) */
	double hrclk;       /* high-rate clock corection (m) */
	int codes[MAXCODE];   /*obs code types*/
	float cbias[MAXCODE]; /* code biases (m) */
	double pbias[MAXCODE]; /* phase biases (m) */
	float  stdpb[MAXCODE]; /* std-dev of phase biases (m) */
	unsigned char update; /* update flag (0:no update,1:update) */
} ssr_t;

typedef struct {        /* TEC grid type */
	gtime_t time;       /* epoch time (GPST) */
	int eph_t[3];       /* TEC grid data size {nlat,nlon,nhgt} */
	double rb;          /* earth radius (km) */
	double lats[3];     /* latitude start/interval (deg) */
	double lons[3];     /* longitude start/interval (deg) */
	double hgts[3];     /* heights start/interval (km) */
	double *data;       /* TEC grid data (tecu) */
	float *rms;         /* RMS values (tecu) */
} tec_t;

typedef struct {
	gtime_t time;
	int ns;
	char type;
	int sats[MAXSAT];
	double bfact[2];
	char tsys[4];
} peph_tHead;

typedef struct {
	int n, nmax;
	int ng, ngmax;
	int ns, nsmax;
	int ne, nemax;
	int nc, ncmax;
	int na, namax;       /* number of almanac data */
	int nt, ntmax;       /* number of tec  added by pwj at 2018.7.11*/
	eph_t *eph;
	geph_t *geph;
	seph_t *seph;
	peph_t *peph;
	pclk_t *pclk;
	alm_t *alm;         /* almanac data */
	tec_t *tec;         /* tec grid data */
	double utc_gps[4];  /* GPS delta-UTC parameters {A0,A1,T,W} */
	double utc_glo[4];  /* GLONASS UTC GPS time parameters */
	double utc_gal[4];  /* Galileo UTC GPS time parameters */
	double utc_qzs[4];  /* QZS UTC GPS time parameters */
	double utc_cmp[4];  /* BeiDou UTC parameters */
	double utc_sbs[4];  /* SBAS UTC parameters */
	double ion_gps[8];  /* GPS iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3} */
	double ion_gal[4];  /* Galileo iono model parameters {ai0,ai1,ai2,0} */
	double ion_qzs[8];  /* QZSS iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3} */
	double ion_cmp[8];  /* BeiDou iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3} */
	int leaps;          /* leap seconds (s) */
	double lam[MAXSAT][NFREQ]; /* carrier wave lengths (m) */
	pcv_t pcvs[MAXSAT];
	ssr_t ssr[MAXSAT];
	ssrion_t ssrion;
	double cbias[MAXSAT][5];/* code bias (0:p1-p2,1:p1-c1,2:p2-c2,3:P1-P3,4:P2-P3) (m) */
	double glo_cpbias[4];    /* glonass code-phase bias {1C,1P,2C,2P} (m) */
	char glo_fcn[MAXPRNGLO + 1]; /* glonass frequency channel number + 8 */
	double ion[MAXSAT][2];      /*ion value and RMS*/
	ephh_t nhead;
//	ssrion_t pssrion[1440];
}nav_t;

typedef  struct {
	double		rinexVersion;				//RINEX��ʽ�İ汾��						F9.2��11X
	char			fileType;					//�ļ�����								A1,11X
	char			gnssType;					//����ϵͳ(G����-GPS��R-GLONASS��M-G+R) A1,X11

	char			program[21];				//���������ļ��ĳ�������				A20
	char			runBy[21];					//�������ݵĵ�λ����					A20
	char			runDate[21];				//�����ļ���������						A20

	char			markerName[61];				//��վ����								A60

	char			markerNumber[21];			//��վ����								A20
	char         markerType[21];             //ver3.                          A20

	char			observer[21];				//�۲�ֵ����							A20
	char			agency[41];					//�۲ⵥλ��							A40

	char			recserialNum[21];		//���ջ����к�							A20
	char			recType[21];				//���ջ�����							A20
	char			recVers[21];				//���ջ��汾��(�ڲ������İ汾��)		A20

	char			antSerialNum[21];			//�������к�							A20
	char			antType[21];				//��������								A20
	char            antSetup[21];

	double		approxCoor[3];					//��վ��������(WGS84)					3F14.4

	int            antennaDeltaType;             //����offset���ͣ�0:HEN,1:XYZ
	double		antennaDeltaHEN[3];				//���߸�(�����±�������ʶ���߶�)		3F14.4
	double     antennaDeltaXYZ[3];
	//�������������ڱ�־�ڶ��򡢱���վ������

	char          antennaSatSys;                         //A1
	char          antennaObsCode[4];               // A3
	double     antennaPhaseCtr[3];
	double     antennaBsightXYZ[3];
	double     antennaZeroDirAzi;
	double     antennaZeroDirXYZ[3];
	double     centerOfMass[3];

	int			flagDouleFrequency;			//����˫��Ƶ���ݱ�ʶ��1��ʾ˫Ƶȫ������2Ϊ˫Ƶ�벨����0��ʾ��Ƶ��

	short int		obsTypeNum;					//�۲�ֵ������Ŀ						I6
	short int		obsTypeList[15];			//�۲�ֵ�����б�(��ʱֻ���ų��õ�7�֣�	9(4X,A2) �˶�������������rinex2  edited by PWJ 2016.1.30
												//������9�ֹ۲�ֵ����ʹ������			6X��9(4X,A2)
												// L1��L2��L5���ز�
												// C1,C2��C/A��α��
												// P1,P2��P��α��
												// D1,D2��������Ƶ��
												// S1,S2��
												// ...
	char                satSysTemp;
	short int          obsNum;
	short int          mulObsNum[NSYS];
	char                obs_Type[4];
	char                obsType[NSYS][MAXOBSTYPE][4];
	char                sigStrengthUnit[21];
	gtime_t	           firstEpoch;					//����Ԫʱ��
	gtime_t           	lastEpoch;					//ĩ��Ԫʱ��
	char				   timeType[4];				//ʱ��ϵͳ��GPS��ʾGPSʱ��GLO��ʾUTC��
													//��GPS/GLONASS�����ļ��б��뺬ʱ��ϵͳ��ʶ��
													//��GPS��ȱʡΪGPSʱ����GLONASS��ȱʡGLO
	double				interval;					//������

	short int			rowNumOneSat;				//һ������������ռ�������۲�������ÿ�д����������͵Ĺ۲����ݣ�
	short int sys;                                //����ϵͳ
	short int tsys;                               //ʱ��ϵͳ
	short int receiverOffset;
	int navsys;
	int leapSeconds;                              //����
	int numSVs;                                   //������

	char comment[MAXCOMMENT][64]; /* comments */

	int flag[2];//flag[0]:1005,1006;flag[1]:1007,1008
	int obstype;
	int freqtype;
}obsHead;

typedef struct {
	char mountPoint[NTRIP_MAXSTR];
	char ID[100];
	char format[100];
}Ntrip_base;

typedef struct {        /* RTCM control struct type */
	int staid;          /* station id */
	int stah;           /* station health */
	int seqno;          /* sequence number for rtcm 2 or iods msm */
	int outtype;        /* output message type */
	gtime_t time;       /* message time */
	gtime_t time_s;     /* message start time */
	obs_t obs;          /* observation data (uncorrected) */
	nav_t nav;
	sta_t sta;          /* station parameters */
	dgps_t *dgps;       /* output of dgps corrections */
	ssr_t ssr[MAXSAT];  /* output of ssr corrections */
	ssrion_t ion;        /*VTEC from CNES*/
	char msg[128];      /* special message */
	char msgtype[256];  /* last message type */
	char msmtype[6][128]; /* msm signal types */
	int obsflag;        /* obs data complete flag (1:ok,0:not complete) */
	int ephsat;         /* update satellite of ephemeris */
	double cp[MAXSAT][NFREQ]; /* carrier-phase measurement */
	unsigned char lock[MAXSAT][NFREQ]; /* lock time */
	unsigned char loss[MAXSAT][NFREQ]; /* loss of lock count */
	gtime_t lltime[MAXSAT][NFREQ]; /* last lock time */
	int nbyte;          /* number of bytes in message buffer */
	int nbit;           /* number of bits in word buffer */
	int len;            /* message length (bytes) */
	unsigned char buff[1200]; /* message buffer */
	unsigned int word;  /* word buffer for rtcm 2 */
	unsigned int nmsg2[100]; /* message count of RTCM 2 (1-99:1-99,0:other) */
	unsigned int nmsg3[300]; /* message count of RTCM 3 (1-299:1001-1299,0:ohter) */
	char opt[256];      /* RTCM dependent options */
} rtcm_t;

typedef struct {        /* SBAS message type */
	int week, tow;       /* receiption time */
	int prn;            /* SBAS satellite PRN number */
	unsigned char msg[29]; /* SBAS message (226bit) padded by 0 */
} sbsmsg_t;

typedef struct {        /* SBAS messages type */
	int n, nmax;         /* number of SBAS messages/allocated */
	sbsmsg_t *msgs;     /* SBAS messages */
} sbs_t;

typedef struct {        /* SBAS fast correction type */
	gtime_t t0;         /* time of applicability (TOF) */
	double prc;         /* pseudorange correction (PRC) (m) */
	double rrc;         /* range-rate correction (RRC) (m/s) */
	double dt;          /* range-rate correction delta-time (s) */
	int iodf;           /* IODF (issue of date fast corr) */
	short udre;         /* UDRE+1 */
	short ai;           /* degradation factor indicator */
} sbsfcorr_t;

typedef struct {        /* SBAS long term satellite error correction type */
	gtime_t t0;         /* correction time */
	int iode;           /* IODE (issue of date ephemeris) */
	double dpos[3];     /* delta position (m) (ecef) */
	double dvel[3];     /* delta velocity (m/s) (ecef) */
	double daf0, daf1;   /* delta clock-offset/drift (s,s/s) */
} sbslcorr_t;

typedef struct {        /* SBAS satellite correction type */
	int sat;            /* satellite number */
	sbsfcorr_t fcorr;   /* fast correction */
	sbslcorr_t lcorr;   /* long term correction */
} sbssatp_t;

typedef struct {        /* SBAS satellite corrections type */
	int iodp;           /* IODP (issue of date mask) */
	int nsat;           /* number of satellites */
	int tlat;           /* system latency (s) */
	sbssatp_t sat[MAXSAT]; /* satellite correction */
} sbssat_t;

typedef struct {        /* SBAS ionospheric correction type */
	gtime_t t0;         /* correction time */
	short lat, lon;      /* latitude/longitude (deg) */
	short give;         /* GIVI+1 */
	float delay;        /* vertical delay estimate (m) */
} sbsigp_t;

typedef struct {        /* IGP band type */
	short x;            /* longitude/latitude (deg) */
	const short *y;     /* latitudes/longitudes (deg) */
	unsigned char bits; /* IGP mask start bit */
	unsigned char bite; /* IGP mask end bit */
} sbsigpband_t;

typedef struct {        /* SBAS ionospheric corrections type */
	int iodi;           /* IODI (issue of date ionos corr) */
	int nigp;           /* number of igps */
	sbsigp_t igp[MAXNIGP]; /* ionospheric correction */
} sbsion_t;

typedef struct {        /* receiver raw data control type */
	gtime_t time;       /* message time */
	gtime_t tobs;       /* observation data time */
	obs_t obs;          /* observation data */
	obs_t obuf;         /* observation data buffer */
	nav_t nav;          /* satellite ephemerides */
	sta_t sta;          /* station parameters */
	int ephsat;         /* sat number of update ephemeris (0:no satellite) */
	sbsmsg_t sbsmsg;    /* SBAS message */
	char msgtype[256];  /* last message type */
	unsigned char subfrm[MAXSAT][380];  /* subframe buffer */
	double lockt[MAXSAT][NFREQ]; /* lock time (s) */
	double icpp[MAXSAT], off[MAXSAT], icpc; /* carrier params for ss2 */
	double prCA[MAXSAT], dpCA[MAXSAT]; /* L1/CA pseudrange/doppler for javad */
	unsigned char halfc[MAXSAT][NFREQ]; /* half-cycle add flag */
	char freqn[MAXOBS]; /* frequency number for javad */
	int nbyte;          /* number of bytes in message buffer */
	int len;            /* message length (bytes) */
	int iod;            /* issue of data */
	int tod;            /* time of day (ms) */
	int tbase;          /* time base (0:gpst,1:utc(usno),2:glonass,3:utc(su) */
	int flag;           /* general purpose flag */
	int outtype;        /* output message type */
	unsigned char buff[MAXRAWLEN]; /* message buffer */
	char opt[256];      /* receiver dependent options */
	double receive_time;/* RT17: Reiceve time of week for week rollover detection */
	unsigned int plen;  /* RT17: Total size of packet to be read */
	unsigned int pbyte; /* RT17: How many packet bytes have been read so far */
	unsigned int page;  /* RT17: Last page number */
	unsigned int reply; /* RT17: Current reply number */
	int week;           /* RT17: week number */
	int leaps;
	double sow;
	double dop[4];
	double blh[3];
	double undulation;
	double venu[3];
	int numSV;
	int fixstatus;
	double ep[6];
	int ubxpvt;
	unsigned char pbuff[255 + 4 + 2]; /* RT17: Packet buffer */
} raw_t;


typedef struct {
	int  		      GNSSSystem;
	int 				SVN;
	int 				PRN;
	int					block;
	double				phasePrealign[NFREQ];
	int					arcLength;
	int                 arcLength_L;
	int					hasBeenCycleslip;
	gtime_t	    	transTime;


	double				position[3];
	double				velocity[3];
	double				ITRFvel[3];
	double				LoS[3];
	double				clockBias;
	double				clockCorrection;
	double				clockDrift;
	double              BRDCVariance;
	double				orientation[3][3];
	double				elevation;
	double				azimuth;
	double              DCB[5];   /* code bias (0:p1-p2,1:p1-c1,2:p2-c2,3:P1-P3,4:P2-P3) (m) */
	double				windUpRadAccumReceiver;
	double				windUpRadAccumSatellite;
	double             windUp;
	double				tropWetMap;
	double				wideLineDis[NFREQ];
	double				geometryFreeDis[NFREQ];
	double				geometricDistance;
	double              wideLineAmb[2];//L1L2 L1L5
	double              geometryFreeAmb[2];//L1L2 L1L5
	double resp[NFREQ]; /* residuals of pseudorange (m) */
	double resc[NFREQ]; /* residuals of carrier-phase (m) */
	double resi[2];		/* residuals of pseudo-ionosphere(GIM and gradient) (m) */
	unsigned char vsat[NFREQ]; /* valid satellite flag */
	unsigned char snr[NFREQ]; /* signal strength (0.25 dBHz) */
	unsigned char fix[NFREQ]; /* ambiguity fix flag (1:fix,2:float,3:hold) */
	unsigned char slip[NFREQ]; /* cycle-slip flag */
	unsigned int lock[NFREQ]; /* lock counter of phase */
	unsigned int outc[NFREQ]; /* obs outage counter of phase */
	unsigned int slipc[NFREQ]; /* cycle-slip counter */
	unsigned int rejc[NFREQ]; /* reject counter */
	unsigned char       slip_gf[NFREQ]; /* gf cycle-slip flag */
	unsigned char       slip_mw[NFREQ]; /* mw cycle-slip flag */
	unsigned char       slip_ll[NFREQ]; /* lli cycle-slip flag */
	unsigned char       slip_tecr[NFREQ]; /* tecr cycle-slip flag */
	unsigned char       slip_l1c1; /* single frequency cycle-slip flag */
	unsigned char       slip_doppler; /*doppler cycle-slip for sf*/
	double mw;
	double gf;
	double l1c1Mean;
	double l1c1Mean2;
	double  deltgf;     /* ǰ����Ԫ��gf֮�� */
	double  dotgf;      /* ǰ����Ԫ��gf�仯�� */
	double  tecr1;       /* TEC Rate at epoch k-1(TECU/s) */
	double  tecr2;     /*  TEC Rate at epoch k-2(TECU/s) */
	double  mwsig;      /* sigma of wide-lane bias */
	int     mwcount;    /* number of wide-lane bias */
	double  tecrmea;
	double  tecrsig;    /* sigma of TECR */
	int     tecrcount;  /* number of TEC Rate */
	double  tecrsquare;
	int     l1c1count;
	gtime_t GFTime;       /* gf*/
	gtime_t MWTime;       /* mw */
	gtime_t TECRTime;     /* tecr*/
	int					hasOrbitsAndClocks;
	int					hasDCBs;
	int					hasDCBsofC1P1;
	int					available;

	double smoothMeas[NFREQ];
	double smoothL[NFREQ];
	int    smoothEpochs[NFREQ];
	double previousL[NFREQ];   /*record previous value of phase observation*/
	gtime_t LTime[NFREQ];      /*record previoustime of phase observation*/

	double previousD[NFREQ];   /*record previous value of doppler observation*/
	gtime_t DTime[NFREQ];      /*record time of doppler observation*/

	double				previousSatPos[3];
	double				previousSatVel[3];
	double				previousSatClk;
	double P_pre_codegross[NFREQ];
	double D_pre_codegross[NFREQ];
	gtime_t t_pre_codegross[NFREQ];
	int vsdoppler[NFREQ];
	int vscode[NFREQ];
} ssat_t;

typedef struct {        /* RTK control/result type */
	gtime_t time;
	double rb[6];       /* base position/velocity (ecef) (m|m/s) */
	double rr[9];       /* rove position/velocity/accelerate (ecef) (m|m/s|m/s/s) */
	double  qr[6];       /* position variance/covariance */
							/* {c_xx,c_yy,c_zz,c_xy,c_yz,c_zx} or */
							/* {c_ee,c_nn,c_uu,c_en,c_nu,c_ue} */
	double qv[6];        /* variance/covariance/ velocity (m/s)  */
	double qa[3];        /* variance Accelerate (m/s/s)  */
	double clk[4];       /*system clock and inter-system bias*/
	int nx, na;          /* number of float states/fixed states */
	double tt;          /* time difference between current and previous (s) */
	double *x, *P;      /* float states and their covariance */
	double *xa, *Pa;     /* fixed states and their covariance */
	unsigned char stat; /* solution status (SOLQ_???) */
	int ns;
	float age;          /* age of differential (s) */
	float ratio;        /* AR ratio factor for valiation */
	double dop[5];        // PDOP value
	double predop[5];
	int nfix;           /* number of continuous fixes of ambiguity */
	ssat_t ssat[MAXSAT]; /* satellite status */
	int neb;            /* bytes in error message buffer */
	double dpos[3];
	double qdpos[6];
	int dposFlag;          //0:no TDCP;1:dpos available;2:doppler 
	double alfa;
	double ztd[3];     
	double *x_, *P_;   
	int presat[MAXSAT];//
	int npresat;
	int presat_vsat[MAXSAT];//
	int npresat_vsat;
	double prerr[3];
	double preqr[6];
	double dopv[3];
	double vclk[4];
	double dopqv[6];
	unsigned char prestat;
	int iniind;
	int outp;
	int contv;
	int zupt;
	double vdop;
	prcopt_t opt;       /* processing options */
	int type;
	double heading;
	double rotation;
	int ngps, nglo,ngal, nbds;
	int prenx;
	int ns_ppp;
	double rcvdcb[NSYS];  //P1P2

	double* coef;           //[MAXTECCOEFF]
	double* Pcoef;
	int coefiniflag;
} sol_t;


typedef struct {        /* solution buffer type */
	int n, nmax;         /* number of solution/max number of buffer */
	int cyclic;         /* cyclic buffer flag */
	int start, end;      /* start/end index */
	gtime_t time;       /* current solution time */
	sol_t *data;        /* solution data */
	double rb[3];       /* reference position {x,y,z} (ecef) (m) */
	unsigned char buff[MAXSOLMSG + 1]; /* message buffer */
	int nb;             /* number of byte in message buffer */
} solbuf_t;

typedef struct {
	unsigned char type;
	unsigned char code;
	int pos;
	int pri;
	int frq;
}dataIndex;


int sbassatsel;     /* SBAS satellite selection (0:all) */
typedef struct {        /* stream type */
	int type;           /* type (STR_???) */
	int mode;           /* mode (STR_MODE_?) */
	int state;          /* state (-1:error,0:close,1:open) */
	unsigned int inb, inr;   /* input bytes/rate */
	unsigned int outb, outr; /* output bytes/rate */
	unsigned int tick, tact; /* tick/active tick */
	unsigned int inbt, outbt; /* input/output bytes at tick */
	lock_t lock;        /* lock flag */
	void *port;         /* type dependent port control struct */
	char path[MAXSTRPATH]; /* stream path */
	char msg[MAXSTRMSG];  /* stream message */
} stream_t;


typedef struct {        /* RTK server type */
	int state;          /* server state (0:stop,1:running) */
	int cycle;          /* processing cycle (ms) */
	int nmeacycle;      /* NMEA request cycle (ms) (0:no req) */
	int nmeareq;        /* NMEA request (0:no,1:nmeapos,2:single sol) */
	double nmeapos[3];  /* NMEA request position (ecef) (m) */
	int buffsize;       /* input buffer size (bytes) */
	int format[MAXSTRPPP];      /* input format {rov,base,corr} */
	solopt_t solopt[2]; /* output solution options {sol1,sol2} */
	int navsel;         /* ephemeris select (0:all,1:rover,2:base,3:corr) */
	int nsbs;           /* number of sbas message */
	int nsol;           /* number of solution buffer */
	sol_t sol;          /* RTK control/result struct */
	int nb[MAXSTRPPP];         /* bytes in input buffers {rov,base} */
	int nsb[2];         /* bytes in soulution buffers */
	int npb[MAXSTRPPP];         /* bytes in input peek buffers */
	unsigned char *buff[MAXSTRPPP]; /* input buffers {rov,base,corr} */
	unsigned char *sbuf[2]; /* output buffers {sol1,sol2} */
	unsigned char *pbuf[MAXSTRPPP]; /* peek buffers {rov,base,corr} */
	unsigned int nmsg[MAXSTRPPP][10]; /* input message counts */
	raw_t  raw[1];                         /* receiver raw control {rov,base,corr} */
	rtcm_t rtcm[MAXSTRPPP];           /* RTCM control {rov,base,corr} */
	gtime_t ftime[MAXSTRPPP];   /* download time {rov,base,corr} */
	char files[MAXSTRPPP][MAXSTRPATH]; /* download paths {rov,base,corr} */
	obs_t obs[MAXOBSBUF]; /* observation data {single receiver for PPP} */
	nav_t nav;          /* navigation data */
	sbsmsg_t sbsmsg[MAXSBSMSG]; /* SBAS message buffer */
	stream_t stream[MAXSTRPPP]; /* streams {rov,base,corr,sol1,sol2,logr,logb,logc} */
	stream_t *moni;     /* monitor stream */
	unsigned int tick;  /* start tick */
	thread_t thread;    /* server thread */
	int cputime;        /* CPU time (ms) for a processing cycle */
	int prcout;         /* missing observation data count */
	int nave;           /* number of averaging base pos */
	double rb_ave[3];   /* averaging base pos */
	char cmds_periodic[MAXSTRPPP][MAXRCVCMD]; /* periodic commands */
	char cmd_reset[MAXRCVCMD]; /* reset command */
	double bl_reset;    /* baseline length to reset (km) */
	lock_t lock;        /* lock flag */
	char staname[40];
	int flag;
	unsigned int prctick;
	unsigned int obstick;
} pppsvr_t;


extern void init_rtcm(rtcm_t *rtcm);
extern void free_rtcm(rtcm_t *rtcm);

extern int stable_func(char *line, char* sep, char **str);
extern int strtok_func(char *line, char* sep, char **str);

extern int rtcmDecode(rtcm_t *rtcm, unsigned char data);
extern int Decode(rtcm_t *rtcm);

extern int encode_rtcm3(rtcm_t* rtcm, int type, int sync);
extern int gen_rtcm3(rtcm_t* rtcm, int type, int sync);



extern unsigned int getbitu(const unsigned char *buff, int pos, int len);
extern int getbits(const unsigned char *buff, int pos, int len);

extern void setbitu(unsigned char *buff, int pos, int len, unsigned int data);
extern void setbits(unsigned char *buff, int pos, int len, int data);
extern unsigned int crc32(const unsigned char *buff, int len);

extern unsigned int crc24q(const unsigned char *buff, int len);
extern unsigned short crc16(const unsigned char *buff, int len);
extern int decode_word(unsigned int word, unsigned char *data);
extern int decode_frame(const unsigned char *buff, eph_t *eph, alm_t *alm,
	double *ion, double *utc, int *leaps);
extern int test_glostr(const unsigned char *buff);
extern int decode_glostr(const unsigned char *buff, geph_t *geph);
extern int decode_bds_d1(const unsigned char *buff, eph_t *eph);
extern int decode_bds_d2(const unsigned char *buff, eph_t *eph);

extern int init_raw(raw_t *raw);
extern void free_raw(raw_t *raw);
extern int input_raw(raw_t *raw, int format, unsigned char data);

extern int input_ubx(raw_t *raw, unsigned char data);
extern int input_ubxf(raw_t *raw, FILE *fp);

extern int gen_ubx(const char *msg, unsigned char *buff);
extern void gen_uniqidbuff(short int *uniqid, unsigned char *uniqidbuff);


extern gtime_t epoch2time(const double *ep);
extern gtime_t gpst2time(int week, double sec);
extern gtime_t doy2time(int year, int doy);
extern gtime_t mjd2time(double mjd);//only mjd to time

extern double time2gpst(gtime_t t, int *week);

extern void time2epoch(gtime_t t, double *ep);
extern void mjd2epoch(double mjd, double *ep);

extern int epoch2doy(const double *ep, int *year);
extern int time2doy(gtime_t t, int *year);

extern double epoch2mjd(const double *ep);
extern double gpst2mjd(int week, double sec);
extern double doy2mjd(int year, int doy);
extern double time2mjd(gtime_t t);

extern int isleapyear(int year);
extern int timestr2ep(char *str, double *ep);
extern double timediff(gtime_t t1, gtime_t t2);
extern gtime_t timeadd(gtime_t t1, double t2);
extern int leapsecond(gtime_t second);
extern gtime_t utc2gpst(gtime_t second);// UTC sec to gtime_t  revise the leapseconds
extern gtime_t gpst2utc(gtime_t second);
extern gtime_t bdt2gpst(gtime_t t);
extern gtime_t gpst2bdt(gtime_t t);
extern gtime_t bdt2time(int week, double second);

extern double MJD2JD(double MJD);
extern double getSidTime(gtime_t time);
extern gtime_t adjweek(gtime_t t, gtime_t t0);
extern gtime_t adjday(gtime_t t, gtime_t t0);
extern gtime_t timeget(void);
extern void timeset(gtime_t t);
extern unsigned int tickget(void);
extern void sleepms(int ms);

extern void createdir(const char *path);
extern int reppath(const char *path, char *rpath, gtime_t time, const char *rov,
	const char *base);

extern int adjgpsweek(int week);
extern int adjbdtweek(int week);


extern int satID2satIndex(char *satID);
extern char* satIndex2satID(char *s, int satIndex);
extern int satIndex2sys(int satIndex);
extern int satno2satIndex(int sys, int prn);
extern int satIndex2satno(int satIndex);

extern char* numInH2str(char *s, int n);

extern double str2num(const char *s, int i, int n);
extern char *time2ObsTimeStr(gtime_t sec, char *timestr);
extern char *time2BRDCTimeStr(gtime_t sec, char *timestr);
extern char *time2peph_tTimeStr(gtime_t sec, char* timestr);
extern char *time2pclk_tTimeStr(gtime_t sec, char* timestr);

extern int str2time(const char *buff, int m, int n, gtime_t *time);

extern void time2str(gtime_t t, char *str, int n);
extern char *time_str(gtime_t t, int n);

extern char *num2BRDCScientific(double a, char *s_brdc);
extern char *num2CLKScientific(double a, char *s_clk);


extern double dot_vector(const double *a, const double *b, int n);

extern double norm(const double *a, int n);
extern int normv3(const double *a, double *b);
extern void cross3(const double *a, const double *b, double *c);

extern void matcpy(double *A, const double *B, int n, int m);

extern double *mat(int m, int n);
extern int *imat(int m, int n);
extern double *zeros(int m, int n);
extern double *eye(int m);

extern void MatShow(double* Mat, int row, int col);
extern double* MatAdd(double* A, double* B, int row, int col);
extern double* MatSub(double* A, double* B, int row, int col);
extern double* MatMul(double* A, int Arow, int Acol, double* B, int Brow, int Bcol);
extern double* MatMulk(double *A, int row, int col, double k);
extern double* MatT(double* A, int row, int col);
extern double  MatDet(double *A, int row);
extern int MatInv(double *A, int row, int col);
extern double  MatACof(double *A, int row, int m, int n);
extern double* MatAdj(double *A, int row, int col);

extern void matmul(const char *tr, int n, int k, int m, double alpha,
	const double *A, const double *B, double beta, double *C);

extern int ludcmp(double *A, int n, double *L, double *U);
extern void lubksb(double *U, double *L, int n, int flag, double *A);
extern int  matinv(double *A, int n);
extern int combinemat(double* A, const double* B, int m, int n, int k, int opt);
extern int LeastSquare(const double *H, const double *v, int m, int n, double *x, double *Q);
extern int LeastSquarePlus(const double *A, const double *y, const int nv, const int nx, double *x, double *Q);
extern int KalmanFilter(double* x, double *P, double *H, double *v,
	double *R, double *Q1, double *Q2, int m, int n);

extern void matprint(const double *A, int n, int m, int p, int q);
extern void matfprint(const double *A, int n, int m, int p, int q, FILE *fp);



extern const double chisqr[];           /* chi-sqr(n) table (alpha=0.001) */
extern const double st0[];                  //unix
extern const double gpst0[];                //GPS
extern const double gst0[]; /* galileo system time reference */
extern const double bdt0[]; /* beidou time reference */
extern const double lam_carr[];
extern const prcopt_t prcopt_default;
extern const solopt_t solopt_default;

extern const  int constallation[];
extern const char constallationid[];

//extern int ssrOutputIndex;


extern char *str2nstr(const char *buff, int n, int m);


/*read antenna pcv and pco*/
extern int ReadAntennaPCV(const char *file, pcvs_t *pcvs);
extern pcv_t* SearchPCV(gtime_t time, int sat, const char* type, pcvs_t* pcvs);
extern void SetPCV(gtime_t time, sta_t *sta, prcopt_t *popt, pcvs_t *pcvs, nav_t *nav);

/*���꺯��*/
extern double Radian2Angle(double radian);
extern double Angle2Radian(double Angle);
extern void CoorGeo2CoorCar(const double *pos, double *x);
extern void CoorPol2CoorCar(const double r, const double *azel, double *x);
extern void CoorCar2CoorGeo(const double *x, double *pos);
extern void CoorPol2CoorGeo(const double r, const double *azel, double *pos);
extern void CoorCar2CoorPol(const double *x, double *r, double *azel);
extern void CoorGeo2CoorPol(const double *pos, double *r, double *azel);
extern void CoorCar2CoorENU(const double *x, const double *dx, double *enu);
extern void CoorGeo2CoorENU(const double *pos, const double *dx, double *enu);
extern void CoorENU2CoorCar(const double* pos, const double* dx, double* enu);
extern void CoorPol2CoorENU(const double r, const double *azel, const double *dx, double *enu);
extern double co_distance(double *x, double *y);

extern void covCoorCar(const double *pos, const double *Q, double *P);

extern void pppITRFToCgcs2000(int year, const double *x, double *y);
extern void cgcs2000ToBD09(const double *x, double *y);

extern void dops(int ns, const double *azel, double elmin, double *dop);

extern double ionmodel(gtime_t t, const double *ion, const double *pos, const double *azel);
extern double ionmapf(const double *pos, const double *azel);
extern double ionppp(const double *pos, const double *azel, double re, double hion, double *posp);
extern double sbstropcorr(gtime_t time, const double *pos, const double *azel, double *var);
extern double tropmodel(gtime_t time, const double *pos, const double *azel, double humi, int flag);
extern double slanttropmodel(gtime_t time, const double *pos, const double *azel,double *var);
extern double tropmapf(gtime_t time, const double pos[], const double azel[], double *mapfw);
extern double prectrop(gtime_t time, const double *pos, const double *azel,
	const double *x, double *dtdx, double *var);

extern double modulo(double a, double mod);
extern double scalarProd(double *a, double *b);
extern void rotate(double *v, double angle, int axis);
extern void findMoon(gtime_t t, double* moonPosition);
extern void findSun(gtime_t t, double* sunPosition);
void GSDtime_sun(gtime_t t, double *gstr, double *slong, double *sra, double *sdec);
extern void fillSolidTideDisplacement(gtime_t time, double *RecPosition, double *displacement);


/* geiod models --------------------------------------------------------------*/
extern int opengeoid(int model, const char *file);
extern void closegeoid(void);
extern double geoidh(const double *pos);


extern int convkml(const char *infile, double *rr, const char *outfile, gtime_t ts,
	gtime_t te, double tint, int qflg, double *offset,
	int tcolor, int pcolor, int outalt, int outtime);

extern void initsolbuf(solbuf_t *solbuf, int cyclic, int nmax);
extern void freesolbuf(solbuf_t *solbuf);

extern void satantoff(gtime_t time, const double *rs, int sat, const nav_t *nav,
	double *dant);

extern void antmodel(pcv_t *pcv, const double *del, const double *azel,
	int opt, int sys, double *dant);
extern void antmodel_s(const pcv_t *pcv, double nadir, double *dant);

extern void xyz2enu(const double *pos, double *E);
extern void  rotateAroundZ(double alpha, double *X);
extern void windupcorr(gtime_t time, const double *rs, const double *rr, const double rotation,
	double *phw);
/*PPP*/
extern double varerr(double SNR,int sat, int sys, double el, int type, const prcopt_t *opt);
extern void InitialParameter(sol_t *sol, double xi, double var, int i);
extern void InitialSolution(sol_t *sol, const prcopt_t *popt);
extern void releaseSolution(sol_t *sol);
extern double geoDistance(int sys, double *rs, double *rr, double *e);
extern double GetElevationAzimuth(const double *pos, const double *dx, double *azel, double *azimuth);
extern double rotationCorrection(int sys, double* rs, double* rr);

extern int ionocorr(gtime_t time, const nav_t *nav, int sat, const double *pos,
	const double *azel, int ionoopt, double *ion, double *var);

extern double norm_legendre(int n, int m, double thta);
extern int ssrstec(gtime_t time, ssrion_t tec, const double *pos, const double *azel, int opt, double *delay, double *var);

extern int satexclude(int sat, int svh, const prcopt_t *opt);
extern int  testsnr(int base, int freq, double el, double snr,
	const snrmask_t *mask);
extern int SPP_EstimateRecPosition(obsd_t *obs, int nobs, nav_t *nav, prcopt_t *popt, sol_t *sol, int *svh, double *azel, int *vsat, double *resp);
extern int Estimate_SPP(obsd_t *obs, int n, nav_t *nav, prcopt_t *popt, sol_t *sol);
extern int Estimate_PPP(obsd_t *obs, int n, nav_t *nav, prcopt_t *popt, sol_t *sol);
extern int Estimate_POS(obsd_t *obs, int n, nav_t *nav, prcopt_t *popt, sol_t *sol);
extern int ProcData(obsd_t *odata, int n, nav_t *nav, prcopt_t *popt, sol_t *sol);
extern int ProcDataFinal(prcopt_t* popt, filopt_t *fopt, solopt_t *sopt);
extern void ppp_afterReadEpochUpdate(sol_t *sol, const obsd_t *obs, int n, int *presat, int npresat);

extern char *strtok_blank(char *line);
extern double avg_std(const double *a, int n, double *std);

extern void select_combination(const int l, const int p, const int n, const int m, int *sn);
extern int findGross(int ppp, int bMulGnss, double *v, const int nv, const int nbad,
	double *std_ex, double *ave_ex, int *ibadsn, const double ratio,
	const double minv, const double stdmin);




extern void readtec(const char *file, nav_t *nav, int opt);
extern int iontec(gtime_t time, const nav_t *nav, const double *pos,
	const double *azel, int opt, double *delay, double *var);

extern void trace(int level, const char *format, ...);
extern void tracet(int level, const char *format, ...);
extern int execcmd(const char *cmd);
extern int uncompress(const char *file, char *uncfile);

/* stream data input and output functions ------------------------------------*/
extern void strinitcom(void);
extern void strinit(stream_t *stream);
extern void strlock(stream_t *stream);
extern void strunlock(stream_t *stream);
extern int  stropen(stream_t *stream, int type, int mode, const char *path);
extern void strclose(stream_t *stream);
extern int  strread(stream_t *stream, unsigned char *buff, int n);
extern int  strwrite(stream_t *stream, unsigned char *buff, int n);
extern void strsync(stream_t *stream1, stream_t *stream2);
extern int  strstat(stream_t *stream, char *msg);
extern void strsum(stream_t *stream, int *inb, int *inr, int *outb, int *outr);
extern void strsetopt(const int *opt);
extern gtime_t strgettime(stream_t *stream);
extern void strsendnmea(stream_t *stream, const double *pos);
extern void strsendcmd(stream_t *stream, const char *cmd);
extern void strsenduniqidcmd(stream_t *stream, const char *cmd);
extern void strsettimeout(stream_t *stream, int toinact, int tirecon);
extern void strsetdir(const char *dir);
extern void strsetproxy(const char *addr);

/* rtk server functions ------------------------------------------------------*/
extern int  rtksvrinit(pppsvr_t *svr);
extern void rtksvrfree(pppsvr_t *svr);

extern void rtksvrlock(pppsvr_t *svr);
extern void rtksvrunlock(pppsvr_t *svr);

extern int rtksvrstart(pppsvr_t* svr, int cycle, int buffsize, int* strs, char** paths, int* formats, int navsel, char** cmds, char** cmds_periodic, char** rcvopts, int nmeacycle, int nmeareq, const double* nmeapos, prcopt_t* prcopt, solopt_t* solopt, stream_t* moni, char* errmsg);
extern int rtksvrstart_update(int i, pppsvr_t* svr, int cycle, int buffsize, int* strs, char** paths, int* formats, int navsel, char** cmds, char** cmds_periodic, char** rcvopts, int nmeacycle, int nmeareq, const double* nmeapos, prcopt_t* prcopt, solopt_t* solopt, stream_t* moni, char* errmsg);
extern void rtksvrstop(pppsvr_t* svr, char** cmds);
extern int  rtksvropenstr(pppsvr_t* svr, int index, int str, const char* path,
	const solopt_t* solopt_t);
extern void rtksvrclosestr(pppsvr_t* svr, int index);

extern int  rtksvrostat(pppsvr_t *svr, int type, gtime_t *time, int *sat,
	double *az, double *el, int **snr, int *vsat);
extern void rtksvrsstat(pppsvr_t *svr, int *sstat, char *msg);
extern int  rtksvrmark(pppsvr_t *svr, const char *name, const char *comment);

extern void readtec(const char *file, nav_t *nav, int opt);

extern void deg2dms(double deg, double *dms, int ndec);
extern double dms2deg(const double *dms);

extern int outnmea_gga(unsigned char *buff, const sol_t *sol);
extern int outnmea_gsa(unsigned char *buff, const sol_t *sol,
	const ssat_t *ssat);
extern int outnmea_gsv(unsigned char *buff, const sol_t *sol,
	const ssat_t *ssat);


extern double GA_obs0[MAXSAT][4];
extern double GA_clkjump;
extern double one_clkjump;

extern short int secureFlag;
extern double global_ifcb[24];


//extern double GetSatWaveLength(int sat, int frq, nav_t *nav);
//
//extern int cmpobs(const void *p1, const void *p2);
//extern int sortobs(obs_t *obs);
//
//extern unsigned char obs2code(const char *obs, int *freq);
//extern const char* code2obs(const unsigned char code, int *frq);
//
//extern int obs2index(double ver, int sys, const unsigned char *code,
//	const char *tobs, const char *mask);
//
//extern int getcodepri(int system, unsigned char code);
//extern int satsys2num(int sys);
//
extern int findMax(double *a, int b);
/*Find the max value in a[b],return the index*/
extern int findIntMax(int *a, int b);

extern int PPPTest();

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
int ReadOBSHead(FILE *fp, obs_t *obs, sta_t *sta, double *rinexVersion, int *tsys, char *obs_Type, char *obsType[NSYS][MAXOBSTYPE], int *obsNum, int *mulObsNum);
int ReadOBSData(FILE *fp, obs_t *obs, double *rinexVersion, int tsys, char *obs_Type, char *obsType[NSYS][MAXOBSTYPE], int *obsNum, int *mulObsNum);
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
//int ReadCLKHead(FILE *fp);
//int ReadCLKData(FILE *fp, nav_t *nav, prcopt_t *opt);
int addclk(nav_t *nav, pclk_t *pclk);
int cmppclk(const void *p1, const void *p2);
void combpclk(nav_t *nav);

void releaseSP3CLK(nav_t *nav);
/***************read sp3,clk file***************/

int readDCBFile(const char *file, nav_t *nav);
int readRcvDCBFile(const char* file, sta_t* sta);
int readSsrionFile(const char *file, nav_t *nav);


int readGimFile(const char *file, nav_t *nav);

extern int readWriteSsrFile(const char *ssrFile, const char *outDir, nav_t *nav,
	const int intervel, const int itype, const int otype);
extern int genSp3Clk(const char *brdcFile, const char *ssrFile, const char *antFile, const char *outDir,
	const int intervel, const int itype, const int otype);


extern int coefficientUpdate(sol_t* sol, double* ipplat, double* ipplon, double* vtec, double* var, int size, int degree, int order);
extern int vtecUpdate(nav_t* nav, sol_t* sol,ssrion_t* ssrion);
extern void initialCoef(sol_t* sol, nav_t* nav, int degree, int order);
extern void freeCoef(sol_t* sol);
extern int ass_ssrion(nav_t* nav, sol_t* sol);

extern int OutputObs(const pppsvr_t *svr, obsHead* opt, const obsd_t* obs, int n, const nav_t* nav, const char* dir);
extern int OutputBrdc(const Ntrip_base* information, const nav_t* nav, const int sat, const int index, const char* dir);
extern int OutputSsr(const Ntrip_base* information, const ssr_t* ssr, const char* dir);
extern int Outputpeph(const Ntrip_base* information, const ssr_t* ssr, const nav_t* nav, const char* dir);
extern int writeSp3Clk(gtime_t time, const ssr_t* ssr, const nav_t* nav, const char* dir, const char* clkName, int itype, int otype, int intervel);



extern int brdcFlag;
extern int ssrFlag;

extern pppsvr_t *pppsvr;
extern nav_t navppp;
extern pcvs_t Global_PCVs;
extern sta_t *stainfo;
extern int corNum;
extern int extObsNum;
extern int outNum;
extern int corThreadFlag;
extern int obsThreadFlag;
extern int test_n;
extern ssrion_t global_ssrion;

extern lock_t global_lock;

extern char pephdir[];
extern char tropdir[];
extern char rawdir[];
extern char obsdir[];
extern char brdmdir[];

extern char ephprefix[];
extern double verbrdc;

/////////////////////////////////////////////////////
extern int prefixflag;	/* 0:BRDC, 1: self-defined */
extern int typeflag;	/* 0:cmd, 1:file, 2:stream*/
extern int filecountflag;
////////////////////////////////////////////////////


extern int outputIntervel;
extern int maxOutage;


#ifdef __cplusplus
}
#endif

#endif // PWJPPP_H
