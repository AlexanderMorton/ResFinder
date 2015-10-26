# an example driver for the VVJJ analysis
# Valerio Ippolito - Harvard University

import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.gROOT.ProcessLine(ROOT.gSystem.ExpandPathName('.x $ROOTCOREDIR/scripts/load_packages.C'))

from ROOT import RF as RF
from ROOT import PullPlotter as PullPlotter


def createDirectoryStructure(input_dir, releaseDir, analysis, inputListTag, outputWSTag):
  import os
  import shutil
  import glob

  # create directories
  newdir = '{releaseDir}/{analysis}/data/{inputListTag}'.format(releaseDir=releaseDir, analysis=analysis, inputListTag=inputListTag, outputWSTag=outputWSTag)
  diagnosticsdir = '{releaseDir}/{analysis}/ws/diagnostics'.format(releaseDir=releaseDir, analysis=analysis, inputListTag=inputListTag, outputWSTag=outputWSTag)
  tmpdir = '{releaseDir}/{analysis}/tmp'.format(releaseDir=releaseDir, analysis=analysis, inputListTag=inputListTag, outputWSTag=outputWSTag)

  # first, we remove the previously existing directories
  dirs = [newdir, diagnosticsdir, tmpdir]
  for dir in dirs:
    if os.path.exists(dir):
      shutil.rmtree(dir)

  # we use makedirs, which takes care of creating intermediate directories
  os.makedirs(newdir)
  os.makedirs(diagnosticsdir)
  os.makedirs(tmpdir)
  
  # copy input trees from input location
  files = glob.glob(os.path.join(input_dir, '*.*'))
  for file in files:
    shutil.copy2(file, newdir)
  

if __name__ == '__main__':
  releaseDir = './test_v5'
  analysis = 'VVJJ_WPRIME'
  inputListTag = 'itest05'
  outputWSTag = 'wtest05'
  doInjection = False
  doObserved = True
  doExternalPdf = True
  producePull = True
  treeLumi = 1.0 # 1/fb
  newLumi = 5.0 # 1/fb

  masses = [3000, 2500, 2000, 1500]


  poi_setups = { 3000 : [60, 0, 30],
                 2500 : [60, 0, 30],
                 2000 : [60, 0, 20],
                 1500 : [75, 0, 20],
               }

