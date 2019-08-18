/**************************************************************************
 * Copyright(c) 2012-2013 Regents of the University of California
 *              Andrew B. Kahng, ilgweon Kang, Siddhartha Nath, Ching-Yao
 *              Liu
 *  Contact     abk@cs.ucsd.edu, igkang@ucsd.edu, sinath@ucsd.edu
 *  Affiliation:    UCSD, Computer Science and Engineering Department,
 *                  La Jolla, CA 92093-0404, USA
 *
 *************************************************************************/

#include "mymeasure.h"
#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <iomanip>
#include <time.h>
#include <sys/time.h>
#include <cmath>
#include <string.h>
#include <cstdio>

using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::ifstream;
using std::ios_base;
using std::setw;
using std::fixed;
using std::right;
using std::setprecision;

// attempts to read the system-dependent data for a process' virtual memory
// size and resident set size, and return the results in KB.
// VmSize:
// Virtual memory size
// rss:
// Resident Set Size: number of pages the process has in real
// memory.  This is just the pages which count toward text,
// data, or stack space.  This does not include pages which
// have not been demand-loaded in, or which are swapped out.

// void CMeasure::process_mem_usage(double& vm_usage, double& resident_set)
void CMeasure::process_mem_usage() {
  // vm_usage	  = 0.0;
  // resident_set = 0.0;
  double vm_usage = 0.0;
  double resident_set = 0.0;

  // 'file' stat seems to give the most reliable results
  //
  ifstream stat_stream("/proc/self/stat", ios_base::in);

  // dummy vars for leading entries in stat that we don't care about
  //
  string pid, comm, state, ppid, pgrp, session, tty_nr;
  string tpgid, flags, minflt, cminflt, majflt, cmajflt;
  string utime, stime, cutime, cstime, priority, nice;
  string O, itrealvalue, starttime;

  // the two fields we want
  //
  unsigned long vsize;
  long rss;

  stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr >>
      tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >>
      stime >> cutime >> cstime >> priority >> nice >> O >> itrealvalue >>
      starttime >> vsize >> rss;  // don't care about the rest

  stat_stream.close();

  long page_size_kb = sysconf(_SC_PAGE_SIZE) /
                      1024;  // in case x86-64 is configured to use 2MB pages
  vm_usage = vsize / 1024.0;
  resident_set = rss * page_size_kb;

  // cout<<comm<<endl;
  printf("virtual memory size : %d\n", vm_usage);
  printf("resident Set Size   : %d\n", resident_set);
  printf("----------------------------------------");
  fflush(stdout);
}

void CMeasure::start_clock() {
  welapsed_secs.clear();
  celapsed_secs.clear();

  struct timeval* time = new timeval;
  if(gettimeofday(time, NULL))
    wbegin = 0;
  else
    wbegin = time->tv_sec + .000001 * time->tv_usec;

  delete time;

  cbegin = clock();
}

void CMeasure::stop_clock(string task) {
  tasks.push_back(task);

  double wend = 0;
  struct timeval* time = new timeval;
  if(gettimeofday(time, NULL))
    wend = 0;
  else
    wend = time->tv_sec + .000001 * time->tv_usec;

  delete time;

  welapsed_secs.push_back(double(wend - wbegin));

  clock_t cend = clock();
  celapsed_secs.push_back(double(cend - cbegin) / CLOCKS_PER_SEC);
}

void CMeasure::accm_clock_start(string task) {
  fatal(task == "", "Empty task is not permitted");
  fatal(inputChk.find(task) != inputChk.end() && inputChk[task] == true,
        "Already started this task:" + task);

  inputChk[task] = true;

  struct timeval time;
  gettimeofday(&time, NULL);
  timeTmpStor[task] = double(time.tv_sec + .000001 * time.tv_usec);
  //	cout << task << ", start: " << double(time.tv_sec + .000001 *
  //time.tv_usec) << endl;
  //	cout << task << ", start: " << timeTmpStor[task] << endl;
}

