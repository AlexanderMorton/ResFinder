import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.gROOT.ProcessLine(ROOT.gSystem.ExpandPathName('.x $ROOTCOREDIR/scripts/load_packages.C'))

from ROOT import RF as RF

def createEmptyDirs(releaseDir, analysis, inputTag):
  dest_data = '{releaseDir}/{analysis}/data/{inputTag}'.format(releaseDir=releaseDir, analysis=analysis, inputTag=inputTag)
  dest_diagn = '{releaseDir}/{analysis}/ws/diagnostics'.format(releaseDir=releaseDir, analysis=analysis, inputTag=inputTag)
  dest_tmp = '{releaseDir}/{analysis}/tmp'.format(releaseDir=releaseDir, analysis=analysis, inputTag=inputTag)

  import os
  import shutil

  for dest in [dest_data, dest_diagn, dest_tmp]:
    if os.path.exists(dest): shutil.rmtree(dest)
    os.makedirs(dest)

  return dest_data

def getInputFromRunI(pathformat, masses, dest):
  hists = { 'signal_{mass}_SR' : { 'nominal'   : 'h_nominal_sig',
                                 },
            'background1_SR'    : { 'nominal'   : 'h_nominal_bkg1',
                                 },
            'background2_SR'    : { 'nominal'   : 'h_nominal_bkg2',
                                 },
            'data_SR'          : { 'nominal'   : 'h_observed',
                                 },
          }

  for mass in masses:
    f = ROOT.TFile.Open(pathformat.format(mass=mass))

    for sampleraw in hists:
      if 'mass' in sampleraw:
        sample = sampleraw.format(mass=mass)
      else:
        sample = sampleraw

      of = ROOT.TFile.Open('{dest}/{sample}.root'.format(dest=dest, sample=sample), 'RECREATE')

      for variation in hists[sampleraw]:
        h = f.Get('{path}'.format(path=hists[sampleraw][variation])) 

        h.SetName(variation)
        h.SetDirectory(of)

      of.Write()
      of.IsA().Destructor(of)

    f.IsA().Destructor(f)

if __name__ == '__main__':
  analysis = 'VVlvqq'
  releaseDir = 'test_Histo'
  inputTag = 'itest03'
  wsTag = 'wtest02'
  doInjection = False

  # import histograms from Run-I code
  run1_input_path_format = '/afs/cern.ch/work/k/kalliopi/public/forValerio/test0/lvqq_{mass}.root'
  masses = [3000, 2500, 2000, 1500, 1000 ]
  dest = createEmptyDirs(releaseDir, analysis, inputTag)

  poi_setups = { 3000 : [60, 0, 30],
                 2500 : [60, 0, 30],
                 2000 : [60, 0, 20],
                 1500 : [75, 0, 20],
                 1000 : [80, 0, 20],
                 1000 : [60, 0, 20]
               }

  getInputFromRunI(run1_input_path_format, masses, dest)

  # run over them
  lvqq = RF.VVlvqqValidationRunner(analysis)
  lvqq.setReleaseDir(releaseDir)
  lvqq.setInputListTag(inputTag)
  lvqq.setOutputWSTag(wsTag)

  # create a new channel (will automatically add data)
  lvqq.addChannel('SR', '') # not using selection criteria

  # loop over signals
  for mass in masses:
    thisSig = 'signal_{mass}'.format(mass=mass)
    lvqq.channel('SR').addSample(thisSig)
    lvqq.channel('SR').sample(thisSig).setNormByLumi(True)
    lvqq.channel('SR').sample(thisSig).multiplyBy('mu', 1.0, 0, 20)

    # declare signal mass value
    lvqq.addSignal(thisSig, mass)


  # add also bkg
  lvqq.channel('SR').addSample('background1')
  lvqq.channel('SR').sample('background1').setNormByLumi(False)
  lvqq.channel('SR').sample('background1').multiplyBy('nbkg', 1.0, 0, 20)
  
  lvqq.channel('SR').addSample('background2')
  lvqq.channel('SR').sample('background2').setNormByLumi(False)
  lvqq.channel('SR').sample('background2').multiplyBy('nbkg', 1.0, 0, 20)

  lvqq.setPOI('mu')

  if doInjection:
    lvqq.setInjectionSample('signal_2000')
    lvqq.setInjectionStrength(4.5)

  # setup limit runner
  lvqq.limitRunner().setPOIScan(20, 0, 20) # nsteps, min, max
  for mass in poi_setups:
    # deal with this signal in an optimised way
    lvqq.limitRunner().setPOIScan(mass, poi_setups[mass][0], poi_setups[mass][1], poi_setups[mass][2])

  lvqq.run()

  # plot
  ROOT.gROOT.ProcessLine('.L atlasstyle/AtlasStyle.C+')
  ROOT.SetAtlasStyle()
  plotter = RF.Plotter('test_VVlvqq', '.')
  plotter.setVarName('m_{lvqq}')
  plotter.setVarUnit('GeV')
  plotter.setSpec(500,  1, 1765.000 * 0.75616 ) 
  plotter.setSpec(1000, 1, 98.950 * 0.79607 )
  plotter.setSpec(1500, 1, 16.110 * 0.81147 )
  plotter.setSpec(2000, 1, 3.810 * 0.81310 )
  plotter.setSpec(2500, 1, 1.076 * 0.81773 )
  plotter.setSpec(3000, 1, 0.334 * 0.81999 )
  plotter.setOutputFormat(RF.Plotter.png)
  plotter.process(lvqq.getStatResults())
