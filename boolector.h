/*  Boolector: Satisfiablity Modulo Theories (SMT) solver.
 *
 *  Copyright (C) 2007-2009 Robert Daniel Brummayer.
 *  Copyright (C) 2007-2012 Armin Biere.
 *  Copyright (C) 2012-2014 Mathias Preiner.
 *  Copyright (C) 2013-2014 Aina Niemetz.
 *
 *  All rights reserved.
 *
 *  This file is part of Boolector.
 *  See COPYING for more information on using this software.
 */
#ifndef BOOLECTOR_H_INCLUDED
#define BOOLECTOR_H_INCLUDED

/*------------------------------------------------------------------------*/

#include <stdio.h>

/*------------------------------------------------------------------------*/

typedef struct Btor Btor;
typedef struct BtorNode BtorNode;
typedef struct BoolectorSort BoolectorSort;

#ifdef BOOLECTOR_FORCE_API_1
#define BoolectorNode BtorNode
#else
#define BOOLECTOR_API_2
typedef struct BoolectorNode BoolectorNode;
#endif

/*------------------------------------------------------------------------*/

/**
 * Preprocessor constant representing status ``unknown``.
 *
 * \see boolector_sat, boolector_limited_sat, boolector_simplify
 */
#define BOOLECTOR_UNKNOWN 0
/**
 * Preprocessor constant representing status ``satisfiable``.
 *
 * \see boolector_sat, boolector_limited_sat, boolector_simplify
 */
#define BOOLECTOR_SAT 10
/**
 * Preprocessor constant representing status ``unsatisfiable``.
 *
 * \see boolector_sat, boolector_limited_sat, boolector_simplify
 */
#define BOOLECTOR_UNSAT 20
/**
 * Preprocessor constant representing status ``parse error``.
 *
 * \see boolector_parse
 */
#define BOOLECTOR_PARSE_ERROR 1

/*------------------------------------------------------------------------*/

/**
 * Create a new instance of Boolector.
 *
 * \return New Boolector instance.
 */
Btor *boolector_new (void);

/**
 * Clone an instance of Boolector.
 *
 * The resulting Boolector instance is an
 * exact copy of given Boolector instance \e btor.
 * Consequently, in a clone and its
 * parent, nodes with the same id correspond to each other.
 * Use \ref boolector_match_node to match corresponding nodes.
 *
 * \param btor original Boolector instance.
 * \return A Boolector instance that is the exact copy of \e btor.
 */
Btor *boolector_clone (Btor *btor);

/**
 * Delete a boolector instance and free its resources.
 *
 * \param btor Boolector instance.
 *
 * .. note::
 *   Expressions that have not been released properly will not be
 *   deleted from memory. Use \ref boolector_get_refs to debug reference
 *   counting. You can also set option ``auto_cleanup`` via \ref
 *   boolector_set_opt in order to do the cleanup automatically.
 */
void boolector_delete (Btor *btor);

/** Set a verbosity message prefix.
 *
 * \param btor Boolector instance.  \param prefix Prefix string.
 */
void boolector_set_msg_prefix (Btor *btor, const char *prefix);

/** Get the number of external references to the boolector library.
 *
 * Internally, Boolector manages an expression DAG with reference counting.
 * Use \ref boolector_release to properly release an expression.  Before you
 * finally call \ref boolector_delete, \ref boolector_get_refs should return 0.
 *
 * \param btor Boolector instance.  \return Number of external references owned
 * by the user.
 */
int boolector_get_refs (Btor *btor);

/** Reset time statistics.
 *
 * \param btor Boolector instance.
 */
void boolector_reset_time (Btor *btor);

/** Reset statistics (time statistics not included).
 *
 * \param btor Boolector instance.
 */
void boolector_reset_stats (Btor *btor);

/** Print statistics.
 *
 * \param btor Boolector instance.
 */
void boolector_print_stats (Btor *btor);

/** Set the output API trace file and enable API tracing.
 *
 * \param btor Boolector instance.  \param apitrace Output file.  \remark The
 * API trace output file can also be set via the environment variable
 * BTORAPITRACE=<filename>.
 */
void boolector_set_trapi (Btor *btor, FILE *apitrace);

/** Return API trace file.
 *
 * \param btor Boolector instance.  \return API trace output file.
 */
FILE *boolector_get_trapi (Btor *btor);

/*------------------------------------------------------------------------*/

/** Add a constraint.
 *
 * Use this function to assert \e node.  Added constraints can not be deleted
 * anymore. After \e node has been asserted, it can be safely released by \ref
 * boolector_release.
 *
 * \param btor Boolector instance.  \param node Bit vector expression with bit
 * width one.
 */
void boolector_assert (Btor *btor, BoolectorNode *node);

/** Add an assumption.
 *
 * Use this function to assume \e node.  You must enable Boolector's
 * incremental usage via
 * \ref boolector_set_opt before you can add assumptions.
 * In contrast to assertions added via \ref boolector_assert,
 * assumptions are discarded after each call to \ref boolector_sat.
 * Assumptions and assertions are logically combined via Boolean ``and``.
 * Assumption handling in Boolector is analogous to assumptions in MiniSAT.
 *
 * \param btor Boolector instance.
 * \param node Bit vector expression with bit width one.
 */
void boolector_assume (Btor *btor, BoolectorNode *node);

/**
 * Determine if assumption \e node is a failed assumption.
 *
 * Failed assumptions are those assumptions, that force an input formula
 * to become unsatisfiable. Failed assumptions handling in Boolector is
 * analogous to failed assumptions in MiniSAT.
 * \param btor Boolector instance.
 * \param node Bit vector expression with bit width one.
 * \return 1 if assumption is failed, and 0 otherwise.
 *
 * \see boolector_assume
 */
int boolector_failed (Btor *btor, BoolectorNode *node);

/**
 * Solve an input formula.
 *
 * An input formula is defined by constraints added via \ref boolector_assert.
 * You can guide the search for a solution to an input formula by making
 * assumptions via \ref boolector_assume.
 * Note that assertions and assumptions are combined by boolean ``and``.
 *
 * If you want to call this function multiple times, you must enable
 * Boolector's incremental usage mode via \ref boolector_set_opt
 * before. Otherwise, this function may only be called once.
 *
 * \param btor Boolector instance.
 * \return \ref BOOLECTOR_SAT if the instance is satisfiable and
 * \ref BOOLECTOR_UNSAT if the instance is unsatisfiable.
 *
 * \see boolector_bv_assignment
 * \see boolector_array_assignment
 **/
int boolector_sat (Btor *btor);

/**
 * Solve an input formula and limit the search by the number of lemmas
 * generated and the number of conflicts encountered by the underlying
 * SAT solver.
 *
 * An input formula is defined by constraints added via \ref boolector_assert.
 * You can guide the search for a solution to an input formula by making
 * assumptions via \ref boolector_assume.
 *
 * If you want to call this function multiple times then you must enable
 * Boolector's incremental usage mode via \ref boolector_set_opt before.
 * Otherwise, this function can only be called once.
 *
 * \param btor Boolector instance.
 * \param lod_limit Limit for lemmas on demand (-1 unlimited).
 * \param sat_limit Conflict limit for SAT solver (-1 unlimited).
 * \return \ref BOOLECTOR_SAT if the input formula is satisfiable
 * (under possibly given assumptions),
 * \ref BOOLECTOR_UNSAT if the instance is unsatisfiable, and
 * \ref BOOLECTOR_UNKNOWN if the instance could not be solved within given
 * limits.
 *
 * \see boolector_bv_assignment
 * \see boolector_array_assignment
 **/
int boolector_limited_sat (Btor *btor, int lod_limit, int sat_limit);

/**
 * Simplify current input formula.
 *
 * \param btor Boolector instance.
 * \return \ref BOOLECTOR_SAT if the input formula was simplified to true,
 * \ref BOOLECTOR_UNSAT if it was simplified to false, and
 * \ref BOOLECTOR_UNKNOWN otherwise.
 *
 * \remark Each call to \ref boolector_sat simplifies the input formula as
 * a preprocessing step.
 */
