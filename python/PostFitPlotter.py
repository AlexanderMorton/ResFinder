import ROOT
import math
ROOT.gROOT.SetBatch(True)
ROOT.gROOT.ProcessLine(ROOT.gSystem.ExpandPathName('.x $ROOTCOREDIR/scripts/load_packages.C'))
from subprocess import call

# TODO
# remove global

class Samp:
  def __init__(self, name, plotShade, norm):
    self.name=name
    self.plotShade=plotShade
    self.norm=norm

def plotInputs(inputsPath, outputPath, channels, backgrounds, signals, signalpt=Samp("",9,1)):
  print "plotInputs info: ", inputsPath, outputPath, channels, backgrounds, signals, signalpt
  #for mass in signal:
  #  thisSig = "h_"+str(mass)+"_TeV_HVT"
  #  #add to list?
  #thisSig = "h_"+str(signal)+"_TeV_HVT"
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

    i=0
    for background in backgrounds:
      i=i+1
      filename = background.name+"_"+channel
      f = ROOT.TFile.Open('{dest}/{sample}.root'.format(dest=inputsPath, sample=filename))
      hist = f.Get("nominal")
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
      #hist.GetXaxis().SetTitle("log_{10}(m_{{\rm VH}} / {\rm MeV})")
      myStack.Add(hist)
      #f.Close()
      #hist.Draw()

    #get signal and add to stack
    if signalpt.name != "":
      filename = signals[0]+str(signalpt.name)+"_"+channel
      sigf = ROOT.TFile.Open('{dest}/{sample}.root'.format(dest=inputsPath, sample=filename))
      sighist = sigf.Get("nominal")
      sighist.Scale(signalpt.norm)
      sighist.SetTitle(str(signalpt.name).replace("_",".")+" GeV HVT")
      sighist.SetFillColor(signalpt.plotShade)
      sighist.SetMarkerColor(signalpt.plotShade)
      sighist.SetFillStyle(1001)
      sighist.SetDirectory(0)
      #sighist.GetXaxis().SetTitle(" log_{10}(m_{{\rm VH}} / {\rm MeV})")
      #sighist.Draw("same HIST L")
      myStack.Add(sighist)
      #sighist.Draw("same")


    #time to draw the ratio plot
    ratiopad.cd()

    #draw middle line on ratio
    mylist=list(myStack.GetHists())
    histTotal = mylist[0].Clone() 
    for i in mylist[1:]:
      histTotal.Add(i)

    hbkgratio = histTotal.Clone()
    hbkgratio.Divide(histTotal)
    hbkgratio.Draw()
    hbkgratio.GetYaxis().SetRangeUser(-1, 1)
    hbkgratio.GetYaxis().SetTitle("Data-MC/Data")
    hbkgratio.GetYaxis().SetLabelSize(0.1)
    hbkgratio.GetYaxis().SetTitleSize(0.15)
    hbkgratio.GetYaxis().SetTitleOffset(0.25)
    hbkgratio.GetYaxis().SetNdivisions(205)
    hbkgratio.GetXaxis().SetTitle(histTotal.GetXaxis().GetTitle())
    hbkgratio.GetXaxis().SetLabelSize(0.1)
    hbkgratio.GetXaxis().SetTitleSize(0.15)
    hbkgratio.GetXaxis().SetTitleOffset(0.75)
    hbkgratio.Draw("e2")

    ratioLine = ROOT.TLine(hbkgratio.GetBinLowEdge(0), 0,
            hbkgratio.GetBinLowEdge(hbkgratio.GetNbinsX()+1), 0)
    ratioLine.SetLineColor(ROOT.kBlack)
    ratioLine.SetLineStyle(7)
    ratioLine.Draw("same")


    #get data
    filename = "data_"+channel
    dataf = ROOT.TFile.Open('{dest}/{sample}.root'.format(dest=inputsPath, sample=filename))
    datahist = dataf.Get("nominal")


    #calc data to MC ratio
    dataratio = datahist.Clone()
    dataratio.Add(histTotal, -1)
    dataratio.Divide(histTotal)
    dataratio.Draw("same")


    #draw the stack and the data
    plotpad.cd()
