#include "pwjppp.h"


extern void matcpy(double *A, const double *B, int m, int n)
{
	memcpy(A, B, sizeof(double)*m*n);
}

extern double *mat(int m, int n)
{
	double *p;
	if (m <= 0 || n <= 0) return NULL;
	if (!(p = (double *)malloc(sizeof(double)*m*n))) {
		printf("matrix memory allocation error: m=%d,n=%d\n", m, n);
	}
	return p;
}

extern int *imat(int m, int n)
{
	int *p;

	if (m <= 0 || n <= 0) return NULL;
	if (!(p = (int *)malloc(sizeof(int)*m*n))) {
		printf("integer matrix memory allocation error: m=%d,n=%d\n", m, n);
	}
	return p;
}

extern double *zeros(int m, int n)
{
	double *p;

	if (m <= 0 || n <= 0) return NULL;
	if (!(p = (double *)calloc(sizeof(double), m*n))) {
		printf("matrix memory allocation error: m=%d,n=%d\n", m, n);
	}
	return p;
}

extern double *eye(int m)
{
	double *p;
	int i;
	if (m <= 0) return NULL;
	if ((p = zeros(m, m)))for (i = 0; i < m; i++)p[i + i*m] = 1.0;
	return p;
}


static int inver_order(int list[], int n)
{
	int i, j;
	int ret = 0;
	for (i = 1; i < n; i++)
		for (j = 0; j < i; j++)
			if (list[j] > list[i])
				ret++;
	return ret;
}

// (det��)���ܣ����ź�������������1����������-1  
static int sgn(int order)
{
	return order % 2 ? -1 : 1;
}

// (det��)���ܣ�����������a��b��ֵ  
static void swap(int *a, int *b)
{
	int m;
	m = *a;
	*a = *b;
	*b = m;
}

// ���ܣ����������ʽ�ĺ��ĺ���  
static double det(double *p, int n, int k, int list[], double sum)
{
	int i;
	if (k >= n)
	{
		int order = inver_order(list, n);
		double item = (double)sgn(order);
		for (i = 0; i < n; i++)
		{
			//item *= p[i][list[i]];  
			item *= *(p + i * n + list[i]);
		}
		return sum + item;
	}
	else
	{
		for (i = k; i < n; i++)
		{
			swap(&list[k], &list[i]);
			sum = det(p, n, k + 1, list, sum);
			swap(&list[k], &list[i]);
		}
	}
	return sum;
}

// ���ܣ�������ʾ  
// �βΣ�(����)�����׵�ַָ��Mat����������row������col��  
// ���أ���  
extern void MatShow(double* Mat, int row, int col)
{
	int i;
	for (i = 0; i < row*col; i++)
	{
		printf("%16lf ", Mat[i]);
		if (0 == (i + 1) % col) printf("\n");
	}
}

// ���ܣ��������  
// �βΣ�(����)����A�׵�ַָ��A������B�׵�ַָ��B������A(Ҳ�Ǿ���B)����row������col  
// ���أ�A+B  
extern double* MatAdd(double* A, double* B, int row, int col)
{
	int i, j;
	double *Out = (double*)malloc(sizeof(double)* row * col);
	for (i = 0; i < row; i++)
		for (j = 0; j < col; j++)
			Out[col*i + j] = A[col*i + j] + B[col*i + j];
	return Out;
}

// ���ܣ��������  
// �βΣ�(����)����A������B������A(Ҳ�Ǿ���B)����row������col  
// ���أ�A-B  
extern double* MatSub(double* A, double* B, int row, int col)
{
	int i, j;
	double *Out = (double*)malloc(sizeof(double)* row * col);
	for (i = 0; i < row; i++)
		for (j = 0; j < col; j++)
			Out[col*i + j] = A[col*i + j] - B[col*i + j];
	return Out;
}

