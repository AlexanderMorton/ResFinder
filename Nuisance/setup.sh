# lxplus only
# SLC5
#source /afs/cern.ch/sw/lcg/contrib/gcc/4.3/x86_64-slc5-gcc43-opt/setup.sh
# SLC6
#source /afs/cern.ch/sw/lcg/contrib/gcc/4.6/x86_64-slc6-gcc46-opt/setup.sh

# ROOT and Python setup (CHECK ROOT VERSION HERE)
#export PYTHONDIR=/afs/cern.ch/sw/lcg/external/Python/2.6.2/x86_64-slc5-gcc43-opt
export PYTHONDIR=/afs/cern.ch/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc46-opt
export LD_LIBRARY_PATH=$PYTHONDIR/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$PYTHONDIR/lib:$LD_LIBRARY_PATH
export PATH=$PYTHONDIR/bin:$PATH
export PYTHONPATH=$PYTHONDIR/lib:$PYTHONPATH

# BOOST
export BOOSTDIR=/afs/cern.ch/sw/lcg/external/Boost/1.47.0_python2.6/x86_64-slc5-gcc43-opt
export BOOSTFLAGS=$BOOSTDIR/include/boost-1_47
export BOOSTLIB=$BOOSTDIR/lib

