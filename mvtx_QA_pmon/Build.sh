echo "-------------- CHANGING SOFTWARE NOW BRO --------------------"

#source /opt/sphenix/core/bin/sphenix_setup.csh -n ana.241
#source /cvmfs/sphenix.sdcc.bnl.gov/x8664_sl7/opt/sphenix/core/bin/sphenix_setup.csh -n ana.221
#source /opt/sphenix/core/bin/sphenix_setup.csh -n ana.200

source /opt/sphenix/core/bin/sphenix_setup.csh -n 

#source /cvmfs/sphenix.sdcc.bnl.gov/x8664_sl7/opt/sphenix/core/bin/sphenix_setup.csh -n ana.260

rm -r install

mkdir install
setenv MYINSTALL $PWD/install/
setenv LD_LIBRARY_PATH $MYINSTALL/lib:$LD_LIBRARY_PATH
set path = ( $MYINSTALL/bin $path )

#source $OPT_SPHENIX/bin/setup_local.csh $MYINSTALL
echo "Build trackbase_historic"



#cd coresoftware/simulation/g4simulation/g4detectors/
autogen.sh --prefix=$MYINSTALL
make
make install


#echo "-------------- DONE CHANGING SOFTWARE NOW BRO --------------------"
