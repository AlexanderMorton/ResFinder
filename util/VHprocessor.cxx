#include <TFile.h>
#include <iostream>
#include <fstream>
#include "ResonanceFinder/VHAnalysisRunner.h"
#include "ResonanceFinder/PullPlotter.h"

using namespace RF;
using namespace std;

void tokenize(const string& str,
	      vector<string>& tokens,
	      const string& delimiters = " ")
{
  // Skip delimiters at beginning.
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);
  // Find first "non-delimiter".
  string::size_type pos     = str.find_first_of(delimiters, lastPos);

  while (string::npos != pos || string::npos != lastPos)
    {
      // Found a token, add it to the vector.
      tokens.push_back(str.substr(lastPos, pos - lastPos));
      // Skip delimiters.  Note the "not_of"
      lastPos = str.find_first_not_of(delimiters, pos);
      // Find next "non-delimiter"
      pos = str.find_first_of(delimiters, lastPos);
    }
}

string trim(string const& source, char const* delims = " \t\r\n") {

  string result(source);
  
  string::size_type index = result.find_last_not_of(delims);
  
  if(index != string::npos)
    result.erase(++index);

  index = result.find_first_not_of(delims);
  if(index != string::npos)
    result.erase(0, index);
  else
    result.erase();
  return result;
}

class Configuration {
  
public: 
  Configuration(string configFile){ 
    _configOK = false;
    
    _doPull = false;
    _doLimit = false;
    _doPValue = false;
    _runFromWS = false;
    _produceWS = false;
    
    _calcType = RF::CalculatorType::Asymptotic;
    _testStat = RF::TestStatType::PL1sided;

    _dataTag = "data";
    _nominalTag = "nominal";
    
    _dataScale = 1;
    _glSigScale = 1;


    parseConfig(configFile);
    if(_configOK) cout << "Configuration valid" << endl;
    else cout << "Configuration INVALID" << endl;
  }

  bool configStatus() { return _configOK; }

  void print(){
    
    cout << "Directory Configuration:" << endl;
    cout << "------------------------------------------------" << endl;
    if(_runFromWS){
      cout << "Source Workspace: " << _wsFileName << endl;
    }
    else
      cout << "Source Histo Directory: " << _histoDir << endl;

    cout << "Analysis Tag: " << _analysisName << endl;
    cout << "Release Tag: " << _releaseTag << endl;
    cout << "Input Tag: " << _inputTag << endl;
    cout << "Output Tag: " << _outputTag << endl;
    cout << "Output Directory: " << _outDir << endl;

    cout << endl << "Calculator Configuration:" << endl;
    cout << "------------------------------------------------" << endl;
    cout << "Run Limits: " << _doLimit << endl;
    cout << "Run Pvalues: " << _doPValue << endl;
    cout << "Run Pulls: " << _doPull << endl;
    cout << "Produce WS: " << _produceWS << endl;
    cout << "Calculator Type: " << _calcType << endl;
    cout << "Test Stat Type: " << _testStat << endl;

    cout << endl << "Mass Scan Info:" << endl;
    cout << "------------------------------------------------" << endl;
    for(uint i=0; i<_massList.size(); i++) cout << "Mass: " << _massList[i] << endl;      

    cout <<endl << "Regions/Channels Defined:" << endl;
    cout << "------------------------------------------------" << endl;
    for(uint i=0; i<_regionNames.size(); i++) cout << "Region: " << _regionNames[i] << endl;

    cout << endl << "Signals Defined:" << endl;
    cout << "------------------------------------------------" << endl;
    for(uint i=0; i<_sigNames.size(); i++) cout << "Signal: " << _sigNames[i] << endl;

    cout << endl << "Backgrounds Defined:" << endl;
    cout << "------------------------------------------------" << endl;
    for(uint i=0; i<_bkgdNames.size(); i++) cout << "Background: " << _bkgdNames[i] << endl;

    cout << endl << "Shape Systematics Defined:" << endl;
    cout << "------------------------------------------------" << endl;
    for(uint i=0; i<_shapeSystNames.size(); i++) cout << "Systematic: " << _shapeSystNames[i] << endl;

    cout << endl << "Normalization Systematics Defined:" << endl;
    cout << "------------------------------------------------" << endl;
    for(uint i=0; i<_normSystNames.size(); i++) cout << "Systematic: " << _normSystNames[i] << endl;

    cout << endl << "Parameter of Interest Scans:" << endl;
    cout << "------------------------------------------------" << endl;
    for(uint i=0; i<_poiMass.size(); i++) cout << "Mass = " << _poiMass[i] << " :  " << _poiSteps[i] << " from " << _poiScanLo[i] << " to " << _poiScanHi[i] << endl;

    cout << endl << "Scaling Factors:" << endl;
    cout << "------------------------------------------------" << endl;
    cout << "Global Signal SF: " << _glSigScale << endl;
    for( auto &ks : _sigScales) 
      cout << "Signal SF: " << ks.first << ":" << ks.second << endl;

    for( auto &kb : _bkgdScales) 
      cout << "Bkgd SF: " << kb.first << ":" << kb.second << endl;


    cout << "Data SF: " << _dataScale << endl;

  }

public:

