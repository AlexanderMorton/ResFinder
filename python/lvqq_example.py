# an example driver for the lvqq analysis
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
  
  releaseDir = './test_v4'
  analysis = 'VVlvqq_HVT'
  inputListTag = 'itest01'
  outputWSTag = 'wtest01'

  producePull = True


  # import files and create directory structure
  createDirectoryStructure('/afs/cern.ch/work/k/kalliopi/public/forFrancesco/v4/treeMedium/', releaseDir, analysis, inputListTag, outputWSTag)

  # define running options
  masses = [1000, 1500, 2000, 2500]

  doInjection = True
  doToys = False
  doOptimisedScan = True
  poi_setups = { # for optimised mu scan
                 2500 : [60, 0, 25],
                 2000 : [60, 0, 20],
                 1500 : [75, 0, 15],
                 1000 : [75, 0, 10],
               }


  lvqq = RF.VVlvqqAnalysisRunner(analysis)
  # define input tree format
  lvqq.setTreeObs('mass')
  lvqq.setTreeWeight('weight')

  lvqq.doPull(producePull);

  # define histogram binning
  lvqq.setNbins(31)
  lvqq.setObsMin(450)
  lvqq.setObsMax(3550)

  # apply directory structure
  lvqq.setReleaseDir(releaseDir)
  lvqq.setInputListTag(inputListTag)
  lvqq.setOutputWSTag(outputWSTag)

  # define channels
  lvqq.addChannel('SR', 'weight != 0')

  # define likelihood model - signals
  for mass in masses:
    sname = 'HVT{mass}'.format(mass=mass)
    lvqq.channel('SR').addSample(sname)
    lvqq.channel('SR').sample(sname).multiplyBy('mu', 1.0, 0, 20)
    lvqq.addSignal(sname, mass)

  # define likelihood model - backgrounds
  lvqq.channel('SR').addSample('Top')
  lvqq.channel('SR').sample('Top').multiplyBy('mu_bkg', 1.0, 0, 20)
  lvqq.channel('SR').addSample('W+Jets')
  lvqq.channel('SR').sample('W+Jets').multiplyBy('mu_bkg', 1.0, 0, 20)

  # define POI
  lvqq.setPOI('mu')
  if not doOptimisedScan:
    lvqq.limitRunner().setPOIScan(20, 0, 20) # nsteps, min, max
  else:
    for mass in masses:
      # deal with this signal in an optimised way
      lvqq.limitRunner().setPOIScan(mass, poi_setups[mass][0], poi_setups[mass][1], poi_setups[mass][2])

  # optional: inject signal
  if doInjection:
    lvqq.setInjectionSample('HVT2000')
    lvqq.setInjectionStrength(4.5)

  # optional: use toys instead of asymptotics
  if doToys:
    lvqq.limitTool().setCalcType(RF.Frequentist)
    lvqq.limitTool().setTestStatType(RF.PL1sided)
    lvqq.limitTool().setNToys(100000)
    lvqq.limitTool().hypoTestInvTool().SetParameter('UseProof', True)
    lvqq.limitTool().hypoTestInvTool().SetParameter('GenerateBinned', True) # speeds up

  # run
  lvqq.run()


  # plot
  ROOT.gROOT.ProcessLine('.L atlasstyle/AtlasStyle.C+')
  ROOT.SetAtlasStyle()
  plotter = RF.Plotter('lvqqtest', '.')
  plotter.setVarName('m_{HVT}')
  plotter.setVarUnit('GeV')
  plotter.setSpec(500 , 1.0, 1.765000 * 1.0 * 0.75616 * 1e3) # note the 1000x factor
  plotter.setSpec(1000, 1.0, 0.098950 * 1.0 * 0.79607 * 1e3)
  plotter.setSpec(1500, 1.0, 0.016110 * 1.0 * 0.81147 * 1e3)
  plotter.setSpec(2000, 1.0, 0.003810 * 1.0 * 0.81310 * 1e3)
  plotter.setSpec(2500, 1.0, 0.001076 * 1.0 * 0.81773 * 1e3)
  plotter.setSpec(3000, 1.0, 0.000334 * 1.0 * 0.81999 * 1e3)
  plotter.setOutputFormat(RF.Plotter.png)
  plotter.process(lvqq.getStatResults())

 
  pullMasses = [2000]

  if producePull : 
    pullplotter = PullPlotter('lvqqpulltest', '.')
  
    for i in pullMasses : pullplotter.addOneMassPoint(i) 
  
    pullplotter.setOutputFormat(RF.Plotter.pdf)
    pullplotter.process(lvqq.getStatResults())
    
