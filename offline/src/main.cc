 ///
 /// @file    main.cc
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-30 16:58:27
 ///

#include "../include/MyConf.h"
#include "../include/rss_parse.h"
#include "../include/DirScanner.h"
#include "../include/Preprocessor.h"

int main()
{
	MyConf::getInstance()->show();

#if 1	
	RssReader rssrd;
	rssrd.loadFeedFiles();
	rssrd.dump();
#endif

	Preprocessor processor = Preprocessor();
	processor.doProcess();

//	DirScanner dir(confMap["yuliao"]);	
//	dir.openDir(confMap["yuliao"]);	

//	dir.print();
}