  vector<string> _sigNames;
  vector<string> _bkgdNames;
  vector<string> _shapeSystNames;
  vector<string> _normSystNames;
  vector<string> _regionNames;

  string _histoDir;// = locConfig[0];
  string _analysisName;// = locConfig[1]; //"WtbL";
  string _releaseTag;// = locConfig[2]; //"test_v1";
  string _inputTag;// = "itest01";
  string _outputTag;// = "wtest01";
  string _outDir;
  string _topDir;
  string _diagDir;
  string _wsFileName;
  string _dataTag;
  string _nominalTag;

  vector<double> _massList;
  vector<double> _poiMass;
  vector<int> _poiSteps;
  vector<double> _poiScanLo;
  vector<double> _poiScanHi;

  map<TString, double> _sigScales;
  map<TString, double> _bkgdScales;
  double _glSigScale;
  double _dataScale;

  bool _doPull;
  bool _doLimit;
  bool _doPValue;
  bool _runFromWS;
  bool _produceWS;
  
  
  RF::CalculatorType _calcType;
  RF::TestStatType _testStat;
  
private:
  
  bool _configOK;
  
  void parseScale(string const& value, bool isSignal){
    vector<string> results;
    tokenize(value,results,":");
    if(results.size() != 2){ cout << "Error parsing scaling factor" << endl; return; }
    
    if(isSignal && (_sigScales.find(results[0]) == _sigScales.end())){
      if((results[0] == string("all")) || (results[0] == string("All"))) _glSigScale = atof(results[1].c_str());
      else _sigScales[results[0]] = atof(results[1].c_str());
    }
    else if(!isSignal && (_bkgdScales.find(results[0]) == _bkgdScales.end())){
      _bkgdScales[results[0]] = atof(results[1].c_str());
    }
    return;
  }

  void parsePOI(string const& value){
    vector<string> results;
    tokenize(value,results,":");
    if(results.size() != 4){ cout << "Error parsing POI Scan Setup" << endl; return; }

    _poiMass.push_back(atof(results[0].c_str()));
    _poiSteps.push_back(atoi(results[1].c_str()));
    _poiScanLo.push_back(atof(results[2].c_str()));
    _poiScanHi.push_back(atof(results[3].c_str()));
    
    return;
  }

  void parseMassList(string const& value){
    vector<string> results;
    tokenize(value,results,":");
    if(results.size() == 0){ cout << "Error parsing mass list" << endl; return; }

    for(uint i=0; i<results.size(); i++){
      _massList.push_back(atof(results[i].c_str()));
    }
    
    return;
  }

