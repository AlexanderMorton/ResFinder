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
  hists = { 'signal_{mass}_SR' : { 'nominal'   : 'WPrime+WH+lvbb_1jet_2btag_ptv1_nominal',
                                 },
            'Top_SR'    : { 'nominal'   : 'ttbar_1jet_2btag_ptv1_nominal',
                          },
            'Wbb_SR'    : { 'nominal'   : 'Wbb_1jet_2btag_ptv1_nominal',
                          },
            'WHSM_SR'   : { 'nominal'   : 'WH125_1jet_1btag_ptv1_nominal',
                          },
            'data_SR'   : { 'nominal'   : 'data_1jet_2btag_ptv1',
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
  analysis = 'Wtb'
  releaseDir = 'test_v1'
  inputTag = 'itest01'
  wsTag = 'wtest01'
  doInjection = False

  # import histograms from Run-I code
  #  run1_input_path_format = '/afs/cern.ch/user/w/wfisher/work/VHreso/VHlvbb_WPRIME/test00/lvbb_sL00_{mass}.root'
  #/afs/cern.ch/work/k/kalliopi/public/forValerio/test0/lvbb_{mass}.root'
  masses = [1000, 1500 ]
  #dest = createEmptyDirs(releaseDir, analysis, inputTag)

  poi_setups = { 
                 1000 : [100, 0, 100],
                 1500 : [100, 0, 100]
               }

#  getInputFromRunI(run1_input_path_format, masses, dest)

  # run over them
  lvbb = RF.VHlvbbAnalysisRunner(analysis)
  lvbb.setReleaseDir(releaseDir)
  lvbb.setInputListTag(inputTag)
  lvbb.setOutputWSTag(wsTag)

  # create a new channel (will automatically add data)
  lvbb.addChannel('J2T2E', '') # not using selection criteria

  # loop over signals
  for mass in masses:
    thisSig = 'WpL{mass}'.format(mass=mass)
    lvbb.channel('J2T2E').addSample(thisSig)
    lvbb.channel('J2T2E').sample(thisSig).setNormByLumi(True)
    lvbb.channel('J2T2E').sample(thisSig).multiplyBy('mu', 1.0, 0, 100)

    # declare signal mass value
    lvbb.addSignal(thisSig, mass)


  # add also bkg
  lvbb.channel('J2T2E').addSample('TTbar')
  lvbb.channel('J2T2E').sample('TTbar').setNormByLumi(False)
  lvbb.channel('J2T2E').sample('TTbar').multiplyBy('nbkg', 1.0, 0, 100)
  
#  lvbb.channel('J2T2E').addSample('Wbb')
#  lvbb.channel('J2T2E').sample('Wbb').setNormByLumi(False)
#  lvbb.channel('J2T2E').sample('Wbb').multiplyBy('nbkg', 1.0, 0, 100)

#  lvbb.channel('J2T2E').addSample('WHSM')
#  lvbb.channel('J2T2E').sample('WHSM').setNormByLumi(False)
#  lvbb.channel('J2T2E').sample('WHSM').multiplyBy('nbkg', 1.0, 0, 100)

  lvbb.setPOI('mu')

   # setup limit runner
  lvbb.limitRunner().setPOIScan(20, 0, 20) # nsteps, min, max
  for mass in poi_setups:
    # deal with this signal in an optimised way
    lvbb.limitRunner().setPOIScan(mass, poi_setups[mass][0], poi_setups[mass][1], poi_setups[mass][2])

  lvbb.run()

  # plot
  ROOT.gROOT.ProcessLine('.L AtlasStyle.C+')
  ROOT.SetAtlasStyle()
  plotter = RF.Plotter('test_VHlvbb', '.')
  plotter.setVarName('m_{lvbb}')
  plotter.setVarUnit('GeV')
#  plotter.setSpec(500,  1, 1765.000 * 0.75616 ) 
#  plotter.setSpec(100, 1, 98.950 * 0.79607 )
  plotter.setSpec(1500, 666*0.0434/2./10., 0.0434/2.)
  plotter.setSpec(1900, 157*0.0434/2./10., 0.0434/2.)
#  plotter.setSpec(2500, 1, 1.076 * 0.81773 )
#  plotter.setSpec(3000, 1, 0.334 * 0.81999 )
  plotter.setOutputFormat(RF.Plotter.png)
  plotter.process(lvbb.getStatResults())