int boolector_simplify (Btor *btor);

/*------------------------------------------------------------------------*/

/**
 * Set the SAT solver to use.
 *
 * Currently, we support ``Lingeling``, ``PicoSAT``, and ``MiniSAT`` as string
 * value of \param solver (case insensitive).  This is however
 * only possible if the corresponding solvers were enabled at compile time.
 * Call this function after \ref boolector_new.
 *
 * \param btor Boolector instance
 * \param solver Solver identifier string.
 * \return Non-zero value if setting the SAT solver was successful.
 */
int boolector_set_sat_solver (Btor *btor, const char *solver);

#ifdef BTOR_USE_LINGELING
/**
 * Use Lingeling as SAT solver.
 *
 * This function is only available if Lingeling was enabled at compile time.
 * Call this function after \ref boolector_new.
 * \param btor Boolector instance.
 * \param optstr Lingeling option string.
 * \param nofork Do not use fork/clone for Lingeling.
 */
int boolector_set_sat_solver_lingeling (Btor *btor,
                                        const char *optstr,
                                        int nofork);
#endif

#ifdef BTOR_USE_PICOSAT
/**
 * Use PicoSAT as SAT solver.
 *
 * This function is only available if PicoSAT was enabled at compile time.
 * Call this function after \ref boolector_new.
 *
 * \param btor Boolector instance.
 */
int boolector_set_sat_solver_picosat (Btor *btor);
#endif

#ifdef BTOR_USE_MINISAT
/**
 * Use MiniSAT as SAT solver.
 *
 * This function is only available if MiniSAT was enabled at compile time.
 * Call this function after \ref boolector_new.
 *
 * \param btor Boolector instance.
 */
int boolector_set_sat_solver_minisat (Btor *btor);
#endif

/*------------------------------------------------------------------------*/

/**
 * Set option.
 *
 * \param btor Boolector instance.
 * \param name Option name.
 * \param val  Option value.
 *
 * List of available options:
 *
 *   - model_gen
 *
 *     Enable (1 or 2) or disable (0) generation of a model for satisfiable
 *     instances.<br>
 *     There are two modes for model generation:
 *     -# generate  model for asserted expressions only (\e value: 1)
 *     -# generate model for all expressions (\e value: 2)
 *
 *   - incremental
 *
 *     Enable (\e value: 1) incremental mode.<br>
 *     Note that incremental usage turns off some optimization techniques.
 *     Disabling incremental usage is currently not supported.
 *
 *   - incremental_all
 *
 *     Enable (\e value: 1) or disable (\e value: 0) incremental solving of
 *     all formulas when parsing an input file.<br>
 *     Note that currently, incremental mode while parsing an input file
 *     is only supported for SMT-LIB v1 input.
 *
 *   - incremental_in_depth
 *
 *     Set incremental in-depth mode width when parsing an input file.<br>
 *     Note that currently, incremental mode is only supported for SMT-LIB v1
 *     input.
 *
 *   - incremental_look_ahead
 *
 *     Set incremental look-ahead mode width when parsing an input file.<br>
 *     Note that currently, incremental mode is only supported for SMT-LIB v1
 *     input.
 *
 *   - incremental_interval
 *
 *     Set incremental interval mode width (when parsing an input file).<br>
 *     Note that currently, incremental mode is only supported for SMT-LIB v1
 *     input.
 *
 *   - input_format
 *
 *     Force input file format (\e value: Btor: -1, SMT-LIB v1: 1,
 *     SMT-LIB v2: 2) when parsing an input file.<br>
 *     If unspecified, Boolector automatically detects the input file format
 *     while parsing.
 *
 *   - output_number_format
 *
 *     Force output number format (\e value: binary: 0, hexadecimal: 1,
 *     decimal: 2).<br>
 *     Boolector uses binary by default.
 *
 *   - output_format
 *
 *     Force output file format (Btor: -1, SMT-LIB v1: 1, SMT-LIB v2: 2).<br>
 *     Boolector uses BTOR by default.
 *
 *   - rewrite_level
 *
 *     Set the rewrite level (\e value: 0-3) of the rewriting engine.<br>
 *     Boolector uses rewrite level 3 by default.
 *     Rewrite levels are classified as follows:
 *     -# 0: no rewriting
 *     -# 1: term level rewriting
 *     -# 2: more simplification techniques
 *     -# 3: full rewriting/simplification
 *
 *     Do not alter the rewrite level of the rewriting engine after creating
 *     expressions.
 *
 *   - rewrite_level_pbr
 *
 *     Set the rewrite level (0-3) for partial beta reduction.<br>
 *     Boolector uses  rewrite level 1 by default.
 *     Rewrite levels are classified as above.
 *
 *   - beta_reduce_all
 *
 *     Enable (\e value: 1) or disable (\e value: 0) the eager elimination of
 *     lambda expressions via beta reduction.
 *
 *   - probe_beta_reduce_all
 *
 *     Enable (\e value: 1) or disable (\e value: 0) probing of
 *     ``beta_reduce_all`` (until a given LOD or SAT conflicts limit).
 *
 *     + pbra_lod_limit
 *
 *       Set lemmas on demand limit for ``probe_beta_reduce_all``.
 *
 *     + pbra_sat_limit
 *
 *       Set SAT conflicts limit for ``probe_beta_reduce_all``.
 *
 *     + pbra_ops_factor
 *
 *       Set factor by which the size of the beta reduced formula may be greater
 *  	than the original formula (for ``probe_beta_reduce_all``).
 *
 *   - dual_prop
 *
 *     Enable (\e value: 1) or disable (\e value: 0) dual propagation
 *     optimization.
 *
 *   - just
 *
 *     Enable (\e value: 1) or disable (\e value: 0) justification optimization.
 *
 *   - ucopt
 *
 *     Enable (\e value: 1) or disable (\e value: 0) unconstrained optimization.
 *
 *   - lazy_synthesize
 *
 *     Enable (\e value: 1) or disable (\e value: 0) lazy synthesis of bit
 *     vector expressions.
 *
 *   - eliminate_slices
 *
 *     Enable (\e value: 1) or disable (\e value: 0) slice elimination on bit
 *     vector variables.
 *
 *   - auto_cleanup
 *
 *     Enable (\e value: 1) or disable (\e value: 0) forced automatic cleanup
 *     of expressions and assignment strings on \ref boolector_delete.
 *
 *   - pretty_print
 *
 *     Enable (\e value: 1) or disable (\e value: 0) pretty printing when
 *     dumping.
 *
 *   - verbosity
 *
 *     Set the level of verbosity.
 *     (0 ... no verbosity, x ... increase verbosity)
 *
 */
void boolector_set_opt (Btor *btor, const char *name, int val);

/**
 * Get the current value of an option.
 *
 * \param btor Btor instance.
 * \param name Option name.
 * \return Current value of \e name.
 */
int boolector_get_opt_val (Btor *btor, const char *name);

/**
 * Get the min value of an option.
 *
 * \param btor Btor instance.
 * \param name Option name.
 * \return Min value of \e name.
 */
int boolector_get_opt_min (Btor *btor, const char *name);

/**
 * Get the max value of an option.
 *
 * \param btor Btor instance.
 * \param name Option name.
 * \return Max value of \e name.
 */
int boolector_get_opt_max (Btor *btor, const char *name);

/**
 * Get the default value of an option.
 *
 * \param btor Btor instance.
 * \param name Option name.
 * \return Default value of \e name.
 */
int boolector_get_opt_dflt (Btor *btor, const char *name);

/**
 * Get the short name of an option.
 *
 * \param btor Btor instance.
 * \param name Option name.
 * \return Short name of \e name.
 */
const char *boolector_get_opt_shrt (Btor *btor, const char *name);

/**
 * Get the description of an option.
 *
 * \param btor Btor instance.
 * \param name Option name.
 * \return Description of \e name.
 */
const char *boolector_get_opt_desc (Btor *btor, const char *name);

