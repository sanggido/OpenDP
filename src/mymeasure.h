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

#define fatal(CONDITION, ERRMESSAGE)                                     \
  {                                                                      \
    if(CONDITION) {                                                      \
      std::cerr << std::endl << (ERRMESSAGE);                            \
      std::cerr << "  (Fatal error in " << fileNameTrim(__FILE__) << ":" \
                << __LINE__ << ")" << std::endl;                         \
      char* __p = (char*)0;                                              \
      __p[0] = 0;                                                        \
      /* Armageddon now */                                               \
    }                                                                    \
  }

#include <ctime>
#include <string>
#include <vector>
#include <unordered_map>


class CMeasure {
 private:
  std::unordered_map< std::string, double > timeStor;

  // this is intended for saving starting clock.
  std::unordered_map< std::string, double > timeTmpStor;

  // true -> recording start
  // false -> recording end.
  std::unordered_map< std::string, bool > inputChk;

 public:
  CMeasure() {}

  double wbegin;

  clock_t cbegin;
  std::vector< std::string > tasks;
  std::vector< double > welapsed_secs, celapsed_secs;

  // functions
  void process_mem_usage();
  void start_clock();
  void stop_clock(std::string task);
  void print_clock();

  void printMemoryUsage(void);

  // print only function
  void print_only(std::string task);

  // accumulation clock functions
  void accm_clock_start(std::string task);
  void accm_clock_end(std::string task);
  void accm_clock_print(std::string task);
};

const char* fileNameTrim(const char* fileName);

#endif
