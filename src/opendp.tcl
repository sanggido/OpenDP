
sta::define_cmd_args "legalize_placement" {[-constraints constraints_file]}

proc legalize_placement { args } {
  sta::parse_key_args "legalize_placement" args \
    keys {-constraints} flags {}

  set odp [get_opendp]

  if { [info exists keys(-constraints)] } {
    set constraints_file $keys(-constraints)
    if { [file readable $constraints_file] } {
      $odp read_constraints $constraint_file
    }
  }

  $odp legalize_place
}
