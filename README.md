# OpenDP
*OpenDP: Open Source Detailed Placement Engine*

| <img src="/doc/image/OpenDP_sample.jpg" width=900px> | 
|:--:| 
| *Visualized detailed placement result using aes_cipher_top design with innovus initial placement; OpenDP_sample.jpg* |

### Verified/supported Technologies
* TSMC 65 (GP/LP)
* ST FDSOI 28
* TSMC 16 (9T)

Legalize a design that has been globally placed.

```
legalize_placement [-constraints constraints_file]
```

### Manual
* [doc/TclCommands.md](doc/TclCommands.md)

### License
* BSD-3-clause License [[Link]](LICENSE)

### Authors
- SangGi Do and Mingyu Woo (respective Ph. D. advisors: Seokhyeong Kang, Andrew B. Kahng).
- parser_helper.cpp is based on Dr. Myung-Chul Kim(IBM)'s ICCAD 2015 contest starter_kit
- Paper reference: S. Do, M. Woo and S. Kang, "Fence-Region-Aware Mixed-Height Standard Cell Legalization", Proc. Great Lakes Symposium on VLSI, 2019, pp. 259-262. [(link)](https://dl.acm.org/citation.cfm?id=3318012)

### Features
- Fence region and multi-height legalization supports. (ICCAD 2017 Contest benchmarks)
- Fragmented ROW supports.
- Macro orientations supports.
