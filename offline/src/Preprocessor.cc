 ///
 /// @file    Preprocessor.cc
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-31 22:45:14
 ///

#include "../include/Preprocessor.h"
#include "../include/MyConf.h"

#include <math.h>
#include <fstream>
#include <sstream>
using std::ofstream;
using std::ifstream;
using std::stringstream;

Preprocessor::Preprocessor()
	:_jieba()
{
}

void Preprocessor::doProcess()
{
	readInfoFromFile();

	cutRedundantPages();

	buildInvertIndexTable();

	storeOnDisk();
}


void Preprocessor::readInfoFromFile()
{
	map<string,string> confMap = MyConf::getInstance()->getConfigMap();

	string pageLibPath = confMap["ripepagelib"];
	string offsetLibPath = confMap["offsetlib"];

	ifstream pageIfs(pageLibPath.c_str());
	ifstream offsetIfs(offsetLibPath.c_str());

	if(!pageIfs.good() || !offsetIfs.good())
	{
		cout << "open pageLib or offsetlib failed" << endl;
		return ;
	}
	string line;
	size_t docId,docOffset,docLen;

	while(getline(offsetIfs,line))
	{
		stringstream  ss(line);
		ss >> docId >> docOffset >> docLen;
		string doc;

		doc.resize(docLen,' ');

		pageIfs.seekg(docOffset,pageIfs.beg);
		pageIfs.read(&*doc.begin(),docLen);

		WebPage webPage(doc,_jieba);
		_pageLib.push_back(webPage);

		_offsetLib.insert(std::make_pair(docId,std::make_pair(docOffset,docLen)));
	}
#if 1	
	for(auto mit : _offsetLib)
	{
		cout << mit.first<< "--> " << "(" << mit.second.first << "," << mit.second.second<< ")"
			<< endl;
	}
#endif 
}

void Preprocessor::cutRedundantPages() 
{
	int cnt = 0; 
#if 0
	for(size_t idx=0;idx != _pageLib.size()-1 ; ++idx)
	{
		 for(size_t j = idx+1; j!=_pageLib.size();++j)
	    {
	    	if(_pageLib[idx] == _pageLib[j])	
	    	{
	    		_pageLib[j] = _pageLib[_pageLib.size()-1];
	    		_pageLib.pop_back();
	    		--j;
	    		++cnt;
	    	}
	    }
	}
#endif 	
	vector<uint64_t> simHashs;
	simHashs.reserve(_pageLib.size());
	cout << "pagelib.size()  =  "  << _pageLib.size() << endl;
	for(auto& iter : _pageLib)
	{
		simHashs.push_back(iter.getSimhash());
	}

	for(size_t idx=0;idx != _pageLib.size()-1 ; ++idx)
	{
		 for(size_t j = idx+1; j!=_pageLib.size();++j)
	    {
	    	if(simHashs[idx] == simHashs[j])	
	    	{
	    		_pageLib[j] = _pageLib[_pageLib.size()-1];
	    		_pageLib.pop_back();
	    		--j;
	    		++cnt;
	    	}
	    }
	}
	cout << "the number of redundantpages is " << cnt << endl;  
}

void Preprocessor::buildInvertIndexTable()
{
	
	for(auto& page : _pageLib)
	{
		auto& wordsMap = page.getWordsMap();
		cout << wordsMap.size() << endl;
		for(auto word : wordsMap)
		{
			cout << "string = " << word.first;
			_invertIndexTable[word.first].push_back(std::make_pair(page.getDocId(),word.second));
		}
	}

	size_t totalFiles = _pageLib.size();

	map<int,double> weightsum;

	for(auto & index : _invertIndexTable)
	{
		int df  = _invertIndexTable[index.first].size();
		int idf = log(static_cast<double>(totalFiles)/df)/log(2) +1 ;
		for(auto & sitem : index.second)
		{
		    double weight = sitem.second * idf;
			weightsum[sitem.first] +=  pow(weight,2);
			sitem.second =weight;
		}
	}
	
	for(auto& iter : _invertIndexTable)
	{
		for(auto& siter : iter.second)
		{
			siter.second = siter.second / sqrt(weightsum[siter.first]);
		}
	}

	cout << "_invertIndexTable.size() = " << _invertIndexTable.size() << endl;
#if 1
	for(auto iter :_invertIndexTable)
	{
		cout << iter.first << "\t";
		for(auto siter : iter.second)
		{
			cout << siter.first <<"\t" << siter.second << "\t" ;
		}
		cout << endl;
	}
#endif
}

void Preprocessor::storeOnDisk()
{
	auto confMap = MyConf::getInstance()->getConfigMap();

	string pagePath = confMap["ripepagelib"]; 
	string offsetPath = confMap["offsetlib"]; 

	ofstream ofsPageLib(pagePath.c_str());	
	ofstream offsetLib(offsetPath.c_str());
	
	if(!ofsPageLib.good() || !offsetLib.good())
	{
		cout << "new pageLib or offsetLib open failed" << endl;
		return ;
	}

	for(auto & page : _pageLib)
	{
		int id = page.getDocId();
		int len  = page.getDoc().size();
		ofstream::pos_type offset = ofsPageLib.tellp();
		ofsPageLib << page.getDoc();

		offsetLib << id << " " << offset << " " << len << "\n";
	}

	ofsPageLib.close();
	offsetLib.close();

	string invertIndexPath = confMap["invertindexlib"];
	cout << "invertIndexPath = "  << invertIndexPath << endl;
	ofstream invertofs(invertIndexPath.c_str());
	if(!invertofs.good())
	{
		cout << "open invertIndexPath failed" << endl;
		return ;
	}

	for(auto iter : _invertIndexTable)
	{
		invertofs << iter.first << "\t";
		for(auto siter : iter.second)
		{
			invertofs << siter.first << "\t" << siter.second << "\t" ;
		}
		invertofs << endl;
	}

	invertofs.close();
}




















