# Usage
    $ ./opendp -lef <*.lef> -lef <*.lef> ... -def <*.def> [Options]

* __-lef__ : \*.lef Location (Multiple lef files supported. __Technology LEF must be ahead of other LEFs.__)
* __-def__ : \*.def Location (Input def file for Detail Placer.)

## Options
* __-output_def <*.def>__ : determine output def location, Default = ./"input_def"_lg.def
* __-group_ignore__    : OpenDP does not consider group region for detailed placement
* __-cpu__  : cpu number for processing < default = 1 , currently not support >
* __-placement_constraints <*.constraints>__ : read constraint file ( format is same as iccad 2017 contest )
