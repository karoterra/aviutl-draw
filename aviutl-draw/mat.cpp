#include "mat.h"

const int N = 8;
const int M = N + 1;
const double eps = 1e-8;

void scaling(double a[][M]) {
	for (int i = 0; i < N; i++) {
		double maxval = abs(a[i][0]);
		for (int j = 1; j < M; j++) {
			double x = abs(a[i][j]);
			if (x > maxval) maxval = x;
		}
		if (maxval < eps) continue;
		for (int j = 0; j < M; j++) {
			a[i][j] /= maxval;
		}
	}
}

void forward(double a[][M]) {
	for (int i = 0, row = 0; i < N; i++) {
		// select pivot
		int ii = row;
		double maxval = abs(a[row][i]);
		for (int j = row + 1; j < N; j++) {
			double x = abs(a[j][i]);
			if (x > maxval) {
				maxval = x;
				ii = j;
			}
		}
		if (maxval < eps) continue;

		// swap row
		if (ii != row) {
			for (int j = i; j < M; j++) {
				double tmp = a[row][j];
				a[row][j] = a[ii][j];
				a[ii][j] = tmp;
			}
		}

		// elimination
		for (int j = i + 1; j < M; j++) {
			a[row][j] /= a[row][i];
		}
		a[row][i] = 1;
		for (int j = row + 1; j < N; j++) {
			for (int k = i + 1; k < M; k++) {
				a[j][k] -= a[j][i] * a[row][k];
			}
			a[j][i] = 0;
		}

		row++;
	}
}

void backward(const double a[][M], double x[]) {
	for (int i = N - 1; i >= 0; i--) {
		int j = 0;
		for (; j < N && abs(a[i][j]) < eps; j++);
		if (j == N) continue;

		x[j] = a[i][N];
		for (int k = j + 1; k < N; k++) {
			x[j] -= a[i][k] * x[k];
		}
	}
}

void getPerspective(Vec2<double> dst[], Vec2<double> src[], Mat<double>& mat) {
	double a[N][M]{};
	double x[N]{};

	for (int i = 0; i < 4; i++) {
		a[i][0] = a[i + 4][3] = src[i].x;
		a[i][1] = a[i + 4][4] = src[i].y;
		a[i][2] = a[i + 4][5] = 1;
		a[i][3] = a[i][4] = a[i][5] = 0;
		a[i + 4][0] = a[i + 4][1] = a[i + 4][2] = 0;
		a[i][6] = -src[i].x * dst[i].x;
		a[i][7] = -src[i].y * dst[i].x;
		a[i + 4][6] = -src[i].x * dst[i].y;
		a[i + 4][7] = -src[i].y * dst[i].y;
		a[i][8] = dst[i].x;
		a[i + 4][8] = dst[i].y;
	}

	scaling(a);
	forward(a);
	backward(a, x);

	mat.m11 = x[0];
	mat.m12 = x[1];
	mat.m13 = x[2];
	mat.m21 = x[3];
	mat.m22 = x[4];
	mat.m23 = x[5];
	mat.m31 = x[6];
	mat.m32 = x[7];
	mat.m33 = 1;
}
