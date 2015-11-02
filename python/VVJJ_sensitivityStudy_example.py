# a more complex example driver, implementing a validation of the ResonanceFinder
# framework in terms of repetition of the Run-2 sensitivity study based on histograms
# Valerio Ippolito - Harvard University

import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.gROOT.ProcessLine(ROOT.gSystem.ExpandPathName('.x $ROOTCOREDIR/scripts/load_packages.C'))

from ROOT import RF as RF

def getInputFromRunI(pathformat, masses, dest):
  import os
  import shutil
  if os.path.exists(dest): shutil.rmtree(dest)
  os.makedirs(dest)

  hists = { 'signal_{mass}_SR' : { 'nominal'   : 'signal/h_nominal_sig',
                                   'pts_up'    : 'signal/h_up_pts',
                                   'pts_down'  : 'signal/h_down_pts',
                                   'ptr_up'    : 'signal/h_up_ptr',
                                   'ptr_down'  : 'signal/h_down_ptr',
                                   'jms_up'    : 'signal/h_up_jms',
                                   'jms_down'  : 'signal/h_down_jms',
                                 },
            'background_SR'    : { 'nominal'   : 'background/h_nominal_bkg',
                                   'fit_up'    : 'background/h_highband',
                                   'fit_down'  : 'background/h_lowband',
                                 },
            'data_SR'          : { 'nominal'   : 'data/h_observed',
                                 },
          }

  for mass in masses:
    f = ROOT.TFile.Open(pathformat.format(mass=mass))

    dir = 'WPRIME_{mass}_S_B_hists'.format(mass=mass)

    for sampleraw in hists:
      if 'mass' in sampleraw:
        sample = sampleraw.format(mass=mass)
      else:
        sample = sampleraw

      of = ROOT.TFile.Open('{dest}/{sample}.root'.format(dest=dest, sample=sample), 'RECREATE')

      for variation in hists[sampleraw]:
        h = f.Get('{dir}/{path}'.format(dir=dir, path=hists[sampleraw][variation]))
        h.SetName(variation)
        h.SetDirectory(of)
      
      of.Write()
      of.IsA().Destructor(of)

    f.IsA().Destructor(f)

if __name__ == '__main__':
  analysis = 'VVJJ_WPRIME'
  releaseDir = 'test_v1'
  inputTag = 'itest03'
  wsTag = 'wtest02'
  doInjection = False

  # import histograms from Run-I code
  run1_input_path_format = '/afs/cern.ch/work/v/vippolit/public/VVJJ/DibjetStatPL_lite/Input/WPRIME/WPRIME_{mass}_measurement_combined_meas_model.root'
  masses = [3000, 2500, 2000, 1500]
  dest = '{releaseDir}/{analysis}/data/{inputTag}'.format(releaseDir=releaseDir, analysis=analysis, inputTag=inputTag)

  poi_setups = { 3000 : [60, 0, 30],
                 2500 : [60, 0, 30],
                 2000 : [60, 0, 20],
                 1500 : [75, 0, 20],
               }

  getInputFromRunI(run1_input_path_format, masses, dest)

  # run over them
  vvjj = RF.VVJJValidationRunner(analysis)
  vvjj.setReleaseDir(releaseDir)
  vvjj.setInputListTag(inputTag)
  vvjj.setOutputWSTag(wsTag)

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
    vvjj.channel('SR').sample(thisSig).multiplyBy('ntrk', 1.0, 0.69, 0.99, RF.MultiplicativeFactor.GAUSSIAN)

    # declare signal mass value
    vvjj.addSignal(thisSig, mass)


  # add also bkg
  vvjj.channel('SR').addSample('background')
  vvjj.channel('SR').sample('background').setNormByLumi(False)
  vvjj.channel('SR').sample('background').multiplyBy('nbkg', 1.0, 0, 2000)
  vvjj.channel('SR').sample('background').addVariation('fit')

  vvjj.setPOI('mu')

  if doInjection:
    vvjj.setInjectionSample('signal_2000')
    vvjj.setInjectionStrength(4.5)

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