// ���ܣ��������  
// �βΣ�(����)����A������A����row������col������B������B����row������col  
// ���أ�A*B  
extern double* MatMul(double* A, int Arow, int Acol, double* B, int Brow, int Bcol)
{
	double *Out = (double*)malloc(sizeof(double)* Arow * Bcol);
	if (Acol != Brow)
	{
		printf("Shit mat!\n");
		return NULL;
	}
	if (Acol == Brow)
	{
		int i, j, k;
		for (i = 0; i < Arow; i++)
			for (j = 0; j < Bcol; j++)
			{
				Out[Bcol*i + j] = 0;
				for (k = 0; k < Acol; k++)
					Out[Bcol*i + j] = Out[Bcol*i + j] + A[Acol*i + k] * B[Bcol*k + j];
			}
	}
	return Out;
}

// ���ܣ���������(ʵ��k���Ծ���A)  
// �βΣ�(����)����A�׵�ַָ�룬��������row������col��ʵ��k  
// ���أ�kA  
extern double* MatMulk(double *A, int row, int col, double k)
{
	int i;
	double *Out = (double*)malloc(sizeof(double)* row * col);
	for (i = 0; i < row * col; i++)
	{
		*Out = *A * k;
		Out++;
		A++;
	}
	Out = Out - row * col;
	return Out;
}

// ���ܣ�����ת��  
// �βΣ�(����)����A�׵�ַָ��A������row������col  
// ���أ�A��ת��  
extern double* MatT(double* A, int row, int col)
{
	int i, j;
	double *Out = (double*)malloc(sizeof(double)* row * col);
	for (i = 0; i < row; i++)
		for (j = 0; j < col; j++)
			Out[row*j + i] = A[col*i + j];
	return Out;
}

// ���ܣ�������ʽֵ  
// �βΣ�(����)����A�׵�ַָ��A������row  
// ���أ�A������ʽֵ  
extern double MatDet(double *A, int row)
{
	int i;
	int *list = (int*)malloc(sizeof(int)* row);
	for (i = 0; i < row; i++)
		list[i] = i;
	double Out = det(A, row, 0, list, 0.0);
	free(list);
	return Out;
}

// ���ܣ��������  
// �βΣ�(����)����A�׵�ַָ��A������row������col  
// ���أ�A����  
extern int MatInv(double *A, int row, int col)
{
	int i;
	double *Out = (double*)malloc(sizeof(double)* row * col);
	double det = MatDet(A, row); //������ʽ  
	if (det == 0)
	{
		printf("Mat can't invert.\n");
		return 0;
	}
	if (det != 0)
	{
		Out = MatAdj(A, row, col); //��������  
		int len = row * row;
		for (i = 0; i < len; i++)
			*(Out + i) /= det;
	}
	matcpy(A, Out, row, col);
	free(Out);
	return 1;
}

// ���ܣ����������ʽ  
// �βΣ�(����)����A�׵�ַָ��A����������row, Ԫ��a���±�m��n(��0��ʼ)��  
// ���أ�NxN ������Ԫ��A(mn)�Ĵ�������ʽ  
extern double MatACof(double *A, int row, int m, int n)
{
	int i;
	int len = (row - 1) * (row - 1);
	double *cofactor = (double*)malloc(sizeof(double)* len);

	int count = 0;
	int raw_len = row * row;
	for (i = 0; i < raw_len; i++)
		if (i / row != m && i % row != n)
			*(cofactor + count++) = *(A + i);
	double ret = MatDet(cofactor, row - 1);
	if ((m + n) % 2)
		ret = -ret;
	free(cofactor);
	return ret;
}

// ���ܣ���������  
// �βΣ�(����)����A�׵�ַָ��A������row������col  
// ���أ�A�İ������  
extern double *MatAdj(double *A, int row, int col)
{
	int i;
	double *Out = (double*)malloc(sizeof(double)* row * col);
	int len = row * row;
	int count = 0;
	for (i = 0; i < len; i++)
	{
		*(Out + count++) = MatACof(A, row, i % row, i / row);
	}
	return Out;
}