/**
 * Get the name of the first option in Boolector's option list.
 *
 * Given a Boolector instance \e btor, you can use this in combination
 * with \ref boolector_next_opt in order to iterate over Boolector options
 * as follows:
 * @code
   for (s = boolector_first_opt (btor); s; s = boolector_next_opt (btor, s))
 {...}
   @endcode
 *
 * \param btor Btor instance.
 * \return Name of the first option in Boolector's option list.
 */
const char *boolector_first_opt (Btor *btor);

/**
 * Given current option \e name, get the name of the next option in Boolector's
 * option list.
 *
 * Given a Boolector instance \e btor, you can use this in combination
 * with \ref boolector_first_opt in order to iterate over Boolector options
 * as follows:
 * @code
   for (s = boolector_first_opt (btor); s; s = boolector_next_opt (btor, s))
 {...}
   @endcode
 *
 * \param btor Btor instance.
 * \param name Option name.
 * \return Name of the next option in Boolector's option list, or 0 if no
 * such next option does exist.
 */
const char *boolector_next_opt (Btor *btor, const char *name);

/*------------------------------------------------------------------------*/

/**
 * Copy expression (increments reference counter).
 *
 * \param btor Boolector instance.
 * \param node Boolector node to be copied.
 * \return Node \e node with reference counter incremented.
 */
BoolectorNode *boolector_copy (Btor *btor, BoolectorNode *node);

/**
 * Release expression (decrements reference counter).
 *
 * \param btor Boolector instance.
 * \param node Boolector node to be released.
 */
void boolector_release (Btor *btor, BoolectorNode *node);

/**
 * Release all expressions and sorts
 *
 * \param btor Boolector instance.
 *
 * \see boolector_release
 * \see boolector_release_sort.
 */
void boolector_release_all (Btor *btor);

/**
 * Create bit vector constant representing the bit vector \e bits.
 *
 * \param btor Boolector instance.
 * \param bits Non-empty and terminated string consisting of zeroes and/or ones.
 * representing the bit vector constant specified by \e bits.
 * \return Bit vector constant with bit width ``strlen (bits)``.
 */
BoolectorNode *boolector_const (Btor *btor, const char *bits);

/**
 * Create bit vector constant zero with bit width \e width.
 *
 * \param btor Boolector instance.
 * \param width Number of bits which must be greater than zero.
 * \return Bit vector constant zero with bit width \e width.
 */
BoolectorNode *boolector_zero (Btor *btor, int width);

/**
 * Create bit vector constant zero with bit width one.
 *
 * \param btor Boolector instance.
 * \return Bit vector constant zero with bit width one.
 */
BoolectorNode *boolector_false (Btor *btor);

/**
 * Create bit vector constant with bit width \e width, where each bit is set to
 * one.
 *
 * \param btor Boolector instance.
 * \param width Number of bits which must be greater than zero.
 * \return Bit vector constant -1 with bit width \e width.
 */
BoolectorNode *boolector_ones (Btor *btor, int width);

/**
 * Create constant true. This is represented by the bit vector constant one
 * with bit width one.
 *
 * \param btor Boolector instance.
 * \return Bit vector constant one with bit width one.
 */
BoolectorNode *boolector_true (Btor *btor);

/**
 * Create bit vector constant one with bit width \e width.
 *
 * \param btor Boolector instance.
 * \param width Number of bits which must be greater than zero.
 * \return Bit vector constant one with bit width \e width.
 */
BoolectorNode *boolector_one (Btor *btor, int width);

/**
 * Create bit vector constant representing the unsigned integer \e u with bit
 * width \e width.
 *
 * The constant is obtained by either truncating bits or by
 * unsigned extension (padding with zeroes).
 *
 * \param btor Boolector instance.
 * \param u Unsigned integer value.
 * \param width Number of bits which must be greater than zero.
 * \return Bit vector constant with bit width \e width.
 */
BoolectorNode *boolector_unsigned_int (Btor *btor, unsigned u, int width);

/**
 * Create bit vector constant representing the signed integer \e i with bit
 * width \e width.
 *
 * The constant is obtained by either truncating bits or by
 * signed extension (padding with ones).
 *
 * \param btor Boolector instance.
 * \param i Signed integer value.
 * \param width Number of bits which must be greater than zero.
 * \return Bit vector constant with bit width \e width.
 */
BoolectorNode *boolector_int (Btor *btor, int i, int width);

/**
 * Create a bit vector variable with bit width \e width and symbol
 * \e symbol.
 *
 * \param btor Boolector instance.
 * \param width Number of bits which must be greater than zero.
 * \param symbol Name of variable.
 * \return Bit vector variable with bit width \e width and symbol \e symbol.
 *
 * \remark
 * Note that in contrast to composite expressions, which are maintained
 * uniquely w.r.t. to their kind, inputs (and consequently, bit width),
 * variables are not. Hence, each call to this function returns a fresh bit
 * vector variable.
 *
 * A variable's symbol is used as a simple means of
 * identification, either when printing a model via \ref boolector_print_model,
 * or generating file dumps via
 * \ref boolector_dump_btor, \ref boolector_dump_smt1, and
 *  \ref boolector_dump_smt2.
 * Note that a symbol must be unique but may be NULL in case that no symbol
 * should be assigned.
 */
BoolectorNode *boolector_var (Btor *btor, int width, const char *symbol);

/**
 * Create a one-dimensional bit vector array of size ``2 ^ index_width``
 * with elements of bit width \e elem_width.
 *
 * \param btor Boolector instance.
 * \param elem_width Bit width of array elements (must be greater than zero).
 * \param index_width Bit width of array indices (must be greater than zero).
 * \param symbol Name of array variable.
 * \return Bit vector array of size ``2 ^ index_width`` with elements of
 * bit width \e elem_width, and symbol \e symbol.
 *
 * \remark Note that in contrast to composite expressions, which are
 * maintained uniquely w.r.t. to their kind, inputs (and consequently,
 * bit width), array variables are not.
 * Hence, each call to \ref boolector_array with the same arguments will return
 * a fresh array variable.
 *
 * An array variable's symbol is used as a simple means of
 * identification, either when printing a model via \ref boolector_print_model,
 * or generating file dumps via
 * \ref boolector_dump_btor, \ref boolector_dump_smt1, and
 *  \ref boolector_dump_smt2.
 * Note that a symbol must be unique but may be NULL in case that no symbol
 * should be assigned.
 */
BoolectorNode *boolector_array (Btor *btor,
                                int elem_width,
                                int index_width,
                                const char *symbol);

/**
 * Create an uninterpreted function with sort \e sort and symbol \e symbol.
 * \param btor Boolector instance.
 *
 * \param sort Sort of the uninterpreted function.
 * \param symbol Name of the uninterpreted function.
 * \return Uninterpreted function of sort \e sort and symbol \e symbol.
 *
 * \remark
 * Note that in contrast to composite expressions, which are maintained
 * uniquely w.r.t. to their kind, inputs (and consequently, bit width),
 * uninterpreted functions are not.
 * Hence, each call to this function returns a fresh uninterpreted function.
 *
 * An uninterpreted function's symbol is used as a simple means of
 * identification, either when printing a model via \ref boolector_print_model,
 * or generating file dumps via
 * \ref boolector_dump_btor, \ref boolector_dump_smt1, and
 *  \ref boolector_dump_smt2.
 * Note that a symbol must be unique but may be NULL in case that no symbol
 * should be assigned.
 *
 * \see boolector_apply, boolector_fun_sort
 */
BoolectorNode *boolector_uf (Btor *btor,
                             BoolectorSort *sort,
                             const char *symbol);

/**
 * Create the one's complement of bit vector \e node.
 *
 * \param btor Boolector instance.
 * \param node Bit Vector node.
 * \return Bit vector representing the one's complement of \e node with the
 * same bit width as \e node.
 */
BoolectorNode *boolector_not (Btor *btor, BoolectorNode *node);

/**
 * Create the two's complement of bit vector \e node.
 *
 * \param btor Boolector instance.
 * \param node Bit vector node.
 * \return Bit vector representing the two's complement of \e node with the
 * same bit width as \e node.
 */
