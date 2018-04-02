///
/// @file    WordQuery.cc
/// @author  sgzed(wunaisong@163.com)
/// @date    2018-04-01 20:49:50
///

#include "../include/WordQuery.h"
#include "../include/MyConf.h"

#include <fstream>
using std::ifstream;
using std::make_pair;

struct SimilarityCompare
{
	SimilarityCompare(vector<double> & base)
		: _base(base)
	{	}

	bool operator()(const pair<int, vector<double> > & lhs, 
			const pair<int, vector<double> > & rhs)
	{
		double lhsCrossProduct = 0;
		double rhsCrossProduct = 0;
		double lhsVectorLength = 0;
		double rhsVectorLength = 0;

		for(size_t index = 0; index != _base.size(); ++index)
		{
			lhsCrossProduct += (lhs.second)[index] * _base[index];
			rhsCrossProduct += (rhs.second)[index] * _base[index];
			lhsVectorLength += pow((lhs.second)[index], 2);
			rhsVectorLength += pow((rhs.second)[index], 2);
		}

		if(lhsCrossProduct / sqrt(lhsVectorLength) < rhsCrossProduct / sqrt(rhsVectorLength))
		{	return false;	}
		else
		{	return true;	}
	}
	vector<double> _base;
};

WordQuery::WordQuery()
	:_jieba()
{
	loadLibrary();	
}

void WordQuery::loadLibrary()
{
	auto confMap = MyConf::getInstance()->getConfigMap();

	ifstream ifsOffset(confMap["offsetlib"].c_str());	

	ifstream ifsPage(confMap["ripepagelib"].c_str());	

	if(!ifsOffset.good() || !ifsPage.good())
	{
		cout << "open offsetlib or pagelib failed" << endl;
		return ;	
	}

	int docId,offset,length;
	string line;
	//int cnt=0;	
	while(getline(ifsOffset,line))
	{
		std::stringstream ss(line);
		ss >> docId >> offset >> length ;

		string doc;
		doc.resize(length,' ');
		ifsPage.seekg(offset,ifsPage.beg);
		ifsPage.read(&*doc.begin(),length);

		WebPage webPage(doc,_jieba);
	   _pageLib.insert(make_pair(docId,webPage));

		_offsetLib.insert(make_pair(docId,make_pair(offset,length)));
	}

	ifsOffset.close();
	ifsPage.close();
#if 1
	for(auto iter : _offsetLib)
	{
		cout<<iter.first<<"-->(" << iter.second.first<<","<<iter.second.second
			<< ")" << endl;
	}
#endif
	string inverPath = confMap["invertindexlib"];
	ifstream ifsInvertIndex(inverPath.c_str());	
	if(!ifsInvertIndex.good())
	{
		cout << "open inverindextable failed" <<endl;
		return;
	}
	string word;
	double weight;
	//cnt = 0;
	while(getline(ifsInvertIndex,line))
	{
		std::stringstream ss(line);
		ss >> word;
		set<pair<int,double>> setId;

		while(ss>>docId>>weight)
		{
			setId.insert(make_pair(docId,weight));
		}
		_invertIndexTable.insert(make_pair(word,setId));
	}
	ifsInvertIndex.close();

#if 1
	for(auto item : _invertIndexTable)
	{
		cout << item.first << "\t" ;
		for(auto sitem : item.second)
		{
			cout << sitem.first << "\t" << sitem.second << "\t";
		}
		cout << endl;
	}
#endif
}

string WordQuery::doQuery(const string& str)
{
	vector<string> queryWords;
	if(str.size() > 0)
	{
		queryWords = _jieba(str.c_str());
	}

	auto stopWords = _jieba.getStopWordList();
#if 1
	for(auto iter= queryWords.begin();iter!=queryWords.end();++iter)
	{
		auto sit = stopWords.find(*iter);
		if(sit != stopWords.end() || *iter==" ")
		{
			queryWords.erase(iter);
			--iter;
		}
	}

	if(queryWords.size()==0)
		return returnNoAnswer();
#endif 

	for(auto item : queryWords)
	{
		cout << item << "#\t";
		auto uit = _invertIndexTable.find(item);
		if(uit == _invertIndexTable.end())
		{
			auto sit = stopWords.find(item);
			if(sit == stopWords.end())
			{
		    	cout << "can't found " << item << endl;
			    return returnNoAnswer();
			}
		}
	}
	cout << endl;
	vector<double> weightList =getQueryWordsWeightVector(queryWords);
	SimilarityCompare similarityCmp(weightList);
	vector<pair<int, vector<double> > > resultVec;

	if(executeQuery(queryWords, resultVec))
	{
		stable_sort(resultVec.begin(), resultVec.end(), similarityCmp);
		vector<int> docIdVec;

		for(auto item : resultVec)
		{
			docIdVec.push_back(item.first);
		}
		return createJson(docIdVec, queryWords);
	}
	else
	{
		return returnNoAnswer();
	}
}

