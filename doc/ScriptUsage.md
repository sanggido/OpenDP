# OpenDP script Usage

    execute.py is excution script for given iccad2017 benchmarks.
    * [execute.py](../src/execute.py)

## Configure options to execute scripts
    You can easily find this part inside of the python scripts.

    * __dirpos__ : (__string__) Benchmark directory position
    * __binaryName__ : (__string__) Compiled OpenDP binary name
    * __outpos__ : (__string__) Set a output folder location - The binary saves 'legalized def' files
    * __logpos__ : (__string__) Set a log folder location - The binary saves standard-output log like des_perf_b_md2_2018-10-6_20:34:24.log
    * __numThread__ : (__int__) Define the number of threads OpenDP can use

## Feed OpenDP with LEF/DEF -- without script

    For example, if you have LEF/DEF benchmarks as (dual LEF)

    ../bench/aes_cipher_top/floorplan.def
    ../bench/aes_cipher_top/tech.lef
    ../bench/aes_cipher_top/cell.lef

    ./OpenDP -tech_lef ../bench/aes_cipher_top/tech.lef -cell_lef ../bench/aes_cipher_top/cell.lef -input_def ../bench/aes_cipher_top/floorplan.def

    or if you have LEF/DEF benchmakrs as (single LEF)

    ../bench/pci_bridge32/floorplan.def
    ../bench/pci_bridge32/tech.lef

    ./OpenDP -lef ../bench/pci_bridge32/tech.lef -input_def ../bench/pci_bridge32/floorplan.def

## Notice

    1. Currently OpenDP does not support multi thread processing.