  void parseConfig(string const& configFile){

    ifstream file(configFile.c_str());

    string line; string name; string value;
    string inSection;  int posEqual;

    while (getline(file,line)) {

      if (! line.length()) continue;

      if (line[0] == '#') continue;
      if (line[0] == '!') continue;
      /*
      if (line[0] == '[') {
	inSection=trim(line.substr(1,line.find(']')-1));
	continue;
      }
      */
      posEqual=line.find('=');
      name  = trim(line.substr(0,posEqual));
      value = trim(line.substr(posEqual+1));
      
      if(name == string("Signal")) _sigNames.push_back(value);
      else if(name == string("Bkgd")) _bkgdNames.push_back(value);
      else if(name == string("ShapeSyst")) _shapeSystNames.push_back(value);
      else if(name == string("NormSyst")) _normSystNames.push_back(value);
      else if(name == string("Region")) _regionNames.push_back(value);
      else if(name == string("Channel")) _regionNames.push_back(value);
      else if(name == string("HistDir")) _histoDir = value;
      else if(name == string("Release")) _releaseTag = value;
      else if(name == string("InputTag")) _inputTag = value;
      else if(name == string("OutputTag")) _outputTag = value;
      else if(name == string("Analysis")) _analysisName = value;
      else if(name == string("WorkspaceFile")) _wsFileName = value;
      else if(name == string("RunPull")) _doPull = atoi(value.c_str());
      else if(name == string("RunLimit")) _doLimit = atoi(value.c_str());
      else if(name == string("RunPValue")) _doPValue = atoi(value.c_str());
      else if(name == string("RunFromWS")) _runFromWS = atoi(value.c_str());
      else if(name == string("ProduceWS")) _produceWS = atoi(value.c_str());
      else if(name == string("SignalScale")) parseScale(value,1);      
      else if(name == string("BkgdScale")) parseScale(value,0);
      else if(name == string("DataScale")) _dataScale = atof(value.c_str());
      else if(name == string("PoiSetup")) parsePOI(value);
      else if(name == string("MassList")) parseMassList(value);
      else if(name == string("CalculatorType")){
	int type = atoi(value.c_str());
	if(type==0) _calcType = RF::CalculatorType::Frequentist;
	else if(type==1) _calcType = RF::CalculatorType::Hybrid;
	else if(type==2) _calcType = RF::CalculatorType::Asymptotic;
	else if(type==3) _calcType = RF::CalculatorType::AsymptoticWithNominalAsimov;
      }
      else if(name == string("TestStatType")){
	int type = atoi(value.c_str());
	if(type==0) _testStat = RF::TestStatType::LEP;
	else if(type==1) _testStat = RF::TestStatType::Tevatron;
	else if(type==2) _testStat = RF::TestStatType::PL2sided;
	else if(type==3) _testStat = RF::TestStatType::PL1sided;
	else if(type==4) _testStat = RF::TestStatType::PLsigned;
	else if(type==5) _testStat = RF::TestStatType::MLE;
	else if(type==6) _testStat = RF::TestStatType::Nobs;
      }
    }


    _topDir = string(_releaseTag).append("/").append(_analysisName);
    _outDir = string(_topDir).append("/data/").append(_inputTag);
    _diagDir = string(_topDir).append("/ws/diagnostics/");

    if(_massList.size()>0) _configOK = true;

    if(!_doPull && !_doLimit && !_doPValue && !_produceWS){
      _configOK = false;
      cout << "Configuration failed:  you didn't specify a job type!" << endl;
    }

  }

};

bool systExists(string outDir, string region, string bkgd, string syst){

  string outFile = outDir;
  outFile.append("/").append(bkgd).append("_").append(region).append(".root");
  TFile* ftest = new TFile(outFile.c_str());
  bool retVal = true;
  if(!ftest) retVal = false;

  string outNameD = string(syst).append("_down");
  string outNameU = string(syst).append("_up");

  TH1F* up = (TH1F*)ftest->Get(outNameU.c_str());
  TH1F* dn = (TH1F*)ftest->Get(outNameD.c_str());

  if(!up || !dn) retVal = false;

  ftest->Close();
  delete ftest;

  return retVal;
}

