# Internal TCL commands

    set opb [get_opendp]
    
After having a opendp_external object, a user can type any TCL commands after one spacing from the object name(e.g. rep).

    $odp [tcl_command]

## Flow Control
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

## Example TCL script
* [run_nangate45_gcd.tcl](../test/run_nangate45_gcd.tcl)
