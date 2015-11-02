import ROOT
import math
ROOT.gROOT.SetBatch(True)
ROOT.gROOT.ProcessLine(ROOT.gSystem.ExpandPathName('.x $ROOTCOREDIR/scripts/load_packages.C'))
ROOT.gROOT.LoadMacro("atlasstyle/AtlasStyle.C")
ROOT.gROOT.LoadMacro("atlasstyle/AtlasUtils.C")
ROOT.gROOT.LoadMacro("atlasstyle/AtlasLabels.C")
from subprocess import call

# TODO
# remove global

class Samp:
  def __init__(self, name, plotShade, norm):
    self.name=name
    self.plotShade=plotShade
    self.norm=norm

def plotInputs(inputsPath, outputPath, channels, backgrounds, signals, signalpt=Samp("",9,1)):
  #for mass in signal:
  #  thisSig = "h_"+str(mass)+"_TeV_HVT"
  #  #add to list?
  #thisSig = "h_"+str(signal)+"_TeV_HVT"
  ROOT.SetAtlasStyle()
  canv = ROOT.TCanvas("canv","canv",800,600);
  for channel in channels:
    print channel
    canv.Clear()
    canv.SetLeftMargin(0.0)
    canv.SetRightMargin(0.0)
    
    # divide into legend and plot pads
    canv.Divide(2, 1)
    
    # format the pad for the plots
    mainpad = canv.cd(1)
    mainpad.Clear()
    mainpad.SetPad(0.0, 0.0, 0.7, 1.0)
    mainpad.Divide(1, 2)

    #pad for the ratio plot
    ratiopad = mainpad.cd(2)
    ratiopad.Clear()
    ratiopad.SetPad(0.0, 0.05, 1.0, 0.3)
    ratiopad.SetTopMargin(0.02)
    ratiopad.SetLeftMargin(0.15)
    ratiopad.SetRightMargin(0.075)
    ratiopad.SetBottomMargin(0.3)



    #this pad will contain the stacked plot
    plotpad = mainpad.cd(1)
    plotpad.Clear()
    plotpad.SetPad(0.0, 0.32, 1.0, 1.0)
    plotpad.SetBottomMargin(0.02)
    plotpad.SetLeftMargin(0.15)
    plotpad.SetRightMargin(0.075)


    myStack = ROOT.THStack("stack", "")
    bkgStack = ROOT.THStack("stack", "")

    for background in backgrounds:
      filename = background.name+"_"+channel
      f = ROOT.TFile.Open('{dest}/{sample}.root'.format(dest=inputsPath, sample=filename))
      hist = f.Get("Nominal")
      print filename
      print hist
      #hist1 = hist.Clone()
      #ROOT.SetOwnership(hist1, False)
      hist.SetName(hist.GetTitle())
      hist.SetFillColor(background.plotShade)
      hist.SetFillStyle(1001)
      hist.SetMarkerColor(background.plotShade)
      hist.SetMarkerStyle(0)
      hist.SetMarkerSize(0)
      hist.SetDirectory(0)
      hist.Scale(background.norm)
      hist.GetXaxis().SetTitle("m(lvJ) [GeV]")
      hist.GetYaxis().SetTitle("events")
      hist.GetYaxis().SetTitleOffset(0)
      hist.SetTitle(background.name)
      bkgStack.Add(hist)
      myStack.Add(hist)
      #f.Close()
      #hist.Draw()

    #get signal and add to stack
    if signalpt.name != "":
      filename = signals[0]+str(signalpt.name)+"_"+channel
      sigf = ROOT.TFile.Open('{dest}/{sample}.root'.format(dest=inputsPath, sample=filename))
      sighist = sigf.Get("Nominal")
      sighist.Scale(signalpt.norm)
      sighist.SetTitle(str(signalpt.name).split("0")[0]+" TeV HVT")
      sighist.SetFillColor(signalpt.plotShade)
      sighist.SetFillStyle(3017)
      sighist.SetMarkerColor(signalpt.plotShade)
      sighist.SetLineColor(signalpt.plotShade)
      sighist.SetDirectory(0)
      #sighist.GetXaxis().SetTitle(" log_{10}(m_{{\rm VH}} / {\rm MeV})")
      #sighist.Draw("same HIST L")
      myStack.Add(sighist)
      #sighist.Draw("same")


    #time to draw the ratio plot
    ratiopad.cd()

    #draw middle line on ratio
    mylist=list(bkgStack.GetHists())
    histTotal = mylist[0].Clone() 
    for i in mylist[1:]:
      histTotal.Add(i)

    #get data
    filename = "data_"+channel
    dataf = ROOT.TFile.Open('{dest}/{sample}.root'.format(dest=inputsPath, sample=filename))
    datahist = dataf.Get("Nominal")


    #calc data to MC ratio
    dataratio = datahist.Clone()
    dataratio.Add(histTotal, -1)
    dataratio.Divide(histTotal)
    dataratio.GetYaxis().SetRangeUser(-1, 1)
    dataratio.GetYaxis().SetTitle("Deviation")
    dataratio.GetYaxis().SetLabelSize(0.1)
    dataratio.GetYaxis().SetTitleSize(0.15)
    dataratio.GetYaxis().SetTitleOffset(0.25)
    dataratio.GetYaxis().SetNdivisions(205)
    dataratio.GetXaxis().SetTitle(histTotal.GetXaxis().GetTitle())
    dataratio.GetXaxis().SetLabelSize(0.1)
    dataratio.GetXaxis().SetTitleSize(0.15)
    dataratio.GetXaxis().SetTitleOffset(0.75)
    dataratio.Draw()

    ratioLine = ROOT.TLine(dataratio.GetBinLowEdge(0), 0,
            dataratio.GetBinLowEdge(dataratio.GetNbinsX()+1), 0)
    ratioLine.SetLineColor(ROOT.kBlack)
    ratioLine.SetLineStyle(7)
    ratioLine.Draw("same")






    #draw the stack and the data
    plotpad.cd()
    maximum=myStack.GetMaximum()
    myStack.SetMaximum(2*maximum)
    myStack.Draw("HIST")
    datahist.Draw("same")
    ROOT.myText(       0.75,  0.74, 1, "#sqrt{s}= 13 TeV");
    ROOT.myText(       0.5,  0.74, 1, "#int L dt = 1.0 fb^{-1}");
    ROOT.ATLASLabel(0.5,0.84,"Internal");
    


    #add legend to canvas
    legpad = canv.cd(2)
    legpad.SetPad(0.7, 0.12, 1.0, 0.93)
    legpad.SetLeftMargin(0.0)
    legpad.SetRightMargin(0.0)
    legpad.SetBorderSize(0)
    
    leg = ROOT.TLegend(0, 0, 1, 1)
    leg.SetShadowColor(ROOT.kWhite)
    leg.SetLineColor(ROOT.kWhite)
    leg.SetFillStyle(0)
    leg.SetEntrySeparation(0.05)
    leg.SetMargin(0.15)

    #overlay any signal points

    l=list(myStack.GetHists())
    l.reverse()
    map(lambda h: leg.AddEntry(h, h.GetTitle(), "f"), l)