void applyNormSyst(string systString, RF::Sample& sample, string sampleName, string regionName){

  vector<string> tokens;
  tokenize(systString, tokens, ":");

  if(tokens.size()!=5){ cout << "applyNormSyst:  Error, incorrect argument length! " << tokens.size() << endl; return; }
  if((tokens[0]!=regionName) && (tokens[0]!=string("ALL")) && (tokens[0]!=string("All"))) return;
  if((tokens[1]!=sampleName) && (tokens[1]!=string("ALL")) && (tokens[1]!=string("All"))) return;

  double systUp = atof(tokens[3].c_str());
  double systDn = atof(tokens[4].c_str());

  string systName = tokens[2];
  if(tokens[0]==regionName) systName.append(regionName);

  sample.multiplyBy(systName.c_str(), 1.0, 1.0-systDn, 1.0+systUp, RF::MultiplicativeFactor::ConstraintType::GAUSSIAN);

  return;
}

void createDirectories(Configuration& myCfg){

  string dirA = string(myCfg._releaseTag).append("/").append(myCfg._analysisName).append("/data/").append(myCfg._inputTag);
  string dirB = string(myCfg._releaseTag).append("/").append(myCfg._analysisName).append("/ws/diagnostics/");
  string dirC = string(myCfg._releaseTag).append("/").append(myCfg._analysisName).append("/tmp/");
  string mkdir = "mkdir -p ";

  string tmp = dirA;
  tmp.insert(0,mkdir);
  dirB.insert(0,mkdir);
  dirC.insert(0,mkdir);

  int resA = system(tmp.c_str());
  int resB = system(dirB.c_str());
  int resC = system(dirC.c_str());

  int resD = 0;
  if(!myCfg._runFromWS){
    tmp = string("cp ").append(myCfg._histoDir).append("/*.root ").append(dirA);
    resD = system(tmp.c_str());
  }

  cout << "Directory creation result: " << resA << "/" << resB << "/" << resC << "/" << resD << endl;

  return;
}

void usage(void){
  cout << endl << "*****************************************************************************************************" << endl;
  cout << "Usage: VHprocessor <config file>" << endl;
  cout << "Read the VHprocessor.README file to learn about the configuration file settings " << endl;
  cout << "*****************************************************************************************************" << endl << endl;
}

