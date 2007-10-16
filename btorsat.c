#include "btorsat.h"
#include "../picosat/picosat.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>

/*------------------------------------------------------------------------*/
/* BEGIN OF DECLARATIONS                                                  */
/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/
/* BtorSATMgr                                                             */
/*------------------------------------------------------------------------*/

struct BtorSATMgr
{
  int id;
  int verbosity;
  BtorMemMgr *mm;
};

/*------------------------------------------------------------------------*/
/* END OF DECLARATIONS                                                    */
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/* BEGIN OF IMPLEMENTATION                                                */
/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/
/* Auxilliary                                                             */
/*------------------------------------------------------------------------*/

static void
print_verbose_msg (char *msg)
{
  assert (msg != NULL);
  fprintf (stderr, "[btorsat] %s", msg);
  fflush (stderr);
}

/*------------------------------------------------------------------------*/
/* BtorSAT                                                                */
/*------------------------------------------------------------------------*/

BtorSATMgr *
btor_new_sat_mgr (BtorMemMgr *mm, int verbosity)
{
  BtorSATMgr *smgr = NULL;
  assert (mm != NULL);
  assert (verbosity >= -1);
  BTOR_NEW (mm, smgr);
  smgr->id        = 1;
  smgr->verbosity = verbosity;
  smgr->mm        = mm;
  return smgr;
}

int
btor_next_cnf_id_sat_mgr (BtorSATMgr *smgr)
{
  assert (smgr != NULL);
  assert (smgr->id < INT_MAX);
  return smgr->id++;
}

int
btor_get_last_cnf_id_sat_mgr (BtorSATMgr *smgr)
{
  assert (smgr != NULL);
  /* check that at least one index has been generated */
  assert (smgr->id > 1);
  return smgr->id - 1;
}

void
btor_delete_sat_mgr (BtorSATMgr *smgr)
{
  assert (smgr != NULL);
  BTOR_DELETE (smgr->mm, smgr);
}

void
btor_init_sat (BtorSATMgr *smgr)
{
  assert (smgr != NULL);
  if (smgr->verbosity >= 3) print_verbose_msg ("initializing PicoSAT\n");
  picosat_init ();
}

void
btor_set_output_sat (BtorSATMgr *smgr, FILE *output)
{
  (void) smgr;
  assert (smgr != NULL);
  assert (output != NULL);
  picosat_set_output (output);
  picosat_set_prefix ("[picosat] ");
}

void
btor_enable_verbosity_sat (BtorSATMgr *smgr)
{
  (void) smgr;
  assert (smgr != NULL);
  picosat_enable_verbosity ();
}

void
btor_print_stats_sat (BtorSATMgr *smgr)
{
  (void) smgr;
  assert (smgr != NULL);
  picosat_stats ();
}

void
btor_add_sat (BtorSATMgr *smgr, int lit)
{
  (void) smgr;
  assert (smgr != NULL);
  picosat_add (lit);
}

void
btor_dump_cnf_sat (BtorSATMgr *smgr, FILE *output)
{
  (void) smgr;
  assert (smgr != NULL);
  assert (output != NULL);
  picosat_print (output);
}

void
btor_assume_sat (BtorSATMgr *smgr, int lit)
{
  (void) smgr;
  assert (smgr != NULL);
  picosat_assume (lit);
}

int
btor_sat_sat (BtorSATMgr *smgr, int limit)
{
  (void) smgr;
  assert (smgr != NULL);
  if (smgr->verbosity > 0) print_verbose_msg ("calling PicoSAT\n");
  return picosat_sat (limit);
}

int
btor_deref_sat (BtorSATMgr *smgr, int lit)
{
  (void) smgr;
  assert (smgr != NULL);
  return picosat_deref (lit);
}

void
btor_reset_sat (BtorSATMgr *smgr)
{
  (void) smgr;
  assert (smgr != NULL);
  if (smgr->verbosity >= 3) print_verbose_msg ("resetting PicoSAT\n");
  picosat_reset ();
}

int
btor_changed_assignments_sat (BtorSATMgr *smgr)
{
  (void) smgr;
  assert (smgr != NULL);
  return picosat_changed ();
}

/*------------------------------------------------------------------------*/
/* END OF IMPLEMENTATION                                                  */
/*------------------------------------------------------------------------*/
