/**************************************************************************
 * Copyright(c) 2012-2013 Regents of the University of California
 *              Andrew B. Kahng, ilgweon Kang, Siddhartha Nath, Ching-Yao
 *              Liu
 *  Contact     abk@cs.ucsd.edu, igkang@ucsd.edu, sinath@ucsd.edu
 *  Affiliation:    UCSD, Computer Science and Engineering Department,
 *                  La Jolla, CA 92093-0404, USA
 *
 *************************************************************************/

/**************************************************************************
 * Incremental Scan Chain
 * mymeasure.h
 * measure memory (KB) and time (seconds).
 *************************************************************************/

#ifndef MYMEASURE_H
#define MYMEASURE_H

#define fatal(CONDITION, ERRMESSAGE)                                \
{  if (CONDITION)                                            		\
	{  std::cerr << std::endl << (ERRMESSAGE) ;                     \
		std::cerr <<"  (Fatal error in " << fileNameTrim(__FILE__)  \
		<< ":" << __LINE__  <<")"<<std::endl; 						\
		char * __p=(char *) 0;__p[0]=0;  							\
		/* Armageddon now */										\
	}                                                               \
}

#include <ctime>
#include <string>
#include <vector>
#include <sparsehash/dense_hash_map>

using google::dense_hash_map;
using   namespace   std;

class CMeasure {
	private:
		dense_hash_map<string, double> timeStor;

		// this is intended for saving starting clock.
		dense_hash_map<string, double> timeTmpStor;

		// true -> recording start
		// false -> recording end.
		dense_hash_map<string, bool> inputChk;

    public:
		CMeasure() {
			timeStor.set_empty_key("");
			timeTmpStor.set_empty_key("");
			inputChk.set_empty_key("");
		}

        double 				wbegin;

		clock_t				cbegin;
        vector<string>      tasks;
        vector<double>      welapsed_secs, celapsed_secs;


        // functions
        void process_mem_usage();
        void start_clock();
        void stop_clock(string task);
        void print_clock();

        void printMemoryUsage(void);

		// print only function
        void print_only(string task);

		// accumulation clock functions
		void accm_clock_start(string task);
		void accm_clock_end(string task);
		void accm_clock_print(string task);
};

const char* fileNameTrim(const char* fileName);

#endif