#    if signalOverlays:
#      map(lambda h: leg.AddEntry(h, h.GetTitle(), "f"), legSigList)
    leg.SetTextSize(1.0/len(leg.GetListOfPrimitives()))
    leg.Draw()



    
    canv.SaveAs(str(outputPath)+str(channel)+"_postfit_"+str(signalpt.name)+".png")
    canv.SaveAs(str(outputPath)+str(channel)+"_postfit_"+str(signalpt.name)+".C")
    canv.SaveAs(str(outputPath)+str(channel)+"_postfit_"+str(signalpt.name)+".eps")
    plotpad.cd()
    plotpad.SetLogy()
    canv.Update()
    canv.SaveAs(str(outputPath)+str(channel)+"_postfit_"+str(signalpt.name)+"_log.png")
    canv.SaveAs(str(outputPath)+str(channel)+"_postfit_"+str(signalpt.name)+"_log.C")
    canv.SaveAs(str(outputPath)+str(channel)+"_postfit_"+str(signalpt.name)+"_log.eps")
    #make stack plot and save


    #save hists to file
    #outfile = ROOT.TFile.Open(channel+"_postfit_"+signalpt.name+".root")
  
def saveHists(inputsPath, outputPath,channels, backgrounds, signals, signalpt=Samp("",9,1)):
  #save all backgrounds as one hist
  for channel in channels:
    filename = backgrounds[0].name+"_"+channel
    f = ROOT.TFile.Open('{dest}/{sample}.root'.format(dest=inputsPath, sample=filename))
    bkgtotal = f.Get("nominal").Clone()
    bkgtotal.Scale(backgrounds[0].norm)
    bkgtotal.SetDirectory(0)
    for background in backgrounds[1:]:
      filename = background.name+"_"+channel
      file1 = ROOT.TFile.Open('{dest}/{sample}.root'.format(dest=inputsPath, sample=filename))
      hist = file1.Get("nominal")
      print "scaling",background.name,background.norm
      #hist.Scale(background.norm)
      hist.SetDirectory(0)
      bkgtotal.Add(hist)
      #add to total hist
        #datahist = dataStack.GetHists()[0].Clone()
        #for hist in dataStack.GetHists()[1:]:
        #  datahist.Add(hist)

    #save signal hist
    if signalpt.name != "":
      filename = signals[0]+str(signalpt.name)+"_"+channel
      sigf = ROOT.TFile.Open('{dest}/{sample}.root'.format(dest=inputsPath, sample=filename))
      sighist = sigf.Get("nominal")
      sighist.Scale(signalpt.norm)
      sighist.SetDirectory(0)

    bkgtotal.SetNameTitle("background","background")
    sighist.SetNameTitle("signal","signal")
    outfile = ROOT.TFile.Open(str(outputPath)+str(channel)+"_postfit_"+str(signalpt.name)+".root", "RECREATE")
    ##outfile.Write(sighist)
    ##outfile.Write(bkgtotal)
    sighist.Write()
    bkgtotal.Write()
    outfile.Close()

