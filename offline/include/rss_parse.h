 ///
 /// @file    rss_parse.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-03 22:59:31
 ///

#ifndef __WD_RSS_PARSE_H__
#define __WD_RSS_PARSE_H__

#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include "tinyxml2.h"
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::shared_ptr;
using namespace tinyxml2;

struct RssItem
{
	string title;
	string link;
	string description;
	string content;
};

class RssReader
{
public:
	RssReader();
	
	void loadFeedFiles();

	void loadFeedFile(const string&);

	void parseRss(XMLDocument& doc);

	void dump();
private:
	vector<string> _files;
	vector<shared_ptr<RssItem>> _rss;
};

#endif
