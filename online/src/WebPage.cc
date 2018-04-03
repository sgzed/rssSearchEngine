///
/// @file    WebPage.cc
/// @author  sgzed(wunaisong@163.com)
/// @date    2018-03-31 21:34:13
///

#include "../include/WebPage.h"
#include "../include/simhash/Simhasher.hpp"
#include "../include/MyConf.h"
#include <stdlib.h>
using namespace simhash;

WebPage::WebPage(string& doc, WordSegmentation& jieba)
{
	_keyWords.reserve(10);
	processDoc(doc,jieba);
}

void WebPage::processDoc(const string& doc,WordSegmentation& jieba)
{
	string docIdHead = "<docid>";
	string docIdTail = "</docid>";
	string docUrlHead = "<link>";
	string docUrlTail = "</link>";
	string docTitleHead = "<title>";
	string docTitleTail = "</title>";
	string docContentHead = "<content>";
	string docContentTail = "</content>";

	int bpos = doc.find(docIdHead);
	int epos = doc.find(docIdTail);
	string docId = doc.substr(bpos+docIdHead.size(),
			epos-bpos-docIdHead.size());
	_docId = atoi(docId.c_str());
	cout << " docid : "  << _docId << endl;

	bpos = doc.find(docTitleHead);
	epos = doc.find(docTitleTail);
	_docTitle = doc.substr(bpos + docTitleHead.size(), 
			epos - bpos - docTitleHead.size());
	cout << "Title : " << _docTitle << endl;
	
	bpos = doc.find(docUrlHead);
	epos = doc.find(docUrlTail);
	_docUrl = doc.substr(bpos + docUrlHead.size(), 
			epos - bpos - docUrlHead.size());
	cout << "Url : " << _docTitle << endl;

	bpos = doc.find(docContentHead);
	epos = doc.find(docContentTail);
	_docContent = doc.substr(bpos + docContentHead.size(),
			epos - bpos - docContentHead.size());
	
	vector<string> wordsVec = jieba(_docContent.c_str());

	set<string>& stopWords = MyConf::getInstance()->getStopWordList();

	for(auto& word : wordsVec)
	{
		auto sit = stopWords.find(word);
		if(sit == stopWords.end())
			++_wordsMap[word];
	}
	
	showKeywords();

	_doc = doc;
	jieba.getTopK(doc,_keyWords,TOPK_NUMBER);
}

void WebPage::showKeywords() const
{
	for(auto iter : _wordsMap)
		cout << iter.first << " --> " << iter.second << endl;
}

uint64_t WebPage::getSimhash()
{
	Simhasher simhasher(DICT_PATH,HMM_PATH,IDF_PATH,STOP_WORD_PATH);

	string s  = _docContent;
	uint64_t  u64 = 0;

	simhasher.make(s,TOPK_NUMBER,u64);
	cout << "hash值为 : " << u64 << endl;
	return u64;
}


bool operator==(const WebPage& lhs,const WebPage& rhs)
{
	Simhasher simhasher(DICT_PATH,HMM_PATH,
			IDF_PATH,STOP_WORD_PATH);
	
	string s1  = lhs.getContent();
	string s2  = rhs.getContent();

	size_t topN = WebPage::TOPK_NUMBER;
	uint64_t u1 =0,u2=0;
	vector<pair<string,double>> res1,res2;
	
//	simhasher.extract(s1,res1,topN);
//	cout << "s1.size() = "<< s1.size() <<  " 关键词序列是 : " << res1 << endl;
	simhasher.make(s1,topN,u1);
//	auto confMap = MyConf::getInstance()->getConfigMap();
	cout << "s1 . hash值为 : " << u1 << endl;
	
//	simhasher.extract(s2,res2,topN);
//	cout << "s2.size() = " << s2.size() << " 关键词序列是 : " << res2 << endl;
	simhasher.make(s2,topN,u2);
	cout << "s2 . hash值为 : " << u2 << endl;

	cout << "s1 == s2 ? --> " << Simhasher::isEqual(u1,u2,6) << endl;

	return Simhasher::isEqual(u1,u2,6);
}

string WebPage::summary(const vector<string>& queryWords)
{
	vector<string> summaryVec;

	istringstream iss(_docContent);

	string line;

	while(iss >> line)
	{
		for(auto word : queryWords)
		{
			if(line.find(word) != string::npos)
			{
				summaryVec.push_back(line);
				break;
			}
		}

		if(summaryVec.size()>=5)
		{ break; }
	}

	string summary;
	for(auto line : summaryVec)
	{
		summary.append(line).append("\n");
	}
	return summary;
}









