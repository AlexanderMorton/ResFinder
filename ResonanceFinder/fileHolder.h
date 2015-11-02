// $Id$
/*
 * @file 
 * @author Aaron Armbruster <aaron.james.armbruster@cern.ch>
 * @date Sep, 2015
 * @brief 
 */

#ifndef __RF_fileHolder_h__
#define __RF_fileHolder_h__



#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <set>

using namespace std;

//class fileHolder
struct fileHolder
{

  fileHolder()
  :
  nrCols(8),
  skipFirstLine(false),
  useStrings(false)
  {

  }
  
  int nrCols;
  bool skipFirstLine;
  vector<vector<double> > rates; // row: mass, col: sample
  vector<double> massPoints;
  vector<string> massPointsS;

  string _name;
  bool useStrings;
  void setUseStrings(bool flag=true) ;

  void setNrCols(int n) ;

  bool readInFile(string name) ; 
  
  void writeOutFile(string name);
  
  vector<double> getCol(int col);
  
  double getRate(int row, int col); 

  double getRateByMass(double mass, int col);

  void setRate(double mass, int col, double val);

  void copy(fileHolder& alt);
  void removeMass(double mass);

  void addMass(double mass, vector<double>& rate);
  void addFile(fileHolder& other);

  void addCol(vector<double>& addRates, int col);
  void useMedian(int col); 

  void skipFirst();

 
};
#endif
