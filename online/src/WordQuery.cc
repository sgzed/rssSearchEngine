///
/// @file    WordQuery.cc
/// @author  sgzed(wunaisong@163.com)
/// @date    2018-04-01 20:49:50
///

#include "../include/WordQuery.h"
#include "../include/MyConf.h"

#include <json/json.h>

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
		cout << "open invertindextable failed" <<endl;
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

	auto& stopWords =MyConf::getInstance()->getStopWordList();
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
	
	for(auto iter : queryWords)
	{
		cout << iter << "#\t";
		auto uit = _invertIndexTable.find(iter);
		if(uit == _invertIndexTable.end())
		{
			auto sit = stopWords.find(iter);
			if(sit == stopWords.end())
			{
				cout << "can't found " << endl;
				return returnNoAnswer();
			}
		}
	}
	cout << endl;
	vector<double> weightList = getQueryWordsWeightVector(queryWords);
	
	SimilarityCompare similarityCmp(weightList);
	vector<pair<int,vector<double>>> resultVec;

	if(executeQuery(queryWords,resultVec))
	{
		stable_sort(resultVec.begin(),resultVec.end(),similarityCmp);
		vector<int> docIdVec;
		for(auto item : resultVec)
		{
			docIdVec.push_back(item.first);
		}
		return createJson(docIdVec,queryWords);
	}
	else
		return returnNoAnswer();
}

vector<double> WordQuery::getQueryWordsWeightVector(vector<string> & queryWords)
{
	int totalFiles = _pageLib.size();
	
	map<string,int> wordsMap;
	for(auto& iter : queryWords)
	{
		++wordsMap[iter];	
	}
	
	double weightsum = 0;
	vector<double> weightList;
	for(auto& iter : queryWords)
	{
		int df = _invertIndexTable[iter].size();
		double idf = log(static_cast<double>(totalFiles/df))/log(2)+1;
		int tf = wordsMap[iter];
		double weight = idf * tf;
		weightsum += pow(weight,2);
		weightList.push_back(weight);
	}

	for(auto& item : weightList)
		item/=sqrt(weightsum);
	cout << "weightList's elem : ";
	for(auto iter :weightList)
		cout << iter << "\t";
	cout << endl;
	return weightList;
}

string WordQuery::returnNoAnswer()
{
	//return string("404 Not Found\n");

	Json::Value root;
	Json::Value arr;
	
	Json::Value elem;
	elem["title"]="404,not found";
	elem["summary"] = "亲,找不到，试试其他的关键词吧!";
	elem["url"] = "";
	arr.append(elem);
	root["files"] = arr;
	Json::StyledWriter writer;
	return writer.write(root);
}

bool WordQuery::executeQuery(const vector<string>& queryWords,
		vector<pair<int,vector<double>>>& resultVec)
{
	if(queryWords.size()==0)
	{
		cout << "empty string , not found " << endl;
		return false;
	}

	typedef set<pair<int,double>>::iterator setIter;
	vector<pair<setIter,int>> iterVec;
	vector<int> maxNum;

	int minIterNum = 0x7FFFFFFF;
		
	for(auto& iter : queryWords)
	{
		int sz = _invertIndexTable[iter].size();
		if(sz == 0)
			return false;
		if(sz < minIterNum)
			minIterNum = sz;
		iterVec.push_back(make_pair(_invertIndexTable[iter].begin(),0));
		maxNum.push_back(sz);
	}

	cout << "minIterNum = " << minIterNum << endl;

	bool isExiting = false;
	
	while(!isExiting)
	{
		size_t idx=0;
		for(;idx!=iterVec.size()-1;++idx)
		{
			if(iterVec[idx].first->first != iterVec[idx+1].first->first)
				break;
		}
		if(idx == iterVec.size()-1)
		{
			vector<double> weightVec;
			int docId = iterVec[0].first->first;

			for(idx=0;idx!=iterVec.size();++idx)
			{
				weightVec.push_back(iterVec[idx].first->second);
				++(iterVec[idx].first);
				++(iterVec[idx].second);
				if(iterVec[idx].second == minIterNum)
					isExiting = true;
			}
			resultVec.push_back(make_pair(docId,weightVec));
		}
		else
		{
			int minDocid = 0x7FFFFFFF;
			int iterIdx;
			for(idx=0;idx!=iterVec.size();++idx)
			{
				if(iterVec[idx].first->first < minDocid)
				{
					minDocid = iterVec[idx].first->first;
					iterIdx = idx;
				}
			}
			++(iterVec[iterIdx].first);
			++(iterVec[iterIdx].second);
			if(iterVec[iterIdx].second == maxNum[iterIdx])
				isExiting = true;
		}
	}
	return true;
}

string WordQuery::createJson(vector<int>& docIdVec,const vector<string>& queryWords)
{
	Json::Value root;
	Json::Value arr;

	int cnt =0;
	
	 //string result;
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
		
			Json::Value elem;
	
			elem["title"] = title;
			elem["summary"] = summary;
			elem["url"] = url;
			arr.append(elem);
			if(++cnt == 20)
				break;
			//result.append(title).append("\n").append(url).append("\n")
			//.append(summary).append("\n");
		}
	}

	root["files"] = arr;
	Json::StyledWriter writer;
	return writer.write(root);
}




