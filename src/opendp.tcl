
sta::define_cmd_args "legalize_placement" {[-constraints constraints_file]}

proc legalize_placement { args } {
  sta::parse_key_args "legalize_placement" args \
    keys {-constraints} flags {}

  set odp [get_opendp]
  if { [db_has_rows] } {
    if { [info exists keys(-constraints)] } {
      set constraints_file $keys(-constraints)
      if { [file readable $constraints_file] } {
	$odp read_constraints $constraint_file
      }
    }
  } else {
    puts "ERROR: no rows defined in design. Use initialize_floorplan to add rows."
  }

  $odp legalize_place
}
