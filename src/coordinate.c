#include "pwjppp.h"

#define TEMPA   6378245.0
#define TEMPEE  0.00669342162296594323

static const double velocities[][7] = {
	{ 72, 75, 36, 39, -26.4, -6.1, 14.0 },
	{ 72, 75, 39, 42, -30.7, -0.6, 8.7 },
	{ 75, 78, 36, 39, -28.8, -6.2, 14.4 },
	{ 75, 78, 39, 42, -32.3, -2.8, 10.6 },
	{ 78, 81, 30, 33, -31.8, -3.7, 12.3 },
	{ 78, 81, 33, 36, -30.7, -2.9, 10.0 },
	{ 78, 81, 36, 39, -29.1, -5.5, 12.1 },
	{ 78, 81, 39, 42, -31.8, -4.4, 10.0 },
	{ 78, 81, 42, 45, -30.8, 1.6, 1.5 },
	{ 81, 84, 27, 30, -36.4, -6.7, 15.9 },
	{ 81, 84, 30, 33, -34, -4.8, 13.8 },
	{ 81, 84, 33, 36, -29.7, -4.1, 9.9 },
	{ 81, 84, 36, 39, -27.3, -4.1, 7.9 },
	{ 81, 84, 39, 42, -31.9, -2.5, 5.7 },
	{ 81, 84, 42, 45, -30.6, -0.2, 2.5 },
	{ 81, 84, 45, 48, -29.4, 2.4, -0.9 },
	{ 84, 87, 27, 30, -38.8, -8.5, 18.1 },
	{ 84, 87, 30, 33, -38.3, -6.7, 13.4 },
	{ 84, 87, 33, 36, -33.1, -4.8, 8.7 },
	{ 84, 87, 36, 39, -28.0, -3.0, 4.0 },
	{ 84, 87, 39, 42, -31.2, -3.8, 5.4 },
	{ 84, 87, 42, 45, -31.6, -1.8, 2.7 },
	{ 84, 87, 45, 48, -28.9, 0.5, -0.5 },
	{ 84, 87, 48, 51, -28.2, -0.1, -0.2 },
	{ 87, 90, 27, 30, -41.2, -9.4, 17.0 },
	{ 87, 90, 30, 33, -44.4, -7.5, 11.8 },
	{ 87, 90, 33, 36, -43.5, -6.7, 8.7 },
	{ 87, 90, 36, 39, -28.2, -4.6, 4.2 },
	{ 87, 90, 39, 42, -29.5, -3.2, 3.1 },
	{ 87, 90, 42, 45, -29.8, -2.3, 1.4 },
	{ 87, 90, 45, 48, -29.8, -2.4, 1.0 },
	{ 87, 90, 48, 51, -28.7, -2.0, 0.7 },
	{ 90, 93, 27, 30, -44.3, -8.7, 11.5 },
	{ 90, 93, 30, 33, -47.3, -7.0, 7.1 },
	{ 90, 93, 33, 36, -56.1, -6.5, 3.9 },
	{ 90, 93, 36, 39, -32.6, -5.8, 4.7 },
	{ 90, 93, 39, 42, -30.6, -2.9, -0.2 },
	{ 90, 93, 42, 45, -31.5, -2.8, 0.3 },
	{ 90, 93, 45, 48, -29.4, -2.3, 0.2 },
	{ 93, 96, 27, 30, -46.7, -4.9, -0.1 },
	{ 93, 96, 30, 33, -51.9 - 4.5, -1.9 },
	{ 93, 96, 33, 36, -56.0, -2.7, -4.3 },
	{ 93, 96, 36, 39, -31.6, -2.6, -1.8 },
	{ 93, 96, 39, 42, -30.1, -2.2, -2.4 },
	{ 93, 96, 42, 45, -31.3, -2.4, -1.9 },
	{ 96, 99, 21, 24, -26.7, 1.3, -13.5 },
	{ 96, 99, 24, 27, -41.7, 0.5, -14.0 },
	{ 96, 99, 27, 30, -50.0, -3.3, -6.9 },
	{ 96, 99, 30, 33, -44.8, -6.4, -2.3 },
	{ 96, 99, 33, 36, -33.8, -3.8, -3.3 },
	{ 96, 99, 36, 39, -30.1, -1.1, -5.5 },
	{ 96, 99, 39, 42, -30.3, -1.2, -5.1 },
	{ 99, 102, 21, 24, -28.6, 0.0, -15 },
	{ 99, 102, 24, 27, -31.3, 2.2, -19 },
	{ 99, 102, 27, 30, -38.2, 2.1, -19.2 },
	{ 99, 102, 30, 33, -43.2, -1.5, -13.3 },
	{ 99, 102, 33, 36, -40.4, -5.3, -5.6 },
	{ 99, 102, 36, 39, -34.1, -3.6, -5.7 },
	{ 99, 102, 39, 42, -29.8, -1.6, -6.5 },
	{ 99, 102, 42, 45, -29.8, -1.6, -6.5 },
	{ 102, 105, 21, 24, -33.7, -3.8, -12 },
	{ 102, 105, 24, 27, -34.0, -0.6, -17.3 },
	{ 102, 105, 27, 30, -35.7, -1.8, -13.8 },
	{ 102, 105, 30, 33, -35.0, -3.2, -10.6 },
	{ 102, 105, 33, 36, -36.3, -4.8, -7.7 },
	{ 102, 105, 36, 39, -34.6, -4.2, -7.1 },
	{ 102, 105, 39, 42, -30.1, -3.1, -6.9 },
	{ 105, 108, 21, 24, -32.1, -5.2, -13.0 },
	{ 105, 108, 24, 27, -33.3, -4.7, -12.8 },
	{ 105, 108, 27, 30, -34.5, -4.0, -12.0 },
	{ 105, 108, 30, 33, -34.1, -3.7, -11.2 },
	{ 105, 108, 33, 36, -34.4, -3.6, -11.1 },
	{ 105, 108, 36, 39, -32.5, -3.7, -9.2 },
	{ 105, 108, 39, 42, -30.2, -3.3, -8.7 },
	{ 105, 108, 42, 45, -30.2, -3.3, -8.7 },
	{ 108, 111, 18, 21, -29.9, -6.5, -14.0 },
	{ 108, 111, 21, 24, -31.2, -6.3, -13.5 },
	{ 108, 111, 24, 27, -32.5, -6.4, -13.6 },
	{ 108, 111, 27, 30, -32.0, -5.4, -12.7 },
	{ 108, 111, 30, 33, -33.2, -5.9, -11.4 },
	{ 108, 111, 33, 36, -32.9, -5.1, -11.0 },
	{ 108, 111, 36, 39, -32.0, -5.7, -9.6 },
	{ 108, 111, 39, 42, -29.5, -4.4, -8.7 },
	{ 108, 111, 42, 45, -29.4, -4.4, -8.7 },
	{ 111, 114, 21, 24, -31.0, -7.7, -14.0 },
	{ 111, 114, 24, 27, -31.1, -7.4, -13.5 },
	{ 111, 114, 27, 30, -31.6, -6.5, -13.7 },
	{ 111, 114, 30, 33, -32.7, -6.8, -12.1 },
	{ 111, 114, 33, 36, -31.2, -6.1, -11.2 },
	{ 111, 114, 36, 39, -31.1, -5.5, -11.0 },
	{ 111, 114, 39, 42, -29.7, -5.1, -10.0 },
	{ 111, 114, 42, 45, -27.1, -3.7, -8.9 },
	{ 114, 117, 21, 24, -30.1, -8.4, -15.3 },
	{ 114, 117, 24, 27, -31.4, -9.1, -15.4 },
	{ 114, 117, 27, 30, -30.5, -8.0, -13.4 },
	{ 114, 117, 30, 33, -31.5, -8.5, -11.6 },
	{ 114, 117, 33, 36, -31.0, -7.3, -11.6 },
	{ 114, 117, 36, 39, -30.6, -6.6, -11.2 },
	{ 114, 117, 39, 42, -29.3, -5.9, -10.7 },
	{ 114, 117, 42, 45, -27.2, -4.8, -9.8 },
	{ 114, 117, 45, 48, -25.5, -5.6, -8.3 },
	{ 114, 117, 48, 51, -26.1, -5.0, -8.2 },
	{ 117, 120, 21, 24, -30.5, -9.3, -15.2 },
	{ 117, 120, 24, 27, -30.8, -9.3, -16.2 },
	{ 117, 120, 27, 30, -30.3, -7.9, -16.1 },
	{ 117, 120, 30, 33, -30.1, -9.4, -12.0 },
	{ 117, 120, 33, 36, -29.5, -8.4, -11.7 },
	{ 117, 120, 36, 39, -29.8, -8.4, -10.8 },
	{ 117, 120, 39, 42, -27.7, -6.9, -10.3 },
	{ 117, 120, 42, 45, -26.9, -7.1, -9.4 },
	{ 117, 120, 45, 48, -25.5, -6.3, -8.5 },
	{ 117, 120, 48, 51, -25.9, -5.4, -8.4 },
	{ 120, 123, 24, 27, -29.9, -9.2, -16.6 },
	{ 120, 123, 27, 30, -30.0, -10.1, -14.8 },
	{ 120, 123, 30, 33, -29.9, -10.1, -13.2 },
	{ 120, 123, 33, 36, -30.7, -9.6, -13.0 },
	{ 120, 123, 36, 39, -27.5, -7.9, -11.7 },
	{ 120, 123, 39, 42, -25.8, -7.9, -10.0 },
	{ 120, 123, 42, 45, -26.0, -7.7, -9.5 },
	{ 120, 123, 45, 48, -24.9, -6.4, -9.4 },
	{ 120, 123, 48, 51, -23.9, -6.3, -7.9 },
	{ 120, 123, 51, 54, -23.2, -5.5, -7.5 },
	{ 123, 126, 39, 42, -24.8, -7.9, -10.3 },
	{ 123, 126, 42, 45, -25.1, -8.0, -10.2 },
	{ 123, 126, 45, 48, -23.7, -7.3, -8.5 },
	{ 123, 126, 48, 51, -23.1, -6.1, -8.2 },
	{ 123, 126, 51, 54, -22.8, -6.3, -7.7 },
	{ 126, 129, 39, 42, -24.5, -7.8, -10.4 },
	{ 126, 129, 42, 45, -23.1, -7.5, -10.6 },
	{ 126, 129, 45, 48, -23.8, -8.0, -9.5 },
	{ 126, 129, 48, 51, -23.4, -6.6, -9.2 },
	{ 126, 129, 51, 54, -22.2, -7.5, -7.5 },
	{ 129, 132, 42, 45, -23.7, -8.7, -10.8 },
	{ 129, 132, 45, 48, -20.8, -8.1, -8.9 },
	{ 129, 132, 48, 51, -21.9, -6.2, -9.6 },
	{ 132, 135, 45, 48, -25.2, -6.6, -13.7 },
	{ 132, 135, 48, 51, -21.3, -8.6, -9.4 }
};
extern double Radian2Angle(double radian)
{
	return radian*R2D;
}
extern double Angle2Radian(double Angle)
{
	return Angle*D2R;
}

