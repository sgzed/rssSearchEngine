 ///
 /// @file    DirSacnner.h
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-31 11:01:52
 ///

#ifndef __WD_DIRSCANNER_H__
#define __WD_DIRSCANNER_H__

#include <string>
#include <vector>
using std::string;

class DirScanner
{
public:

	DirScanner(const string& dir);

	void openDir(const string& _dir);

	const std::vector<string>& getFiles() const
	{
		return _files;
	}

	void print() const ;

private:
	string _dir;
	std::vector<string> _files;
};

#endif
