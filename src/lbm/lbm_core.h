#ifndef LBM_CORE_H
#define LBM_CORE_H

static const int D = 2;
static const int Q = 9;

static const int cx[Q] = {
	0,
	1,
	0,
	-1,
	0,
	1,
	-1,
	-1,
	1
};

static const int cy[Q] = {
	0,
	0,
	1,
	0,
	-1,
	1,
	1,
	-1,
	-1
};

static const int w[Q] = {
	4.0/9.0,
	1.0/9.0,
	1.0/9.0,
	1.0/9.0,
	1.0/9.0,
	1.0/36.0,
	1.0/36.0,
	1.0/36.0,
	1.0/36.0
};






#endif