BoolectorNode *boolector_neg (Btor *btor, BoolectorNode *node);

/**
 * Create *or* reduction of node \e node.
 *
 * All bits of node \e node are combined by a Boolean *or*.
 *
 * \param btor Boolector instance.
 * \param node Bit vector node.
 * \return Bit vector with bit width one.
 */
BoolectorNode *boolector_redor (Btor *btor, BoolectorNode *node);

/**
 * Create *xor* reduction of node \e node.
 *
 * All bits of \e node are combined by a Boolean *xor*.
 *
 * \param btor Boolector instance.
 * \param node Bit vector node.
 * \return Bit vector with bit width one.
 */
BoolectorNode *boolector_redxor (Btor *btor, BoolectorNode *node);

/**
 * Create *and* reduction of node \e node.
 *
 * All bits of \e node are combined by a Boolean *and*.
 *
 * \param btor Boolector instance.
 * \param node Bit vector node.
 * \return Bit vector with bit width one.
 */
BoolectorNode *boolector_redand (Btor *btor, BoolectorNode *node);

/**
 * Create a bit vector slice of \e node from index \e upper to index \e lower.
 *
 * \param btor Boolector instance.
 * \param node Bit vector node.
 * \param upper Upper index which must be greater than or equal to zero, and
 * less than the bit width of \e node.
 * \param lower Lower index which must be greater than or equal to zero, and
 * less than or equal to \e upper.
 * \return Bit vector with bit width ``upper - lower + 1``.
 */
BoolectorNode *boolector_slice (Btor *btor,
                                BoolectorNode *node,
                                int upper,
                                int lower);

/**
 * Create unsigned extension.
 *
 * The bit vector \e node is padded with \e width * zeroes.
 *
 * \param btor Boolector instance.
 * \param node Bit vector node.
 * \param width Number of zeroes to pad.
 * \return A bit vector extended by \e width zeroes.
 */
BoolectorNode *boolector_uext (Btor *btor, BoolectorNode *node, int width);

/**
 * Create signed extension.
 *
 * The bit vector \e node is padded with \e width bits where the value
 * depends on the value of the most significant bit of node ``n``.
 *
 * \param btor Boolector instance.
 * \param node Bit vector node.
 * \param width Number of bits to pad.
 * \return A bit vector extended by \e width bits.
 */
BoolectorNode *boolector_sext (Btor *btor, BoolectorNode *node, int width);

/**
 * Create boolean implication.
 *
 * The parameters \e n0 and \e n1 must have bit width one.
 *
 * \param btor Boolector instance.
 * \param n0 Bit vector node representing the premise.
 * \param n1 Bit vector node representing the conclusion.
 * \return Implication n0 => n1 with bit width one.
 */
BoolectorNode *boolector_implies (Btor *btor,
                                  BoolectorNode *n0,
                                  BoolectorNode *n1);

/**
 * Create Boolean equivalence.
 *
 * The parameters \e n0 and \e n1 must have bit width one.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Equivalence n0 <=> n1 with bit width one.
 */
BoolectorNode *boolector_iff (Btor *btor, BoolectorNode *n0, BoolectorNode *n1);

/**
 * Create a bit vector *xor*.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with the same bit width as the operands.
 */
BoolectorNode *boolector_xor (Btor *btor, BoolectorNode *n0, BoolectorNode *n1);

/**
 * Create a bit vector *xnor*.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with the same bit width as the operands.
 */
BoolectorNode *boolector_xnor (Btor *btor,
                               BoolectorNode *n0,
                               BoolectorNode *n1);

/**
 * Create a bit vector *and*.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with the same bit width as the operands.
 */
BoolectorNode *boolector_and (Btor *btor, BoolectorNode *n0, BoolectorNode *n1);

/**
 * Create a bit vector *nand*.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with the same bit width as the operands.
 */
BoolectorNode *boolector_nand (Btor *btor,
                               BoolectorNode *n0,
                               BoolectorNode *n1);

/**
 * Create a bit vector *or*.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with the same bit width as the operands.
 */
BoolectorNode *boolector_or (Btor *btor, BoolectorNode *n0, BoolectorNode *n1);

/**
 * Create a bit vector *nor*.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with the same bit width as the operands.
 */
BoolectorNode *boolector_nor (Btor *btor, BoolectorNode *n0, BoolectorNode *n1);

/**
 * Create bit vector or array equality.
 *
 * Both operands are either bit vectors with the same bit width or arrays
 * of the same type.
 *
 * \param btor Boolector instance.
 * \param n0 First operand.
 * \param n1 Second operand.
 * \return Bit vector with bit width one.
 */
BoolectorNode *boolector_eq (Btor *btor, BoolectorNode *n0, BoolectorNode *n1);

/**
 * Create bit vector or array inequality.
 *
 * Both operands are either bit vectors with the same bit width or arrays
 * of the same type.
 *
 * \param btor Boolector instance.
 * \param n0 First operand.
 * \param n1 Second operand.
 * \return Bit vector with bit width one.
 */
BoolectorNode *boolector_ne (Btor *btor, BoolectorNode *n0, BoolectorNode *n1);

/**
 * Create bit vector addition.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector addition with the same bit width as the operands.
 */
BoolectorNode *boolector_add (Btor *btor, BoolectorNode *n0, BoolectorNode *n1);

/**
 * Create an unsigned bit vector addition overflow detection.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with bit width one, which indicates if the addition of
 * \e n0 and \e n1 overflows in case both operands are treated unsigned.
 */
BoolectorNode *boolector_uaddo (Btor *btor,
                                BoolectorNode *n0,
                                BoolectorNode *n1);

/**
 * Create a signed bit vector addition overflow detection.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with bit width one, which indicates if the addition of
 * \e n0 and \e n1 overflows in case both operands are treated signed.
 */
BoolectorNode *boolector_saddo (Btor *btor,
                                BoolectorNode *n0,
                                BoolectorNode *n1);

/**
 * Create a bitvector multiplication.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector multiplication with the same bit width as the operands.
 */
BoolectorNode *boolector_mul (Btor *btor, BoolectorNode *n0, BoolectorNode *n1);

/**
 * Create an unsigned bit vector multiplication overflow detection.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with bit width one, which indicates if the multiplication
 * of \e n0 and \e n1 overflows in case both operands are treated unsigned.
 */
BoolectorNode *boolector_umulo (Btor *btor,
                                BoolectorNode *n0,
                                BoolectorNode *n1);

/**
 * Create signed multiplication overflow detection.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with bit width one, which indicates if the multiplication
 * of \e n0 and \e n1 overflows in case both operands are treated signed.
 */
BoolectorNode *boolector_smulo (Btor *btor,
                                BoolectorNode *n0,
                                BoolectorNode *n1);

/**
 * Create an unsigned less than.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with bit width one.
 */
BoolectorNode *boolector_ult (Btor *btor, BoolectorNode *n0, BoolectorNode *n1);

/**
 * Create a signed less than.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with bit width one.
 */
BoolectorNode *boolector_slt (Btor *btor, BoolectorNode *n0, BoolectorNode *n1);

/**
 * Create an unsigned less than or equal.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with bit width one.
 */
BoolectorNode *boolector_ulte (Btor *btor,
                               BoolectorNode *n0,
                               BoolectorNode *n1);

/**
 * Create a signed less than or equal.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with bit width one.
 */
BoolectorNode *boolector_slte (Btor *btor,
                               BoolectorNode *n0,
                               BoolectorNode *n1);

/**
 * Create an unsigned greater than.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with bit width one.
 */
BoolectorNode *boolector_ugt (Btor *btor, BoolectorNode *n0, BoolectorNode *n1);

/**
 * Create a signed greater than.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with bit width one.
 */
BoolectorNode *boolector_sgt (Btor *btor, BoolectorNode *n0, BoolectorNode *n1);

/**
 * Create an unsigned greater than or equal.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with bit width one.
 */
BoolectorNode *boolector_ugte (Btor *btor,
                               BoolectorNode *n0,
                               BoolectorNode *n1);

/**
 * Create a signed greater than or equal.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with bit width one.
 */
