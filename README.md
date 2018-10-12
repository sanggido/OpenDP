# OpenDP
*OpenDP: Open Source Detailed Placement Engine*

| <img src="/doc/image/OpenDP_sample.jpg" width=900px> | 
|:--:| 
| *Visualized detailed placement result using aes_cipher_top design with innovus initial placement; OpenDP_sample.jpg* |
### Required
* GCC compiler and libstdc++ static library >= 5.4.0
* Recommended OS: Centos6 or Centos7 or Ubuntu 16.04

### How To Compile
$ git clone --recursive https://github.com/sanggido/OpenDP.git

    $ cd ~/OpenDP
    $ make clean
    $ make 

### How To Execute (using script)
    // unzip iccad2017 benchmarks
    $ cd ~/OpenDP/bench
    $ tar -xvf benchmarks.tar.gz

    // Check doc/ScriptUsage.md in detail
    $ cd ~/OpenDP/src
    $ ./execute.py 1

### How To Execute (using binary)
    // Check doc/ScriptUsage.md in detail
    $ cd ~/OpenDP/src
    $ ./OpenDP -lef <lefile> -input_def <defile> [-options]

### Manual
* [doc/ScriptUsage.md](doc/ScriptUsage.md)
* [doc/BinaryArguments.md](doc/BinaryArguments.md)

### License
* BSD-3-clause License [[Link]](LICENSE)

### 3rd Party Module List
* Google Dense Hash Map

### Authors
- SangGi Do and Mingyu Woo (respective Ph. D. advisors: Seokhyeong Kang, Andrew B. Kahng).
- parser_helper.cpp is based on Dr. Myung-Chul Kim(IBM)'s ICCAD 2015 contest starter_kit
- Paper reference: S. Do, M. Woo and S. Kang, "Fence-Region Aware Mixed-Height Standard Cell Legalization", to appear in International SoC Design Conference, 2018
