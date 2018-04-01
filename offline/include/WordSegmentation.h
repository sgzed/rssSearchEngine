///
/// @file    WordSegmentation.h
/// @author  sgzed(wunaisong@163.com)
/// @date    2018-03-31 21:08:55
///

#ifndef _WD_WORD_SEGMENTATION_H_
#define _WD_WORD_SEGMENTATION_H_


#include "../include/cppjieba/Jieba.hpp"

#include <iostream>
#include <string>
#include <vector>
using std::cout;
using std::endl;
using std::string;
using std::vector;

		const char * const DICT_PATH = "/home/sgzed/cppjieba/dict/jieba.dict.utf8";
		const char * const HMM_PATH = "/home/sgzed/cppjieba/dict/hmm_model.utf8";
		const char * const USER_DICT_PATH = "/home/sgzed/cppjieba/dict/user.dict.utf8";
		const char * const IDF_PATH ="/home/sgzed/cppjieba/dict/idf.utf8";
		const char * const STOP_WORD_PATH ="/home/sgzed/cppjieba/dict/stop_words.utf8";

class WordSegmentation
{
	public:
		WordSegmentation()
			:_jieba(DICT_PATH,HMM_PATH,USER_DICT_PATH,IDF_PATH,STOP_WORD_PATH)
		{
			cout << "jieba init" << endl;
		}

		vector<string> operator()(const char* src)
		{
			vector<string> words;
			_jieba.CutAll(src,words);
			return words;
		}

		void getTopK(string doc,vector<cppjieba::KeywordExtractor::Word>& keywords,int k)
		{	
			_jieba.extractor.Extract(doc,keywords,k);	
		}

	private:
		cppjieba::Jieba _jieba;
};

#endif