BoolectorNode *boolector_sgte (Btor *btor,
                               BoolectorNode *n0,
                               BoolectorNode *n1);

/**
 * Create a logical shift left.
 *
 * Given node \e n1, the value it represents is the number of zeroes shifted
 * into node \e n0 from the right.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand where the bit width is a power of two
 * and greater than 1.
 * \param n1 Second bit vector operand with bit width log2 of the bit width of
 * \e n0.
 * \return Bit vector with the same bit width as \e n0.
 */
BoolectorNode *boolector_sll (Btor *btor, BoolectorNode *n0, BoolectorNode *n1);

/**
 * Create a logical shift right.
 *
 * Given node \e n1, the value it represents is the number of zeroes shifted
 * into node \e n0 from the left.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand where the bit width is a power of two
 * and greater than 1.
 * \param n1 Second bit vector operand with bit width log2 of
 * the bit width of \e n0.
 * \return Bit vector with the same bit width as \e n0 and \e n1.
 */
BoolectorNode *boolector_srl (Btor *btor, BoolectorNode *n0, BoolectorNode *n1);

/**
 * Create an arithmetic shift right.
 *
 * Analogously to \ref boolector_srl, but
 * whether zeroes or ones are shifted in depends on the most significant bit
 * of \e n0.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand where the bit width is a power of two
 * and greater than 1.
 * \param n1 Second bit vector operand with bit width log2 of
 * the bit width of \e n0.
 * \return Bit vector with the same bit width as \e n0.
 */
BoolectorNode *boolector_sra (Btor *btor, BoolectorNode *n0, BoolectorNode *n1);

/**
 * Create a rotate left.
 *
 * Given bit vector node \e n1, the value it represents is the number of bits
 * by which node \e n0 is rotated to the left.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand where the bit width is a power of two
 * and greater than 1.
 * \param n1 Second bit vector operand with bit width log2 of
 * the bit width of \e n0.
 * \return Bit vector with the same bit width as \e n0.
 */
BoolectorNode *boolector_rol (Btor *btor, BoolectorNode *n0, BoolectorNode *n1);

/**
 * Create a rotate right.
 *
 * Given bit vector node \e n1, the value it represents is the number of bits by
 * which node \e n0 is rotated to the right.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand where the bit width is a power of two
 * and greater than 1.
 * \param n1 Second bit vector operand with bit width log2 of
 * the bit width of \e n0.
 * \return Bit vector with the same bit width as \e n0.
 */
BoolectorNode *boolector_ror (Btor *btor, BoolectorNode *n0, BoolectorNode *n1);

/**
 * Create a bit vector subtraction.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with the same bit width as the operands.
 */
BoolectorNode *boolector_sub (Btor *btor, BoolectorNode *n0, BoolectorNode *n1);

/**
 * Create an unsigned bit vector subtraction overflow detection.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with bit width one, which indicates if the subtraction
 * of \e n0 and \e n1 overflows in case both operands are treated unsigned.
 */
BoolectorNode *boolector_usubo (Btor *btor,
                                BoolectorNode *n0,
                                BoolectorNode *n1);

/**
 * Create a signed bit vector subtraction overflow detection.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with bit width one, which indicates if the subtraction
 * of \e n0 and \e n1 overflows in case both operands are treated signed.
 */
BoolectorNode *boolector_ssubo (Btor *btor,
                                BoolectorNode *n0,
                                BoolectorNode *n1);

/**
 * Create unsigned division.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 * If \e n1 is zero, then the result is -1.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with the same bit width as the operands.
 *
 * \remark The behavior that division by zero returns -1 does not exactly
 * comply with the SMT-LIB standard 1.2 and 2.0 where division by zero is
 * handled as uninterpreted function. Our semantics are motivated by
 * real circuits where division by zero cannot be uninterpreted and of course
 * returns a result.
 */
BoolectorNode *boolector_udiv (Btor *btor,
                               BoolectorNode *n0,
                               BoolectorNode *n1);

/**
 * Create signed division.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with the same bit width as the operands.
 *
 * \remark Note that signed division is expressed by means of unsigned
 * division, where either node is normalized in case that its sign bit is 1.
 * If the sign bits of ``a`` and ``b`` do not match, two's complement
 * is performed on the result of the previous unsigned division.
 * Hence, the behavior in case of a division by zero depends on
 * \ref boolector_udiv.
 */
BoolectorNode *boolector_sdiv (Btor *btor,
                               BoolectorNode *n0,
                               BoolectorNode *n1);

/**
 * Create a signed bit vector division overflow detection.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 * An overflow can happen if \e n0 represents INT_MIN and \e n1 represents -1.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with bit width one.
 * \return Bit vector with bit width one, which indicates if the division
 * of \e n0 and \e n1 overflows in case both operands are treated signed.
 *
 * \remark Unsigned division cannot overflow.
 */
BoolectorNode *boolector_sdivo (Btor *btor,
                                BoolectorNode *n0,
                                BoolectorNode *n1);

/**
 * Create an unsigned remainder.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 * If \e n1 is zero, then the result is \e n0.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with the same bit width as the operands.
 *
 * \remark As in \ref boolector_udiv the behavior if \e n1 is zero, does
 * not exactly comply with the SMT-LIB standard 1.2 and 2.0 where the result
 * is handled as uninterpreted function. Our semantics are motivated by
 * real circuits, where results can not be uninterpreted.
 */
BoolectorNode *boolector_urem (Btor *btor,
                               BoolectorNode *n0,
                               BoolectorNode *n1);

/**
 * Create a signed remainder.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 * If \e n1 is zero, then the result is \e n0.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with the same bit width as the operands.
 *
 * \remark Analogously to \ref boolector_sdiv,
 * the signed remainder is expressed by means of the unsigned remainder,
 * where either node is normalized in case that its sign bit is 1.
 * Hence, in case that \e n1 is zero, the result depends on
 * \ref boolector_urem.
 */
BoolectorNode *boolector_srem (Btor *btor,
                               BoolectorNode *n0,
                               BoolectorNode *n1);

/**
 * Create a, signed remainder where its sign matches the sign of the divisor.
 *
 * The parameters \e n0 and \e n1 must have the same bit width.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with the same bit width as the operands.
 *
 * \remark If \e n1 is zero, the behavior of this function depends on
 * \ref boolector_urem.
 */
BoolectorNode *boolector_smod (Btor *btor,
                               BoolectorNode *n0,
                               BoolectorNode *n1);

/**
 * Create the concatenation of two bit vectors.
 *
 * \param btor Boolector instance.
 * \param n0 First bit vector operand.
 * \param n1 Second bit vector operand.
 * \return Bit vector with the bit width ``bit width of n0 + bit width of n1``.
 */
BoolectorNode *boolector_concat (Btor *btor,
                                 BoolectorNode *n0,
                                 BoolectorNode *n1);

/**
 * Create a read on array \e n_array at position \e n_index.
 *
 * \param btor Boolector instance.
 * \param n_array Array operand.
 * \param n_index Bit vector index. The bit width of \e n_index must have
 * the same bit width as the indices of \e n_array.
 * \return Bit vector with the same bit width as the elements of \e n_array.
 */
BoolectorNode *boolector_read (Btor *btor,
                               BoolectorNode *n_array,
                               BoolectorNode *n_index);

/**
 * Create a write on array \e n_array at position \e n_index with value
 * \e n_value.
 *
 * The array is updated at exactly one position, all other elements remain
 * unchanged.
 * The bit width of \e n_index must be the same as the bit width of the
 * indices of \e n_array.
 * The bit width of \e n_value must be the same as the bit width of the
 * elements of \e n_array.
 *
 * \param btor Boolector instance.
 * \param n_array Array operand.
 * \param n_index Bit vector index.
 * \param n_value Bit vector value.
 * \return An array where the value at index \e n_index has been updated with
 * \e n_value.
 */
BoolectorNode *boolector_write (Btor *btor,
                                BoolectorNode *n_array,
                                BoolectorNode *n_index,
                                BoolectorNode *n_value);

