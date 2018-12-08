== Compile Analyzer ==

To compile the "Ana" SLAC Test 2018 quartz + GEM detector analyzer:
```git clone git@github.com:sbujlab/slac-test-ana
cd slac-test-ana
cd analyzer
make```

== Initialize Environment ==

Open your .bashrc and add these lines
```export ANALYZER=~/path-to-analyzer/slac-test-ana/analyzer
export PATH=$ANALYZER:$PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ANALYZER```
Then source your modified .bashrc

== Compile Decoders ==

From the top level of the repository
```cd gems
cd SBS-Offline
make
cd ..
ln -s SBS-Offline/libsbs.so```

== Running the Analyzer ==

Execute ```./gemana.sh``` and it will prompt you for a run number to analyze the data