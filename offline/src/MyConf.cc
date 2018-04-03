///
/// @file    MyConf.cc
/// @author  sgzed(wunaisong@163.com)
/// @date    2018-03-26 21:43:47
///

#include "../include/MyConf.h"

#include <string>
#include <fstream>
#include <sstream>
#include <utility>
#include <iostream>
using std::map;
using std::ifstream;
using std::cout;  using std::endl;
using std::istringstream;

map<string,string> MyConf::_configMap ;

set<string> MyConf::_stopWordList;

pthread_once_t MyConf::_ponce = PTHREAD_ONCE_INIT;

MyConf* MyConf::_value =NULL;

const char * const STOP_WORD_PATH ="/home/sgzed/cppjieba/dict/stop_words.utf8";

MyConf* MyConf::getInstance()
{
		pthread_once(&_ponce,&MyConf::init);
		assert(_value != NULL);
		return _value;
}

void MyConf::init()
{
	_value = new MyConf();
	string filepath("./conf/my.conf");
	ifstream ifs(filepath);
	string line,key,value;
	if(!ifs.good())
	{
		cout << "open " << filepath << " failed" << endl;
		return ;
	}
	else
	{
		while(getline(ifs,line))
		{
			istringstream iss(line);
			iss >> key >> value;
			_configMap.insert(make_pair(key,value));
		}
		ifs.close();
	}
	
	ifstream stopifs(STOP_WORD_PATH);	

	//string line;

	while(getline(stopifs,line))
	{
		istringstream iss(line);
		string word;
		iss >> word;
		cout <<  word << " " ;
		_stopWordList.insert(word);
	}
	cout << endl;
	::atexit(destroy);
}

map<string,string>& MyConf::getConfigMap()
{
	return _configMap;
}

void MyConf::show()
{
	for(auto iter : _configMap)
		cout <<  iter.first << " " << iter.second << endl;;
}

set<string>& MyConf::getStopWordList()
{
	return _stopWordList;
}