/**
 * Create an if-then-else.
 *
 * If condition \e n_cond is true, then \e n_then is returned, else \e n_else
 * is returned.
 * Nodes \e n_then and \e n_else must be either both arrays or both bit vectors.
 *
 * \param btor Boolector instance.
 * \param n_cond Bit vector condition with bit width one.
 * \param n_then Array or bit vector operand representing the ``if`` case.
 * \param n_else Array or bit vector operand representing the ``else`` case.
 * \return Either \e n_then or n_else.
 */
BoolectorNode *boolector_cond (Btor *btor,
                               BoolectorNode *n_cond,
                               BoolectorNode *n_then,
                               BoolectorNode *n_else);

/**
 * Create function parameter.
 *
 * This kind of node is used to create parameterized expressions, which are
 * used to create functions. Once a parameter is bound to a function, it
 * cannot be re-used in other functions.
 *
 * \param btor Boolector instance.
 * \param width Number of bits which must be greater than zero.
 * \param symbol Name of parameter.
 * \return Parameter expression with bit width \e width and symbol \e symbol.
 *
 * \see boolector_fun, boolector_apply
 */
BoolectorNode *boolector_param (Btor *btor, int width, const char *symbol);

/**
 * Create a function with body \e node parameterized over parameters
 * \e param_nodes.
 *
 * This kind of node is similar to macros in the SMT-LIB standard 2.0.
 * Note that as soon as a parameter is bound to a function, it can not be
 * reused in other functions.
 * Call a function via \ref boolector_apply.
 *
 * \param btor Boolector instance.
 * \param param_nodes Parameters of function.
 * \param paramc Number of parameters.
 * \param node Function body paramterized over \e param_nodes.
 * \return Function over parameterized expression \e node.
 * \see boolector_apply, boolector_param
 */
BoolectorNode *boolector_fun (Btor *btor,
                              BoolectorNode **param_nodes,
                              int paramc,
                              BoolectorNode *node);

/**
 * Create a function application on function \e n_fun with arguments
 * \e arg_nodes.
 *
 * \param btor Boolector instance.
 * \param arg_nodes Arguments to be applied.
 * \param argc Number of arguments to be applied.
 * \param n_fun Function expression.
 * \return Function application on function \e n_fun with arguments \e
 * arg_nodes.
 *
 * \see boolector_fun, boolector_uf
 */
BoolectorNode *boolector_apply (Btor *btor,
                                BoolectorNode **arg_nodes,
                                int argc,
                                BoolectorNode *n_fun);

/**
 * Create bit vector expression that increments bit vector \e node by one.
 *
 * \param btor Boolector instance.
 * \param node Bit vector operand.
 * \result Bit vector with the same bit width as \e node incremented by one.
 */
BoolectorNode *boolector_inc (Btor *btor, BoolectorNode *node);

/**
 * Create bit vector expression that decrements bit vector \e node by one.
 *
 * \param btor Boolector instance.
 * \param node Bit vector operand.
 * \result Bit vector with the same bit width as \e node decremented by one.
 */
BoolectorNode *boolector_dec (Btor *btor, BoolectorNode *node);

/*------------------------------------------------------------------------*/

/**
 * Return the Boolector instance to which \e node belongs.
 *
 * \param node Boolector node.
 * \return Boolector instance.
 */
Btor *boolector_get_btor (BoolectorNode *node);

/**
 * Get the id of a given node.
 *
 * \param btor Boolector instance.
 * \param node Boolector node.
 * \return Id of \e node.
 */
int boolector_get_id (Btor *btor, BoolectorNode *node);

/**
 * Retrieve the node belonging to Boolector instance \e btor that matches
 * given \e id.
 *
 * \param btor Boolector instance.
 * \param id Boolector node id.
 * \return The Boolector node that matches given \e node in Boolector instance
 * \e btor by id.
 *
 * \remark Note that matching a node against another increases the reference
 * count of the returned match, which must therefore be released appropriately
 * (\ref boolector_release).
 */
BoolectorNode *boolector_match_node_by_id (Btor *btor, int id);

/**
 * Retrieve the node belonging to Boolector instance \e btor that matches
 * given BoolectorNode \e node by id. This is intended to be used for handling
 * expressions of a cloned instance (\ref boolector_clone).
 *
 * \param btor Boolector instance.
 * \param node Boolector node.
 * \return The Boolector node that matches given \e node in Boolector instance
 * \e btor by id.
 *
 * \remark Note that matching a node against another increases the reference
 * count of the returned match, which must therefore be released appropriately
 * (\ref boolector_release).
 * Only nodes created before the \ref boolector_clone call can be matched.
 */
BoolectorNode *boolector_match_node (Btor *btor, BoolectorNode *node);

/**
 * Get the symbol of an expression.
 *
 * \param btor Boolector instance.
 * \param var Array or bit vector variable, parameter, uninterpreted function.
 * \return Symbol of expression.
 *
 * \see boolector_var
 * \see boolector_array
 * \see boolector_uf
 * \see boolector_param
 */
const char *boolector_get_symbol (Btor *btor, BoolectorNode *var);

/**
 * Set the symbol of an expression.
 *
 * Expression must be either an array or
 * bit vector variable, a parameter, or an uninterpreted function).
 *
 * \param btor Boolector instance.
 * \param var Array or bit vector variable, parameter, uninterpreted function.
 * \param symbol The symbol to be set.
 *
 * \see boolector_var
 * \see boolector_array
 * \see boolector_uf
 * \see boolector_param
 */
void boolector_set_symbol (Btor *btor, BoolectorNode *var, const char *symbol);

/**
 * Get the bit width of an expression.
 *
 * If the expression
 * is an array, it returns the bit width of the array elements.
 * If the expression
 * is a function, it returns the bit width of the function's return value.
 *
 * \param btor Boolector instance.
 * \param node Boolector node.
 * \return Bit width of \e node.
 */
int boolector_get_width (Btor *btor, BoolectorNode *node);

/**
 * Get the bit width of indices of \e n_array.
 *
 * \param btor Boolector instance.
 * \param n_array Array operand.
 * \return Bit width of indices of \e n_array
 */
int boolector_get_index_width (Btor *btor, BoolectorNode *n_array);

/**
 * Get the bit vector of a constant node as a bit string.
 *
 * \param btor Boolector instance.
 * \param node Constant node.
 * \return String representing the bits of \e node.
 */
const char *boolector_get_bits (Btor *btor, BoolectorNode *node);

/**
 * Get the arity of function \e node.
 *
 * \param btor Boolector instance.
 * \param node Function node.
 * \return Arity of \e node.
 */
int boolector_get_fun_arity (Btor *btor, BoolectorNode *node);

/**
 * Determine if given node is a constant node.
 *
 * \param btor Boolector instance.
 * \param node Boolector node.
 * \return True if \e node is a constant, and false otherwise.
 */
int boolector_is_const (Btor *btor, BoolectorNode *node);

/**
 * Determine if given node is a bit vector variable.
 *
 * \param btor Boolector instance.
 * \param node Boolector node.
 * \return True if \e node is a bit vector variable, and false otherwise.
 */
int boolector_is_var (Btor *btor, BoolectorNode *node);

/**
 * Determine if given node is an array node.
 *
 * \param btor Boolector instance.
 * \param node Boolector node.
 * \return True if \e node is an array, and false otherwise.
 */
int boolector_is_array (Btor *btor, BoolectorNode *node);

/**
 * Determine if expression is an array variable.
 *
 * \param btor Boolector instance.
 * \param node Boolector node.
 * \return True if \e node is an array variable, and false otherwise.
 */
int boolector_is_array_var (Btor *btor, BoolectorNode *node);

/**
 * Determine if given node is a parameter node.
 *
 * \param btor Boolector instance.
 * \param node Boolector node.
 * \return True if \e node is a parameter, and false otherwise.
 */
int boolector_is_param (Btor *btor, BoolectorNode *node);

/**
 * Determine if given parameter node is bound by a function.
 *
 * \param btor Boolector instance.
 * \param node Parameter node.
 * \return True if \e node is bound, and false otherwise.
 */
