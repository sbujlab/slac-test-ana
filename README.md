# SLAC Test 2018 Analysis Package

## Compile Analyzer

To compile the "Ana" SLAC Test 2018 quartz + GEM detector analyzer:
```
git clone git@github.com:sbujlab/slac-test-ana
cd slac-test-ana
mkdir CODA

Open .bashrc and add these lines
```
export CODA=~/path-to-analyzer/slac-test-ana/CODA
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/path-to-analyzer/evio/include
```

cd evio
scons install
(may need to install scons)

Open .bashrc and add the line
```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/path-to-analyzer/CODA/Linux-x86_64/lib
```

cd analyzer
make
```

## Initialize Environment

Open your .bashrc and add these lines
```
export ANALYZER=~/path-to-analyzer/slac-test-ana/analyzer
export PATH=$ANALYZER:$PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ANALYZER
```
Then source your modified .bashrc

## Compile Decoders

From the top level of the repository clone the SBS-offline repository from the sbujlab organization and checkout the "slac-test-2018" branch, then
```
cd SBS-Offline
make
cd ..
cd gems
ln -s ../SBS-Offline/libsbs.so
```

## Running the Analyzer

Execute ```./gemana.sh``` and it will prompt you for a run number to analyze the data
