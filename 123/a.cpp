#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
char str[110*386];
float l[1866819][134];
double x[133], theta[133], temp[1866819], minus[133], lasttheta[133];
int main() {
	freopen("train_data.txt", "r", stdin);
	int index, in = 0, p = 0, i, j, len;
	double num = 0, flag = 1.0, ans = 0.0;
	double frac = 1.0;
	for (i = 0; i < 1866819; i++) {
		gets(str);
		len = strlen(str);
		l[i][133] = str[0] - '0';
		num = 0;
		for (j = 1; j < len; j++) {
			if (str[j] == ':') {
				index = (int)(num + 0.5); num = 0; frac = 1.0; flag = 1.0;
			}
			else if (str[j] == ' ') {
				if (index < 133) l[i][index] = num;
				num = 0; frac = 1.0; flag = 1.0;
			}
			else if (str[j] == '.') {
				frac *= 0.1;
			}
			else if (str[j] == '-') {
				flag = -1.0;
			}
			else if (frac < 1.0) {
				num = num + (str[j] - '0') * frac * flag;
				frac *= 0.1;
			} else {
				num = num * 10 + (str[j] - '0') * flag;
			}
		}
		if (index < 133 && num > 0) l[i][index] = num;
	}
	fclose(stdin);
	printf("load succeeded\n");
	double alpha = 1e-15, err = 0.0, lasterr = 1e17, th, ran;
	int ss, round;
	int myrank, numprocs = 199;
	for (round = 0; round < 60; round++) {
		MPI_Status status;
		MPI_Init(NULL, NULL);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
		int bm = 1866819 / 199;
		MPI_Barrier(MPI_COMM_WORLD);
		for (i = 0; i < 133; i++) {
			lasttheta[i] = theta[i];
		}
		MPI_Scatter(l, bm * 133, MPI_FLOAT, bA, bm * 133, MPI_FLOAT, 0, MPI_COMM_WORLD);
		for (i = 0; i < bm; i++) {
			temp[i] = 0.0;
			for (j = 0; j < 133; j++)
				temp[i] += theta[j] * l[i][j];
			temp[i] -= l[i][133];
		}
		MPI_Gather(temp,  bm * 133, MPI_FLOAT, C, bm * 133)
		for (i = 0; i < 133; i++) {
			minus[i] = 0.0;
			for (j = 0; j < 1866819; j++)
				minus[i] += temp[j] * l[j][i] / 1866819.0 * alpha;
			theta[i] -= minus[i];
		}
		err = 0.0;
		for (i = 0; i < 1866819; i++) {
			th = -l[i][133];
			for (j = 0; j < 133; j++) {
				th += theta[j] * l[i][j];
			}
			err += th * th;
		}
		printf("%d %.6lf %.6lf %.6lf\n", round, alpha, err, lasterr); 
		if (err > lasterr) {
			for (int i = 0; i < 133; i++) {
				theta[i] = lasttheta[i];
			}
			ran = (double)rand() / 327680.0; 
			alpha *= 0.1; round--;
		}
		else {
			ran = (double)rand() / 327680.0; 
			lasterr = err; alpha *= 1 + (0.1 * ran);
		}
	}
	freopen("b.txt", "w", stdout);
	printf("%.6lf\n", err);
	fclose(stdout);
	freopen("test_data", "r", stdin);
	freopen("submission2.csv", "w", stdout);
	printf("id,label\n");
	int cnt = 0;
	while (gets(str)) {
		len = strlen(str);
		in = 0; p = 0; flag = 1.0; frac = 1.0;
		while (str[p] != ' ') {
			in = in * 10 + str[p] - '0'; p++;
		}
		for (j = p; j < len; j++) {
			if (str[j] == ':') {
				index = (int)(num + 0.5); num = 0; frac = 1.0;
			}
			else if (str[j] == ' ') {
				if (index < 133) x[index] = num;
				num = 0; frac = 1.0; flag = 1.0;
			}
			else if (str[j] == '.') {
				frac *= 0.1; flag = 1.0;
			}
			else if (str[j] == '-') {
				flag = -1.0;
			}
			else if (frac < 1.0) {
				num = num + (str[j] - '0') * frac * flag;
				frac *= 0.1;
			} else {
				num = num * 10 + (str[j] - '0') * flag;
			}
		}
		if (index < 133 && num > 0) x[index] = num;
		ans = 0.0;
		for (j = 0; j < 133; j++) {
			ans += x[j] * theta[j];
		}
		printf("%d,%.6lf\n", ++cnt, ans);
	}
	fclose(stdin); fclose(stdout);
	freopen("theta.txt", "w", stdout);
	for (int j = 0; j < 133; j++) printf("%.9lf ", theta[j]);
	printf("%.9lf\n", theta[133]);
	fclose(stdout);
}