//pos[0]:b, pos[1]:l, pos[3]:h,   b/l is angle//    x[3]:xyz
extern void CoorGeo2CoorCar(const double *pos, double *x)
{
	double b, l, h;
	b = Angle2Radian(pos[0]); l = Angle2Radian(pos[1]); h = pos[2];
	double W = sqrt(1 - pow(sin(b), 2)*EE_WGS84);
	double NN = A_WGS84 / W;
	x[0] = (NN + h)*cos(b)*cos(l);
	x[1] = (NN + h)*cos(b)*sin(l);
	x[2] = ((1 - EE_WGS84)*NN + h)*sin(b);
}

//azel[0]:az  azel[1]:el
extern void CoorPol2CoorCar(const double r, const double *azel, double *x)
{
	double az, el;
	az = Angle2Radian(azel[0]); el = Angle2Radian(azel[1]);
	x[0] = r*cos(el)*cos(az);
	x[1] = r*cos(el)*sin(az);
	x[2] = r*sin(el);
}

extern void CoorCar2CoorGeo(const double *x, double *pos)
{
	double b, l, h;
	l = (pow(x[0], 2) + pow(x[1], 2))>1e-12 ? Radian2Angle(atan2(x[1], x[0])) : 0.0;

	double B1 = atan2(x[2], sqrt(pow(x[0], 2) + pow(x[1], 2)));  //����γ�ȵĳ�ʼֵ
	double B2; //���ڴ��Ŵ���γ�ȵ���ֵ
	double W;  //�������������ڴ����м���
	double NN; //î��Ȧ���ʰ뾶

	for (;;)    //����γ�ȵĵ������⣬ֱ������Ϊ0.00000001��
	{
		W = sqrt(1 - EE_WGS84 * pow(sin(B1), 2));	                                                   //�����ز���ѧ��P99��ʽ4-9		
		NN = A_WGS84 / W;                                                                             //�����ز���ѧ��P107��ʽ4-64
		B2 = atan2(x[2] + NN * EE_WGS84 * sin(B1), sqrt(pow(x[0], 2) + pow(x[1], 2))); //�����ز���ѧ��P103��ʽ4-32
		if (fabs(B2 - B1)*R2D < 0.00000001)
		{
			b = (pow(x[0], 2) + pow(x[1], 2))>1e-12 ? Radian2Angle(B2) : (x[2]>0.0 ? 90.0 : -90.0);
			break;
		}
		else
		{
			B1 = B2;
		}
	}
	W = sqrt(1 - EE_WGS84 * pow(sin(Angle2Radian(b)), 2));
	NN = A_WGS84 / W;
	h = sqrt(pow(x[0], 2) + pow(x[1], 2)) / cos(Angle2Radian(b)) - NN;
	pos[0] = b; pos[1] = l; pos[2] = h;
}

