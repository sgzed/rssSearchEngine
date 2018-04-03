#include "../include/rss_parse.h"
#include "../include/MyConf.h"
#include "../include/DirScanner.h"

#include <fstream>
#include <sstream>
#include <boost/regex.hpp>
#include <stdlib.h>

// struct RssItem
// {
// 	string title;
// 	string link;
// 	string description;
// 	string content;
// };

RssReader::RssReader()
{
	auto confMap =  MyConf::getInstance()->getConfigMap();

	string dir = confMap["yuliao"];

	DirScanner _dir(dir);

	_dir.openDir(dir);
	_files = _dir.getFiles();

}


void RssReader::loadFeedFiles()
{
	for(size_t idx=0;idx!=_files.size();++idx)
	{
		loadFeedFile(_files[idx]);
		cout << "has load "  << idx+1 <<" rss files" << endl;
//		::sleep(1);
	}
}

void RssReader::loadFeedFile(const string& filename)
{
	XMLDocument doc;

	cout << "file = " << filename << endl;
	int res=doc.LoadFile(filename.c_str());
	if(res!=0)
	{
		cout<<"load xml file failed"<<endl;
	}
	else
	{
		parseRss(doc);
	}
}

void RssReader::parseRss(XMLDocument& doc)
{
		cout << "parse XML ing" << endl;
			
		XMLElement* root=doc.FirstChildElement();

		XMLElement* channelNode=root->FirstChildElement("channel");

		XMLElement* userNode=channelNode->FirstChildElement("item");

		if(userNode==NULL)
			cout <<"Here" << endl;
		else
			cout << "found" << endl;

		while(userNode!=NULL)
		{
			shared_ptr<RssItem>  item(new RssItem);
			cout << "1----->" << endl;
			XMLElement * titleNode = userNode->FirstChildElement("title");
			item->title = titleNode->GetText();
			XMLElement * linkNode = userNode->FirstChildElement("link");
			item->link = linkNode->GetText();
			XMLElement * descriptionNode = userNode->FirstChildElement("description");
			item->description = descriptionNode->GetText();
			const char* iContent = nullptr;
			XMLElement * contentNode = userNode->FirstChildElement("content:encoded");
			cout << "2----->" << endl;
			if(contentNode!=NULL)
			{
				 iContent = contentNode->GetText();
			}
			else
				iContent = item->description.c_str();
			string szReg = "<.*?>";
			boost::regex fmt(szReg);
			string content = boost::regex_replace(string(iContent),fmt,string(""));
			item->content  = content;
			userNode = userNode->NextSiblingElement();
			_rss.push_back(item);
	   }
}

const string int2str(int num)
{
	std::ostringstream oss ;
	oss << num;
	return oss.str();
}

void RssReader::dump()
{
	int i=1;

	auto confMap = MyConf::getInstance()->getConfigMap();
	cout << "ripepagelib = " <<  confMap["ripepagelib"] << endl;
	std::ofstream ofs(confMap["ripepagelib"]);
	//std::ofstream ofs(confMap["ripepagelib"]);
	
	cout << "offsetlib = " <<  confMap["offsetlib"] << endl;
	std::ofstream offset(confMap["offsetlib"]);

	if(!ofs.good() || !offset.good())
	{
		cout << "open pagelib or offsetlib failed" <<endl;
		return ;
	}
	string doc;
	std::ofstream::pos_type pos;
	for(auto item : _rss)	
	{
		doc ="<doc>\n <docid>";
		string count = int2str(i);
		//cout << "cnt  = " << i << endl;
		doc.append(count);
		doc.append("</docid>\n ");
		doc.append("<title>");
		doc.append(item->title);
		doc.append("</title>\n  ");
		doc.append("<link>");
		doc.append(item->link);
		doc.append("</link>\n  ");
		doc.append("<description>");
		doc.append(item->description);
		doc.append("</description>\n  ");
		doc.append("<content>");
		doc.append(item->content);
		doc.append("</content>\n");
		doc.append("</doc>\n");
		offset << i << " " <<  ofs.tellp() << " " << doc.size() << "\n";
		ofs << doc;	
		++i;
	}
	offset.close();
	ofs.close();
}



