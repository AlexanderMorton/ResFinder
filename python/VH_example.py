
import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.gROOT.ProcessLine(ROOT.gSystem.ExpandPathName('.x $ROOTCOREDIR/scripts/load_packages.C'))

from ROOT import RF as RF

def getInputs(inputfile, outpath):
  f = ROOT.TFile.Open(inputfile)
  for region in f.GetListOfKeys():#loop over analysis regions
    histDict = {}
    for systematic in (region.ReadObj()).GetListOfKeys():#for each region loop over systematics
      sysDir = systematic.ReadObj()
      if "Nominal" in sysDir.GetName():
        if "unblind" in sysDir.GetName():
          dataStack = sysDir.Get("data_vh_m")
        else:
          #make data stack sum of backgrounds in blinded region
          dataStack = sysDir.Get("bkg_vh_m")

        datahist = dataStack.GetHists()[0].Clone()
        for hist in dataStack.GetHists()[1:]:
          datahist.Add(hist)
        print region.GetName()
        print sysDir.GetName()
        print datahist.Integral()
        

      #get all the histograms in this systematic
      #add them to a data structure so we can make the files in the output dir
      #data structure is dict[background] = {[(systematic name, TH1D syshist), (other systematic name, TH1D othersyshist),...]}
      backgroundStack = sysDir.Get("bkg_vh_m")
      signalStack = sysDir.Get("sig_vh_m")
      for hist in signalStack.GetHists():
        title =  hist.GetTitle()
        tuple = (sysDir.GetName(), hist) 
        if title in histDict:
          histDict[title].append(tuple)
        else:
          histDict[title] = [tuple]
      for hist in backgroundStack.GetHists():
        title =  hist.GetTitle()
        tuple = (sysDir.GetName(), hist) 
        if title in histDict:
          histDict[title].append(tuple)
        else:
          histDict[title] = [tuple]
    #make output files
    for key, value in histDict.iteritems():
      filename = str(key)
      filename = filename.replace("+", "_")
      filename = filename.replace(" ", "_")
      filename = filename.replace("#", "_")
      filename = filename.replace(".", "_")
      filename = "h_"+filename+"_"+str(region.ReadObj().GetName())
      filename.replace("lvJ", "_lvJ")
      of = ROOT.TFile.Open('{dest}/{sample}.root'.format(dest=outpath, sample=filename), 'RECREATE')
      #replace + with _ (Z+jets -> Z_jets etc)
      #replace spaces with _
      #t#bart?
      for syst in value:
        histname =syst[0]
        h=syst[1]
        if histname == "Nominal":
          histname = "nominal"
        histname = histname.replace(".", "_")
        h.SetName(histname)
        title = h.GetTitle()
        #title = title.replace(".", "_")
        title = title.replace(" ", "_")
        if "." in title:
          title="signal"
        h.SetTitle(title)
        h.SetDirectory(of)
      of.Write()
      of.IsA().Destructor(of)
    #make data output file
    #one data per region (no systematics. :P )
    #change later?
    filename = "data_"+str(region.ReadObj().GetName())
    of = ROOT.TFile.Open('{dest}/{sample}.root'.format(dest=outpath, sample=filename), 'RECREATE')
    datahist.SetName("nominal")
    datahist.SetDirectory(of)
    of.Write()
    of.IsA().Destructor(of)


  f.IsA().Destructor(f)




if __name__ == '__main__':
  analysis = 'VH'
  releaseDir = 'test_v1'
  inputTag = 'itest03'
  wsTag = 'wtest02'
  dest = '{releaseDir}/{analysis}/data/{inputTag}'.format(releaseDir=releaseDir, analysis=analysis, inputTag=inputTag)
  #inputfile = '/afs/cern.ch/work/p/pmullen/DBLStatsFramework/lvJ.root'
  inputfile = '/afs/cern.ch/work/p/pmullen/DBLStatsFramework/TH1F.root'
  getInputs(inputfile, dest)

#set steps and range for scan
  poi_setups = { 1.0 : [60, 0, 30],
                 1.6 : [60, 0, 30],
                 2.0 : [60, 0, 20],
               }


  masses = [1.0, 1.6, 2.0]

  #use the vvjj runner for now, maybe create our own?
  vvjj = RF.VVJJValidationRunner(analysis)
  vvjj.setReleaseDir(releaseDir)
  vvjj.setInputListTag(inputTag)
  vvjj.setOutputWSTag(wsTag)
  #vvjj.histoCollection().setTagNominal("Nominal")
#  vvjj.histoCollection().setTagData("doesnotwant")

  vvjj.addChannel('lvJ_unblind_SR', '') # not using selection criteria

  for mass in masses:
