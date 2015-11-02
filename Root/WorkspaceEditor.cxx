#include "ResonanceFinder/WorkspaceEditor.h"

#include <RooArgSet.h>
#include <RooAbsArg.h>
#include <RooAbsData.h>
#include <RooStats/ModelConfig.h>

#include <vector>
#include <string>
#include <sstream>

using namespace RooFit;

//Copy all iterable components from one workspace to another
//This will preserve existing components in the target workspace
//Probably the neatest way to replace a workspace component: put it in the target first
void WorkspaceEditor::CopyWorkspaceContents(RooWorkspace * InputWorkspace, RooWorkspace * OutputWorkspace)
{
   std::cout << "WorkspaceEditor :: copying " << InputWorkspace << " (" << InputWorkspace->GetName() << ") into "
             << OutputWorkspace << " (" << OutputWorkspace->GetName() << ")" << std::endl;

   //Copy functions, variables and sets to output workspace
   std::cout << "WorkspaceEditor :: importing functions, variables and sets" << std::endl;
   TIterator * inputIterator = InputWorkspace->componentIterator();
   while (TObject * inputObject = inputIterator->Next()) {
      if (inputObject->IsA()->InheritsFrom("RooAbsArg")) {
         OutputWorkspace->import(*(RooAbsArg*)inputObject, RecycleConflictNodes());//, Silence());
      } else if (inputObject->IsA()->InheritsFrom("RooArgSet")) {
         OutputWorkspace->import(*(RooArgSet*)inputObject, RecycleConflictNodes());//, Silence());
      } else {
         std::stringstream errorString;
         errorString << "Unexpected object in input workspace: " << inputObject->GetName() << " (" << inputObject->Class() << ")";
         throw std::runtime_error(errorString.str());
      }
   }

   //Copy data objects to output workspace
   std::cout << "WorkspaceEditor :: importing datasets" << std::endl;
   for (RooAbsData * inputData : InputWorkspace->allData()) {
      OutputWorkspace->import(*inputData);
   }


   //Copy generic objects to output workspace
   std::cout << "WorkspaceEditor :: importing generic objects" << std::endl;
   for (TObject * inputObject : InputWorkspace->allGenericObjects()) {
      // try ModelConfig copy (see RF::WorkspaceCollection::addWorkspace for rationale)
      RooStats::ModelConfig *mc = dynamic_cast<RooStats::ModelConfig*>(inputObject);
      if (mc) {
         const TString mcName = mc->GetName();

         std::cout << "WorkspaceEditor :: ModelConfig named " << mcName << " found!" << std::endl;
         std::cout << "WorkspaceEditor :: original content:" << std::endl;
         mc->Print();

         // create empty ModelConfig linked to the output workspace
         RooStats::ModelConfig tmp(mcName, OutputWorkspace);

         // import it, replacing the original one if any
         OutputWorkspace->import(tmp, kTRUE);

         // retrieve a pointer to the new ModelConfig
         RooStats::ModelConfig *hook = dynamic_cast<RooStats::ModelConfig*>(OutputWorkspace->obj(mcName));

         // fill manually this ModelConfig
         // note: this should also create associated NP sets in the workspace
         if (mc->GetConditionalObservables()) hook->SetConditionalObservables(*mc->GetConditionalObservables());
         if (mc->GetConstraintParameters()) hook->SetConstraintParameters(*mc->GetConstraintParameters());
         if (mc->GetGlobalObservables()) hook->SetGlobalObservables(*mc->GetGlobalObservables());
         if (mc->GetNuisanceParameters()) hook->SetNuisanceParameters(*mc->GetNuisanceParameters());
         if (mc->GetObservables()) hook->SetObservables(*mc->GetObservables());
         if (mc->GetParametersOfInterest()) hook->SetParametersOfInterest(*mc->GetParametersOfInterest());
         if (mc->GetPdf()) hook->SetPdf(*mc->GetPdf());
         if (mc->GetPriorPdf()) hook->SetPriorPdf(*mc->GetPriorPdf());
         if (mc->GetProtoData()) hook->SetProtoData(*mc->GetProtoData());
         if (mc->GetSnapshot()) hook->SetSnapshot(*mc->GetSnapshot());
         if (hook->GetWS() != OutputWorkspace) throw std::logic_error("Fix to ModelConfig copying does not work");

         std::cout << "WorkspaceEditor :: content of copied object:" << std::endl;
         hook->Print();
      } else {
         // simply copy the object
         OutputWorkspace->import(*inputObject, false);
      }
   }

   //Check through a list of common named sets to copy
   //The collection of named sets is not iterable, so we have to guess
   std::cout << "WorkspaceEditor :: importing a list of common named sets" << std::endl;
   std::vector< std::string > commonSetNames = { "ModelConfig_GlobalObservables", "ModelConfig_Observables",
                                                 "ModelConfig_NuisParams", "ModelConfig_POI", "globalObservables", "observables",
                                                 "coefList", "constraintTerms", "likelihoodTerms", "obsAndWeight", "observablesSet", "shapeList"
                                               };
   for (std::string setName : commonSetNames) {
      //Try a common set name
      RooArgSet const* inputSet = InputWorkspace->set(setName.c_str());

      //If the set exists, copy it
      if (inputSet) {
         std::cout << "WorkspaceEditor ::        -> " << setName << std::endl;
         OutputWorkspace->defineSet(setName.c_str(), *inputSet);
      }
   }



   std::cout << "WorkspaceEditor :: content of original workspace:" << std::endl;
   InputWorkspace->Print();
   std::cout << "WorkspaceEditor :: content of new workspace:" << std::endl;
   OutputWorkspace->Print();

   return;
}
