# Quick Start Guide

Welcome to the Linux driver for X-Bows programming.

Here's a quick way to get started.  This should work on Ubuntu 18.04 but may have to
be adapted for other Linux flavors.


0. Install requirements

The code is C++11, so the compiler should be gcc 6 or newer.  It will likely
work with 4.9 or newer but you'll have to set the std=c++11 flag.

```sh
sudo apt-get install cmake g++ libyaml-cpp-dev
sudo apt-get install autotools-dev autoconf automake libtool
sudo apt-get install libudev-dev libusb-1.0-0-dev
sudo apt-get install libboost-all-dev
```

1. Obtain the source code

```sh
git clone https://github.com/jlquinn/xbows-driver.git
```

2. Build the driver

```sh
cd xbows-driver
git submodule update --init
mkdir build
cd build
cmake ..
make -j
cd ..
```

3. Run the driver program

```sh
build/src/kbdtest2 share/test-driver-layer.yaml
```
