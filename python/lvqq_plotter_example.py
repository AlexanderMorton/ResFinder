# an example plotter for the lvqq analysis
# Valerio Ippolito - Harvard University

import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.gROOT.ProcessLine(ROOT.gSystem.ExpandPathName('.x $ROOTCOREDIR/scripts/load_packages.C'))

from ROOT import RF as RF

if __name__ == '__main__':
  
  releaseDir = './test_v4'
  analysis = 'VVlvqq_HVT'
  outputWSTag = 'wtest01'

  # retrieve results
  results = RF.StatisticalResultsCollection('lvqqtest')
  filename = '{releaseDir}/{analysis}/ws/results_{analysis}_{outputWSTag}.root'.format(releaseDir=releaseDir, analysis=analysis, outputWSTag=outputWSTag)
  results.retrieve(filename)


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
  plotter.process(results)

