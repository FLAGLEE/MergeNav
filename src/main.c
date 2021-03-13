#include "pwjppp.h"
#include "configuration.h"

pppsvr_t *pppsvr=NULL;            // ppp server struct
nav_t navppp = { 0 };
pcvs_t Global_PCVs = { 0 };
sta_t* stainfo = NULL;
stream_t monistr;           // monitor stream
short int secureFlag = 0;
int corNum = 2;//1 for brdc and ssr ; 2 reserved
int extObsNum = 1;
int outNum=1;
int corThreadFlag = 0;
int obsThreadFlag = 0;
int test_n = 0;

lock_t global_lock = { 0 };

ssrion_t global_ssrion = { 0 };

char pephdir[300] = "E:\\ppp\\peph\\";
char tropdir[300] = "E:\\ppp\\trop\\";
char rawdir[300] = "E:\\ppp\\raw\\";
char obsdir[300] = "E:\\ppp\\obs\\";
char brdmdir[300] = "E:\\ppp\\brdm\\";


char ephprefix[100] = "brdc";
double verbrdc = 3.0;

int outputIntervel = 30;
int maxOutage = 1800;

int prefixflag = 0;	/* 0:BRDC, 1: self-defined */
int typeflag = 0;	/* 0:cmd, 1:file, 2:stream*/
int filecountflag = 0;


int loadoptxt(int typeflag, char File[], char info[][100], int strs[], int* corNum) {
	
	char* p, buff[200];
	int i;
	int n = 0;

	FILE* Fp = fopen(File, "r");
	
	while (fgets(buff, 200, Fp))
	{
		if (strlen(buff) < 5 || buff[0] == '%')
			continue;
		for (i = 0, p = strtok(buff, " \n"); p; i++, p = strtok(NULL, " \n"))
		{
			// first column, BRDC or OUT
			if (i == 0)
			{
				if (!strstr(buff, "BRDC") && !strstr(buff, "OUT"))
				{
					printf("Segment is wrong, %s\n", buff);
					fclose(Fp);
					sleepms(3000);
					exit(0);
				}
			}
			// second column, path
			else if (i == 1)
			{
				if (strstr(buff, "BRDC"))
				{
					strcpy(info[n], p);
				}
				else if (strstr(buff, "OUT"))
				{
					strcpy(brdmdir, p);
				}
			}
			// third column, type
			else {

				if (strstr(buff, "BRDC"))
				{
					strs[n] = atoi(p);
				}
				else if (strstr(buff, "OUT"))
				{
					verbrdc = atof(p);
				}
			}

		}
		// num of paths
		if (strstr(buff, "BRDC"))
		{
			n++;
		}

	}
	fclose(Fp);
	corNum = n;

	if (n) {

		if (typeflag == 2) {
			printf("read stream path: \n");
		}
		else if (typeflag == 1) {
			printf("read file path: \n");
		}
		
		for (int i = 0; i < n; i++) {
			printf("\t%s\n", info[i]);
		}
		return n;
	}

}


