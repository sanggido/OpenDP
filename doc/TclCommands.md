# Usage with TCL Interpreter

OpenDP has internal TCL Interpreter. The following line will create opendb_external objects.

    opendp_external odp 
    
After having a opendp_external object, a user can type any TCL commands after one spacing from the object name(e.g. rep).

    odp [tcl_command]


## File I/O Commands
* __import_lef__ [file_name] : \*.lef location (Multiple lef files supported. __Technology LEF must be ahead of other LEFs.__)
* __import_def__ [file_name] : \*.def location (Required due to FloorPlan information)
* __export_def__ [file_name] : Output DEF location
   
## Flow Control
* __init_opendp__ : Initialize OpenDP's structure based on LEF and DEF.
* __legalize_place__ : Legalize placed cells.
* __check_legality__ : Report overlaps between each cell.


## Query results
__Note that the following commands will work after init_opendp and legalize_place commands__
* __get_utilization__ : Returns utilizations. [float]
* __get_sum_displacement__ : Returns sum of displacement between given and legalized design [float]
* __get_average_displacement__ : Returns average of displacement between given and legalized design [float]
* __get_max_displacement__ : Print max of displacement between given and legalized design [float]
* __get_original_hpwl__ : Returns HPWL from the given design. [float]
* __get_legalized_hpwl__ : Returns HPWL after the legalized design. [float]

## Example TCL scripts
* [run_nangate45_gcd.tcl](../test/run_nangate45_gcd.tcl)

FYI, All of the TCL commands are defined in the [opendp_external.h](../src/opendp_external.h) header files.
