#include "circuit.h"
      
opendp::circuit::circuit() 
: GROUP_IGNORE(false),
        num_fixed_nodes(0),
        num_cpu(1),
        DEFVersion(""),
        DEFDelimiter("/"),
        DEFBusCharacters("[]"),
        design_name(""),
        DEFdist2Microns(0),
        sum_displacement(0.0),
        displacement(400.0),
        max_disp_const(0.0),
        max_utilization(100.0),
        wsite(0),
        max_cell_height(1),
        rowHeight(0.0f), 
        fileOut(0) {

    macros.reserve(128);
    layers.reserve(32);
    rows.reserve(4096);
    sub_regions.reserve(100);

#ifdef USE_GOOGLE_HASH
    macro2id.set_empty_key(
        INITSTR); /* OPENDP_HASH_MAP between macro name and ID */
    cell2id.set_empty_key(
        INITSTR); /* OPENDP_HASH_MAP between cell  name and ID */
    pin2id.set_empty_key(
        INITSTR); /* OPENDP_HASH_MAP between pin   name and ID */
    net2id.set_empty_key(
        INITSTR); /* OPENDP_HASH_MAP between net   name and ID */
    row2id.set_empty_key(
        INITSTR); /* OPENDP_HASH_MAP between row   name and ID */
    site2id.set_empty_key(
        INITSTR); /* OPENDP_HASH_MAP between site  name and ID */
    layer2id.set_empty_key(
        INITSTR); /* OPENDP_HASH_MAP between layer name and ID */
    via2id.set_empty_key(INITSTR);
    group2id.set_empty_key(INITSTR); /* group between name -> index */
#endif
};