int boolector_is_bound_param (Btor *btor, BoolectorNode *node);

/**
 * Determine if given node is a function node.
 *
 * \param btor Boolector instance.
 * \param node Boolector node.
 * \return True if \e node is a function, and false otherwise.
 */
int boolector_is_fun (Btor *btor, BoolectorNode *node);

/**
 * Check if sorts of given arguments matches the function signature.
 *
 * \param btor Boolector instance.
 * \param arg_nodes Arguments to be checked.
 * \param argc Number of arguments to be checked.
 * \param n_fun Function expression.
 * \return -1 if all sorts are correct, otherwise it returns the position of
 * the incorrect argument.
 */
int boolector_fun_sort_check (Btor *btor,
                              BoolectorNode **arg_nodes,
                              int argc,
                              BoolectorNode *n_fun);

/**
 * Generate an assignment string for bit vector expression if \ref
 * boolector_sat has returned \ref BOOLECTOR_SAT and model generation has been
 * enabled.
 *
 * The expression can be an arbitrary bit vector expression which
 * occurs in an assertion or current assumption. The assignment string has to
 * be freed by \ref boolector_free_bv_assignment.
 * \param btor Boolector instance.
 * \param node Bit vector expression.
 * \return String representing a satisfying assignment to bit vector variables
 * and a consistent assignment for arbitrary bit vector expressions.
 * Each character of the string can be ``0``, ``1`` or ``x``. The latter
 * represents that the corresponding bit can be assigned arbitrarily.
 *
 * \see For enabling model generation see boolector_set_opt
 */
const char *boolector_bv_assignment (Btor *btor, BoolectorNode *node);

/**
 * Free an assignment string for bit vectors.
 *
 * \param btor Boolector instance.
 * \param assignment String which has to be freed.
 *
 * \see boolector_bv_assignment
 */
void boolector_free_bv_assignment (Btor *btor, const char *assignment);

/**
 * Generate a model for an array expression.
 *
 * If \ref boolector_sat has returned \ref BOOLECTOR_SAT and model generation
 * has been enabled.
 * The function creates and stores the array of indices into \e indices and the
 * array of corresponding values into \e values. The number size of \e indices
 * resp. \e values is stored into \e size. The array model simply inspects the
 * set of reads rho, which is associated with each array expression. See our
 * publication <a
 * href="http://fmv.jku.at/papers/BrummayerBiere-SMT08.pdf">Lemmas on Demand for
 * the Extensional Theory of Arrays</a> for details. At indices that do not
 * occur in the model, it is assumed that the array stores a globally unique
 * default value, for example 0.  The bit vector assignments to the indices and
 * values have to be freed by \ref boolector_free_bv_assignment. Furthermore,
 * the user has to free the array of indices and the array of values,
 * respectively of size \e size.
 *
 * \param btor Boolector instance.
 * \param n_array Array operand for which the array model should be built.
 * \param indices Pointer to array of index strings.
 * \param values Pointer to array of value strings.
 * \param size Pointer to size.
 *
 * \see For enabling model generation see \ref boolector_set_opt.
 */
void boolector_array_assignment (Btor *btor,
                                 BoolectorNode *n_array,
                                 char ***indices,
                                 char ***values,
                                 int *size);

/**
 * Free an assignment string for arrays of bit vectors.
 *
 * \param btor Boolector instance.
 * \param indices Array of index strings of size \e size.
 * \param values Array of values strings of size \e size.
 * \param size Size of arrays \e indices and \e values.
 *
 * \see boolector_array_assignment
 */
void boolector_free_array_assignment (Btor *btor,
                                      char **indices,
                                      char **values,
                                      int size);

/**
 * Generate a model for an uninterpreted function.
 * The function creates and stores the assignments of the function's arguments
 * to array \e args and the function's return values to array \e values.
 * Arrays \e args and \e values represent assignment pairs of arguments and
 * values, i.e., instantiating a function with args[i] yields value values[i].
 * For functions with arity > 1 args[i] contains a space separated string of
 * argument assignments, where the order of the assignment strings corresponds
 * to the order of the function's arguments.
 * \param btor Boolector instance.
 * \param n_uf Uninterpreted function node.
 * \param args Pointer to array of argument assignment strings.
 * \param values Pointer to array of value assignment strings.
 * \param size Size of arrays \e args and \e values.
 *
 * \remark
 * This function can only be called if \ref boolector_sat returned
 * \ref BOOLECTOR_SAT and model generation was enabled.
 *
 * \see For enabling model generation see \ref boolector_set_opt.
 */
void boolector_uf_assignment (
    Btor *btor, BoolectorNode *n_uf, char ***args, char ***values, int *size);

/**
 * Free assignment strings for uninterpreted functions.
 *
 * \param btor Boolector instance.
 * \param args Array of argument strings of size \e size.
 * \param values Array of value string of size \e size.
 * \param size Size of arrays \e args and \e values.
 *
 * \see boolector_uf_assignment
 */
void boolector_free_uf_assignment (Btor *btor,
                                   char **args,
                                   char **values,
                                   int size);

/**
 * Print model to output file. This function prints the model for all inputs
 * to the output file \e file.
 *
 * \param btor Boolector instance.
 * \param file Output file.
 */
void boolector_print_model (Btor *btor, FILE *file);

/*------------------------------------------------------------------------*/

/**
 * Create Boolean sort.
 *
 * \param btor Boolector instance.
 * \return Sort of type Boolean.
 *
 * \remark Currently, sorts in Boolector are used for uninterpreted functions,
 * only.
 *
 * \see boolector_uf
 */
BoolectorSort *boolector_bool_sort (Btor *btor);

/**
 * Create bit vector sort of bit width \e width.
 *
 * \param btor Boolector instance.
 * \param width Bit width.
 * \return Bit vector sort of bit width \e width.
 *
 * \remark Currently, sorts in Boolector are used for uninterpreted functions,
 * only.
 * \see boolector_uf
 */
BoolectorSort *boolector_bitvec_sort (Btor *btor, int width);

/**
 * Create function sort.
 *
 * \param btor Boolector instance.
 * \param domain A list of all the function arguments' sorts.
 * \param arity Number of elements in domain (must be > 0).
 * \param codomain The sort of the function's return value.
 * \return Function sort which maps given domain to given codomain.
 *
 * \remark Currently, sorts in Boolector are used for uninterpreted functions,
 * only.
 * \see boolector_uf
 */
BoolectorSort *boolector_fun_sort (Btor *btor,
                                   BoolectorSort **domain,
                                   int arity,
                                   BoolectorSort *codomain);

/**
 * Release sort (decrements reference counter).
 *
 * \param btor Boolector instance.
 * \param sort Sort to be released.
 */
void boolector_release_sort (Btor *btor, BoolectorSort *sort);

/**
 * Determine if ``n0`` and ``n1`` have the same sort or not.
 * \param btor Boolector instance.
 * \param n0 First operand.
 * \param n1 Second operand.
 * \return True if \e n0 and \e n1 have the same sort, and false otherwise.
 */
int boolector_is_equal_sort (Btor *btor, BoolectorNode *n0, BoolectorNode *n1);

/*------------------------------------------------------------------------*/

/**
 * Parse input file.
 *
 * Input file format may be either BTOR, SMT-LIB v1, or
 * SMT-LIB v2, the file type is detected automatically.
 * If the parser encounters an error, an explanation of that error is stored
 * in \e error_msg. If the input file specifies a (known) status of the input
 * formula (either sat or unsat), that status is stored in \e status.
 *
 * \param btor Boolector instance.
 * \param file Input file.
 * \param file_name Input file name.
 * \param error_msg Error message.
 * \param status Status of the input formula.
 * \return In the incremental case (right now SMT-LIB v1 only) the function
 * returns either \ref BOOLECTOR_SAT, \ref BOOLECTOR_UNSAT or
 * \ref BOOLECTOR_UNKNOWN, otherwise it always returns \ref BOOLECTOR_UNKNOWN.
 * If a parse error occurs the function returns \ref BOOLECTOR_PARSE_ERROR.
 */
