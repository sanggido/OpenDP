
sta::define_cmd_args "legalize_placement" {[-constraints constraints_file]}

proc legalize_placement { args } {
  sta::parse_key_args "legalize_placement" args \
    keys {-constraints} flags {}

  if { [info exists keys(-constraints)] } {
    set constraints_file $keys(-constraints)
    if { [file readable $constraints_file] } {
      $odp read_constraints $constraint_file
    } else {
      puts "Warning: cannot read $constraints_file"
    }
  }
  if { [db_has_rows] } {
    set odp [get_opendp]
    $odp legalize_place
  } else {
    puts "Error: no rows defined in design. Use initialize_floorplan to add rows."
  }
}
