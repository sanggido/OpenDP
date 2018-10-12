# Usage
## Single LEF
    $ ./OpenDP -lef <*.lef> -input_def <*.def> [Options]

    * __-lef__ : \*.lef Location
    * __-def__ : \*.def Location

## dual LEF
    $ ./OpenDP -tech_lef <*.lef> -cell_lef <*.lef> -input_def <*.def> [Options]

    * __-tech_lef__ : \*.lef Location
    * __-cell_lef__ : \*.lef Location
    * __-def__ : \*.def Location

## Options
* __-placement_constraints <*.constraints>__ : read constraint file ( format is same as iccad 2017 contest )
* __-output_def <*.def>__ : determine output def location, Default = ./"input_def"_lg.def
* __-group_ignore__    : OpenDP does not consider group region for detailed placement
* __-cpu__  : cpu number for processing < default = 1 , currently not support >