extern void CoorPol2CoorGeo(const double r, const double *azel, double *pos)
{
	double x[3] = { 0.0 };
	CoorPol2CoorCar(r, azel, x);
	CoorCar2CoorGeo(x, pos);
}


extern void CoorCar2CoorPol(const double *x, double *r, double *azel)
{
	*r = sqrt(pow(x[0], 2) + pow(x[1], 2) + pow(x[2], 2));
	azel[0] = Radian2Angle(atan2(x[1], x[0]));
	azel[1] = Radian2Angle(atan2(x[2], sqrt(pow(x[0], 2) + pow(x[1], 2))));
}

extern void CoorGeo2CoorPol(const double *pos, double *r, double *azel)        //������д��ģ��
{
	double x[3] = { 0.0 };
	CoorGeo2CoorCar(pos, x);
	CoorCar2CoorPol(x, r, azel);
}


extern  void CoorCar2CoorENU(const double *x, const double *dx, double *enu)
{
	double pos[3] = { 0 };
	double E[9];
	CoorCar2CoorGeo(x, pos);
	xyz2enu(pos, E);
	matmul("NN", 3, 1, 3, 1, E, dx, 0, enu);
}


extern void CoorENU2CoorCar(const double* pos, const double* enu, double* dx)
{
	double E[9];

	xyz2enu(pos, E);
	matmul("TN", 3, 1, 3, 1.0, E, enu, 0.0, dx);
}

