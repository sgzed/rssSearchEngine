 ///
 /// @file    Singletion.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-26 20:11:06
 

#ifndef __WD_MYCONF_H__
#define __WD_MYCONF_H__

#include <pthread.h>
#include <stdlib.h>
#include <assert.h>

#include <map>
#include <set>
#include <string>
using std::map;
using std::set;
using std::string;

class MyConf 
{
public:
	static MyConf* getInstance();
	static void init();
	void show(); 
	map<string,string>& getConfigMap();
	set<string>& getStopWordList();

private:
	MyConf()
	{}

	~MyConf()
	{} 

	static void  destroy()
	{ delete _value;}

	static map<string,string> _configMap;
	static set<string> _stopWordList;
	static pthread_once_t _ponce; 
	static MyConf* _value;
};

#endif
