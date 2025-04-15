source /cvmfs/dune.opensciencegrid.org/products/dune/setup_dune.sh
setup larsoft v10_04_04 -q e26:prof
setup dunesw v10_04_04d01 -q e26:prof
setup dunesim v10_04_04d01 -q e26:prof
cd /exp/dune/app/users/mking/maira_area_03052025_1/
source localProducts*/setup
mrbslp
mrbsetenv
export WIRECELL_PATH=/cvmfs/dune.opensciencegrid.org/products/dune/dunereco/v10_04_04d01/wire-cell-cfg:$WIRECELL_PATH