extern void CoorGeo2CoorENU(const double *pos, const double *dx, double *enu)        //������д��ģ��
{
	double E[9];
	xyz2enu(pos, E);
	matmul("NN", 3, 1, 3, 1, E, dx, 0, enu);
}

extern void CoorPol2CoorENU(const double r, const double *azel, const double *dx, double *enu)        //������д��ģ��
{
	double x[3] = { 0.0 };
	CoorPol2CoorCar(r, azel, x);
	CoorCar2CoorENU(x, dx, enu);
}



extern double co_distance(double *x, double *y)
{
	return sqrt(pow(x[0] - y[0], 2) + pow(x[1] - y[1], 2) + pow(x[2] - y[2], 2));
}

extern void covCoorCar(const double *pos, const double *Q, double *P)
{
	double E[9], EQ[9];
	xyz2enu(pos, E);
	matmul("TN", 3, 3, 3, 1.0, E, Q, 0.0, EQ);
	matmul("NN", 3, 3, 3, 1.0, EQ, E, 0.0, P);
}


//2010 to current year
static double epochTransform(int year, double x, double dx)
{
	return x + dx*(year - 2010);
}

static void velocitiesTransform(double *pos, double *v)
{
	int i, j;
	for (i = 0; i<sizeof(velocities) / sizeof(velocities[0]); i++)
	{
		if (pos[1] >= velocities[i][0] && pos[1] <= velocities[i][1] &&
			pos[0] >= velocities[i][2] && pos[0] <= velocities[i][3])
		{
			for (j = 0; j<3; j++)
				v[j] = velocities[i][4 + j];
		}
	}
}

