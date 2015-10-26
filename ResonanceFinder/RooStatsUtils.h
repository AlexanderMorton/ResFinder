#ifndef __RF__RooStatsUtils_h__
#define __RF__RooStatsUtils_h__

namespace RF {
   typedef enum {
      Frequentist = 0,
      Hybrid = 1,
      Asymptotic = 2,
      AsymptoticWithNominalAsimov = 3
   } CalculatorType;

   typedef enum {
      LEP = 0,
      Tevatron = 1,
      PL2sided = 2,
      PL1sided = 3,
      PLsigned = 4,
      MLE = 5,
      Nobs = 6,
      ROPL = 11 // ???
   } TestStatType;
}

#endif
