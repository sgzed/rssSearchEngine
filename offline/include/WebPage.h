 ///
 /// @file    WebPage.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-31 21:16:38
 ///

#ifndef __WD_WEBPAGE_H__
#define __WD_WEBPAGE_H__

#include "WordSegmentation.h"
#include <vector>
#include <map>
#include <set>
#include <string>
using std::map;
using std::vector;
using std::string;
using std::set;

class WebPage
{
	friend bool operator == (const WebPage& lhs,const WebPage& rhs);
	//friend bool operator < (const WebPage& )

public:
	static const int TOPK_NUMBER = 15;
	
	WebPage(string& doc,WordSegmentation& jieba);

	int getDocId() 
	{ return _docId; }

	map<string,int>& getWordsMap()
	{ return _wordsMap; }

	string getDoc() const 
	{ return _doc;}

	string getContent() const
	{ return _docContent; }

	uint64_t getSimhash();
	
	void showKeywords() const;

private:
	void processDoc(const string& dic,WordSegmentation& jieba);

private:
	string _doc;
	int _docId;
	string _docTitle;
	string _docUrl;
	string _docContent;
	string _docSummary;    // 摘要
	
	//vector<string> _topWords;
	vector<cppjieba::KeywordExtractor::Word> _keyWords;
	map<string,int> _wordsMap;

};


#endif