static int outOfChina(double *pos)
{
	return (pos[1]<72.004 || pos[1]>137.8347) || ((pos[0]<0.8293 || pos[0]>55.8271) || 0);
}
//current year
extern void pppITRFToCgcs2000(int year, const double *x, double *y)
{
	double pos[3], v[3] = { 0 };
	double T1, T2, T3, R1, R2, R3, D;
	int i;
	T1 = epochTransform(year, ITRF_T1, ITRF_dT1) / 1000.0;
	T2 = epochTransform(year, ITRF_T2, ITRF_dT2) / 1000.0;
	T3 = epochTransform(year, ITRF_T3, ITRF_dT3) / 1000.0;
	D = epochTransform(year, ITRF_D, ITRF_dD) / 1000.0;
	R1 = epochTransform(year, ITRF_R1, ITRF_dR1) / 1000.0;
	R2 = epochTransform(year, ITRF_R2, ITRF_dR2) / 1000.0;
	R3 = epochTransform(year, ITRF_R3, ITRF_dR3) / 1000.0;

	y[0] = x[0] + T1 + D*x[0] - R3*x[1] + R2*x[2];
	y[1] = x[1] + T2 + R3*x[0] + D*x[1] - R1*x[2];
	y[2] = x[2] + T3 - R2*x[0] + R1*x[1] + D*x[2];
	CoorCar2CoorGeo(y, pos);
	velocitiesTransform(pos, v);
	for (i = 0; i < 3; i++)
		y[i] = y[i] - v[i] * (year - 2000) / 1000;

}


static double transformLat(double *y)
{
	double ret;
	ret = -100 + 2.0*y[1] + 3.0*y[0] + 0.2*y[0] * y[0] + 0.1*y[0] * y[1] + 0.2*sqrt(fabs(y[1]));
	ret += (20.0*sin(6.0*y[1] * PI) + 20.0*sin(2.0*y[1] * PI))*2.0 / 3.0;
	ret += (20.0*sin(y[0] * PI) + 40.0*sin(y[0] / 3.0*PI))*2.0 / 3.0;
	ret += (160.0*sin(y[0] / 12.0*PI) + 320 * sin(y[0] * PI / 30.0))*2.0 / 3.0;
	return ret;
}

static double transformLng(double *y)
{
	double ret;
	ret = 300.0 + y[1] + 2.0*y[0] + 0.1*y[1] * y[1] + 0.1*y[1] * y[0] + 0.1*sqrt(fabs(y[1]));
	ret += (20.0*sin(6.0*y[1] * PI) + 20.0*sin(2.0*y[1] * PI))*2.0 / 3.0;
	ret += (20.0*sin(y[1] * PI) + 40.0*sin(y[1] / 3.0*PI))*2.0 / 3.0;
	ret += (150.0*sin(y[1] / 12.0*PI) + 300.0*sin(y[1] / 30.0*PI))*2.0 / 3.0;
	return ret;
}
//x:xyz,y:lat,long
static void cgcs2000ToGCJ02(const double *x, double *y)
{
	double pos[3], bl[2];
	int i;
	double dlat, dlng, radlat, magic, sqrtmagic;
	CoorCar2CoorGeo(x, pos);
	if (outOfChina(pos))
	{
		for (i = 0; i<2; i++)
			y[i] = pos[i];
		return;
	}
	bl[0] = pos[0] - 35.0;
	bl[1] = pos[1] - 105.0;
	dlat = transformLat(bl);
	dlng = transformLng(bl);
	radlat = pos[0] * D2R;
	magic = sin(radlat);
	magic = 1 - TEMPEE*magic*magic;
	sqrtmagic = sqrt(magic);
	dlat = (dlat*180.0) / ((TEMPA*(1 - TEMPEE)) / (magic*sqrtmagic)*PI);
	dlng = (dlng*180.0) / (TEMPA / sqrtmagic*cos(radlat)*PI);
	y[0] = pos[0] + dlat;
	y[1] = pos[1] + dlng;

}

//x:lat,long,y:lat,long
static void GCJ02ToBD09(const double *x, double *y)
{
	double z, theta;
	z = sqrt(x[1] * x[1] + x[0] * x[0]) + 0.00002*sin(x[0] * PI*3000.0 / 180.0);
	theta = atan2(x[0], x[1]) + 0.000003*cos(x[1] * PI*3000.0 / 180.0);
	y[1] = z*cos(theta) + 0.0065;
	y[0] = z*sin(theta) + 0.006;
}

//x:xyz,y:lat,long
extern void cgcs2000ToBD09(const double *x, double *y)
{
	double gcj[2];
	//WGS84->GCJ02
	cgcs2000ToGCJ02(x, gcj);
	//GCJ02->BD09
	GCJ02ToBD09(gcj, y);
}