#    thisSig = 'signal_{mass}'.format(mass=mass)
    thisSig = "h_"+str(mass)+"_TeV_HVT"
    thisSig = thisSig.replace(".", "_")#str(mass)+"_TeV_HVT"
    vvjj.channel('lvJ_unblind_SR').addSample(thisSig)
    vvjj.channel('lvJ_unblind_SR').sample(thisSig).setNormByLumi(True)
    ##not sure what this does, maybe allows mu to float by a factor of 300?
    vvjj.channel('lvJ_unblind_SR').sample(thisSig).multiplyBy('mu', 1.0, 0, 500)
##    vvjj.channel('lvJ_ununblind_SR').sample(thisSig).addVariation('ptr')
##    vvjj.channel('lvJ_ununblind_SR').sample(thisSig).addVariation('pts')
##    vvjj.channel('lvJ_ununblind_SR').sample(thisSig).addVariation('jms')
##adding systematics, need to check how the hist names are calculated cause it might add and extra underscore
#    vvjj.channel('lvJ_ununblind_SR').sample(thisSig).addVariation('MUON_EFF_SYS__1')


    # add ntrk systematics
    #vvjj.channel('SR').sample(thisSig).multiplyBy('ntrk', 1.0, 0.69, 0.99, RF.MultiplicativeFactor.GAUSSIAN)

    # declare signal mass value
    thismass = int(float(mass)*1000)
    vvjj.addSignal(thisSig, mass)




  vvjj.channel('lvJ_unblind_SR').addSample('h_t_bart')
  vvjj.channel('lvJ_unblind_SR').sample('h_t_bart').setNormByLumi(False)
#allows background to float?
  vvjj.channel('lvJ_unblind_SR').sample('h_t_bart').multiplyBy('nbkg', 1.0, 0, 2000)
#  vvjj.channel('lvJ_unblind_SR').sample('h_t_bart').addVariation('fit')

  vvjj.channel('lvJ_unblind_SR').addSample('h_diboson')
  vvjj.channel('lvJ_unblind_SR').sample('h_diboson').setNormByLumi(False)
  vvjj.channel('lvJ_unblind_SR').sample('h_diboson').multiplyBy('nbkg', 1.0, 0, 2000)

  vvjj.channel('lvJ_unblind_SR').addSample('h_single_top')
  vvjj.channel('lvJ_unblind_SR').sample('h_single_top').setNormByLumi(False)
  vvjj.channel('lvJ_unblind_SR').sample('h_single_top').multiplyBy('nbkg', 1.0, 0, 2000)

  vvjj.channel('lvJ_unblind_SR').addSample('h_W_jets')
  vvjj.channel('lvJ_unblind_SR').sample('h_W_jets').setNormByLumi(False)
  vvjj.channel('lvJ_unblind_SR').sample('h_W_jets').multiplyBy('nbkg', 1.0, 0, 2000)

  vvjj.channel('lvJ_unblind_SR').addSample('h_Z_jets')
  vvjj.channel('lvJ_unblind_SR').sample('h_Z_jets').setNormByLumi(False)
  vvjj.channel('lvJ_unblind_SR').sample('h_Z_jets').multiplyBy('nbkg', 1.0, 0, 2000)




  vvjj.setPOI('mu')

  # setup limit runner
  vvjj.limitRunner().setPOIScan(20, 0, 20) # nsteps, min, max
#  for mass in poi_setups:
#    # deal with this signal in an optimised way
#    print mass, poi_setups[mass][0]
#    vvjj.limitRunner().setPOIScan(mass, poi_setups[mass][0], poi_setups[mass][1], poi_setups[mass][2])

  print "running"
  vvjj.run()

  # plot
  ROOT.gROOT.ProcessLine('.L atlasstyle/AtlasStyle.C+')
  ROOT.SetAtlasStyle()
  plotter = RF.Plotter('VHtest', '.')
  plotter.setVarName('m_{VH}')
  plotter.setVarUnit('TeV')
#these are totally wrong - just to check that we could plot something
#mass point, branching ratio, cross section
  #plotter.setSpec(1.0, 0.699*0.676, 35.547)
  #plotter.setSpec(1.6, 0.699*0.676, 2.6605)
  #plotter.setSpec(2.0, 0.699*0.676, 8.941)
  plotter.setSpec(1.0, 0.699*0.676, 138.6)
  plotter.setSpec(1.6, 0.699*0.676, 17.15)
  plotter.setSpec(2.0, 0.699*0.676, 5.65)
#  plotter.setSpec(3000, 0.699*0.676, 0.79763)
  plotter.setOutputFormat(RF.Plotter.png)
  plotter.process(vvjj.getStatResults())









