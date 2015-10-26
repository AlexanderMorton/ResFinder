#include "ResonanceFinder/BinnedWorkspaceBuilder.h"
#include "ResonanceFinder/WorkspaceEditor.h"

#include <RooStats/HistFactory/Measurement.h>
#include <RooStats/ModelConfig.h>
#include <RooStats/HistFactory/MakeModelAndMeasurementsFast.h>

#include <RooRealVar.h>
#include <RooAddPdf.h>

#include <sstream>

RF::BinnedWorkspaceBuilder::BinnedWorkspaceBuilder(TString name) : IWorkspaceBuilder(name)
{
}

RF::BinnedWorkspaceBuilder::~BinnedWorkspaceBuilder()
{
}

void RF::BinnedWorkspaceBuilder::setOutputFilePrefix(TString outputFilePrefix)
{
   m_outputFilePrefix = outputFilePrefix;
}

TString RF::BinnedWorkspaceBuilder::outputFilePrefix()const
{
   return m_outputFilePrefix;
}

void RF::BinnedWorkspaceBuilder::setHistoCollection(RF::HistoCollection& hc)
{
   m_hc = hc;
}

RF::HistoCollection &RF::BinnedWorkspaceBuilder::histoCollection()
{
   return m_hc;
}

void RF::BinnedWorkspaceBuilder::build()
{
   RF::HistoCollection &hc = histoCollection();

   // persist histograms
   const TString hfile = dest() + "_histos.root";
   hc.persist(hfile);

   // create a measurement
   RooStats::HistFactory::Measurement hf_meas(name(), name());

   hf_meas.SetOutputFilePrefix(outputFilePrefix().Data());
   hf_meas.SetExportOnly(kTRUE);

   hf_meas.SetPOI(poi().Data());

   hf_meas.SetLumi(1.0); // convention: all histos already normalised to L
   hf_meas.SetLumiRelErr(0.10);

   const TString nominal = m_hc.tagNominal();

   // replace HistFactory terms with manual terms
   std::map<TString, TString> replaceTermsHistFactoryObsName;
   std::map<TString, std::pair<TString, TString>> replaceTermsFactory;
   std::map<TString, std::pair<RooAbsPdf*, TString>> replaceTermsPdf;

   for (auto chanName : channels()) {
      RF::Channel &thisChan = channel(chanName);

      RooStats::HistFactory::Channel hf_chan(chanName.Data());
      hf_chan.SetData(hc.getDataName(chanName).Data(), hfile.Data());

      for (auto sampleName : thisChan.samples()) {
         RF::Sample &thisSample = thisChan.sample(sampleName);
         const TString theName = sampleName + "_" + chanName;

         RooStats::HistFactory::Sample hf_sample(theName.Data(),
                                                 hc.getHistoName(chanName, sampleName, nominal).Data(),
                                                 hfile.Data()
                                                );
         hf_sample.SetNormalizeByTheory(thisSample.normByLumi());

         // variations
         for (auto varName : thisSample.variations()) {
            RF::Variation &thisVar = thisSample.variation(varName);

            hf_sample.AddHistoSys(thisVar.name().Data(),
                                  hc.getHistoName(chanName, sampleName, thisVar.name_down()).Data(),
                                  hfile.Data(),
                                  "",
                                  hc.getHistoName(chanName, sampleName, thisVar.name_up()).Data(),
                                  hfile.Data(),
                                  ""
                                 );

         }

         // multiplicative factors
         for (auto facName : thisSample.factors()) {
            RF::MultiplicativeFactor &thisFactor = thisSample.factor(facName);

            if (thisFactor.constraint() == RF::MultiplicativeFactor::ConstraintType::GAUSSIAN) {
               hf_sample.AddOverallSys(facName.Data(),
                                       thisFactor.min(),
                                       thisFactor.max()
                                      );
            } else if (thisFactor.constraint() == RF::MultiplicativeFactor::ConstraintType::FREE) {
               hf_sample.AddNormFactor(facName.Data(),
                                       thisFactor.value(),
                                       thisFactor.min(),
                                       thisFactor.max(),
                                       kFALSE
                                      );
            } else if (thisFactor.constraint() == RF::MultiplicativeFactor::ConstraintType::CONSTANT) {
               hf_sample.AddNormFactor(facName.Data(),
                                       thisFactor.value(),
                                       thisFactor.min(),
                                       thisFactor.max(),
                                       kTRUE
                                      );
            } else {
               throw std::runtime_error("Constraint type not implemented");
            }
         }

         // replace terms
         // don't allow this if there are shape uncertainties
         const Bool_t isExternalPdf = (thisSample.getFitFunctionPdf() != 0);
         const Bool_t isExternalFactory = (thisSample.getFitFunctionFactoryCommand() != "");

         if (isExternalPdf || isExternalFactory) {
            if (thisSample.variations().size() > 0) {
               throw std::runtime_error("Coexistence of external pdf and shape variations is not implemented");
            }

            // replace the term for this channel
            // it's called e.g. "background_SR_SR_nominal" by HistFactory
            const TString term_to_replace = sampleName + "_" + chanName + "_" + chanName + "_nominal";

            replaceTermsHistFactoryObsName[term_to_replace] = "obs_x_" + chanName; // name from HistoToWorkspaceFactoryFast::MakeSingleChannelWorkspace

            if (isExternalFactory) {
               // build factory
               replaceTermsFactory[term_to_replace] = std::make_pair<TString, TString>(thisSample.getFitFunctionFactoryCommand(), thisSample.getFitFunctionObservable());
            } else if (isExternalPdf) {
               replaceTermsPdf[term_to_replace] = std::make_pair<RooAbsPdf*, TString>(thisSample.getFitFunctionPdf(), thisSample.getFitFunctionObservable());
            }

            // most important thing:
            // we provide a pdf, but HistFactory uses functions (i.e. not normalised to 1 but to expectation, and returning pdf*binWidth instead of pdf value)
            // so we have to:
            //   1) add a multiplicative factor to have the function normalised to histogram area
            //   2) remove the 1/binwidth factor added by HistFactory by default (we provide a pdf)
            //   3) make sure the input pdf is normalized to 1
            // point 3) is dealt with by the factory command issued afterwards
            const TString constFactorName = "renormFactor_" + theName;

            const Double_t nominalNormalisation = hc.getHistogram(chanName, sampleName, nominal)->Integral()  // area
                                                  * hc.getHistogram(chanName, sampleName, nominal)->GetBinWidth(1)  // bin width
                                                  ;

            hf_sample.AddNormFactor(constFactorName.Data(),
                                    nominalNormalisation,
                                    nominalNormalisation,
                                    nominalNormalisation,
                                    kTRUE
                                   );
         } // external pdf

         hf_chan.AddSample(hf_sample);
      }
      hf_meas.AddChannel(hf_chan);
   }

   hf_meas.CollectHistograms();
   hf_meas.PrintTree();
   hf_meas.PrintXML(dest().Data());

   // make the histfactory workspace
   RooWorkspace * histfactoryOutput = RooStats::HistFactory::MakeModelAndMeasurementFast(hf_meas);

   // edit the workspace
   if (replaceTermsFactory.size() != 0 || replaceTermsPdf.size() != 0) {
      std::cout << "BinnedWorkspaceBuilder :: overriding HistFactory standards" << std::endl;
      RooWorkspace::autoImportClassCode(kTRUE);

      RooWorkspace *new_ws = new RooWorkspace("new_ws");

      TString new_nuisance_parameters("");

      // first, factories
      for (auto &kv : replaceTermsFactory) {
         const TString term_to_replace = kv.first;
         const TString command = kv.second.first;
         const TString obs = kv.second.second;
         const TString rename_obs = replaceTermsHistFactoryObsName[term_to_replace];

         std::cout << "BinnedWorkspaceBuilder :: replacing " << term_to_replace << " with " << std::endl;
         std::cout << "BinnedWorkspaceBuilder ::     " << command << std::endl;
         std::cout << "BinnedWorkspaceBuilder :: (observable is " << obs << " and will be called " << rename_obs << ")" << std::endl;

         // trick:
         // import in tmp workspace, so that we don't have to know the name of the factory object
         // then, rename the observable
         RooWorkspace tmp_ws("tmp_ws");
         RooAbsArg *new_object_raw = tmp_ws.factory(command);
         // we have to encapsulate pdf in RooAddPdf to make sure it's normalised to 1 when used in a RooProduct
         RooAbsArg *new_object = tmp_ws.factory(TString::Format("SUM::%s(%s)", term_to_replace.Data(), new_object_raw->GetName()));
         RooArgSet nset(*tmp_ws.var(obs));
         RooAddPdf *addpdf = dynamic_cast<RooAddPdf*>(new_object);
	 std::cout << "DEBAGGO " << new_object << " " << addpdf << std::endl;
         addpdf->fixAddCoefNormalization(nset);

//       new_object->SetName(term_to_replace); // not needed anymore (SUM:: above is already named)
         new_ws->import(*tmp_ws.pdf(term_to_replace), RooFit::RenameVariable(obs, rename_obs));

         // add nuisance parameters to NP list
         // note: a bit involved to make things compile smoothly
         const RooAbsArg &theVar = *new_ws->arg(rename_obs);
         if (!theVar.isConstant()) {
            RooArgSet theObs(theVar);
            const TString pdf_params = new_ws->pdf(term_to_replace)->getParameters(theObs)->contentsString();
            if (new_nuisance_parameters != "") new_nuisance_parameters += ",";
            new_nuisance_parameters += pdf_params;
         }
      }

      // then, RooAbsPdfs (use with care!)
      for (auto &kv : replaceTermsPdf) {
         const TString term_to_replace = kv.first;
         RooAbsPdf *pdf_raw = kv.second.first;
         const TString obs = kv.second.second;
         const TString rename_obs = replaceTermsHistFactoryObsName[term_to_replace];

         std::cout << "BinnedWorkspaceBuilder :: replacing " << term_to_replace << " with " << std::endl;
         std::cout << "BinnedWorkspaceBuilder ::     " << pdf_raw->GetName() << std::endl;
         std::cout << "BinnedWorkspaceBuilder :: (observable is " << obs << " and will be called " << rename_obs << ")" << std::endl;

         // we have to encapsulate pdf in RooAddPdf to make sure it's normalised to 1 when used in a RooProduct
         RooWorkspace tmp_ws("tmp_ws");
         tmp_ws.import(*pdf_raw);
         RooAbsArg *pdf = tmp_ws.factory(TString::Format("SUM::(%s)", pdf_raw->GetName()));
         pdf->SetName(term_to_replace);
         RooAddPdf *addpdf = dynamic_cast<RooAddPdf*>(pdf);
         RooArgSet nset(*tmp_ws.var(obs));
         addpdf->fixAddCoefNormalization(nset);

         new_ws->import(*tmp_ws.pdf(term_to_replace), RooFit::RenameVariable(obs, rename_obs));

         // add nuisance parameters to NP list
         // note: a bit involved to make things compile smoothly
         // note: using "obs" instead of "rename_obs" as we start from original (pre-import) pdf,
         //       should not make any difference
         const RooAbsArg &theVar = *new_ws->arg(rename_obs);
         if (!theVar.isConstant()) {
            RooArgSet theObs(theVar);
            const TString pdf_params = new_ws->pdf(term_to_replace)->getParameters(theObs)->contentsString();
            if (new_nuisance_parameters != "") new_nuisance_parameters += ",";
            new_nuisance_parameters += pdf_params;
         }
      }



      // eventually, combine workspaces
      WorkspaceEditor::CopyWorkspaceContents(histfactoryOutput, new_ws);

      // copy binning (very important!) and set normalisation factor
      // to take into account the normalisation convention of RooHistFunc in HistFactory
      // (i.e. the fact it's normalised to expectation)
      for (auto &kv : replaceTermsHistFactoryObsName) {
         const TString theNewPdf = kv.first;
         const TString theObs = kv.second;

         // first, set binning
         const Int_t nbins = histfactoryOutput->var(theObs)->getBinning().numBins();

         new_ws->var(theObs)->setBins(nbins);
      }

      // remove the histfactory workspace, and rename the new one
      std::string workspaceName = histfactoryOutput->GetName();
      delete histfactoryOutput;
      new_ws->SetName(workspaceName.c_str());

      // access the modelconfig, to update NP sets
      RooStats::ModelConfig * outputModel = dynamic_cast<RooStats::ModelConfig*>(new_ws->obj(modelConfigName()));

      // update the model config with nuisance parameters associated to the additional function
      const TString nuisSetName = "ModelConfig_NuisParams";
      const RooArgSet *nuisSet = outputModel->GetNuisanceParameters();

      // sanity check for our assumptions
      if (nuisSet != new_ws->set(nuisSetName)) throw std::logic_error("link to nuisance parameters set not valid");

      std::cout << "BinnedWorkspaceBuilder :: original set of nuisance parameters in ModelConfig: " << std::endl;
      nuisSet->Print();

      // extend
      new_ws->extendSet(nuisSetName, new_nuisance_parameters);

      std::cout << "BinnedWorkspaceBuilder :: new set of nuisance parameters in ModelConfig: " << std::endl;
      nuisSet->Print();

      setWS(new_ws);
   } // edit workspace
   else {
      //Use the histfactory output directly
      setWS(histfactoryOutput);
   }
}

const char* RF::BinnedWorkspaceBuilder::wsName()
{
   return "combined";
}

const char* RF::BinnedWorkspaceBuilder::modelConfigName()
{
   return "ModelConfig";
}
