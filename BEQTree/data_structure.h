#ifndef _DATA_STRUCTURE_H
#define _DATA_STRUCTURE_H
#include<string>
#include<vector>
using namespace std;

struct Cnt{
    int att;
    int value;
    int op;     //  op==0 -> "="  op==1 -> ">="  op==2 -> "<="
};

struct IntervalCnt{
    int att;
    int lowValue, highValue;
};

struct Sub {
    int id;
    int size; 								//number of predicates
    vector<Cnt> constraints;				//list of constraints
};

struct IntervalSub{
    int id;
    int size;
	double lowlng,highlng,lowlat,highlat;
    vector<IntervalCnt> constraints;
};

struct ConElement {
    int att;
    int val;
    int subID;
};

struct Combo{
    int val;
    int subID;
};

struct IntervalCombo{
    int lowValue, highValue;
    int subID;
};

struct Pair{
    int att;
    int value;
};

struct Pub{
	int id;
    int size;
	double lng,lat;
    vector<Pair> pairs;
};

struct beqnode{
	beqnode(): num(0), isleaf(true) {};
	int num;
	double cx,cy,rx,ry; //center and radius
	bool isleaf;
	beqnode* children[4];
	vector<pair<int, int> > list[20];
	vector<pair<double, int> > listy;
};


#endif //_DATA_STRUCTURE_H
