 ///
 /// @file    DirScanner.cc
 /// @author  sgzed(wunaisong@163.com)
 /// @date    2018-03-31 11:07:01
 ///

#include "DirScanner.h"

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <iostream>
using std::endl;  using std::cout;

DirScanner::DirScanner(const string& dir)
	:_dir(dir)
{
}

void DirScanner::openDir(const string& _dir)
{
	DIR * pdir = ::opendir(_dir.c_str());
	if(NULL == pdir)
	{
		perror("opendir");
		exit(EXIT_FAILURE);
	}
	
	::chdir(_dir.c_str());	
	struct dirent* mydirent;
	struct stat mystat;

	while(NULL != (mydirent=::readdir(pdir)))
	{
		::stat(mydirent->d_name,&mystat);
		
		if(S_ISDIR(mystat.st_mode))
		{
			if(0==::strncmp(mydirent->d_name,".",1) || 0==::strncmp(mydirent->d_name,"..",2))
			{
				continue;
			}
			else
			{
				openDir(mydirent->d_name);	
			}
		}
		else
		{
			string filename;				
			filename.append(::getcwd(NULL,0)).append("/")
				.append(mydirent->d_name);
			_files.push_back(filename);
		}
	}
}

void DirScanner::print() const
{
	for(auto iter : _files)
	{
		cout << iter << endl;
	}
}





