// Author: Daniel Buescher, using copyFiles.C from Rene Brun

// Usage: %s source_file target_dir

// Script to split a root file (containing histograms) into several files,
// based on the first layer of sub directories in the input file.
// The target directory is created if not existing.

#include <string>

#include "TROOT.h"
#include <TFile.h>
#include <TError.h>
#include "TKey.h"
#include "TTree.h"

//#include <../tutorials/io/copyFiles.C>

void CopyDir(TDirectory *source) {
  // copy all objects and subdirs of directory source to the current directory
  TDirectory *adir = gDirectory;
  //loop on all entries of this directory
  TKey *key;
  TIter nextkey(source->GetListOfKeys());
  while ((key = (TKey*) nextkey())) {
    const char *classname = key->GetClassName();
    TClass *cl = gROOT->GetClass(classname);
    if (!cl) continue;
    if (cl->InheritsFrom(TDirectory::Class())) {
      source->cd(key->GetName());
      TDirectory *subdir = gDirectory;
      adir->mkdir(key->GetName())->cd();
      CopyDir(subdir);
      adir->cd();
    } else if (cl->InheritsFrom(TTree::Class())) {
      TTree *T = (TTree*) source->Get(key->GetName());
      adir->cd();
      TTree *newT = T->CloneTree(-1, "fast");
      newT->Write();
    } else {
      source->cd();
      TObject *obj = key->ReadObj();
      adir->cd();
      obj->Write();
      delete obj;
    }
  }
  adir->SaveSelf(kTRUE);
  adir->cd();
}

int main(int argc, char **argv) {

  const char* APP_NAME = argv[0];
  if (argc != 3) {
    Error(APP_NAME, "Usage: %s source_file target_dir", APP_NAME);
    return 1;
  }

  const char* inFileName = argv[1];
  const char* targetDir = argv[2];

  TFile* inFile = TFile::Open(inFileName, "READ");
  if (!inFile) {
    //Error(APP_NAME, "Couldn't open file '%s'", inFileName);
    return EXIT_FAILURE;
  }
  Info(APP_NAME, "Opened source file '%s'", inFileName);

  std::string command = "mkdir -vp ";
  command += targetDir;
  if (system(command.c_str())) {
    //Error(APP_NAME, "Couldn't create target directory '%s'.", targetDir);
    return EXIT_FAILURE;
  }
  Info(APP_NAME, "Using target directory '%s'", targetDir);

  // loop over keys in source file,
  // create output file for each TDirectory,
  // copy content recursively
  TKey *key;
  TIter nextkey(inFile->GetListOfKeys());
  while ((key = (TKey*) nextkey())) {
    const char *classname = key->GetClassName();
    TClass *cl = gROOT->GetClass(classname);
    if (!cl) continue;
    if (cl->InheritsFrom(TDirectory::Class())) {
      TString dirName = key->GetName();
      TString outFileName = targetDir + ("/" + dirName + ".root");
      TDirectory* subDir = (TDirectory*) inFile -> Get(dirName);
      TFile* outFile = TFile::Open(outFileName, "RECREATE");
      if (!outFile) {
        //Error(APP_NAME, "Couldn't open file '%s'", outFileName.Data());
        return EXIT_FAILURE;
      }
      Info(APP_NAME, "Opened target file '%s'", outFileName.Data());
      outFile -> cd();
      CopyDir(subDir);
      outFile -> Close();
      delete outFile;
    }
    // TODO: copy objects from the base dir?
  }
  Info(APP_NAME, "Done.");
}

