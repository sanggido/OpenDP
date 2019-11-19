set exp_folder ./exp
set design gcd 

set TIME_start [clock clicks -milliseconds]

opendp_external odp
odp import_lef nangate45-bench/tech/NangateOpenCellLibrary.lef
odp import_def nangate45-bench/${design}/${design}_replace.def

odp init_opendp
odp legalize_place

odp export_def ./${design}_legalized.def

set TIME_taken [expr [clock clicks -milliseconds] - $TIME_start]

if {![file exists $exp_folder/]} {
  exec mkdir $exp_folder
}

puts "Legality          : $legality"
puts "Runtime           : $TIME_taken"
puts "Sum displacement: : [odp get_sum_displacement]"
puts "Avg displacement: : [odp get_average_displacement]"
puts "Max displacement: : [odp get_max_displacement]"
puts "Original HPWL     : [odp get_original_hpwl]"
puts "Legalized HPWL    : [odp get_legalized_hpwl]"