vector<double> WordQuery::getQueryWordsWeightVector(vector<string> & queryWords)
{
	map<string, int> wordFreqMap;
	for(auto item : queryWords)
	{
		++wordFreqMap[item];
	}
	vector<double> weightList;
	double weightSum = 0;
	int totalPageNum = _offsetLib.size();

	for(auto & item : queryWords)
	{
		int df = _invertIndexTable[item].size();
		double idf = log(static_cast<double>(totalPageNum) / df + 0.05) / log(2); 
		int tf = wordFreqMap[item];
		double w = idf * tf;
		weightSum += pow(w, 2);
		weightList.push_back(w);
	}
	for(auto & item : weightList)
	{
		item /= sqrt(weightSum);
	}
	cout << "weightList's elem: ";
	for(auto item : weightList)
	{
		cout << item << "\t";
	}
	cout << endl;
	return weightList;
}

string WordQuery::returnNoAnswer()
{
	return string("404 Not Found\n");
}

bool WordQuery::executeQuery(const vector<string>& queryWords,
		vector<pair<int,vector<double>>>& resultVec)
{
	if(queryWords.size() == 0)
	{
		cout << "empty string not found " << endl;
		return false;
	}

	typedef set<pair<int,double>>::iterator setIter;
	//typedef set<pair<int,double>> Set;
	vector<pair<setIter,int>> iterVec;
	vector<int> maxNum;
	int minIterNum = 0x7FFFFFFF;

	for(auto item : queryWords)
	{
		int sz = _invertIndexTable[item].size();
		if(sz==0)
			return false;
		//miniternum 迭代器能++的最大次数???
		if(minIterNum > sz)
			minIterNum = sz;

		iterVec.push_back(make_pair(_invertIndexTable[item].begin(),0));
		maxNum.push_back(sz);
	}
	cout << "minIterNum = " << minIterNum << endl;

	bool isExisting = false;
	while(!isExisting)
	{
		size_t  idx = 0;

		for(;idx!=iterVec.size()-1;++idx)
		{
			if((iterVec[idx].first)->first != iterVec[idx+1].first->first)
				break;
		}
		if(idx == iterVec.size()-1)
		{
			// 找到的文档编号相同，将其放到resultVec中
			vector<double> weightVec;
			int docId = iterVec[0].first->first;

			for(idx=0;idx!=iterVec.size();++idx)
			{
				weightVec.push_back(iterVec[idx].first->second);
				++(iterVec[idx].first);  
				++(iterVec[idx].second);
				//如果查找的次数超过出现最少的字符的次数，那么结束查找
				if(iterVec[idx].second == minIterNum)
				{ isExisting = true; }
			}
			resultVec.push_back(make_pair(docId,weightVec));
		}
		else
		{
			int minDocId = 0x7FFFFFFF;
			int iterIdx;
			for(idx=0;idx!=iterVec.size();++idx)
			{
				if(iterVec[idx].first->first < minDocId)
				{
					minDocId = iterVec[idx].first->first;
					iterIdx = idx;
				}
			}
			++(iterVec[iterIdx].first);
			//这里有问题，最小docid为控制循环的退出的条件是最小查询的次数?
			//所以我在前面增加了一个vector用来保存每个关键字对应的set的大小
			++(iterVec[iterIdx].second);
			if(iterVec[iterIdx].second == maxNum[iterIdx])
			{ isExisting = true; }
		}
	}
	return true;
}

string WordQuery::createJson(vector<int>& docIdVec,const vector<string>& queryWords)
{
	string result;
	for(auto id : docIdVec)
	{
		auto  webpage  = _pageLib.find(id);
		if(webpage != _pageLib.end())
		{
			string summary =webpage->second.summary(queryWords);
		//	string title = _pageLib[id].getTitle();
		//	string url = _pageLib[id].getUrl();
			string title = webpage->second.getTitle();
			string url = webpage->second.getUrl();
			result.append(title).append("\n").append(url).append("\n")
			.append(summary).append("\n");
		}
	}
	return result;
}