extern void matmul(const char *tr, int m, int k, int n, double alpha,
	const double *A, const double *B, double beta, double *C)
{
	double d;
	int i, j, x, f = tr[0] == 'N' ? (tr[1] == 'N' ? 1 : 2) : (tr[1] == 'N' ? 3 : 4);

	for (i = 0; i<m; i++) for (j = 0; j<k; j++) {
		d = 0.0;
		switch (f) {
		case 1: for (x = 0; x<n; x++) d += A[x + i*n] * B[j + x*k]; break;
		case 2: for (x = 0; x<n; x++) d += A[x + i*n] * B[x + j*n]; break;
		case 3: for (x = 0; x<n; x++) d += A[i + x*m] * B[j + x*k]; break;
		case 4: for (x = 0; x<n; x++) d += A[i + x*m] * B[x + j*n]; break;
		}
		if (beta == 0.0) C[j + i*k] = alpha*d; else C[j + i*k] = alpha*d + beta*C[j + i*k];
	}
}


/* LU decomposition ----------------------------------------------------------*/
extern int ludcmp(double *A, int n, double *L, double *U)
{
	int i, j, k;
	double s, t, sum = 1;
	double *a;
	a = mat(n, n);
	matcpy(a, A, n, n);

	for (i = 1; i < n; i++)
		a[i*n] = a[i*n] / a[0];   //����L����ĵ�1��
								  //	a[0+i*n] = A[0+i*n] / A[0+0*n];   //����L����ĵ�1��  

	for (k = 1; k < n; k++)
	{
		for (j = k; j < n; j++)
		{
			s = 0;
			for (i = 0; i < k; i++)
				s = s + a[k*n + i] * a[i*n + j];   //�ۼ�  
			a[k*n + j] = a[k*n + j] - s; //����U���������Ԫ��  
		}
		for (i = k + 1; i < n; i++)
		{
			t = 0;
			for (j = 0; j < k; j++)
				t = t + a[i*n + j] * a[j*n + k];   //�ۼ�  
			a[i*n + k] = (a[i*n + k] - t) / a[k*n + k];    //����L���������Ԫ��  
		}
	}

	for (i = 0; i < n; i++)
	{
		for (j = 0; j<n; j++)
		{
			if (i>j)
			{
				L[i*n + j] = a[i*n + j];
				U[i*n + j] = 0;
			}//���i>j��˵���д����У��������������ǲ��֣��ó�L��ֵ��U��//Ϊ0  
			else
			{
				U[i*n + j] = a[i*n + j];
				if (i == j)
					L[i*n + j] = 1;  //�������i<j��˵����С���У��������������ǲ��֣��ó�U��//ֵ��L��Ϊ0  
				else
					L[i*n + j] = 0;
			}
		}
	}
	for (i = 0; i < n; i++)
		sum *= U[i + i*n];
	if (sum == 0) {
		free(a);
		return -1;
	}
	free(a);
	return 0;
}
//flag=1 inverse 
//flag=0 A=LU;
extern void lubksb(double *U, double *L, int n, int flag, double *A)
{
	int i, j, k;
	double s;
	if (flag == 1) {
		double *u, *r;
		u = zeros(n, n); r = zeros(n, n);
		for (i = 0; i < n; i++) /*�����U���� */
		{
			u[i*n + i] = 1 / U[i*n + i];//�Խ�Ԫ�ص�ֵ��ֱ��ȡ����  
			for (k = i - 1; k >= 0; k--)
			{
				s = 0;
				for (j = k + 1; j <= i; j++)
					s = s + U[k*n + j] * u[j*n + i];
				u[k*n + i] = -s / U[k*n + k];//�������㣬���е������εõ�ÿһ��ֵ��  
			}
		}
		for (i = 0; i < n; i++) //�����L����   
		{
			r[i*n + i] = 1; //�Խ�Ԫ�ص�ֵ��ֱ��ȡ����������Ϊ1  
			for (k = i + 1; k < n; k++)
			{
				for (j = i; j <= k - 1; j++)
					r[k*n + i] = r[k*n + i] - L[k*n + j] * r[j*n + i];   //�������㣬����˳�����εõ�ÿһ��ֵ  
			}
		}
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				for (k = 0; k < n; k++)
				{
					A[i*n + j] += u[i*n + k] * r[k*n + j];
				}
			}
		}
		free(u); free(r);
	}
	else
	{
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				for (k = 0; k < n; k++)
				{
					A[i*n + j] += L[i*n + k] * U[k*n + j];
				}
			}
		}
	}

}


