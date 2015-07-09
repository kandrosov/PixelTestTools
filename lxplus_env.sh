export BOOST_BASE=$( cd $CMSSW_BASE; scram tool info boost | grep BOOST_BASE | sed s/BOOST_BASE=// )
export BOOST_INCLUDE_PATH=$BOOST_BASE/include
export CPATH=$ROOTSYS/include:$BOOST_INCLUDE_PATH
export LIBRARY_PATH=$ROOTSYS/lib:$BOOST_BASE/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$BOOST_BASE/lib