int boolector_parse (Btor *btor,
                     FILE *file,
                     const char *file_name,
                     char **error_msg,
                     int *status);

/**
 * Parse input file in BTOR format.
 *
 * See \ref boolector_parse.
 *
 * \param btor Boolector instance.
 * \param file Input file.
 * \param file_name Input file name.
 * \param error_msg Error message.
 * \param status Status of the input formula.
 * \return \ref BOOLECTOR_UNKNOWN or \ref BOOLECTOR_PARSE_ERROR if a parse
 * error occurred.
 */
int boolector_parse_btor (Btor *btor,
                          FILE *file,
                          const char *file_name,
                          char **error_msg,
                          int *status);

/**
 * Parse input file in SMT-LIB v1 format.
 *
 * See \ref boolector_parse.
 *
 * \param btor Boolector instance.
 * \param file Input file.
 * \param file_name Input file name.
 * \param error_msg Error message.
 * \param status Status of the input formula.
 * \return In the incremental case (right now SMT-LIB v1 only) the function
 * returns either \ref BOOLECTOR_SAT, \ref BOOLECTOR_UNSAT or
 * \ref BOOLECTOR_UNKNOWN, otherwise it always returns \ref BOOLECTOR_UNKNOWN.
 * If a parse error occurs the function returns \ref BOOLECTOR_PARSE_ERROR.
 */
int boolector_parse_smt1 (Btor *btor,
                          FILE *file,
                          const char *file_name,
                          char **error_msg,
                          int *status);

/**
 * Parse input file in SMT-LIB v2 format. See \ref boolector_parse.
 *
 * \param btor Boolector instance.
 * \param file Input file.
 * \param file_name Input file name.
 * \param error_msg Error message.
 * \param status Status of the input formula.
 * \return \ref BOOLECTOR_UNKNOWN or \ref BOOLECTOR_PARSE_ERROR if a parse
 * error occurred.
 */
int boolector_parse_smt2 (Btor *btor,
                          FILE *file,
                          const char *file_name,
                          char **error_msg,
                          int *status);

/*------------------------------------------------------------------------*/

/**
 * Recursively dump \e node to file in
 * <a href="http://fmv.jku.at/papers/BrummayerBiereLonsing-BPR08.pdf">BTOR</a>
 * format.
 *
 * \param btor Boolector instance.
 * \param file File to which the expression should be dumped.
 * The file must be have been opened by the user before.
 * \param node The expression which should be dumped.
 */
void boolector_dump_btor_node (Btor *btor, FILE *file, BoolectorNode *node);

/**
 * Dump formula to file in
 * <a href="http://fmv.jku.at/papers/BrummayerBiereLonsing-BPR08.pdf">BTOR</a>
 * format.
 *
 * \param btor Boolector instance.
 * \param file File to which the formula should be dumped.
 * The file must be have been opened by the user before.
 */
void boolector_dump_btor (Btor *btor, FILE *file);

#if 0
/**
 * Dump formula to file in BTOR 2.0 format.
 *
 * \param btor Boolector instance.
 * \param file File to which the formula should be dumped.
 * The file must be have been opened by the user before.
 */
void boolector_dump_btor2 (Btor * btor, FILE * file);
#endif

/**
 * Recursively dump \e node to file in
 * <a href="http://smtlib.cs.uiowa.edu/papers/format-v1.2-r06.08.30.pdf">SMT-LIB
 * v1</a> format.
 *
 * \param btor Boolector instance.
 * \param file File to which the expression should be dumped.
 * The file must be have been opened by the user before.
 * \param node The expression which should be dumped.
 */
void boolector_dump_smt1_node (Btor *btor, FILE *file, BoolectorNode *node);

/**
 * Dump formula to file in <a
 * href="http://smtlib.cs.uiowa.edu/papers/format-v1.2-r06.08.30.pdf">SMT-LIB
 * v1</a> format.
 *
 * \param btor Boolector instance
 * \param file Output file.
 */
void boolector_dump_smt1 (Btor *btor, FILE *file);

/**
 * Recursively dump \e node to file in
 *<a
 *href="http://smtlib.cs.uiowa.edu/papers/smt-lib-reference-v2.0-r12.09.09.pdf">SMT-LIB
 *2.0</a> format.
 *
 * \param btor Boolector instance.
 * \param file File to which the expression should be dumped.
 * The file must be have been opened by the user before.
 * \param node The expression which should be dumped.
 */
void boolector_dump_smt2_node (Btor *btor, FILE *file, BoolectorNode *node);

/**
 * Dumps formula to file in
 *
 * <a
 * href="http://smtlib.cs.uiowa.edu/papers/smt-lib-reference-v2.0-r12.09.09.pdf">SMT-LIB
 * 2.0</a> format.
 *
 * \param btor Boolector instance
 * \param file Output file.
 */
void boolector_dump_smt2 (Btor *btor, FILE *file);

/* DEPRECATED API */

/**
 * .. deprecated:: 2.0
 * Enable model generation.
 *
 * \param btor Boolector instance.
 *
 * \remark
 * This function is deprecated, use \ref boolector_set_opt with model_gen=1
 * instead.
 *
 * \see boolector_set_opt
 */
#ifdef __GNUC__
__attribute__ ((deprecated))
#endif
void
boolector_enable_model_gen (Btor *btor);

/**
 * .. deprecated:: 2.0
 * Enable model generation for all reads.
 *
 * \param btor Boolector instance.
 *
 * \remark
 * This function is deprecated, use \ref boolector_set_opt with model_gen=2
 * instead.
 *
 * \see boolector_set_opt
 */
#ifdef __GNUC__
__attribute__ ((deprecated))
#endif
void
boolector_generate_model_for_all_reads (Btor *btor);

/**
 * .. deprecated:: 2.0
 * Enable incremental usage.
 *
 * \param btor Boolector instance.
 *
 * \see boolector_set_opt
 *
 * \remark
 * This function is deprecated, use \ref boolector_set_opt with incremental=1
 * instead.
 */
#ifdef __GNUC__
__attribute__ ((deprecated))
#endif
void
boolector_enable_inc_usage (Btor *btor);

/**
 * .. deprecated:: 2.0
 * Set the rewrite level of the rewriting engine.
 *
 * \param btor Boolector instance.
 * \param val Rewrite level ranging from
 * 0 (no rewriting) to 3 (full rewriting).
 *
 * \see boolector_set_opt
 *
 * \remark
 * This function is deprecated use \ref boolector_set_opt with
 * rewrite_level=0...3 instead.
 */
#ifdef __GNUC__
__attribute__ ((deprecated))
#endif
void
boolector_set_rewrite_level (Btor *btor, int val);

/**
 * .. deprecated:: 2.0
 * Set level of verbosity.
 *
 * \param btor Boolector instance.
 * \param val Verbosity level.
 *
 * \see boolector_set_opt
 *
 * \remark
 * This function is deprecated, use \ref boolector_set_opt with
 * verbosity=<int> instead.
 */
#ifdef __GNUC__
__attribute__ ((deprecated))
#endif
void
boolector_set_verbosity (Btor *btor, int val);

/**
 * .. deprecated:: 2.0
 *
 * Set log level.
 * \param btor Boolector instance.
 * \param val Log level.
 *
 * \see boolector_set_opt
 *
 * \remark
 * This function is deprecated, use \ref boolector_set_opt with loglevel=<int>
 * instead.
 */
#ifdef __GNUC__
__attribute__ ((deprecated))
#endif
void
boolector_set_loglevel (Btor *btor, int val);

/**
 * .. deprecated:: 2.0
 * Get the symbol of a variable.
 *
 * \param btor Boolector instance.
 * \param var Array or bit vector variable.
 * \return Symbol of variable.
 *
 * \see boolector_set_opt
 * \see boolector_set_opt
 *
 * \remark
 * This function is deprecated, use \ref boolector_get_symbol instead.
 */
#ifdef __GNUC__
__attribute__ ((deprecated))
#endif
const char *
boolector_get_symbol_of_var (Btor *btor, BoolectorNode *var);

#endif