int main(int argc, char **argv){

  const char* inConfig = "";
  if(argc>1){
    inConfig = argv[1];
  }
  else{ usage(); return 0;}

  Configuration myConfig(inConfig);
  myConfig.print();

  if(!myConfig.configStatus()){
    cout << "Configuration Failed: Check your config file!" << endl;
    return 0;
  }

  createDirectories(myConfig);

  VHAnalysisRunner vhAna(myConfig._analysisName);
  vhAna.setReleaseDir(myConfig._releaseTag);
  vhAna.setInputListTag(myConfig._inputTag);
  vhAna.setOutputWSTag(myConfig._outputTag);
  vhAna.setTagNominal(myConfig._nominalTag);
  vhAna.setTagData(myConfig._dataTag);

  //set up the calculation
  vhAna.doPull(myConfig._doPull);
  vhAna.doLimit(myConfig._doLimit);
  vhAna.doPValue(myConfig._doPValue);
  vhAna.setCalculatorType(myConfig._calcType);
  vhAna.setTestStatType(myConfig._testStat);

  //Add custom POI scans
  for(uint i=0; i<myConfig._poiMass.size(); i++){
    vhAna.limitRunner().setPOIScan(myConfig._poiMass[i],myConfig._poiSteps[i],myConfig._poiScanLo[i],myConfig._poiScanHi[i]);
  }
    
  if(myConfig._runFromWS){
    //If we already have the WS, we can just use it to run immediately
    vhAna.runFromFile(myConfig._wsFileName);
  }
  else{
    //If we don't have the WS, just build from the histogram inputs
    
    //For each channel/region listed, create a channel
    for(uint rr=0; rr<myConfig._regionNames.size(); rr++){
      vhAna.addChannel(myConfig._regionNames[rr],"");
      
      //Add each signal listed, for each mass in the specified range
      for(uint ss=0; ss<myConfig._sigNames.size(); ss++){
	for(uint mm=0; mm<myConfig._massList.size(); mm++){
	  
	  string thisSigName = myConfig._sigNames[ss];
	  thisSigName.append(to_string((int)(myConfig._massList[mm])));

	  vhAna.channel(myConfig._regionNames[rr]).addSample(thisSigName);
	  vhAna.channel(myConfig._regionNames[rr]).sample(thisSigName).setNormByLumi(kTRUE);
	  vhAna.channel(myConfig._regionNames[rr]).sample(thisSigName).multiplyBy("mu", 1.0, -10, 1e6);
	  
	  //Add shape systematics from TH1 format
	  for(uint syst=0; syst<myConfig._shapeSystNames.size(); syst++){
	    if(systExists(myConfig._outDir, myConfig._regionNames[rr], thisSigName, myConfig._shapeSystNames[syst])) vhAna.channel(myConfig._regionNames[rr]).sample(thisSigName).addVariation(myConfig._shapeSystNames[syst]);
	  }
	  
	  //Add flat systematics from specified list
	  for(uint syst=0; syst<myConfig._normSystNames.size(); syst++){
	    applyNormSyst(myConfig._normSystNames[syst],vhAna.channel(myConfig._regionNames[rr]).sample(thisSigName), myConfig._sigNames[ss], myConfig._regionNames[rr]);
	  }
	  
	  //Register this signal
	  vhAna.addSignal(thisSigName,myConfig._massList[mm]);
	}
      }
      
      //Add each background listed
      for(uint bb=0; bb<myConfig._bkgdNames.size(); bb++){
	
	//This is a bit dangerous, but we should specify all QCD samples have names including "QCD"
	bool isQCD = (myConfig._bkgdNames[bb].find("QCD") != std::string::npos);
	
	vhAna.channel(myConfig._regionNames[rr]).addSample(myConfig._bkgdNames[bb]);
	
	if(!isQCD) vhAna.channel(myConfig._regionNames[rr]).sample(myConfig._bkgdNames[bb]).setNormByLumi(kFALSE);
	else vhAna.channel(myConfig._regionNames[rr]).sample(myConfig._bkgdNames[bb]).setNormByLumi(kTRUE);
	
	//Add shape systematics from TH1 format
	for(uint syst=0; syst<myConfig._shapeSystNames.size(); syst++){
	  if(systExists(myConfig._outDir, myConfig._regionNames[rr], myConfig._bkgdNames[bb], myConfig._shapeSystNames[syst])) vhAna.channel(myConfig._regionNames[rr]).sample(myConfig._bkgdNames[bb]).addVariation(myConfig._shapeSystNames[syst]);
	}
	
	//Add flat systematics as listed
	for(uint syst=0; syst<myConfig._normSystNames.size(); syst++){
	  applyNormSyst(myConfig._normSystNames[syst],vhAna.channel(myConfig._regionNames[rr]).sample(myConfig._bkgdNames[bb]), myConfig._bkgdNames[bb], myConfig._regionNames[rr]);
	}
	
      }
    }
  
    //Will we always use mu?  Maybe
    vhAna.setPOI("mu");
    
    //Apply any scale factors the user has specified
    for( auto &ks : myConfig._sigScales) vhAna.scaleSignal(ks.second,ks.first);
    for( auto &kb : myConfig._bkgdScales) vhAna.scaleBkgd(kb.second,kb.first);
    vhAna.scaleData(myConfig._dataScale);
    if(myConfig._glSigScale!=1) vhAna.scaleSignal(myConfig._glSigScale, "All");

        
    //If we've specified a calculation, do it!
    if(myConfig._doPull || myConfig._doLimit || myConfig._doPValue) vhAna.run();

    if(myConfig._doPull){
      PullPlotter pPlot("vhPullPlots",myConfig._diagDir);
      pPlot.addOneMassPoint(1000);
      pPlot.setOutputFormat(PullPlotter::OutputFormat::pdf);
      //      StatisticalResultsCollection& src = vhAna.getStatResults();
      pPlot.process(vhAna.getStatResults());
    }

    //If we're just making a WS, then just do that
    if(myConfig._produceWS) vhAna.produceWS();

  }

  return 0;
}