extern int  matinv(double *A, int n)
{
	int i, j;
	double *B, *L, *U;
	B = mat(n, n); L = zeros(n, n); U = zeros(n, n);
	matcpy(B, A, n, n);
	if (ludcmp(B, n, L, U)) { free(B); free(L); free(U); return -1; }
	for (i = 0; i<n; i++)
		for (j = 0; j<n; j++) A[i + j*n] = 0.0;
	lubksb(U, L, n, 1, A);
	free(B); free(L); free(U);
	return 0;
}


/* Combine matrix------------------------------------------------------
* read positions from station position file
* args   : double  *file      I   station position file containing
*                               lat(deg) lon(deg) height(m) name in a line
*          int		opt      I   0, colum; 1, row; 2, diagonal
*          int		m,n      I   row and col of A
*          int		k        I   dim of B different to the common one
*                               (all 0 if search error)
* return : none
*-----------------------------------------------------------------------------*/
extern int combinemat(double* A, const double* B, int m, int n, int k, int opt)
{
	int i, j, info = 1;
	double* tA, tmp;


	if (opt == 0) {//column order combine
		matcpy(A + m * n, B, m, k);
		info = 0;
	}
	else if (opt == 1) {//row order combine
		for (i = 0; i < n; i++) {
			for (j = 0; j < m; j++) A[j + i * (m + k)] = A[j + i * m];
			for (j = m; j < m + k; j++) A[j + i * (m + k)] = B[j - m + i * m];
		}
		info = 0;
	}
	else if (opt == 2) {
		tA = mat(m, n);
		matcpy(tA, A, m, n);
		/*for(i=0;i<m;i++)
		for(j=0;j<n;j++) tA[i+m*j]=A[i+m*j];*/
		for (i = 0; i < m + k; i++)
			for (j = 0; j < m + k; j++) {
				if (i < m && j < m) { tmp = tA[j + i * m]; A[j + i * (m + k)] = tmp; }
				else if (i >= m && j >= m) { tmp = B[j - m + (i - m) * k]; A[j + i * (m + k)] = tmp; }
				else A[j + i * (m + k)] = 0.0;
			}

		info = 0;
		free(tA);
	}

	//free(tA); 
	return info;
}

extern int LeastSquare(const double *H, const double *v, int m, int n, double *x, double *Q)
{
	double *Hv;
	int info;

	if (m<n) return -1;
	Hv = mat(n, 1);
	matmul("TN", n, 1, m, 1.0, H, v, 0.0, Hv); /* Hv=H'*v */
	matmul("TN", n, n, m, 1.0, H, H, 0.0, Q);  /* Q=H'*H */
	if (!(info = matinv(Q, n))) matmul("NN", n, 1, n, 1.0, Q, Hv, 0.0, x); /* x=Q^-1*H'v */
	free(Hv);
	return info;
}


extern int LeastSquarePlus(const double *A, const double *y, const int nv, const int nx, double *x, double *Q)
{
	int i, j, k, info = 0;
	int *ix;
	double *A_, *x_, *Q_;

	ix = imat(nx, 1);

	for (i = k = 0; i<nx; i++) {
		for (j = 0; j<nv; j++) {
			if (fabs(A[j*nx + i])>1.0e-10) {
				ix[k++] = i;
				j = 1000;
			}
		}
	}

	A_ = mat(k*nv, 1); x_ = mat(k, 1); Q_ = mat(k*k, 1);

	for (j = 0; j<k; j++) {
		for (i = 0; i<nv; i++) {
			A_[i*k + j] = A[i*nx + ix[j]];
		}

		x_[j] = x[ix[j]];
	}

	/* least square estimation */
	info = LeastSquare(A_, y, nv, k, x_, Q_);

	for (i = 0; i<nx*nx; i++) Q[i] = 0.0;

	for (i = 0; i<k; i++) {
		x[ix[i]] = x_[i];

		for (j = 0; j<k; j++)
			Q[ix[i] + ix[j] * nx] = Q_[i + j*k];
	}

	free(ix); free(A_); free(x_); free(Q_);

	return info;
}

