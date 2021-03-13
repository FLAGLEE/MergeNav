#include "pwjppp.h"

static const int solq_nmea[] = {  /* nmea quality flags to rtklib sol quality */
								  /* nmea 0183 v.2.3 quality flags: */
								  /*  0=invalid, 1=gps fix (sps), 2=dgps fix, 3=pps fix, 4=rtk, 5=float rtk */
								  /*  6=estimated (dead reckoning), 7=manual input, 8=simulation */

	SOLQ_NONE ,SOLQ_SINGLE, SOLQ_PPP,
    SOLQ_NONE, SOLQ_NONE, SOLQ_NONE
};

/* output solution in the form of nmea GGA sentence --------------------------*/
extern int outnmea_gga(unsigned char *buff, const sol_t *sol)
{
	gtime_t time;
	double h, ep[6], pos[3], dms1[3], dms2[3], dop = 1.0;
	int solq;
	char *p = (char *)buff, *q, sum;

	trace(3, "outnmea_gga:\n");

	if (sol->stat <= SOLQ_NONE) {
		p += sprintf(p, "$GPGGA,,,,,,,,,,,,,,");
		for (q = (char *)buff + 1, sum = 0; *q; q++) sum ^= *q;
		p += sprintf(p, "*%02X%c%c", sum, 0x0D, 0x0A);
		return p - (char *)buff;
	}
	for (solq = 0; solq<8; solq++) if (solq_nmea[solq] == sol->stat) break;
	if (solq >= 8) solq = 0;
	time = gpst2utc(sol->time);
	if (time.sec >= 0.995) { time.time++; time.sec = 0.0; }
	time2epoch(time, ep);
	CoorCar2CoorGeo(sol->rr, pos);
	pos[0] = pos[0] * D2R;
	pos[1] = pos[1] * D2R;
	h = 0;
	deg2dms(fabs(pos[0])*R2D, dms1, 7);
	deg2dms(fabs(pos[1])*R2D, dms2, 7);
	p += sprintf(p, "$GPGGA,%02.0f%02.0f%05.2f,%02.0f%010.7f,%s,%03.0f%010.7f,%s,%d,%02d,%.1f,%.3f,M,%.3f,M,%.1f,",
		ep[3], ep[4], ep[5], dms1[0], dms1[1] + dms1[2] / 60.0, pos[0] >= 0 ? "N" : "S",
		dms2[0], dms2[1] + dms2[2] / 60.0, pos[1] >= 0 ? "E" : "W", solq,
		sol->ns, dop, pos[2] - h, h, sol->age);
	for (q = (char *)buff + 1, sum = 0; *q; q++) sum ^= *q; /* check-sum */
	p += sprintf(p, "*%02X%c%c", sum, 0x0D, 0x0A);
	return p - (char *)buff;
}
/* output solution in the form of nmea GSA sentences -------------------------*/
extern int outnmea_gsa(unsigned char *buff, const sol_t *sol,
	const ssat_t *ssat)
{
	double azel[MAXSAT * 2], dop[4];
	int i, sat, sys, nsat, prn[MAXSAT];
	char *p = (char *)buff, *q, *s, sum;

	trace(3, "outnmea_gsa:\n");

	if (sol->stat <= SOLQ_NONE) {
		p += sprintf(p, "$GPGSA,A,1,,,,,,,,,,,,,,,");
		for (q = (char *)buff + 1, sum = 0; *q; q++) sum ^= *q;
		p += sprintf(p, "*%02X%c%c", sum, 0x0D, 0x0A);
		return p - (char *)buff;
	}
	/* GPGSA: gps/sbas */
	for (sat = 1, nsat = 0; sat <= MAXSAT&&nsat<12; sat++) {
		if (!ssat[sat - 1].available || ssat[sat - 1].elevation <= 0.0) continue;
		sys = satIndex2sys(sat);
		prn[nsat] = satIndex2satno(sat);
		if (sys != SYS_GPS&&sys != SYS_SBS) continue;
		if (sys == SYS_SBS) prn[nsat] += 33 - MINPRNSBS;
		azel[0 + nsat * 2] = ssat[sat - 1].azimuth;
		azel[1 + nsat * 2] = ssat[sat - 1].elevation;
		nsat++;
	}
	if (nsat>0) {
		s = p;
		p += sprintf(p, "$GPGSA,A,%d", sol->stat <= 0 ? 1 : 3);
		for (i = 0; i<12; i++) {
			if (i<nsat) p += sprintf(p, ",%02d", prn[i]);
			else        p += sprintf(p, ",");
		}
		dops(nsat, azel, 0.0, dop);
		p += sprintf(p, ",%3.1f,%3.1f,%3.1f,1", dop[1], dop[2], dop[3]);
		for (q = s + 1, sum = 0; *q; q++) sum ^= *q; /* check-sum */
		p += sprintf(p, "*%02X%c%c", sum, 0x0D, 0x0A);
	}
	/* GLGSA: glonass */
	for (sat = 1, nsat = 0; sat <= MAXSAT&&nsat<12; sat++) {
		if (!ssat[sat - 1].available || ssat[sat - 1].elevation <= 0.0) continue;
		sys = satIndex2sys(sat);
		prn[nsat] = satIndex2satno(sat);
		if (sys != SYS_GLO) continue;
		azel[0 + nsat * 2] = ssat[sat - 1].azimuth;
		azel[1 + nsat * 2] = ssat[sat - 1].elevation;
		nsat++;
	}
	if (nsat>0) {
		s = p;
		p += sprintf(p, "$GLGSA,A,%d", sol->stat <= 0 ? 1 : 3);
		for (i = 0; i<12; i++) {
			if (i<nsat) p += sprintf(p, ",%02d", prn[i] + 64);
			else        p += sprintf(p, ",");
		}
		dops(nsat, azel, 0.0, dop);
		p += sprintf(p, ",%3.1f,%3.1f,%3.1f,2", dop[1], dop[2], dop[3]);
		for (q = s + 1, sum = 0; *q; q++) sum ^= *q; /* check-sum */
		p += sprintf(p, "*%02X%c%c", sum, 0x0D, 0x0A);
	}
	/* GAGSA: galileo */
	for (sat = 1, nsat = 0; sat <= MAXSAT&&nsat<12; sat++) {
		if (!ssat[sat - 1].available || ssat[sat - 1].elevation <= 0.0) continue;
		sys = satIndex2sys(sat);
		prn[nsat] = satIndex2satno(sat);
		if (sys != SYS_GAL) continue;
		azel[0 + nsat * 2] = ssat[sat - 1].azimuth;
		azel[1 + nsat * 2] = ssat[sat - 1].elevation;
		nsat++;
	}
	if (nsat>0) {
		s = p;
		p += sprintf(p, "$GAGSA,A,%d", sol->stat <= 0 ? 1 : 3);
		for (i = 0; i<12; i++) {
			if (i<nsat) p += sprintf(p, ",%02d", prn[i]);
			else        p += sprintf(p, ",");
		}
		dops(nsat, azel, 0.0, dop);
		p += sprintf(p, ",%3.1f,%3.1f,%3.1f,3", dop[1], dop[2], dop[3]);
		for (q = s + 1, sum = 0; *q; q++) sum ^= *q; /* check-sum */
		p += sprintf(p, "*%02X%c%c", sum, 0x0D, 0x0A);
	}
	return p - (char *)buff;
}
/* output solution in the form of nmea GSV sentence --------------------------*/
extern int outnmea_gsv(unsigned char *buff, const sol_t *sol,
	const ssat_t *ssat)
{
	double az, el, snr;
	int i, j, k, n, sat, prn, sys, nmsg, sats[MAXSAT];
	char *p = (char *)buff, *q, *s, sum;

	trace(3, "outnmea_gsv:\n");

	if (sol->stat <= SOLQ_NONE) {
		p += sprintf(p, "$GPGSV,1,1,0,,,,,,,,,,,,,,,,");
		for (q = (char *)buff + 1, sum = 0; *q; q++) sum ^= *q;
		p += sprintf(p, "*%02X%c%c", sum, 0x0D, 0x0A);
		return p - (char *)buff;
	}
	/* GPGSV: gps/sbas */
	for (sat = 1, n = 0; sat<MAXSAT&&n<12; sat++) {
		sys = satIndex2sys(sat);
		prn = satIndex2satno(sat);
		if (sys != SYS_GPS&&sys != SYS_SBS) continue;
		if (ssat[sat - 1].available&&ssat[sat - 1].elevation>0.0) sats[n++] = sat;
	}
	nmsg = n <= 0 ? 0 : (n - 1) / 4 + 1;

	for (i = k = 0; i<nmsg; i++) {
		s = p;
		p += sprintf(p, "$GPGSV,%d,%d,%02d", nmsg, i + 1, n);

		for (j = 0; j<4; j++, k++) {
			if (k<n) {
				sys = satIndex2sys(sats[k]);
				prn = satIndex2satno(sats[k]);
				if (sys == SYS_SBS) prn += 33 - MINPRNSBS;
				az = ssat[sats[k] - 1].azimuth; if (az<0.0) az += 360.0;
				el = ssat[sats[k] - 1].elevation;
				snr = ssat[sats[k] - 1].snr[0] * 0.25;
				p += sprintf(p, ",%02d,%02.0f,%03.0f,%02.0f", prn, el, az, snr);
			}
			else p += sprintf(p, ",,,,");
		}
		p += sprintf(p, ",1"); /* L1C/A */
		for (q = s + 1, sum = 0; *q; q++) sum ^= *q; /* check-sum */
		p += sprintf(p, "*%02X%c%c", sum, 0x0D, 0x0A);
	}
	/* GLGSV: glonass */
	for (sat = 1, n = 0; sat<MAXSAT&&n<12; sat++) {
		sys = satIndex2sys(sat);
		prn = satIndex2satno(sat);
		if (sys != SYS_GLO) continue;
		if (ssat[sat - 1].available&&ssat[sat - 1].elevation>0.0) sats[n++] = sat;
	}
	nmsg = n <= 0 ? 0 : (n - 1) / 4 + 1;

	for (i = k = 0; i<nmsg; i++) {
		s = p;
		p += sprintf(p, "$GLGSV,%d,%d,%02d", nmsg, i + 1, n);

		for (j = 0; j<4; j++, k++) {
			if (k<n) {
				prn = satIndex2satno(sats[k]);
				prn += 64; /* 65-99 */
				az = ssat[sats[k] - 1].azimuth; if (az<0.0) az += 360.0;
				el = ssat[sats[k] - 1].elevation;
				snr = ssat[sats[k] - 1].snr[0] * 0.25;
				p += sprintf(p, ",%02d,%02.0f,%03.0f,%02.0f", prn, el, az, snr);
			}
			else p += sprintf(p, ",,,,");
		}
		p += sprintf(p, ",1"); /* L1C/A */
		for (q = s + 1, sum = 0; *q; q++) sum ^= *q; /* check-sum */
		p += sprintf(p, "*%02X%c%c", sum, 0x0D, 0x0A);
	}
	/* GAGSV: galileo */
	for (sat = 1, n = 0; sat<MAXSAT&&n<12; sat++) {
		sys = satIndex2sys(sat);
		prn = satIndex2satno(sat);
		if (sys != SYS_GAL) continue;
		if (ssat[sat - 1].available&&ssat[sat - 1].elevation>0.0) sats[n++] = sat;
	}
	nmsg = n <= 0 ? 0 : (n - 1) / 4 + 1;

	for (i = k = 0; i<nmsg; i++) {
		s = p;
		p += sprintf(p, "$GAGSV,%d,%d,%02d", nmsg, i + 1, n);

		for (j = 0; j<4; j++, k++) {
			if (k<n) {
				prn = satIndex2satno(sats[k]); /* 1-36 */
				az = ssat[sats[k] - 1].azimuth; if (az<0.0) az += 360.0;
				el = ssat[sats[k] - 1].elevation;
				snr = ssat[sats[k] - 1].snr[0] * 0.25;
				p += sprintf(p, ",%02d,%02.0f,%03.0f,%02.0f", prn, el, az, snr);
			}
			else p += sprintf(p, ",,,,");
		}
		p += sprintf(p, ",7"); /* L1BC */
		for (q = s + 1, sum = 0; *q; q++) sum ^= *q; /* check-sum */
		p += sprintf(p, "*%02X%c%c", sum, 0x0D, 0x0A);
	}
	return p - (char *)buff;
}