void CMeasure::accm_clock_end(string task) {
  fatal(timeTmpStor.find(task) == timeStor.end(), "Cannot find task name!");
  fatal(inputChk.find(task) == inputChk.end(), "task is not initlalized");
  fatal(inputChk[task] == false, "Already ended this task:" + task);

  inputChk[task] = false;

  struct timeval time;
  gettimeofday(&time, NULL);
  if(timeStor.find(task) == timeStor.end()) {
    timeStor[task] = 0.0f;
  }

  timeStor[task] +=
      double((time.tv_sec + .000001 * time.tv_usec) - timeTmpStor[task]);
  //	cout << task << ", end: " << time.tv_sec + .000001* time.tv_usec <<
  //endl;
  //	cout << task << "calculated: " << timeStor[task] << endl;
}

void CMeasure::accm_clock_print(string task) {
  fatal(timeStor.find(task) == timeStor.end(), "Cannot find task in timeStor");

  cout << " tasks" << setw(25) << "Wtime" << endl;
  cout << " " << task << right << setw(30 - task.size()) << fixed
       << setprecision(3) << timeStor[task] << endl;
}

void CMeasure::print_clock() {
  printf(" %-25s %10s %10s\n", "tasks", "Wtime", "Ctime");
  for(int i = 0; i < tasks.size(); i++) {
    //		int length1 = log10(celapsed_secs[i]) > 0? log10(celapsed_secs[i]):
    //0;
    //		int length2 = log10(welapsed_secs[i]) > 0? log10(welapsed_secs[i]):
    //0;
    //		int length = length1 + length2;

    printf(" %-25s %10.3f %10.3f\n", tasks[i].c_str(), welapsed_secs[i],
           celapsed_secs[i]);
    //		cout << tasks[i] << right << setw(30-tasks[i].size())
    //				<< fixed << setprecision(3) << welapsed_secs[i]
    //				<< setw(15) << celapsed_secs[i] << endl;
  }
  fflush(stdout);
}

void CMeasure::printMemoryUsage(void) {
  ifstream status("/proc/self/status");
  string data;

  if(status.good()) {
    for(int i = 0; i < 2; ++i) getline(status, data);

    // account for login and loginlinux versions
    getline(status, data);
    if(data.find("SleeAVG") == string::npos)
      for(int i = 0; i < 7; ++i) getline(status, data);
    else
      for(int i = 0; i < 6; ++i) getline(status, data);

    // vmPeak
    getline(status, data);
    cout << endl << "----------------------------------------";
    cout << endl << "### VmPeak\t\t\t: " << data << endl;
  }
  status.close();

  status.open("/proc/self/stat");
  if(status.good()) {
    double vmsize;
    for(unsigned i = 0; i < 22; ++i) {
      status >> data;
    }
    status >> vmsize;

    cout << "### Memory Usage\t: " << vmsize / 1048576. << " MB" << endl;
  }
  status.close();
}

void CMeasure::print_only(string task) {
  double wend = 0;
  struct timeval* time = new timeval;
  if(gettimeofday(time, NULL))
    wend = 0;
  else
    wend = time->tv_sec + .000001 * time->tv_usec;

  delete time;

  double welapsed = wend - wbegin;

  clock_t cend = clock();
  double celapsed = (cend - cbegin) / CLOCKS_PER_SEC;

  printf("%-25s %10.3f %10.3f\n", task.c_str(), welapsed, celapsed);
  fflush(stdout);

  //	cout << task << right << setw(30-task.size())
  //		<< fixed << setprecision(3) << welapsed
  //		<< setw(15) << celapsed << endl;
}

const char* fileNameTrim(const char* fileName) {
  const char pathDelim = '/';
  const char* sp = fileName;
  const char* leftDelim = strchr(fileName, pathDelim);
  const char* rightDelim = strrchr(fileName, pathDelim);
  while(leftDelim != rightDelim) {
    sp = ++leftDelim;
    leftDelim = strchr(sp, pathDelim);
  }
  return sp;
}
