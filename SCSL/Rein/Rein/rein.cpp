#include "rein.h"

void Rein::insert(IntervalSub &sub, int64_t &origintime)
{
		Timer t;
		double poss = 1;
		int level;
		for(int j = 0; j < sub.size; ++j){
			IntervalCnt &cnt = sub.constraints[j];
			poss *=  (cnt.highValue-cnt.lowValue)*1.0/valDom;
			level = (int) floor(log(poss)/log(0.1)/Dom*newlevel);
		}
		int indexId=LvSize[level].size(),buckStep=LvBuckStep[level];
		subtolevel[sub.id] = level;
		originlevel[sub.id] = level;
		for (int j = 0; j < sub.size; ++j){
			IntervalCnt &cnt = sub.constraints[j];
			kCombo c;
			c.val = cnt.lowValue;
			c.subID = sub.id;
			c.indexId = indexId;
			data[level][cnt.att][0][c.val / buckStep].push_back(c);
			c.val = cnt.highValue;
			data[level][cnt.att][1][c.val / buckStep].push_back(c);
		}
		LvSize[level].push_back(sub.id);
		++subnum;
		
		origintime=t.elapsed_nano();
    //}
    
}




void Rein::match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList, vector<double> &matchDetailPub)
{

	Timer t;
	
    for (int siIndex=0; siIndex<newlevel; ++siIndex)
    {
		
		vector<bool> bits(LvSize[siIndex].size(), false);
		int bucks=LvBuck[siIndex],buckStep=LvBuckStep[siIndex];
        for (int i = 0; i < pub.size; i++)
        {
         
            int value = pub.pairs[i].value, att = pub.pairs[i].att, buck = value / buckStep;
            vector<kCombo> &data_0 = data[siIndex][att][0][buck];

  
            int data_0_size = data_0.size();
            for (int k = 0; k < data_0_size; k++)
                if (data_0[k].val > value)
                    bits[data_0[k].indexId] = true;

            for (int j = buck + 1; j < bucks; j++){
                vector<kCombo> &data_1 = data[siIndex][att][0][j];
   
                int data_1_size = data_1.size();
                for (int k = 0; k < data_1_size; k++)
                    bits[data_1[k].indexId] = true;
            }
 

 
            vector<kCombo> &data_2 = data[siIndex][att][1][buck];
            int data_2_size = data_2.size();
            for (int k = 0; k < data_2_size; k++)
                if (data_2[k].val < value)
                    bits[data_2[k].indexId] = true;

            for (int j = buck - 1; j >= 0; j--){
                vector<kCombo> &data_3 = data[siIndex][att][1][j];
                //if (data_3.empty()) break;
                int data_3_size = data_3.size();
                for (int k = 0; k < data_3_size; k++)
                    bits[data_3[k].indexId] = true;
            }
        }

        //Timer t5;
        vector<int> &_Lv = LvSize[siIndex];
        int b = _Lv.size();
        for (int i=0; i<b; ++i)
            if (!bits[i] && _Lv[i]!=-1)
            {
                matchDetailPub.push_back(t.elapsed_nano() / 1000000.0);
                ++matchSubs;
				++countlist[_Lv[i]];
            }
    }


}

int Rein::change(const vector<IntervalSub> &subList,int cstep, double matchingtime, string &windowcontent){
	int changenum=0,totalshouldchange=0,limitnum=4000;
	bool stopflag=false;
	if(!firstchange){
		limitnum = (int) (matchingtime * cstep * limitscale / adjustcost);
	}
	for(int i=0;i<subnum;++i){
		int level=originlevel[i]-(int)round((double)countlist[i]/cstep*originlevel[i]),oldlevel=subtolevel[i];
		if(level == oldlevel) continue;
		int distancelevel = abs(level-oldlevel);
		changeaction tmp;
		tmp.id = i;
		tmp.oldlevel = oldlevel;
		tmp.newlevel = level;
		changelist[distancelevel].push_back(tmp);
		++totalshouldchange;
		
	}
	cout<<"totalshouldchange: "<<totalshouldchange<<endl;
	windowcontent += to_string(totalshouldchange) +"\t";
	Timer t;
	for(int i=newlevel-1;i>0;--i){
		int l=changelist[i].size();
		for(int ii=0;ii<l;++ii){
			changeaction &action = changelist[i][ii];
			int id=action.id,oldlevel=action.oldlevel,level=action.newlevel;
			int indexId=LvSize[level].size(),oldindexId=-1,buckStep=LvBuckStep[level],oldbuckStep=LvBuckStep[oldlevel];
			subtolevel[id]=level;
			IntervalSub sub=subList[id];
			for(int j=0;j<sub.size;++j){
				IntervalCnt cnt = sub.constraints[j];
				vector<kCombo>::iterator ed=data[oldlevel][cnt.att][0][cnt.lowValue/oldbuckStep].end();
				bool fl=false;
				for(vector<kCombo>::iterator it=data[oldlevel][cnt.att][0][cnt.lowValue/oldbuckStep].begin();it!=ed;++it)
					if(it->subID==id){
						oldindexId = it->indexId;
						data[oldlevel][cnt.att][0][cnt.lowValue/oldbuckStep].erase(it);
						fl=true;
						break;
					}
				if(!fl)cout<<"error!not found lowValue"<<endl;
				fl=false;
				ed=data[oldlevel][cnt.att][1][cnt.highValue/oldbuckStep].end();
				for(vector<kCombo>::iterator it=data[oldlevel][cnt.att][1][cnt.highValue/oldbuckStep].begin();it!=ed;++it)
					if(it->subID==id){
						data[oldlevel][cnt.att][1][cnt.highValue/oldbuckStep].erase(it);
						fl=true;
						break;
					}
				if(!fl)cout<<"error!not found highValue"<<endl;
				
				
				kCombo c;
				c.val = cnt.lowValue;
				c.subID = id;
				c.indexId = indexId;
				data[level][cnt.att][0][c.val / buckStep].push_back(c);
				c.val = cnt.highValue;
				data[level][cnt.att][1][c.val / buckStep].push_back(c);
			}
			if(oldindexId!=-1 && LvSize[oldlevel][oldindexId]==sub.id)
					LvSize[oldlevel][oldindexId]=-1;
			else cout<<"error!not found LvSize "<<oldindexId<<" "<<LvSize[oldlevel][oldindexId]<<" "<<sub.id<<endl;
			LvSize[level].push_back(sub.id);
			++changenum;
			if(changenum>=limitnum){
				stopflag=true;
				break;
			}
		}
		if(stopflag)break;
	}
	if(firstchange){
		adjustcost = (double)t.elapsed_nano() / 1000000 / changenum;
		firstchange = false;
	}
	for(int i=newlevel-1;i>0;--i) changelist[i].clear();
	memset(countlist, 0, sizeof(countlist));
	windowcontent += to_string(changenum) +"\n";
	return changenum;
}
	
void Rein::check(){
	for(int i=0;i<newlevel;++i){
		vector<int> &_Lv = LvSize[i];
		int l=_Lv.size();
		int num=0;
		for(int j=0;j<l;++j)
			if(_Lv[j]!=-1)
				++num;
		cout<<i<<' '<<l<<' '<<num<<endl;
	}
}

