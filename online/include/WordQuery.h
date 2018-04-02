 ///
 /// @file    WordQuery.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-04-01 20:24:17
 ///

#ifndef __WD_WORDQUERY_H__
#define __WD_WORDQUERY_H__

#include "WebPage.h"
#include "WordSegmentation.h"

#include <unordered_map>
using std::unordered_map;
using std::pair;

class WordQuery
{
public:
	WordQuery();

	string doQuery(const string& str);
private:
	void loadLibrary();
	vector<double> getQueryWordsWeightVector(vector<string>& querys);

	bool executeQuery(const vector<string> & queryWords
			, vector<pair<int, vector<double> > > & resultVec);

	string createJson(vector<int>& docIdVec,const vector<string> & queryWords);

	string returnNoAnswer();
	
private:
	WordSegmentation _jieba;
	unordered_map<int,WebPage> _pageLib;
	unordered_map<int,pair<int,int>> _offsetLib;
	unordered_map<string,set<pair<int,double>>> _invertIndexTable;
};

#endif
