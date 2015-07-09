#!/bin/bash

export ROOTSYS=/soft/root/5.34.14
export PXAR_PATH=/soft/pxar/main.source
export PSI46TEST=/soft/psi46test/git
export PSI46EXPERT=/soft/psi46expert/107.bare
export PIXEL_TEST_TOOLS_PATH=/soft/PixelTestTools

export LD_LIBRARY_PATH=/usr/local/lib:/usr/lib:$ROOTSYS/lib:$PSI46EXPERT/lib
export PATH=/usr/local/bin:/usr/bin:/bin:$ROOTSYS/bin:$PXAR_PATH/bin:$PSI46TEST/bin:$PSI46EXPERT/bin:$PIXEL_TEST_TOOLS_PATH/bin
export PYTHONPATH=$ROOTSYS/lib
export CPATH=$ROOTSYS/include
export LIBRARY_PATH=$ROOTSYS/lib
