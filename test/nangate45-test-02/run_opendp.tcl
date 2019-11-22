set exp_folder ./exp
set design aes_cipher_top 

set TIME_start [clock clicks -milliseconds]

read_lef ../nangate45-bench/tech/NangateOpenCellLibrary.lef
read_def ../nangate45-bench/${design}/${design}_replace.def

opendp_external odp
odp legalize_place

set TIME_taken [expr [clock clicks -milliseconds] - $TIME_start]

if {![file exists $exp_folder/]} {
  exec mkdir $exp_folder
}

set fp [open ./exp/${design}.rpt w]
set legality [odp check_legality]

puts $fp "Legality          : $legality"
puts $fp "Runtime           : $TIME_taken"
puts $fp "Sum displacement: : [odp get_sum_displacement]"
puts $fp "Avg displacement: : [odp get_average_displacement]"
puts $fp "Max displacement: : [odp get_max_displacement]"
puts $fp "Original HPWL     : [odp get_original_hpwl]"
puts $fp "Legalized HPWL    : [odp get_legalized_hpwl]"

close $fp


