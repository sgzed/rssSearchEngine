 ///
 /// @file    Preprocessor.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-31 22:38:59
 ///

#ifndef _WD_PAGE_LIB_PREPROCESSOR_H_
#define _WD_PAGE_LIB_PREPROCESSOR_H_


#include "../include/WebPage.h"
#include "../include/WordSegmentation.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::pair;
using std::map;
using std::unordered_map;
using std::set;

class Preprocessor
{
public:
	Preprocessor();

	void doProcess();

private:
	void readInfoFromFile();
	void cutRedundantPages();

	void storeOnDisk();
private:
	WordSegmentation _jieba;
	vector<WebPage> _pageLib;
	unordered_map<int,pair<int,int>> _offsetLib;
};


#endif  























