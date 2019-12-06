This is the final project of the academic course "18737 Engineering Safe Software Systems" 2019 Fall in 
Carnegie Mellon University Sillicon Valley. We apply the testing tools KLEE & AFL to wpa_cli, a open-source 
command-line interface communicates with Wi-Fi management tool wpa_supplicant. 
Please refer to http://w1.fi/cgit/hostap/ for more details about the wpa_cli/wpa_supplicant.


# 1) Environment Setup
## KLEE

These steps are tested under 18.04.1-Ubuntu.
Please refer to KLEE website https://klee.github.io/build-llvm60 for more detail.

1. **Install dependencies** 

<pre>
  "sudo apt-get install \
    build-essential curl libcap-dev git cmake libncurses5-dev \
    python-minimal python-pip unzip libtcmalloc-minimal4 \
    libgoogle-perftools-dev libsqlite3-dev doxygen"
  "pip3 install tabulate"
  "sudo apt-get install clang-6.0 llvm-6.0 llvm-6.0-dev llvm-6.0-tools"
  "sudo apt-get install cmake bison flex libboost-all-dev python perl minisat"
</pre>

2. **Build STP as the constraint solver**

<pre>
  "git clone https://github.com/stp/stp"
  "cd stp"
  "mkdir build"
  "cd build"
  "cmake .."
  "make -j8"
  "sudo make install"
</pre>

3. **Build uClibc and the POSIX environment model for KLEE**

<pre>
  "git clone https://github.com/klee/klee-uclibc.git"
  "cd klee-uclibc"  
  "./configure --make-llvm-lib"
  "make -j8"
</pre>

4. **Build KLEE source code**

<pre>
  "git clone https://github.com/klee/klee.git"
  "cd klee"
  "mkdir build"
  "cd build"
  "cmake \
      -DENABLE_SOLVER_STP=ON \
      -DENABLE_POSIX_RUNTIME=ON \
      -DENABLE_KLEE_UCLIBC=ON \
      -DKLEE_UCLIBC_PATH=/home/shaomin/Desktop/klee-uclibc \
      -DENABLE_UNIT_TESTS=OFF \
      -DENABLE_SYSTEM_TESTS=OFF \
      -DLLVM_CONFIG_BINARY=/usr/bin/llvm-config-6.0 \
      -DLLVMCC=/usr/bin/clang-6.0 \
      -DLLVMCXX=/usr/bin/clang++-6.0  \
      /home/shaomin/Desktop/klee"
   "make -j8"
   "cp ./bin/* /usr/bin"
   "cp -r ./../include/klee /usr/include"
</pre>

## AFL
1. **Install afl and afl-cov**

<pre>
  "sudo apt-get install afl afl-cov"
</pre>

# Run testing

All added testing codes and scripts locate at wpa_supplicant/


************************************************
Run klee.
************************************************
Generally speaking, we don't want to mix normal codes and test codes in the same file. To run klee test, I create a 
wrapper at "wpa_supplicant/klee" used to connect wpa_cli.c and klee instead of changing the original file. 

The klee generates the symbolic arguments for the wpa_cli. The number and the format of arguments are defined in 
"wpa_supplicant/klee/buidl.sh". The wrapper only accepts the readable arguments.


Step 1. Modify variable KLEE_SYMBOLIC_ENVIRONMENT in "wpa_supplicant/klee/buidl.sh" if needed.

Step 2. Run "wpa_supplicant/klee/buidl.sh"

Step 3. Analyze the results
   "klee-stats klee-last"
   "kcachegrind klee-last/run.istats"


************************************************
Environment Setup for klee
************************************************
Step 1. Install afl and afl-cov
  "sudo apt-get install afl afl-cov"
  

************************************************
Run afl.
************************************************
Step 1. Modify corpus file in "wpa_supplicant/afl/iput" if needed.

Step 2. Use afl-gcc to re-build wpa_cli
  "wpa_supplicant/afl/build.sh wpa_cli"

Step 3. Run "wpa_supplicant/afl/buidl.sh"
  "wpa_supplicant/afl/build.sh"
  
  afl may require you to modify system configurations. Use ignore arguments to bypass it.
  "wpa_supplicant/afl/build.sh ignore"
 
  