def makePostFitPlots(analysis, releaseDir, inputTag, outputTag, massPoints, channels, backgrounds, signals):

  #path to the input histograms
  inputsPath = '{releaseDir}/{analysis}/data/{inputTag}'.format(releaseDir=releaseDir, analysis=analysis, inputTag=inputTag)

  #desired output path
  outputPath = '{releaseDir}/{analysis}/ws/diagnostics/'.format(releaseDir=releaseDir, analysis=analysis)

  #path to the post-fit results
  postFitPath = '{releaseDir}/{analysis}/ws/diagnostics/sbfit_{analysis}_{outputTag}'.format(releaseDir=releaseDir, analysis=analysis, outputTag=outputTag)

  #Limit setting results. Used to get the 95% CL value of mu
  resultsFile = '{releaseDir}/{analysis}/ws/results_{analysis}_{outputTag}.root'.format(releaseDir=releaseDir, analysis=analysis, outputTag=outputTag)

  massPointFiles = []
  for mass in massPoints:
    massPointFiles.append('{path}_{mass}p000.root'.format(path=postFitPath,mass=mass))

  #get background norm for this mass point
  fidx = 0
  for filename in massPointFiles:
    f = ROOT.TFile.Open(filename)
    postFitvars = f.Get("fitresult_simPdf_obsData")
    for bkg in backgrounds:
      try:
        if bkg.name == "diboson":
          print "using diboson, not Diboson"
          scalefactor = postFitvars.floatParsFinal().find("alpha_XS_diboson").getVal() 
        elif bkg.name == "stop":
          print "using st, not stop"
          scalefactor = postFitvars.floatParsFinal().find("alpha_XS_st").getVal() 
        elif bkg.name == "Z":
          print "using zjets, not Z"
          scalefactor = postFitvars.floatParsFinal().find("alpha_XS_zjets").getVal() 
        else:
          scalefactor = postFitvars.floatParsFinal().find("alpha_XS_"+bkg.name).getVal() 
        scalefactor = 1+(0.25*scalefactor)
        print "got new norm val"
      except:  
        scalefactor = 1 
        print "using scale factor of 1" 
      bkg.norm = scalefactor
      bkg.norm = 1
      print "norm val", bkg.name,"=",bkg.norm
    f.Close()
#    masspt = filename.split("_")[1]
#    masspt = masspt.split("p")[0]
    masspt = massPoints[fidx]
    fidx += 1

    #get 95% CL mu value
    #same for all mass points
    f = ROOT.TFile.Open(resultsFile)
    mytree = f.Get("stats")
    print mytree
    for event in mytree:
      #this is gonna cause problems at some point
      thismass =  round(event.point,2)
      print "mass1",thismass
      print "mass2",masspt
      if int(thismass) == masspt:
        print "getting SF for signal from ws"
        scalefactor = event.obs_upperlimit
        scalefactor = 0.001*scalefactor
        scalefactor = 0.1
        signalsample = Samp(masspt,2,scalefactor) 
        break
      else: 
        signalsample = Samp(masspt,2,1) 
    #scalefactor = postFitvars.floatParsFinal().find("mu").getVal() 
    #scalefactor = 1
    plotInputs(inputsPath, outputPath, channels, backgrounds, signals, signalsample)
    #saveHists(inputsPath, outputPath, channels, backgrounds, signals, signalsample)