#    datahist.GetXaxis().SetTitle(" log_{10}(m_{{\rm VH}} / {\rm MeV})")
    myStack.Draw("HIST")
    datahist.Draw("same")
    #myStack.GetXaxis().SetTitle("log_{10}(m_{VH} / MeV)")
    


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
    plotpad.cd()
    plotpad.SetLogy()
    canv.Update()
    canv.SaveAs(str(outputPath)+str(channel)+"_postfit_"+str(signalpt.name)+"_log.png")
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

def makePostFitPlots(analysis, releaseDir, inputTag, massPoints, channels, backgrounds, signals):

  #path to the input histograms
  inputsPath = '{releaseDir}/{analysis}/data/{inputTag}'.format(releaseDir=releaseDir, analysis=analysis, inputTag=inputTag)

  #desired output path
  outputPath = '{releaseDir}/{analysis}/ws/diagnostics/'.format(releaseDir=releaseDir, analysis=analysis)

  #path to the post-fit results
  postFitPath = '{releaseDir}/{analysis}/ws/diagnostics/sbfit_{analysis}_{inputTag}'.format(releaseDir=releaseDir, analysis=analysis, inputTag=inputTag)

  #Limit setting results. Used to get the 95% CL value of mu
  resultsFile = '{releaseDir}/{analysis}/ws/results_{analysis}_{inputTag}.root'.format(releaseDir=releaseDir, analysis=analysis, inputTag=inputTag)

  massPointFiles = []
  for mass in massPoints:
    massPointFiles.append('{path}_{mass}p000.root'.format(path=postFitPath,mass=mass))

  #get background norm for this mass point
  fidx = 0
  for filename in massPointFiles:
    print "FileName ", filename
    f = ROOT.TFile.Open(filename)
    postFitvars = f.Get("fitresult_simPdf_obsData")
    for bkg in backgrounds:
      try:
        scalefactor = postFitvars.floatParsFinal().find("nbkg_"+bkg.name).getVal() 
        print "got new norm val", bkg.name,"=",bkg.norm
      except:  
        scalefactor = 1 
        print "using scale factor of 1", bkg.name,"=",bkg.norm 
      bkg.norm = scalefactor
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
        signalsample = Samp(masspt,2,scalefactor) 
        break
      else: 
        signalsample = Samp(masspt,2,1) 
    #scalefactor = postFitvars.floatParsFinal().find("mu").getVal() 
    #scalefactor = 1
    plotInputs(inputsPath, outputPath, channels, backgrounds, signals, signalsample)
    saveHists(inputsPath, outputPath, channels, backgrounds, signals, signalsample)

if __name__ == '__main__':

  #mass points we want to produce
  massPoints =[ 1000,  ]

  #Channels included in the fit. These are used to build the input histogram filenames
  channels = ['lvJ_0tag_0additionaltag_unblind_SR', 'lvJ_1tag_0additionaltag_unblind_SR', 'lvJ_2tag_0additionaltag_unblind_SR', 'lvJ_0tag_1additionaltag_unblind_SR', 'lvJ_1tag_1additionaltag_unblind_SR', 'lvJ_2tag_1additionaltag_unblind_SR']

  #Backgrounds included in the fit. These are used to build the input histogram filenames
  #name, colour, default norm
  #the default norm is replaced with those in the massPointFiles
  backgrounds = [
    Samp('ttbar',0,1), 
    Samp('single_top',1,1), 
    Samp('diboson',3,1), 
    Samp('W_b',4,1), 
    Samp('W_c',5,1), 
    Samp('W_lf',6,1), 
    Samp('Z_jets',8,1)
  ]

  signals = ['HVT']
#'{releaseDir}/{analysis}/data/{inputTag}'
  #path used to store the histograms
  analysis = 'WHlvbb_Wprime'
  releaseDir = 'run2_current'
  inputTag = 'flat_syst_Oct20'

  #run it
  print "Make plots!"
  makePostFitPlots(analysis, releaseDir, inputTag, massPoints, channels, backgrounds, signals)