int main(int argc, char *argv[])
{
	double nmeapos[3];
	int i, strs[MAXSTRINPPP] = { 0 };
	char* paths[MAXSTRINPPP], * cmds[MAXSTRINPPP] = { 0 }, * cmds_periodic[MAXSTRINPPP] = { 0 }, * rcvopts[MAXSTRINPPP] = { 0 }, info[MAXSTRINPPP][100];

	int formats[MAXSTRINPPP] = { 0 };

	prcopt_t prcopt = prcopt_default;
	solopt_t solopt = solopt_default;
	int cycle = 10;
	int buffsize = 32768;
	int navsel = 3;
	int nmeacycle = 1000;
	int nmeareq = 2;
	char errmsg[100] = { 0 };
	char cmdm8t[1024] = "";

	

	eph_t  eph0 = { 0,-1,-1 };
	geph_t geph0 = { 0,-1 };
	seph_t seph0 = { 0 };
	ephh_t nhead0 = { 0 };

	char* p, buff[200], currDir[MAX_PATH], streamFile[MAX_PATH], fileFile[MAX_PATH];
	int n = 0;

	

	initlock(&global_lock);

	GetCurrentDirectoryA(MAX_PATH, currDir);


	printf("\nStart receiving broadcast ephemeris...\n");

	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-p") && i + 1 < argc) {
			printf("Self-defined prefix is %s\n\n", argv[i+1]);
			strcpy(ephprefix, argv[++i]);
			prefixflag = 1;
			break;
		}
	}
	if (!prefixflag) {
		printf("Default prefix is brdc\n\n");
	}


	/* load stream options from configuration file */
	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-s")) {
			typeflag = 2;
			printf("Read nav from stream\n\n");
			sprintf(streamFile, "%s\\%s", currDir, "stream.txt");
			if (!loadoptxt(typeflag, streamFile, info, strs, &corNum)) {
				printf("no input path\n");
				return -1;
			}
			
		}
		else if (!strcmp(argv[i], "-f")) {
			typeflag = 1;
			printf("Read nav from file\n\n");
			sprintf(fileFile, "%s\\%s", currDir, "file.txt");
			if (!loadoptxt(typeflag, fileFile, info, strs, &corNum)) {
				printf("no input path\n");
				return -1;
			}
			
		}
		else if (!strcmp(argv[i], "-o") && i + 1 < argc) {
			strcpy(brdmdir, argv[++i]);
			printf("output path is %s\n", brdmdir);
			verbrdc = 2.0;
		}
		else if (!strcmp(argv[i], "-p") && i + 1 < argc) { ++i; continue; }
		else if (n < MAXSTRINPPP) {
			strcpy(info[n], argv[i]);
			printf("file path %d is %s\n", n+1, info[n]);
			strs[n++] = 2;
		}
	}
	if (!typeflag) {
		corNum = n;
	}


	for (i = 0; i< MAXSTRINPPP; i++) {
		paths[i] = (char *)malloc(256 * sizeof(char));
		paths[i][0] = '\0';
	}
	for (i = 0; i < corNum; i++)
	{
		strcpy(paths[i], info[i]);
		formats[i] = STRFMT_RTCM3;

		cmds[i] = (char *)malloc(256 * sizeof(char));
		rcvopts[i] = (char *)malloc(256 * sizeof(char));
		cmds[i][0] = rcvopts[i][0] = '\0';
	}
	pppsvr = (pppsvr_t*)malloc(sizeof(pppsvr_t) * (corNum));

	for (i = 0; i < corNum; i++)
	{
		rtksvrinit(pppsvr + i);
	}

	if (!(navppp.eph = (eph_t*)malloc(sizeof(eph_t) * (MAXSAT) * 2)) ||
		!(navppp.geph = (geph_t*)malloc(sizeof(geph_t) * NSATGLO * 2)) ||
		!(navppp.seph = (seph_t*)malloc(sizeof(seph_t) * NSATSBS * 2))) {
		return 0;
	}

	for (i = 0; i < (MAXSAT) * 2; i++) navppp.eph[i] = eph0;
	for (i = 0; i < NSATGLO * 2; i++) navppp.geph[i] = geph0;
	for (i = 0; i < NSATSBS * 2; i++) navppp.seph[i] = seph0;
	navppp.n = (MAXSAT) * 2;
	navppp.ng = NSATGLO * 2;
	navppp.ns = NSATSBS * 2;
	navppp.nhead = nhead0;

	
	//printf("%d file\n", corNum);
	printf("\nStart fusion...\n");
	rtksvrstart(pppsvr, cycle, buffsize, strs, paths, formats, navsel, cmds, cmds_periodic, rcvopts, nmeacycle, nmeareq, nmeapos,&prcopt, &solopt, &monistr, errmsg);
	//system("pause");


	for (i = 0; i < MAXSTRINPPP; i++)
	{
		free(paths[i]);
	}
	for (i = 0; i < MAXSTRINPPP; i++) 
	{
		free(rcvopts[i]);
		free(cmds[i]);
	}
	//printf("pthread join\n");

	for (int i = 0; i < corNum; i++) {
		if (pppsvr[i].thread > 0) {
			printf("thread is %d\n", pppsvr[i].thread);
			WaitForSingleObject(pppsvr[i].thread, INFINITE);


		}
	}
	
	//if (typeflag == 2) {
	//	if (pppsvr[0].thread > 0) {
	//		WaitForSingleObject(pppsvr[0].thread, INFINITE);
	//		//pthread_join(pppsvr[0].thread, NULL);

	//	}
	//}
	//else {
	//	if (pppsvr[0].thread > 0) {
	//		WaitForSingleObject(pppsvr[0].thread, INFINITE);
	//		//pthread_join(pppsvr[0].thread, NULL);

	//	}
	//}

	
		
	printf("Fusion finish.\n");
	return 0;

}