# createDirectoryStructure('/afs/cern.ch/work/y/yygao/public/VVJJStatInput/results_CamKt12SplitFilteredMu100SmallR30YCut4/syst_100', releaseDir, analysis, inputListTag, outputWSTag)
  createDirectoryStructure('/afs/cern.ch/work/v/vippolit/public//VVJJ/test_inputs_20150508', releaseDir, analysis, inputListTag, outputWSTag)

  vvjj = RF.VVJJAnalysisRunner(analysis)
  vvjj.setTreeObs('x')
  vvjj.setTreeWeight('weight')
  vvjj.setLumiRescale(treeLumi, newLumi)
  vvjj.setNbins(25)
  vvjj.setObsMin(1050)
  vvjj.setObsMax(3550)
  vvjj.setReleaseDir(releaseDir)
  vvjj.setInputListTag(inputListTag)
  vvjj.setOutputWSTag(outputWSTag)

  vvjj.doPull(producePull);


  if not doExternalPdf:
    # set up function for up/down shape variations
    # syntax is the same as RooWorkspace::factory
    # first argument is the factory command, second argument is the name of the observable
    # (dijet mass) used in this formula
    vvjj.setFitFunction("CEXPR::b('pow(1 - x/sqrts, p2-xi*p3) * pow(x/sqrts, -p3)', {x[1050,3550],sqrts[13000],xi[4.3],p2[5.28833e+01,-100,100],p3[-1.72355e+00,-100,100]})", "x")
    # (note that the line above is already the default in the VVJJAnalysisRunner constructor)


  vvjj.setPtrName('ptr') # to obtain down variation automatically as 2*nominal-up !

  # create a new channel (will automatically add data)
  vvjj.addChannel('SR', '') # not using selection criteria

  # loop over signals
  for mass in masses:
    thisSig = 'signal_{mass}'.format(mass=mass)
    vvjj.channel('SR').addSample(thisSig)
    vvjj.channel('SR').sample(thisSig).setNormByLumi(True)
    vvjj.channel('SR').sample(thisSig).multiplyBy('mu', 1.0, 0, 300)
    vvjj.channel('SR').sample(thisSig).addVariation('ptr')
    vvjj.channel('SR').sample(thisSig).addVariation('pts')
    vvjj.channel('SR').sample(thisSig).addVariation('jms')

    # add ntrk systematics
    # note: you should also rescale sig and bkg accordingly, hence it's commented here
    # vvjj.channel('SR').sample(thisSig).multiplyBy('ntrk', 1.0, 0.69, 0.99, RF.MultiplicativeFactor.GAUSSIAN)

    # declare signal mass value
    vvjj.addSignal(thisSig, mass)

  # add also bkg
  vvjj.channel('SR').addSample('background')
  vvjj.channel('SR').sample('background').setNormByLumi(False)
  vvjj.channel('SR').sample('background').multiplyBy('nbkg', 1.0, 0, 2000)
  if not doExternalPdf:
    vvjj.channel('SR').sample('background').addVariation('fit')
  else:
    # copy and load background from Run-1 analysis
    import os
    os.system('cp /afs/cern.ch/user/v/vippolit/public/RF/Background.* .')
    ROOT.gROOT.ProcessLine('.L Background.cxx+')

    vvjj.channel("SR").sample("background").setFitFunction("Background::b(x[1050,3550],p2[5.28833e+01,-100,100],p3[-1.72355e+00,-100,100],1)", "x")
   #vvjj.channel("SR").sample("background").setFitFunction("Background::b(x[1050,3550],p2[5.28833e+01,-100,100],p3[-1.72355e+00,-100,100],2)", "x")
   #vvjj.channel("SR").sample("background").setFitFunction("Background::b(x[1050,3550],p2[5.28833e+01,-100,100],p3[-1.72355e+00,-100,100],3)", "x")
   #vvjj.channel("SR").sample("background").setFitFunction("Background::b(x[1050,3550],p2[5.28833e+01,-100,100],p3[-1.72355e+00,-100,100],4)", "x")

  vvjj.setPOI('mu')

  if doInjection:
    vvjj.setInjectionSample('signal_2000')
    vvjj.setInjectionStrength(4.5)
  if doObserved:
    if doInjection:
      raise RuntimeError('Injection presumes you do not compute observed limits, in any case...')
    vvjj.setDoObserved(True)
  else:
    vvjj.setDoObserved(False)

  # setup limit runner
  vvjj.limitRunner().setPOIScan(20, 0, 20) # nsteps, min, max
  for mass in poi_setups:
    # deal with this signal in an optimised way
    vvjj.limitRunner().setPOIScan(mass, poi_setups[mass][0], poi_setups[mass][1], poi_setups[mass][2])

  vvjj.run()

  # plot
  ROOT.gROOT.ProcessLine('.L atlasstyle/AtlasStyle.C+')
  ROOT.SetAtlasStyle()
  plotter = RF.Plotter('VVJJtest', '.')
  plotter.setVarName('m_{W}')
  plotter.setVarUnit('GeV')
  plotter.setSpec(1500, 0.699*0.676, 35.547)
  plotter.setSpec(2000, 0.699*0.676, 8.941)
  plotter.setSpec(2500, 0.699*0.676, 2.6605)
  plotter.setSpec(3000, 0.699*0.676, 0.79763)
  plotter.setOutputFormat(RF.Plotter.png)
  plotter.process(vvjj.getStatResults())

  
  pullMasses = [2000]

  if producePull : 
    pullplotter = PullPlotter('vvjjpulltest', '.')
  
    for i in pullMasses : pullplotter.addOneMassPoint(i) 
  
    pullplotter.setOutputFormat(RF.Plotter.pdf)
    pullplotter.process(vvjj.getStatResults())