if __name__ == '__main__':

  #mass points we want to produce
  massPoints =[ 1000 ]

  #Channels included in the fit. These are used to build the input histogram filenames
#  channels = ['lvJ_0tag_0additionaltag_unblind_SR_vh_m', 'lvJ_1tag_0additionaltag_unblind_SR_vh_m', 'lvJ_2tag_0additionaltag_unblind_SR_vh_m', 'lvJ_0tag_1padditionaltag_unblind_SR_vh_m', 'lvJ_1tag_1padditionaltag_unblind_SR_vh_m', 'lvJ_2tag_1padditionaltag_unblind_SR_vh_m']

  channels = ['lvJ_0tag_0additionaltag_blind_highMH_vh_m',
            'lvJ_0tag_0additionaltag_blind_lowMH_vh_m',
            'lvJ_0tag_0additionaltag_blind_SR_vh_m',
            'lvJ_0tag_0additionaltag_unblind_highMH_vh_m',
            'lvJ_0tag_0additionaltag_unblind_lowMH_vh_m',
            'lvJ_0tag_0additionaltag_unblind_SR_vh_m',
            'lvJ_0tag_1padditionaltag_blind_highMH_vh_m',
            'lvJ_0tag_1padditionaltag_blind_lowMH_vh_m',
            'lvJ_0tag_1padditionaltag_blind_SR_vh_m',
            'lvJ_0tag_1padditionaltag_unblind_highMH_vh_m',
            'lvJ_0tag_1padditionaltag_unblind_lowMH_vh_m',
            'lvJ_0tag_1padditionaltag_unblind_SR_vh_m',
            'lvJ_1tag_0additionaltag_blind_highMH_vh_m',
            'lvJ_1tag_0additionaltag_blind_lowMH_vh_m',
            'lvJ_1tag_0additionaltag_blind_SR_vh_m',
            'lvJ_1tag_0additionaltag_unblind_highMH_vh_m',
            'lvJ_1tag_0additionaltag_unblind_lowMH_vh_m',
            'lvJ_1tag_0additionaltag_unblind_SR_vh_m',
            'lvJ_1tag_1padditionaltag_blind_highMH_vh_m',
            'lvJ_1tag_1padditionaltag_blind_lowMH_vh_m',
            'lvJ_1tag_1padditionaltag_blind_SR_vh_m',
            'lvJ_1tag_1padditionaltag_unblind_highMH_vh_m',
            'lvJ_1tag_1padditionaltag_unblind_lowMH_vh_m',
            'lvJ_1tag_1padditionaltag_unblind_SR_vh_m',
            'lvJ_2tag_0additionaltag_blind_highMH_vh_m',
            'lvJ_2tag_0additionaltag_blind_lowMH_vh_m',
            'lvJ_2tag_0additionaltag_blind_SR_vh_m',
            'lvJ_2tag_0additionaltag_unblind_highMH_vh_m',
            'lvJ_2tag_0additionaltag_unblind_lowMH_vh_m',
            'lvJ_2tag_0additionaltag_unblind_SR_vh_m',
            'lvJ_2tag_1padditionaltag_blind_highMH_vh_m',
            'lvJ_2tag_1padditionaltag_blind_lowMH_vh_m',
            'lvJ_2tag_1padditionaltag_blind_SR_vh_m',
            'lvJ_2tag_1padditionaltag_unblind_highMH_vh_m',
            'lvJ_2tag_1padditionaltag_unblind_lowMH_vh_m',
            'lvJ_2tag_1padditionaltag_unblind_SR_vh_m']



  #Backgrounds included in the fit. These are used to build the input histogram filenames
  #name, colour, default norm
  #the default norm is replaced with those in the massPointFiles
  backgrounds = [
    Samp('Z',419,1),
    Samp('Diboson',618,1), 
    Samp('stop',402,1), 
    Samp('Wl',867,1), 
    Samp('Wc',594,1), 
    Samp('Wb',600,1), 
    Samp('ttbar',800,1)
  ]

  signals = ['HVTWHlvqq']

  #path used to store the histograms
  analysis = 'WHlvbb_Wprime'
  releaseDir = '151029_WadeRun/Results/run2_current/'
  inputTag = 'flat_syst'
  outputTag = 'flat_syst'

  #run it
  makePostFitPlots(analysis, releaseDir, inputTag, outputTag, massPoints, channels, backgrounds, signals)


