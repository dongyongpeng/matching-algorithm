#include <iostream>
#include "htree.h"
using namespace std;
int main(int argc, char **argv)
{
    int subs;           // Number of subscriptions.
    int pubs;           // Number of publications.
    int atts;           // Total number of attributes, i.e. dimensions.
    int cons;           // Number of constraints(predicates) in one sub.
    int m;              // Number of constraints in one pub.
    int valDom;         // Cardinality of values.
    double width;       // Width of a predicate.
    int level;
    int cells;

    freopen("paras.txt", "r", stdin);
    cin >> subs >> pubs >> atts >> cons >> m >> valDom >> level >> cells;

    vector<double> insertTimeList;
    vector<double> matchTimeList;
    vector<double> matchSubList;

    // Initiate generator
    intervalGenerator gen(subs, pubs, atts, cons, m, valDom);
    gen.GenSubList();
    gen.GenPubList();


    Htree a(atts, level, valDom);

    // insert
    for (int i = 0; i < subs; i++)
    {
        Timer subStart;

        a.insert(gen.subList[i]);                       // Insert sub[i] into data structure.

        int64_t insertTime = subStart.elapsed_nano();   // Record inserting time in nanosecond.
        insertTimeList.push_back((double) insertTime / 1000000);
    }


    // match
    for (int i = 0; i < pubs; i++)
    {
        int matchSubs = 0;                              // Record the number of matched subscriptions.
        Timer matchStart;

        a.match(gen.pubList[i], matchSubs, gen.subList);

        int64_t eventTime = matchStart.elapsed_nano();  // Record matching time in nanosecond.
        matchTimeList.push_back((double) eventTime / 1000000);
        matchSubList.push_back(matchSubs);
    }


    // output
    string outputFileName = "htree.txt";
    string content = Util::Int2String(subs) + "\t" + Util::Double2String(Util::Average(insertTimeList)) + "\t" +
                     Util::Double2String(Util::Average(matchTimeList)) + "\t" +
                     Util::Double2String(Util::Average(matchSubList));
    Util::WriteData(outputFileName.c_str(), content);


    // check
    //Util::Check(gen, "htree");

    return 0;
}