static int KalmanFilter_(double* x, double *P, double *H, double *v,
	double *R, double *Q1, double *Q2, int m, int n, double *xp, double *Pp)
{
	double *F = mat(n, m), *Q = mat(m, m), *K = mat(n, m), *I = eye(n), *P1 = mat(n, n), *P2 = mat(n, n), *R1 = mat(m, n);
	int info;
	matcpy(Q, R, m, m);
	matcpy(xp, x, n, 1);
	matmul("NT", n, m, n, 1.0, P, H, 0.0, F);
	matmul("NN", m, m, n, 1.0, H, F, 1.0, Q);    /* Q=H*P*H'+R */

	if (Q1) matcpy(Q1, Q, m, m); //Q1:��Ϣ��Э������
	if (!(info = matinv(Q, m)))
	{
		matmul("NN", n, m, m, 1.0, F, Q, 0.0, K);   /* K=P*H'*Q^-1 */
		matmul("NN", n, 1, m, 1.0, K, v, 1.0, xp);  /* xp=x+K*v */
		matmul("NN", n, n, m, -1.0, K, H, 1.0, I);  /* I=(I-K*H) */
		matmul("NN", n, n, n, 1.0, I, P, 0.0, P1);  /* P1=(I-K*H)*P */
		matmul("NT", n, n, n, 1.0, P1, I, 0.0, P2); /* P2=(I-K*H)*P*(I-K*H)' */
		matcpy(Pp, P2, n, n);
		matmul("NN", n, m, m, 1.0, K, R, 0.0, R1);  /* R1=K*R */
		matmul("NT", n, n, m, 1.0, R1, K, 1.0, Pp); /* Pp=P2+K*R*K'=(I-K*H)*P*(I-K*H)'+K*R*K' */
	}
	else {
		printf("Q mat invert failed!\n");
	}
	if (Q2) { //Q2:�в��Э������
		matcpy(Q2, R, m, m);
		matmul("NT", n, m, n, -1.0, Pp, H, 0.0, F); /* Q2=R-H*Pp*H' */
		matmul("NN", m, m, n, 1.0, H, F, 1.0, Q2);
	}
	free(P1); free(P2); free(R1);
	free(F); free(Q); free(K); free(I);

	return info;
}

//m:row n:col
extern int KalmanFilter(double* x, double *P, double *H, double *v,
	double *R, double *Q1, double *Q2, int m, int n)
{
	double *x_, *xp_, *P_, *Pp_, *H_;
	int i, j, k, info, *ix;

	ix = imat(n, 1); for (i = k = 0; i<n; i++) if (x[i] != 0.0&&P[i + i*n]>0.0) ix[k++] = i;
	x_ = mat(k, 1); xp_ = mat(k, 1); P_ = mat(k, k); Pp_ = mat(k, k); H_ = mat(m, k);
	for (i = 0; i<k; i++) {
		x_[i] = x[ix[i]];
		for (j = 0; j<k; j++) P_[i + j*k] = P[ix[i] + ix[j] * n];
		for (j = 0; j<m; j++) H_[i + j*k] = H[ix[i] + j*n];
	}

	info = KalmanFilter_(x_, P_, H_, v, R, Q1, Q2, m, k, xp_, Pp_);

	for (i = 0; i < k; i++)
	{
		x[ix[i]] = xp_[i];
		for (j = 0; j<k; j++)  P[ix[i] + ix[j] * n] = Pp_[i + j*k];
	}
	free(ix); free(x_); free(xp_); free(P_); free(Pp_); free(H_);
	return info;
}








