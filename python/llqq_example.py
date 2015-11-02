# an example driver for the llqq analysis
# Valerio Ippolito - Harvard University

import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.gROOT.ProcessLine(ROOT.gSystem.ExpandPathName('.x $ROOTCOREDIR/scripts/load_packages.C'))

from ROOT import RF as RF

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
  
  releaseDir = './testllqq'
  analysis = 'VVllqq_HVT'
  inputListTag = 'itest01'
  outputWSTag = 'wtest01'

  # import files and create directory structure
  createDirectoryStructure('/afs/cern.ch/work/l/lxu/public/Run2/DBllqq/InputTree_TopRelax', releaseDir, analysis, inputListTag, outputWSTag)

  # define running options
  masses = [1100, 1400, 1600, 1800, 2000]
  doInjection = True
  #doInjection = False
  doToys = False
  #doToys = True
  doOptimisedScan = True
  poi_setups = { # for optimised mu scan
                 2500 : [1000, 0, 100],
                 2000 : [1000, 0, 100],
                 1800 : [400, 0, 50],
                 1600 : [200, 0, 30],
                 1500 : [50, 0, 30],
                 1400 : [50, 0, 30],
                 1100 : [50, 0, 20],
                 1000 : [50, 0, 20],
               }


  llqq = RF.VVllqqAnalysisRunner(analysis)
  # define input tree format
  llqq.setTreeObs('Mlljj')
  llqq.setTreeWeight('weight')

  # define histogram binning
  llqq.setNbins(7)
  llqq.setObsMin(400)
  llqq.setObsMax(2500)

  # apply directory structure
  llqq.setReleaseDir(releaseDir)
  llqq.setInputListTag(inputListTag)
  llqq.setOutputWSTag(outputWSTag)

  # define channels
  ## SR
  llqq.addChannel('SR', 'weight != 0')
  ## Z Control region
  llqq.addChannel('ZCR', 'weight != 0')
  ## Top Control region
  llqq.addChannel('TopCR', 'weight != 0')

  ## scale to 4 fb-1
  #lumi_scale=4e3/78.
  lumi_scale=1.

  # define likelihood model - signals
  for cat in ['SR', 'ZCR', 'TopCR']:
  #for cat in ['SR']:
    for mass in masses:
      sname = 'HVT_{mass}'.format(mass=mass)
      llqq.channel(cat).addSample(sname)
      llqq.channel(cat).sample(sname).multiplyBy('mu', 1.0, 0, 100000)
      ## scale the signal also
      llqq.channel(cat).sample(sname).multiplyBy('mu_lumi', lumi_scale, lumi_scale, lumi_scale)
      llqq.addSignal(sname, mass)

    # define likelihood model - backgrounds
    llqq.channel(cat).addSample('Top')
    llqq.channel(cat).sample('Top').multiplyBy('mu_bkg_top', 1.0, 0, 20)
    llqq.channel(cat).sample('Top').multiplyBy('mu_lumi', lumi_scale, lumi_scale, lumi_scale)
    #llqq.channel(cat).sample('Top').addVariation("topnm")
    ## no contribution from W+jets, ignore it for now
    #llqq.channel(cat).addSample('W_jets')
    #llqq.channel(cat).sample('W_jets').multiplyBy('mu_bkg_W', 1.0, 0, 20)
    #llqq.channel(cat).sample('W_jets').multiplyBy('mu_lumi', lumi_scale, lumi_scale, lumi_scale)
    #llqq.channel(cat).sample('W_jets').addVariation("wjetsnm")
    if cat!='TopCR':  ## negative bins for Z+jets in TopCR
      llqq.channel(cat).addSample('Z_jets')
      llqq.channel(cat).sample('Z_jets').multiplyBy('mu_bkg_Z', 1.0, 0, 20)
      llqq.channel(cat).sample('Z_jets').multiplyBy('mu_lumi', lumi_scale, lumi_scale, lumi_scale)
    #llqq.channel(cat).sample('Z_jets').addVariation("zjetsnm")
    llqq.channel(cat).addSample('Diboson')
    llqq.channel(cat).sample('Diboson').multiplyBy('mu_bkg_DB', 1.0, 0, 20)
    llqq.channel(cat).sample('Diboson').multiplyBy('mu_lumi', lumi_scale, lumi_scale, lumi_scale)
    #llqq.channel(cat).sample('Diboson').addVariation("dbnm")

  # define POI
  llqq.setPOI('mu')
  if not doOptimisedScan:
    llqq.limitRunner().setPOIScan(20, 0, 20) # nsteps, min, max
  else:
    for mass in masses:
      # deal with this signal in an optimised way
      llqq.limitRunner().setPOIScan(mass, poi_setups[mass][0], poi_setups[mass][1], poi_setups[mass][2])

  # optional: inject signal
  if doInjection:
    llqq.setInjectionSample('HVT_2000')
    #llqq.setInjectionStrength(1.0)
    llqq.setInjectionStrength(0.0)

  # optional: use toys instead of asymptotics
  if doToys:
    llqq.limitTool().setCalcType(RF.Frequentist)
    llqq.limitTool().setTestStatType(RF.PL1sided)
    llqq.limitTool().setNToys(100000)
    llqq.limitTool().hypoTestInvTool().SetParameter('UseProof', True)
    llqq.limitTool().hypoTestInvTool().SetParameter('GenerateBinned', True) # speeds up

  # run
  llqq.run()


  # plot
  ROOT.gROOT.ProcessLine('.L atlasstyle/AtlasStyle.C+')
  ROOT.SetAtlasStyle()
  plotter = RF.Plotter('llqqtest', '.')
  plotter.setVarName('m_{HVT}')
  plotter.setVarUnit('GeV')
  br=0.033658*3*0.676 ## Br(ZW->llqq)=Br(Z->ll)*Br(W->qq)
  plotter.setSpec(600 , br, 2.653e-1 * 1.0 * 1.0 * 1e0) # note the 1000x factor
  plotter.setSpec(1100, br, 1.902e-2 * 1.0 * 1.0 * 1e0)
  plotter.setSpec(1400, br, 6.672e-3 * 1.0 * 1.0 * 1e0)
  plotter.setSpec(1500, br, 4.810e-3 * 1.0 * 1.0 * 1e0)
  plotter.setSpec(1600, br, 3.505e-3 * 1.0 * 1.0 * 1e0)
  plotter.setSpec(1800, br, 1.947e-3 * 1.0 * 1.0 * 1e0)
  plotter.setSpec(2000, br, 1.126e-3 * 1.0 * 1.0 * 1e0)
  plotter.setOutputFormat(RF.Plotter.png)
  plotter.process(llqq.getStatResults())

