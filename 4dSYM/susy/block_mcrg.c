// -----------------------------------------------------------------
// Blocking function for MCRG
// Just multiplies together "neighboring" links separated by 2^block sites
// Saves all 2^d products in sub-lattices of original gauge configuration
// Overwrite links, use site->mom for temporary storage
#include "susy_includes.h"

void block_mcrg(int block) {
  register int dir, i;
  register site *s;
  int disp[4], j, k;
  msg_tag *tag;

  // Multiply pre-set offsets by 2^block
  for (dir = XUP; dir < NUMLINK; dir++) {
    for (j = 0; j < NDIMS; j++) {
      disp[j] = offset[dir][j];
      for (k = 1; k < block; k++)
        disp[j] *= 2;
    }

    tag = start_general_gather_site(F_OFFSET(linkf[dir]),
                                    sizeof(su3_matrix_f), disp,
                                    EVENANDODD, gen_pt[0]);
    wait_general_gather(tag);

    FORALLSITES(i, s) {
      mult_su3_nn_f(&(s->linkf[dir]), (su3_matrix_f *)(gen_pt[0][i]),
                    &(s->mom[dir]));
    }
    cleanup_gather(tag);
  }

  // Overwrite original links
  FORALLSITES(i, s) {
    for (dir = XUP; dir < NUMLINK; dir++)
      su3mat_copy_f(&(s->mom[dir]), &(s->linkf[dir]));
  }
}
// -----------------------------------------------------------------