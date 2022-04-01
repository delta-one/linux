/****************************************************************************
Copyright (c) 2006 - 2015, Armin Biere, Johannes Kepler University.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>

#include "picosat.h"

/* By default code for 'all different constraints' is disabled, since 'NADC'
 * is defined.
 */
#define NADC

/* By default we enable failed literals, since 'NFL' is undefined.
 *
#define NFL
 */

/* By default we 'detach satisfied (large) clauses', e.g. NDSC undefined.
 *
#define NDSC
 */

/* Do not use luby restart schedule instead of inner/outer.
 *
#define NLUBY
 */

/* Enabling this define, will use gnuplot to visualize how the scores evolve.
 *
#define VISCORES
 */
#ifdef VISCORES
// #define WRITEGIF		/* ... to generate a video */
#endif

#ifdef VISCORES
#ifndef WRITEGIF
#include <unistd.h> /* for 'usleep' */
#endif
#endif

#ifdef RCODE
#include <R.h>
#endif

#define MINRESTART 100 /* minimum restart interval */
#define MAXRESTART 1000000 /* maximum restart interval */
#define RDECIDE 1000 /* interval of random decisions */
#define FRESTART 110 /* restart increase factor in percent */
#define FREDUCE 110 /* reduce increase factor in percent  */
#define FREDADJ 121 /* reduce increase adjustment factor */
#define MAXCILS 10 /* maximal number of unrecycled internals */
#define FFLIPPED 10000 /* flipped reduce factor */
#define FFLIPPEDPREC 10000000 /* flipped reduce factor precision */
#define INTERRUPTLIM 1024 /* check interrupt after that many decisions */

#ifndef TRACE
#define NO_BINARY_CLAUSES /* store binary clauses more compactly */
#endif

/* For debugging purposes you may want to define 'LOGGING', which actually
 * can be enforced by using './configure.sh --log'.
 */
#ifdef LOGGING
#define LOG(code)                                                              \
	do {                                                                   \
		code;                                                          \
	} while (0)
#else
#define LOG(code)                                                              \
	do {                                                                   \
	} while (0)
#endif
#define NOLOG(code)                                                            \
	do {                                                                   \
	} while (0) /* log exception */
#define ONLYLOG(code)                                                          \
	do {                                                                   \
		code;                                                          \
	} while (0) /* force logging */

#define FALSE ((Val)-1)
#define UNDEF ((Val)0)
#define TRUE ((Val)1)

#define COMPACT_TRACECHECK_TRACE_FMT 0
#define EXTENDED_TRACECHECK_TRACE_FMT 1
#define RUP_TRACE_FMT 2

#define NEWN(p, n)                                                             \
	do {                                                                   \
		(p) = new (ps, sizeof(*(p)) * (n));                            \
	} while (0)
#define CLRN(p, n)                                                             \
	do {                                                                   \
		memset((p), 0, sizeof(*(p)) * (n));                            \
	} while (0)
#define CLR(p) CLRN(p, 1)
#define DELETEN(p, n)                                                          \
	do {                                                                   \
		delete (ps, p, sizeof(*(p)) * (n));                            \
		(p) = 0;                                                       \
	} while (0)

#define RESIZEN(p, old_num, new_num)                                           \
	do {                                                                   \
		size_t old_size = sizeof(*(p)) * (old_num);                    \
		size_t new_size = sizeof(*(p)) * (new_num);                    \
		(p) = resize(ps, (p), old_size, new_size);                     \
	} while (0)

#define ENLARGE(start, head, end)                                              \
	do {                                                                   \
		unsigned old_num = (ptrdiff_t)((end) - (start));               \
		size_t new_num = old_num ? (2 * old_num) : 1;                  \
		unsigned count = (head) - (start);                             \
		assert((start) <= (end));                                      \
		RESIZEN((start), old_num, new_num);                            \
		(head) = (start) + count;                                      \
		(end) = (start) + new_num;                                     \
	} while (0)

#define NOTLIT(l) (ps->lits + (1 ^ ((l)-ps->lits)))

#define LIT2IDX(l) ((ptrdiff_t)((l)-ps->lits) / 2)
#define LIT2IMPLS(l) (ps->impls + (ptrdiff_t)((l)-ps->lits))
#define LIT2INT(l) ((int)(LIT2SGN(l) * LIT2IDX(l)))
#define LIT2SGN(l) (((ptrdiff_t)((l)-ps->lits) & 1) ? -1 : 1)
#define LIT2VAR(l) (ps->vars + LIT2IDX(l))
#define LIT2HTPS(l) (ps->htps + (ptrdiff_t)((l)-ps->lits))
#define LIT2JWH(l) (ps->jwh + ((l)-ps->lits))

#ifndef NDSC
#define LIT2DHTPS(l) (ps->dhtps + (ptrdiff_t)((l)-ps->lits))
#endif

#ifdef NO_BINARY_CLAUSES
typedef uintptr_t Wrd;
#define ISLITREASON(C) (1 & (Wrd)C)
#define LIT2REASON(L)                                                          \
	(assert(L->val == TRUE), ((Cls *)(1 + (2 * (L - ps->lits)))))
#define REASON2LIT(C) ((Lit *)(ps->lits + ((Wrd)C) / 2))
#endif

#define ENDOFCLS(c) ((void *)((Lit **)(c)->lits + (c)->size))

#define SOC ((ps->oclauses == ps->ohead) ? ps->lclauses : ps->oclauses)
#define EOC (ps->lhead)
#define NXC(p) (((p) + 1 == ps->ohead) ? ps->lclauses : (p) + 1)

#define OIDX2IDX(idx) (2 * ((idx) + 1))
#define LIDX2IDX(idx) (2 * (idx) + 1)

#define ISLIDX(idx) ((idx)&1)

#define IDX2OIDX(idx) (assert(!ISLIDX(idx)), (idx) / 2 - 1)
#define IDX2LIDX(idx) (assert(ISLIDX(idx)), (idx) / 2)

#define EXPORTIDX(idx)                                                         \
	((ISLIDX(idx) ? (IDX2LIDX(idx) + (ps->ohead - ps->oclauses)) :         \
			      IDX2OIDX(idx)) +                                       \
	 1)

#define IDX2CLS(i)                                                             \
	(assert(i),                                                            \
	 (ISLIDX(i) ? ps->lclauses : ps->oclauses)[(i) / 2 - !ISLIDX(i)])

#define IDX2ZHN(i) (assert(i), (ISLIDX(i) ? ps->zhains[(i) / 2] : 0))

#define CLS2TRD(c) (((Trd *)(c)) - 1)
#define CLS2IDX(c) ((((Trd *)(c)) - 1)->idx)

#define CLS2ACT(c)                                                             \
	((Act *)((assert((c)->learned)), assert((c)->size > 2), ENDOFCLS(c)))

#define VAR2LIT(v) (ps->lits + 2 * ((v)-ps->vars))
#define VAR2RNK(v) (ps->rnks + ((v)-ps->vars))

#define RNK2LIT(r) (ps->lits + 2 * ((r)-ps->rnks))
#define RNK2VAR(r) (ps->vars + ((r)-ps->rnks))

#define BLK_FILL_BYTES 8
#define SIZE_OF_BLK (sizeof(Blk) - BLK_FILL_BYTES)

#define PTR2BLK(void_ptr)                                                      \
	((void_ptr) ? (Blk *)(((char *)(void_ptr)) - SIZE_OF_BLK) : 0)

#define AVERAGE(a, b) ((b) ? (((double)a) / (double)(b)) : 0.0)
#define PERCENT(a, b) (100.0 * AVERAGE(a, b))

#ifndef RCODE
#define ABORT(msg)                                                             \
	do {                                                                   \
		fputs("*** picosat: " msg "\n", stderr);                       \
		abort();                                                       \
	} while (0)
#else
#define ABORT(msg)                                                             \
	do {                                                                   \
		Rf_error(msg);                                                 \
	} while (0)
#endif

#define ABORTIF(cond, msg)                                                     \
	do {                                                                   \
		if (!(cond))                                                   \
			break;                                                 \
		ABORT(msg);                                                    \
	} while (0)

#define ZEROFLT (0x00000000u)
#define EPSFLT (0x00000001u)
#define INFFLT (0xffffffffu)

#define FLTCARRY (1u << 25)
#define FLTMSB (1u << 24)
#define FLTMAXMANTISSA (FLTMSB - 1)

#define FLTMANTISSA(d) ((d)&FLTMAXMANTISSA)
#define FLTEXPONENT(d) ((int)((d) >> 24) - 128)

#define FLTMINEXPONENT (-128)
#define FLTMAXEXPONENT (127)

#define CMPSWAPFLT(a, b)                                                       \
	do {                                                                   \
		Flt tmp;                                                       \
		if (((a) < (b))) {                                             \
			tmp = (a);                                             \
			(a) = (b);                                             \
			(b) = tmp;                                             \
		}                                                              \
	} while (0)

#define UNPACKFLT(u, m, e)                                                     \
	do {                                                                   \
		(m) = FLTMANTISSA(u);                                          \
		(e) = FLTEXPONENT(u);                                          \
		(m) |= FLTMSB;                                                 \
	} while (0)

#define INSERTION_SORT_LIMIT 10

#define SORTING_SWAP(T, p, q)                                                  \
	do {                                                                   \
		T tmp = *(q);                                                  \
		*(q) = *(p);                                                   \
		*(p) = tmp;                                                    \
	} while (0)

#define SORTING_CMP_SWAP(T, cmp, p, q)                                         \
	do {                                                                   \
		if ((cmp)(ps, *(p), *(q)) > 0)                                 \
			SORTING_SWAP(T, p, q);                                 \
	} while (0)

#define QUICKSORT_PARTITION(T, cmp, a, l, r)                                   \
	do {                                                                   \
		T pivot;                                                       \
		int j;                                                         \
		i = (l)-1; /* result in 'i' */                                 \
		j = (r);                                                       \
		pivot = (a)[j];                                                \
		for (;;) {                                                     \
			while ((cmp)(ps, (a)[++i], pivot) < 0)                 \
				;                                              \
			while ((cmp)(ps, pivot, (a)[--j]) < 0)                 \
				if (j == (l))                                  \
					break;                                 \
			if (i >= j)                                            \
				break;                                         \
			SORTING_SWAP(T, (a) + i, (a) + j);                     \
		}                                                              \
		SORTING_SWAP(T, (a) + i, (a) + (r));                           \
	} while (0)

#define QUICKSORT(T, cmp, a, n)                                                \
	do {                                                                   \
		int l = 0, r = (n)-1, m, ll, rr, i;                            \
		assert(ps->ihead == ps->indices);                              \
		if (r - l <= INSERTION_SORT_LIMIT)                             \
			break;                                                 \
		for (;;) {                                                     \
			m = (l + r) / 2;                                       \
			SORTING_SWAP(T, (a) + m, (a) + r - 1);                 \
			SORTING_CMP_SWAP(T, cmp, (a) + l, (a) + r - 1);        \
			SORTING_CMP_SWAP(T, cmp, (a) + l, (a) + r);            \
			SORTING_CMP_SWAP(T, cmp, (a) + r - 1, (a) + r);        \
			QUICKSORT_PARTITION(T, cmp, (a), l + 1, r - 1);        \
			if (i - l < r - i) {                                   \
				ll = i + 1;                                    \
				rr = r;                                        \
				r = i - 1;                                     \
			} else {                                               \
				ll = l;                                        \
				rr = i - 1;                                    \
				l = i + 1;                                     \
			}                                                      \
			if (r - l > INSERTION_SORT_LIMIT) {                    \
				assert(rr - ll > INSERTION_SORT_LIMIT);        \
				if (ps->ihead == ps->eoi)                      \
					ENLARGE(ps->indices, ps->ihead,        \
						ps->eoi);                      \
				*ps->ihead++ = ll;                             \
				if (ps->ihead == ps->eoi)                      \
					ENLARGE(ps->indices, ps->ihead,        \
						ps->eoi);                      \
				*ps->ihead++ = rr;                             \
			} else if (rr - ll > INSERTION_SORT_LIMIT) {           \
				l = ll;                                        \
				r = rr;                                        \
			} else if (ps->ihead > ps->indices) {                  \
				r = *--ps->ihead;                              \
				l = *--ps->ihead;                              \
			} else                                                 \
				break;                                         \
		}                                                              \
	} while (0)

#define INSERTION_SORT(T, cmp, a, n)                                           \
	do {                                                                   \
		T pivot;                                                       \
		int l = 0, r = (n)-1, i, j;                                    \
		for (i = r; i > l; i--)                                        \
			SORTING_CMP_SWAP(T, cmp, (a) + i - 1, (a) + i);        \
		for (i = l + 2; i <= r; i++) {                                 \
			j = i;                                                 \
			pivot = (a)[i];                                        \
			while ((cmp)(ps, pivot, (a)[j - 1]) < 0) {             \
				(a)[j] = (a)[j - 1];                           \
				j--;                                           \
			}                                                      \
			(a)[j] = pivot;                                        \
		}                                                              \
	} while (0)

#ifdef NDEBUG
#define CHECK_SORTED(cmp, a, n)                                                \
	do {                                                                   \
	} while (0)
#else
#define CHECK_SORTED(cmp, a, n)                                                \
	do {                                                                   \
		int i;                                                         \
		for (i = 0; i < (n)-1; i++)                                    \
			assert((cmp)(ps, (a)[i], (a)[i + 1]) <= 0);            \
	} while (0)
#endif

#define SORT(T, cmp, a, n)                                                     \
	do {                                                                   \
		T *aa = (a);                                                   \
		int nn = (n);                                                  \
		QUICKSORT(T, cmp, aa, nn);                                     \
		INSERTION_SORT(T, cmp, aa, nn);                                \
		assert(ps->ihead == ps->indices);                              \
		CHECK_SORTED(cmp, aa, nn);                                     \
	} while (0)

#define WRDSZ (sizeof(long) * 8)

#ifdef RCODE
#define fprintf(...)                                                           \
	do {                                                                   \
	} while (0)
#define vfprintf(...)                                                          \
	do {                                                                   \
	} while (0)
#define fputs(...)                                                             \
	do {                                                                   \
	} while (0)
#define fputc(...)                                                             \
	do {                                                                   \
	} while (0)
#endif

typedef unsigned Flt; /* 32 bit deterministic soft float */
typedef Flt Act; /* clause and variable activity */
typedef struct Blk Blk; /* allocated memory block */
typedef struct Cls Cls; /* clause */
typedef struct Lit Lit; /* literal */
typedef struct Rnk Rnk; /* variable to score mapping */
typedef signed char Val; /* TRUE, UNDEF, FALSE */
typedef struct Var Var; /* variable */
#ifdef TRACE
typedef struct Trd Trd; /* trace data for clauses */
typedef struct Zhn Zhn; /* compressed chain (=zain) data */
typedef unsigned char Znt; /* compressed antecedent data */
#endif

#ifdef NO_BINARY_CLAUSES
typedef struct Ltk Ltk;

struct Ltk {
	Lit **start;
	unsigned count : WRDSZ == 32 ? 27 : 32;
	unsigned ldsize : WRDSZ == 32 ? 5 : 32;
};
#endif

struct Lit {
	Val val;
};

struct Var {
	unsigned mark : 1; /*bit 1*/
	unsigned resolved : 1; /*bit 2*/
	unsigned phase : 1; /*bit 3*/
	unsigned assigned : 1; /*bit 4*/
	unsigned used : 1; /*bit 5*/
	unsigned failed : 1; /*bit 6*/
	unsigned internal : 1; /*bit 7*/
	unsigned usedefphase : 1; /*bit 8*/
	unsigned defphase : 1; /*bit 9*/
	unsigned msspos : 1; /*bit 10*/
	unsigned mssneg : 1; /*bit 11*/
	unsigned humuspos : 1; /*bit 12*/
	unsigned humusneg : 1; /*bit 13*/
	unsigned partial : 1; /*bit 14*/
#ifdef TRACE
	unsigned core : 1; /*bit 15*/
#endif
	unsigned level;
	Cls *reason;
#ifndef NADC
	Lit **inado;
	Lit **ado;
	Lit ***adotabpos;
#endif
};

struct Rnk {
	Act score;
	unsigned pos : 30; /* 0 iff not on heap */
	unsigned moreimportant : 1;
	unsigned lessimportant : 1;
};

struct Cls {
	unsigned size;

	unsigned collect : 1; /* bit 1 */
	unsigned learned : 1; /* bit 2 */
	unsigned locked : 1; /* bit 3 */
	unsigned used : 1; /* bit 4 */
#ifndef NDEBUG
	unsigned connected : 1; /* bit 5 */
#endif
#ifdef TRACE
	unsigned collected : 1; /* bit 6 */
	unsigned core : 1; /* bit 7 */
#endif

#define LDMAXGLUE 25 /* 32 - 7 */
#define MAXGLUE ((1 << LDMAXGLUE) - 1)

	unsigned glue : LDMAXGLUE;

	Cls *next[2];
	Lit *lits[2];
};

#ifdef TRACE
struct Zhn {
	unsigned ref : 31;
	unsigned core : 1;
	Znt *liz;
	Znt znt[0];
};

struct Trd {
	unsigned idx;
	Cls cls[0];
};
#endif

struct Blk {
#ifndef NDEBUG
	union {
		size_t size; /* this is what we really use */
		void *as_two_ptrs[2]; /* 2 * sizeof (void*) alignment of data */
	} header;
#endif
	char data[BLK_FILL_BYTES];
};

enum State {
	RESET = 0,
	READY = 1,
	SAT = 2,
	UNSAT = 3,
	UNKNOWN = 4,
};

enum Phase {
	POSPHASE,
	NEGPHASE,
	JWLPHASE,
	RNDPHASE,
};

struct PicoSAT {
	enum State state;
	enum Phase defaultphase;
	int last_sat_call_result;

	FILE *out;
	char *prefix;
	int verbosity;
	int plain;
	unsigned LEVEL;
	unsigned max_var;
	unsigned size_vars;

	Lit *lits;
	Var *vars;
	Rnk *rnks;
	Flt *jwh;
	Cls **htps;
#ifndef NDSC
	Cls **dhtps;
#endif
#ifdef NO_BINARY_CLAUSES
	Ltk *impls;
	Cls impl, cimpl;
	int implvalid, cimplvalid;
#else
	Cls **impls;
#endif
	Lit **trail, **thead, **eot, **ttail, **ttail2;
#ifndef NADC
	Lit **ttailado;
#endif
	unsigned adecidelevel;
	Lit **als, **alshead, **alstail, **eoals;
	Lit **CLS, **clshead, **eocls;
	int *rils, *rilshead, *eorils;
	int *cils, *cilshead, *eocils;
	int *fals, *falshead, *eofals;
	int *mass, szmass;
	int *mssass, szmssass;
	int *mcsass, nmcsass, szmcsass;
	int *humus, szhumus;
	Lit *failed_assumption;
	int extracted_all_failed_assumptions;
	Rnk **heap, **hhead, **eoh;
	Cls **oclauses, **ohead, **eoo; /* original clauses */
	Cls **lclauses, **lhead, **EOL; /* learned clauses */
	int *soclauses, *sohead, *eoso; /* saved original clauses */
	int saveorig;
	int partial;
#ifdef TRACE
	int trace;
	Zhn **zhains, **zhead, **eoz;
	int ocore;
#endif
	FILE *rup;
	int rupstarted;
	int rupvariables;
	int rupclauses;
	Cls *mtcls;
	Cls *conflict;
	Lit **added, **ahead, **eoa;
	Var **marked, **mhead, **eom;
	Var **dfs, **dhead, **eod;
	Cls **resolved, **rhead, **eor;
	unsigned char *levels, *levelshead, *eolevels;
	unsigned *dused, *dusedhead, *eodused;
	unsigned char *buffer, *bhead, *eob;
	Act vinc, lscore, ilvinc, ifvinc;
#ifdef VISCORES
	Act fvinc, nvinc;
#endif
	Act cinc, lcinc, ilcinc, fcinc;
	unsigned srng;
	size_t current_bytes;
	size_t max_bytes;
	size_t recycled;
	double seconds, flseconds;
	double entered;
	unsigned nentered;
	int measurealltimeinlib;
	char *rline[2];
	int szrline, RCOUNT;
	double levelsum;
	unsigned iterations;
	int reports;
	int lastrheader;
	unsigned calls;
	unsigned decisions;
	unsigned restarts;
	unsigned simps;
	unsigned fsimplify;
	unsigned isimplify;
	unsigned reductions;
	unsigned lreduce;
	unsigned lreduceadjustcnt;
	unsigned lreduceadjustinc;
	unsigned lastreduceconflicts;
	unsigned llocked; /* locked large learned clauses */
	unsigned lrestart;
#ifdef NLUBY
	unsigned drestart;
	unsigned ddrestart;
#else
	unsigned lubycnt;
	unsigned lubymaxdelta;
	int waslubymaxdelta;
#endif
	unsigned long long lsimplify;
	unsigned long long propagations;
	unsigned long long lpropagations;
	unsigned fixed; /* top level assignments */
#ifndef NFL
	unsigned failedlits;
	unsigned ifailedlits;
	unsigned efailedlits;
	unsigned flcalls;
#ifdef STATS
	unsigned flrounds;
	unsigned long long flprops;
	unsigned long long floopsed, fltried, flskipped;
#endif
	unsigned long long fllimit;
	int simplifying;
	Lit **saved;
	unsigned saved_size;
#endif
	unsigned conflicts;
	unsigned contexts;
	unsigned internals;
	unsigned noclauses; /* current number large original clauses */
	unsigned nlclauses; /* current number large learned clauses */
	unsigned olits; /* current literals in large original clauses */
	unsigned llits; /* current literals in large learned clauses */
	unsigned oadded; /* added original clauses */
	unsigned ladded; /* added learned clauses */
	unsigned loadded; /* added original large clauses */
	unsigned lladded; /* added learned large clauses */
	unsigned addedclauses; /* oadded + ladded */
	unsigned vused; /* used variables */
	unsigned llitsadded; /* added learned literals */
	unsigned long long visits;
#ifdef STATS
	unsigned loused; /* used large original clauses */
	unsigned llused; /* used large learned clauses */
	unsigned long long bvisits;
	unsigned long long tvisits;
	unsigned long long lvisits;
	unsigned long long othertrue;
	unsigned long long othertrue2;
	unsigned long long othertruel;
	unsigned long long othertrue2u;
	unsigned long long othertruelu;
	unsigned long long ltraversals;
	unsigned long long traversals;
#ifdef TRACE
	unsigned long long antecedents;
#endif
	unsigned uips;
	unsigned znts;
	unsigned assumptions;
	unsigned rdecisions;
	unsigned sdecisions;
	size_t srecycled;
	size_t rrecycled;
	unsigned long long derefs;
#endif
	unsigned minimizedllits;
	unsigned nonminimizedllits;
#ifndef NADC
	Lit ***ados, ***hados, ***eados;
	Lit ***adotab;
	unsigned nadotab;
	unsigned szadotab;
	Cls *adoconflict;
	unsigned adoconflicts;
	unsigned adoconflictlimit;
	int addingtoado;
	int adodisabled;
#endif
	unsigned long long flips;
#ifdef STATS
	unsigned long long FORCED;
	unsigned long long assignments;
	unsigned inclreduces;
	unsigned staticphasedecisions;
	unsigned skippedrestarts;
#endif
	int *indices, *ihead, *eoi;
	unsigned sdflips;

	unsigned long long saved_flips;
	unsigned saved_max_var;
	unsigned min_flipped;

	void *emgr;
	picosat_malloc enew;
	picosat_realloc eresize;
	picosat_free edelete;

	struct {
		void *state;
		int (*function)(void *);
	} interrupt;

#ifdef VISCORES
	FILE *fviscores;
#endif
};

typedef PicoSAT PS;

static Flt packflt(unsigned m, int e)
{
	Flt res;
	assert(m < FLTMSB);
	assert(FLTMINEXPONENT <= e);
	assert(e <= FLTMAXEXPONENT);
	res = m | ((unsigned)(e + 128) << 24);
	return res;
}

static Flt base2flt(unsigned m, int e)
{
	if (!m)
		return ZEROFLT;

	if (m < FLTMSB) {
		do {
			if (e <= FLTMINEXPONENT)
				return EPSFLT;

			e--;
			m <<= 1;

		} while (m < FLTMSB);
	} else {
		while (m >= FLTCARRY) {
			if (e >= FLTMAXEXPONENT)
				return INFFLT;

			e++;
			m >>= 1;
		}
	}

	m &= ~FLTMSB;
	return packflt(m, e);
}

static Flt addflt(Flt a, Flt b)
{
	unsigned ma, mb, delta;
	int ea, eb;

	CMPSWAPFLT(a, b);
	if (!b)
		return a;

	UNPACKFLT(a, ma, ea);
	UNPACKFLT(b, mb, eb);

	assert(ea >= eb);
	delta = ea - eb;
	if (delta < 32)
		mb >>= delta;
	else
		mb = 0;
	if (!mb)
		return a;

	ma += mb;
	if (ma & FLTCARRY) {
		if (ea == FLTMAXEXPONENT)
			return INFFLT;

		ea++;
		ma >>= 1;
	}

	assert(ma < FLTCARRY);
	ma &= FLTMAXMANTISSA;

	return packflt(ma, ea);
}

static Flt mulflt(Flt a, Flt b)
{
	unsigned ma, mb;
	unsigned long long accu;
	int ea, eb;

	CMPSWAPFLT(a, b);
	if (!b)
		return ZEROFLT;

	UNPACKFLT(a, ma, ea);
	UNPACKFLT(b, mb, eb);

	ea += eb;
	ea += 24;
	if (ea > FLTMAXEXPONENT)
		return INFFLT;

	if (ea < FLTMINEXPONENT)
		return EPSFLT;

	accu = ma;
	accu *= mb;
	accu >>= 24;

	if (accu >= FLTCARRY) {
		if (ea == FLTMAXEXPONENT)
			return INFFLT;

		ea++;
		accu >>= 1;

		if (accu >= FLTCARRY)
			return INFFLT;
	}

	assert(accu < FLTCARRY);
	assert(accu & FLTMSB);

	ma = accu;
	ma &= ~FLTMSB;

	return packflt(ma, ea);
}

static Flt ascii2flt(const char *str)
{
	Flt ten = base2flt(10, 0);
	Flt onetenth = base2flt(26843546, -28);
	Flt res = ZEROFLT, tmp, base;
	const char *p = str;
	int ch;

	ch = *p++;

	if (ch != '.') {
		if (!isdigit(ch))
			return INFFLT; /* better abort ? */

		res = base2flt(ch - '0', 0);

		while ((ch = *p++)) {
			if (ch == '.')
				break;

			if (!isdigit(ch))
				return INFFLT; /* better abort? */

			res = mulflt(res, ten);
			tmp = base2flt(ch - '0', 0);
			res = addflt(res, tmp);
		}
	}

	if (ch == '.') {
		ch = *p++;
		if (!isdigit(ch))
			return INFFLT; /* better abort ? */

		base = onetenth;
		tmp = mulflt(base2flt(ch - '0', 0), base);
		res = addflt(res, tmp);

		while ((ch = *p++)) {
			if (!isdigit(ch))
				return INFFLT; /* better abort? */

			base = mulflt(base, onetenth);
			tmp = mulflt(base2flt(ch - '0', 0), base);
			res = addflt(res, tmp);
		}
	}

	return res;
}

#if defined(VISCORES)

static double flt2double(Flt f)
{
	double res;
	unsigned m;
	int e, i;

	UNPACKFLT(f, m, e);
	res = m;

	if (e < 0) {
		for (i = e; i < 0; i++)
			res *= 0.5;
	} else {
		for (i = 0; i < e; i++)
			res *= 2.0;
	}

	return res;
}

#endif

static int log2flt(Flt a)
{
	return FLTEXPONENT(a) + 24;
}

static int cmpflt(Flt a, Flt b)
{
	if (a < b)
		return -1;

	if (a > b)
		return 1;

	return 0;
}

static void *new (PS *ps, size_t size)
{
	size_t bytes;
	Blk *b;

	if (!size)
		return 0;

	bytes = size + SIZE_OF_BLK;

	if (ps->enew)
		b = ps->enew(ps->emgr, bytes);
	else
		b = malloc(bytes);

	ABORTIF(!b, "out of memory in 'new'");
#ifndef NDEBUG
	b->header.size = size;
#endif
	ps->current_bytes += size;
	if (ps->current_bytes > ps->max_bytes)
		ps->max_bytes = ps->current_bytes;
	return b->data;
}

static void delete (PS *ps, void *void_ptr, size_t size)
{
	size_t bytes;
	Blk *b;

	if (!void_ptr) {
		assert(!size);
		return;
	}

	assert(size);
	b = PTR2BLK(void_ptr);

	assert(size <= ps->current_bytes);
	ps->current_bytes -= size;

	assert(b->header.size == size);

	bytes = size + SIZE_OF_BLK;
	if (ps->edelete)
		ps->edelete(ps->emgr, b, bytes);
	else
		free(b);
}

static void *resize(PS *ps, void *void_ptr, size_t old_size, size_t new_size)
{
	size_t old_bytes, new_bytes;
	Blk *b;

	b = PTR2BLK(void_ptr);

	assert(old_size <= ps->current_bytes);
	ps->current_bytes -= old_size;

	if ((old_bytes = old_size)) {
		assert(old_size && b && b->header.size == old_size);
		old_bytes += SIZE_OF_BLK;
	} else
		assert(!b);

	if ((new_bytes = new_size))
		new_bytes += SIZE_OF_BLK;

	if (ps->eresize)
		b = ps->eresize(ps->emgr, b, old_bytes, new_bytes);
	else
		b = realloc(b, new_bytes);

	if (!new_size) {
		assert(!b);
		return 0;
	}

	ABORTIF(!b, "out of memory in 'resize'");
#ifndef NDEBUG
	b->header.size = new_size;
#endif

	ps->current_bytes += new_size;
	if (ps->current_bytes > ps->max_bytes)
		ps->max_bytes = ps->current_bytes;

	return b->data;
}

static unsigned int2unsigned(int l)
{
	return (l < 0) ? 1 + 2 * -l : 2 * l;
}

static Lit *int2lit(PS *ps, int l)
{
	return ps->lits + int2unsigned(l);
}

static Lit **end_of_lits(Cls *c)
{
	return (Lit **)c->lits + c->size;
}

#if !defined(NDEBUG) || defined(LOGGING)

static void dumplits(PS *ps, Lit **l, Lit **end)
{
	int first;
	Lit **p;

	if (l == end) {
		/* empty clause */
	} else if (l + 1 == end) {
		fprintf(ps->out, "%d ", LIT2INT(l[0]));
	} else {
		assert(l + 2 <= end);
		first = (abs(LIT2INT(l[0])) > abs(LIT2INT(l[1])));
		fprintf(ps->out, "%d ", LIT2INT(l[first]));
		fprintf(ps->out, "%d ", LIT2INT(l[!first]));
		for (p = l + 2; p < end; p++)
			fprintf(ps->out, "%d ", LIT2INT(*p));
	}

	fputc('0', ps->out);
}

static void dumpcls(PS *ps, Cls *c)
{
	Lit **end;

	if (c) {
		end = end_of_lits(c);
		dumplits(ps, c->lits, end);
#ifdef TRACE
		if (ps->trace)
			fprintf(ps->out, " clause(%u)", CLS2IDX(c));
#endif
	} else
		fputs("DECISION", ps->out);
}

static void dumpclsnl(PS *ps, Cls *c)
{
	dumpcls(ps, c);
	fputc('\n', ps->out);
}

void dumpcnf(PS *ps)
{
	Cls **p, *c;

	for (p = SOC; p != EOC; p = NXC(p)) {
		c = *p;

		if (!c)
			continue;

#ifdef TRACE
		if (c->collected)
			continue;
#endif

		dumpclsnl(ps, *p);
	}
}

#endif

static void delete_prefix(PS *ps)
{
	if (!ps->prefix)
		return;

	delete (ps, ps->prefix, strlen(ps->prefix) + 1);
	ps->prefix = 0;
}

static void new_prefix(PS *ps, const char *str)
{
	delete_prefix(ps);
	assert(str);
	ps->prefix = new (ps, strlen(str) + 1);
	strcpy(ps->prefix, str);
}

static PS *init(void *pmgr, picosat_malloc pnew, picosat_realloc presize,
		picosat_free pdelete)
{
	PS *ps;

#if 0
  int count = 3 - !pnew - !presize - !pdelete;

  ABORTIF (count && !pnew, "API usage: missing 'picosat_set_new'");
  ABORTIF (count && !presize, "API usage: missing 'picosat_set_resize'");
  ABORTIF (count && !pdelete, "API usage: missing 'picosat_set_delete'");
#endif

	ps = pnew ? pnew(pmgr, sizeof *ps) : malloc(sizeof *ps);
	ABORTIF(!ps, "failed to allocate memory for PicoSAT manager");
	memset(ps, 0, sizeof *ps);

	ps->emgr = pmgr;
	ps->enew = pnew;
	ps->eresize = presize;
	ps->edelete = pdelete;

	ps->size_vars = 1;
	ps->state = RESET;
	ps->defaultphase = JWLPHASE;
#ifdef TRACE
	ps->ocore = -1;
#endif
	ps->lastrheader = -2;
#ifndef NADC
	ps->adoconflictlimit = UINT_MAX;
#endif
	ps->min_flipped = UINT_MAX;

	NEWN(ps->lits, 2 * ps->size_vars);
	NEWN(ps->jwh, 2 * ps->size_vars);
	NEWN(ps->htps, 2 * ps->size_vars);
#ifndef NDSC
	NEWN(ps->dhtps, 2 * ps->size_vars);
#endif
	NEWN(ps->impls, 2 * ps->size_vars);
	NEWN(ps->vars, ps->size_vars);
	NEWN(ps->rnks, ps->size_vars);

	/* because '0' pos denotes not on heap
   */
	ENLARGE(ps->heap, ps->hhead, ps->eoh);
	ps->hhead = ps->heap + 1;

	ps->vinc = base2flt(1, 0); /* initial var activity */
	ps->ifvinc = ascii2flt("1.05"); /* var score rescore factor */
#ifdef VISCORES
	ps->fvinc = ascii2flt("0.9523809"); /*     1/f =     1/1.05 */
	ps->nvinc = ascii2flt("0.0476191"); /* 1 - 1/f = 1 - 1/1.05 */
#endif
	ps->lscore = base2flt(1, 90); /* var activity rescore limit */
	ps->ilvinc = base2flt(1, -90); /* inverse of 'lscore' */

	ps->cinc = base2flt(1, 0); /* initial clause activity */
	ps->fcinc = ascii2flt("1.001"); /* cls activity rescore factor */
	ps->lcinc = base2flt(1, 90); /* cls activity rescore limit */
	ps->ilcinc = base2flt(1, -90); /* inverse of 'ilcinc' */

	ps->lreduceadjustcnt = ps->lreduceadjustinc = 100;
	ps->lpropagations = ~0ull;

#ifndef RCODE
	ps->out = stdout;
#else
	ps->out = 0;
#endif
	new_prefix(ps, "c ");
	ps->verbosity = 0;
	ps->plain = 0;

#ifdef NO_BINARY_CLAUSES
	memset(&ps->impl, 0, sizeof(ps->impl));
	ps->impl.size = 2;

	memset(&ps->cimpl, 0, sizeof(ps->impl));
	ps->cimpl.size = 2;
#endif

#ifdef VISCORES
	ps->fviscores = popen("/usr/bin/gnuplot -background black"
			      " -xrm 'gnuplot*textColor:white'"
			      " -xrm 'gnuplot*borderColor:white'"
			      " -xrm 'gnuplot*axisColor:white'",
			      "w");
	fprintf(ps->fviscores, "unset key\n");
	// fprintf (ps->fviscores, "set log y\n");
	fflush(ps->fviscores);
	system("rm -rf /tmp/picosat-viscores");
	system("mkdir /tmp/picosat-viscores");
	system("mkdir /tmp/picosat-viscores/data");
#ifdef WRITEGIF
	system("mkdir /tmp/picosat-viscores/gif");
	fprintf(ps->fviscores,
		"set terminal gif giant animate opt size 1024,768 x000000 xffffff"
		"\n");

	fprintf(ps->fviscores,
		"set output \"/tmp/picosat-viscores/gif/animated.gif\"\n");
#endif
#endif
	ps->defaultphase = JWLPHASE;
	ps->state = READY;
	ps->last_sat_call_result = 0;

	return ps;
}

static size_t bytes_clause(PS *ps, unsigned size, unsigned learned)
{
	size_t res;

	res = sizeof(Cls);
	res += size * sizeof(Lit *);
	res -= 2 * sizeof(Lit *);

	if (learned && size > 2)
		res += sizeof(Act); /* add activity */

#ifdef TRACE
	if (ps->trace)
		res += sizeof(Trd); /* add trace data */
#else
	(void)ps;
#endif

	return res;
}

static Cls *new_clause(PS *ps, unsigned size, unsigned learned)
{
	size_t bytes;
	void *tmp;
#ifdef TRACE
	Trd *trd;
#endif
	Cls *res;

	bytes = bytes_clause(ps, size, learned);
	tmp = new (ps, bytes);

#ifdef TRACE
	if (ps->trace) {
		trd = tmp;

		if (learned)
			trd->idx = LIDX2IDX(ps->lhead - ps->lclauses);
		else
			trd->idx = OIDX2IDX(ps->ohead - ps->oclauses);

		res = trd->cls;
	} else
#endif
		res = tmp;

	res->size = size;
	res->learned = learned;

	res->collect = 0;
#ifndef NDEBUG
	res->connected = 0;
#endif
	res->locked = 0;
	res->used = 0;
#ifdef TRACE
	res->core = 0;
	res->collected = 0;
#endif

	if (learned && size > 2) {
		Act *p = CLS2ACT(res);
		*p = ps->cinc;
	}

	return res;
}

static void delete_clause(PS *ps, Cls *c)
{
	size_t bytes;
#ifdef TRACE
	Trd *trd;
#endif

	bytes = bytes_clause(ps, c->size, c->learned);

#ifdef TRACE
	if (ps->trace) {
		trd = CLS2TRD(c);
		delete (ps, trd, bytes);
	} else
#endif
		delete (ps, c, bytes);
}

static void delete_clauses(PS *ps)
{
	Cls **p;
	for (p = SOC; p != EOC; p = NXC(p))
		if (*p)
			delete_clause(ps, *p);

	DELETEN(ps->oclauses, ps->eoo - ps->oclauses);
	DELETEN(ps->lclauses, ps->EOL - ps->lclauses);

	ps->ohead = ps->eoo = ps->lhead = ps->EOL = 0;
}

#ifdef TRACE

static void delete_zhain(PS *ps, Zhn *zhain)
{
	const Znt *p, *znt;

	assert(zhain);

	znt = zhain->znt;
	for (p = znt; *p; p++)
		;

	delete (ps, zhain, sizeof(Zhn) + (p - znt) + 1);
}

static void delete_zhains(PS *ps)
{
	Zhn **p, *z;
	for (p = ps->zhains; p < ps->zhead; p++)
		if ((z = *p))
			delete_zhain(ps, z);

	DELETEN(ps->zhains, ps->eoz - ps->zhains);
	ps->eoz = ps->zhead = 0;
}

#endif

#ifdef NO_BINARY_CLAUSES
static void lrelease(PS *ps, Ltk *stk)
{
	if (stk->start)
		DELETEN(stk->start, (1 << (stk->ldsize)));
	memset(stk, 0, sizeof(*stk));
}
#endif

#ifndef NADC

static unsigned llength(Lit **a)
{
	Lit **p;
	for (p = a; *p; p++)
		;
	return p - a;
}

static void resetadoconflict(PS *ps)
{
	assert(ps->adoconflict);
	delete_clause(ps, ps->adoconflict);
	ps->adoconflict = 0;
}

static void reset_ados(PS *ps)
{
	Lit ***p;

	for (p = ps->ados; p < ps->hados; p++)
		DELETEN(*p, llength(*p) + 1);

	DELETEN(ps->ados, ps->eados - ps->ados);
	ps->hados = ps->eados = 0;

	DELETEN(ps->adotab, ps->szadotab);
	ps->szadotab = ps->nadotab = 0;

	if (ps->adoconflict)
		resetadoconflict(ps);

	ps->adoconflicts = 0;
	ps->adoconflictlimit = UINT_MAX;
	ps->adodisabled = 0;
}

#endif

static void reset(PS *ps)
{
	ABORTIF(!ps || ps->state == RESET,
		"API usage: reset without initialization");

	delete_clauses(ps);
#ifdef TRACE
	delete_zhains(ps);
#endif
#ifdef NO_BINARY_CLAUSES
	{
		unsigned i;
		for (i = 2; i <= 2 * ps->max_var + 1; i++)
			lrelease(ps, ps->impls + i);
	}
#endif
#ifndef NADC
	reset_ados(ps);
#endif
#ifndef NFL
	DELETEN(ps->saved, ps->saved_size);
#endif
	DELETEN(ps->htps, 2 * ps->size_vars);
#ifndef NDSC
	DELETEN(ps->dhtps, 2 * ps->size_vars);
#endif
	DELETEN(ps->impls, 2 * ps->size_vars);
	DELETEN(ps->lits, 2 * ps->size_vars);
	DELETEN(ps->jwh, 2 * ps->size_vars);
	DELETEN(ps->vars, ps->size_vars);
	DELETEN(ps->rnks, ps->size_vars);
	DELETEN(ps->trail, ps->eot - ps->trail);
	DELETEN(ps->heap, ps->eoh - ps->heap);
	DELETEN(ps->als, ps->eoals - ps->als);
	DELETEN(ps->CLS, ps->eocls - ps->CLS);
	DELETEN(ps->rils, ps->eorils - ps->rils);
	DELETEN(ps->cils, ps->eocils - ps->cils);
	DELETEN(ps->fals, ps->eofals - ps->fals);
	DELETEN(ps->mass, ps->szmass);
	DELETEN(ps->mssass, ps->szmssass);
	DELETEN(ps->mcsass, ps->szmcsass);
	DELETEN(ps->humus, ps->szhumus);
	DELETEN(ps->added, ps->eoa - ps->added);
	DELETEN(ps->marked, ps->eom - ps->marked);
	DELETEN(ps->dfs, ps->eod - ps->dfs);
	DELETEN(ps->resolved, ps->eor - ps->resolved);
	DELETEN(ps->levels, ps->eolevels - ps->levels);
	DELETEN(ps->dused, ps->eodused - ps->dused);
	DELETEN(ps->buffer, ps->eob - ps->buffer);
	DELETEN(ps->indices, ps->eoi - ps->indices);
	DELETEN(ps->soclauses, ps->eoso - ps->soclauses);
	delete_prefix(ps);
	delete (ps, ps->rline[0], ps->szrline);
	delete (ps, ps->rline[1], ps->szrline);
	assert(getenv("LEAK") ||
	       !ps->current_bytes); /* found leak if failing */
#ifdef VISCORES
	pclose(ps->fviscores);
#endif
	if (ps->edelete)
		ps->edelete(ps->emgr, ps, sizeof *ps);
	else
		free(ps);
}

inline static void tpush(PS *ps, Lit *lit)
{
	assert(ps->lits < lit && lit <= ps->lits + 2 * ps->max_var + 1);
	if (ps->thead == ps->eot) {
		unsigned ttail2count = ps->ttail2 - ps->trail;
		unsigned ttailcount = ps->ttail - ps->trail;
#ifndef NADC
		unsigned ttailadocount = ps->ttailado - ps->trail;
#endif
		ENLARGE(ps->trail, ps->thead, ps->eot);
		ps->ttail = ps->trail + ttailcount;
		ps->ttail2 = ps->trail + ttail2count;
#ifndef NADC
		ps->ttailado = ps->trail + ttailadocount;
#endif
	}

	*ps->thead++ = lit;
}

static void assign_reason(PS *ps, Var *v, Cls *reason)
{
#if defined(NO_BINARY_CLAUSES) && !defined(NDEBUG)
	assert(reason != &ps->impl);
#else
	(void)ps;
#endif
	v->reason = reason;
}

static void assign_phase(PS *ps, Lit *lit)
{
	unsigned new_phase, idx;
	Var *v = LIT2VAR(lit);

#ifndef NFL
	/* In 'simplifying' mode we only need to keep 'min_flipped' up to date if
   * we force assignments on the top level.   The other assignments will be
   * undone and thus we can keep the old saved value of the phase.
   */
	if (!ps->LEVEL || !ps->simplifying)
#endif
	{
		new_phase = (LIT2SGN(lit) > 0);

		if (v->assigned) {
			ps->sdflips -= ps->sdflips / FFLIPPED;

			if (new_phase != v->phase) {
				assert(FFLIPPEDPREC >= FFLIPPED);
				ps->sdflips += FFLIPPEDPREC / FFLIPPED;
				ps->flips++;

				idx = LIT2IDX(lit);
				if (idx < ps->min_flipped)
					ps->min_flipped = idx;

				NOLOG(fprintf(ps->out, "%sflipped %d\n",
					      ps->prefix, LIT2INT(lit)));
			}
		}

		v->phase = new_phase;
		v->assigned = 1;
	}

	lit->val = TRUE;
	NOTLIT(lit)->val = FALSE;
}

inline static void assign(PS *ps, Lit *lit, Cls *reason)
{
	Var *v = LIT2VAR(lit);
	assert(lit->val == UNDEF);
#ifdef STATS
	ps->assignments++;
#endif
	v->level = ps->LEVEL;
	assign_phase(ps, lit);
	assign_reason(ps, v, reason);
	tpush(ps, lit);
}

inline static int cmp_added(PS *ps, Lit *k, Lit *l)
{
	Val a = k->val, b = l->val;
	Var *u, *v;
	int res;

	if (a == UNDEF && b != UNDEF)
		return -1;

	if (a != UNDEF && b == UNDEF)
		return 1;

	u = LIT2VAR(k);
	v = LIT2VAR(l);

	if (a != UNDEF) {
		assert(b != UNDEF);
		res = v->level - u->level;
		if (res)
			return res; /* larger level first */
	}

	res = cmpflt(VAR2RNK(u)->score, VAR2RNK(v)->score);
	if (res)
		return res; /* smaller activity first */

	return u - v; /* smaller index first */
}

static void sorttwolits(Lit **v)
{
	Lit *a = v[0], *b = v[1];

	assert(a != b);

	if (a < b)
		return;

	v[0] = b;
	v[1] = a;
}

inline static void sortlits(PS *ps, Lit **v, unsigned size)
{
	if (size == 2)
		sorttwolits(
			v); /* same order with and with out 'NO_BINARY_CLAUSES' */
	else
		SORT(Lit *, cmp_added, v, size);
}

#ifdef NO_BINARY_CLAUSES
static Cls *setimpl(PS *ps, Lit *a, Lit *b)
{
	assert(!ps->implvalid);
	assert(ps->impl.size == 2);

	ps->impl.lits[0] = a;
	ps->impl.lits[1] = b;

	sorttwolits(ps->impl.lits);
	ps->implvalid = 1;

	return &ps->impl;
}

static void resetimpl(PS *ps)
{
	ps->implvalid = 0;
}

static Cls *setcimpl(PS *ps, Lit *a, Lit *b)
{
	assert(!ps->cimplvalid);
	assert(ps->cimpl.size == 2);

	ps->cimpl.lits[0] = a;
	ps->cimpl.lits[1] = b;

	sorttwolits(ps->cimpl.lits);
	ps->cimplvalid = 1;

	return &ps->cimpl;
}

static void resetcimpl(PS *ps)
{
	assert(ps->cimplvalid);
	ps->cimplvalid = 0;
}

#endif

static int cmp_ptr(PS *ps, void *l, void *k)
{
	(void)ps;
	return ((char *)l) - (char *)k; /* arbitrarily already reverse */
}

static int cmp_rnk(Rnk *r, Rnk *s)
{
	if (!r->moreimportant && s->moreimportant)
		return -1;

	if (r->moreimportant && !s->moreimportant)
		return 1;

	if (!r->lessimportant && s->lessimportant)
		return 1;

	if (r->lessimportant && !s->lessimportant)
		return -1;

	if (r->score < s->score)
		return -1;

	if (r->score > s->score)
		return 1;

	return -cmp_ptr(0, r, s);
}

static void hup(PS *ps, Rnk *v)
{
	int upos, vpos;
	Rnk *u;

#ifndef NFL
	assert(!ps->simplifying);
#endif

	vpos = v->pos;

	assert(0 < vpos);
	assert(vpos < ps->hhead - ps->heap);
	assert(ps->heap[vpos] == v);

	while (vpos > 1) {
		upos = vpos / 2;

		u = ps->heap[upos];

		if (cmp_rnk(u, v) > 0)
			break;

		ps->heap[vpos] = u;
		u->pos = vpos;

		vpos = upos;
	}

	ps->heap[vpos] = v;
	v->pos = vpos;
}

static Cls *add_simplified_clause(PS *, int);

inline static void add_antecedent(PS *ps, Cls *c)
{
	assert(c);

#ifdef NO_BINARY_CLAUSES
	if (ISLITREASON(c))
		return;

	if (c == &ps->impl)
		return;
#elif defined(STATS) && defined(TRACE)
	ps->antecedents++;
#endif
	if (ps->rhead == ps->eor)
		ENLARGE(ps->resolved, ps->rhead, ps->eor);

	assert(ps->rhead < ps->eor);
	*ps->rhead++ = c;
}

#ifdef TRACE

#ifdef NO_BINARY_CLAUSES
#error "can not combine TRACE and NO_BINARY_CLAUSES"
#endif

#endif /* TRACE */

static void add_lit(PS *ps, Lit *lit)
{
	assert(lit);

	if (ps->ahead == ps->eoa)
		ENLARGE(ps->added, ps->ahead, ps->eoa);

	*ps->ahead++ = lit;
}

static void push_var_as_marked(PS *ps, Var *v)
{
	if (ps->mhead == ps->eom)
		ENLARGE(ps->marked, ps->mhead, ps->eom);

	*ps->mhead++ = v;
}

static void mark_var(PS *ps, Var *v)
{
	assert(!v->mark);
	v->mark = 1;
	push_var_as_marked(ps, v);
}

#ifdef NO_BINARY_CLAUSES

static Cls *impl2reason(PS *ps, Lit *lit)
{
	Lit *other;
	Cls *res;
	other = ps->impl.lits[0];
	if (lit == other)
		other = ps->impl.lits[1];
	assert(other->val == FALSE);
	res = LIT2REASON(NOTLIT(other));
	resetimpl(ps);
	return res;
}

#endif

/* Whenever we have a top level derived unit we really should derive a unit
 * clause otherwise the resolutions in 'add_simplified_clause' become
 * incorrect.
 */
static Cls *resolve_top_level_unit(PS *ps, Lit *lit, Cls *reason)
{
	unsigned count_resolved;
	Lit **p, **eol, *other;
	Var *u, *v;

	assert(ps->rhead == ps->resolved);
	assert(ps->ahead == ps->added);

	add_lit(ps, lit);
	add_antecedent(ps, reason);
	count_resolved = 1;
	v = LIT2VAR(lit);

	eol = end_of_lits(reason);
	for (p = reason->lits; p < eol; p++) {
		other = *p;
		u = LIT2VAR(other);
		if (u == v)
			continue;

		add_antecedent(ps, u->reason);
		count_resolved++;
	}

	/* Some of the literals could be assumptions.  If at least one
   * variable is not an assumption, we should resolve.
   */
	if (count_resolved >= 2) {
#ifdef NO_BINARY_CLAUSES
		if (reason == &ps->impl)
			resetimpl(ps);
#endif
		reason = add_simplified_clause(ps, 1);
#ifdef NO_BINARY_CLAUSES
		if (reason->size == 2) {
			assert(reason == &ps->impl);
			reason = impl2reason(ps, lit);
		}
#endif
		assign_reason(ps, v, reason);
	} else {
		ps->ahead = ps->added;
		ps->rhead = ps->resolved;
	}

	return reason;
}

static void fixvar(PS *ps, Var *v)
{
	Rnk *r;

	assert(VAR2LIT(v) != UNDEF);
	assert(!v->level);

	ps->fixed++;

	r = VAR2RNK(v);
	r->score = INFFLT;

#ifndef NFL
	if (ps->simplifying)
		return;
#endif

	if (!r->pos)
		return;

	hup(ps, r);
}

static void use_var(PS *ps, Var *v)
{
	if (v->used)
		return;

	v->used = 1;
	ps->vused++;
}

static void assign_forced(PS *ps, Lit *lit, Cls *reason)
{
	Var *v;

	assert(reason);
	assert(lit->val == UNDEF);

#ifdef STATS
	ps->FORCED++;
#endif
	assign(ps, lit, reason);

#ifdef NO_BINARY_CLAUSES
	assert(reason != &ps->impl);
	if (ISLITREASON(reason)) {
		reason = setimpl(ps, lit, NOTLIT(REASON2LIT(reason)));
		assert(reason);
	}
#endif
	LOG(fprintf(ps->out, "%sassign %d at level %d by ", ps->prefix,
		    LIT2INT(lit), ps->LEVEL);
	    dumpclsnl(ps, reason));

	v = LIT2VAR(lit);
	if (!ps->LEVEL)
		use_var(ps, v);

	if (!ps->LEVEL && reason->size > 1) {
		reason = resolve_top_level_unit(ps, lit, reason);
		assert(reason);
	}

#ifdef NO_BINARY_CLAUSES
	if (ISLITREASON(reason) || reason == &ps->impl) {
		/* DO NOTHING */
	} else
#endif
	{
		assert(!reason->locked);
		reason->locked = 1;
		if (reason->learned && reason->size > 2)
			ps->llocked++;
	}

#ifdef NO_BINARY_CLAUSES
	if (reason == &ps->impl)
		resetimpl(ps);
#endif

	if (!ps->LEVEL)
		fixvar(ps, v);
}

#ifdef NO_BINARY_CLAUSES

static void lpush(PS *ps, Lit *lit, Cls *c)
{
	int pos = (c->lits[0] == lit);
	Ltk *s = LIT2IMPLS(lit);
	unsigned oldsize, newsize;

	assert(c->size == 2);

	if (!s->start) {
		assert(!s->count);
		assert(!s->ldsize);
		NEWN(s->start, 1);
	} else {
		oldsize = (1 << (s->ldsize));
		assert(s->count <= oldsize);
		if (s->count == oldsize) {
			newsize = 2 * oldsize;
			RESIZEN(s->start, oldsize, newsize);
			s->ldsize++;
		}
	}

	s->start[s->count++] = c->lits[pos];
}

#endif

static void connect_head_tail(PS *ps, Lit *lit, Cls *c)
{
	Cls **s;
	assert(c->size >= 1);
	if (c->size == 2) {
#ifdef NO_BINARY_CLAUSES
		lpush(ps, lit, c);
		return;
#else
		s = LIT2IMPLS(lit);
#endif
	} else
		s = LIT2HTPS(lit);

	if (c->lits[0] != lit) {
		assert(c->size >= 2);
		assert(c->lits[1] == lit);
		c->next[1] = *s;
	} else
		c->next[0] = *s;

	*s = c;
}

#ifdef TRACE
static void zpush(PS *ps, Zhn *zhain)
{
	assert(ps->trace);

	if (ps->zhead == ps->eoz)
		ENLARGE(ps->zhains, ps->zhead, ps->eoz);

	*ps->zhead++ = zhain;
}

static int cmp_resolved(PS *ps, Cls *c, Cls *d)
{
#ifndef NDEBUG
	assert(ps->trace);
#else
	(void)ps;
#endif
	return CLS2IDX(c) - CLS2IDX(d);
}

static void bpushc(PS *ps, unsigned char ch)
{
	if (ps->bhead == ps->eob)
		ENLARGE(ps->buffer, ps->bhead, ps->eob);

	*ps->bhead++ = ch;
}

static void bpushu(PS *ps, unsigned u)
{
	while (u & ~0x7f) {
		bpushc(ps, u | 0x80);
		u >>= 7;
	}

	bpushc(ps, u);
}

static void bpushd(PS *ps, unsigned prev, unsigned this)
{
	unsigned delta;
	assert(prev < this);
	delta = this - prev;
	bpushu(ps, delta);
}

static void add_zhain(PS *ps)
{
	unsigned prev, this, count, rcount;
	Cls **p, *c;
	Zhn *res;

	assert(ps->trace);
	assert(ps->bhead == ps->buffer);
	assert(ps->rhead > ps->resolved);

	rcount = ps->rhead - ps->resolved;
	SORT(Cls *, cmp_resolved, ps->resolved, rcount);

	prev = 0;
	for (p = ps->resolved; p < ps->rhead; p++) {
		c = *p;
		this = CLS2TRD(c)->idx;
		bpushd(ps, prev, this);
		prev = this;
	}
	bpushc(ps, 0);

	count = ps->bhead - ps->buffer;

	res = new (ps, sizeof(Zhn) + count);
	res->core = 0;
	res->ref = 0;
	memcpy(res->znt, ps->buffer, count);

	ps->bhead = ps->buffer;
#ifdef STATS
	ps->znts += count - 1;
#endif
	zpush(ps, res);
}

#endif

static void add_resolved(PS *ps, int learned)
{
#if defined(STATS) || defined(TRACE)
	Cls **p, *c;

	for (p = ps->resolved; p < ps->rhead; p++) {
		c = *p;
		if (c->used)
			continue;

		c->used = 1;

		if (c->size <= 2)
			continue;

#ifdef STATS
		if (c->learned)
			ps->llused++;
		else
			ps->loused++;
#endif
	}
#endif

#ifdef TRACE
	if (learned && ps->trace)
		add_zhain(ps);
#else
	(void)learned;
#endif
	ps->rhead = ps->resolved;
}

static void incjwh(PS *ps, Cls *c)
{
	Lit **p, *lit, **eol;
	Flt *f, inc, sum;
	unsigned size = 0;
	Var *v;
	Val val;

	eol = end_of_lits(c);

	for (p = c->lits; p < eol; p++) {
		lit = *p;
		val = lit->val;

		if (val && ps->LEVEL > 0) {
			v = LIT2VAR(lit);
			if (v->level > 0)
				val = UNDEF;
		}

		if (val == TRUE)
			return;

		if (val != FALSE)
			size++;
	}

	inc = base2flt(1, -size);

	for (p = c->lits; p < eol; p++) {
		lit = *p;
		f = LIT2JWH(lit);
		sum = addflt(*f, inc);
		*f = sum;
	}
}

static void write_rup_header(PS *ps, FILE *file)
{
	char line[80];
	int i;

	sprintf(line, "%%RUPD32 %u %u", ps->rupvariables, ps->rupclauses);

	fputs(line, file);
	for (i = 255 - strlen(line); i >= 0; i--)
		fputc(' ', file);

	fputc('\n', file);
	fflush(file);
}

static Cls *add_simplified_clause(PS *ps, int learned)
{
	unsigned num_true, num_undef, num_false, size, count_resolved;
	Lit **p, **q, *lit, **end;
	unsigned litlevel, glue;
	Cls *res, *reason;
	int reentered;
	Val val;
	Var *v;
#if !defined(NDEBUG) && defined(TRACE)
	unsigned idx;
#endif

	reentered = 0;

REENTER:

	size = ps->ahead - ps->added;

	add_resolved(ps, learned);

	if (learned) {
		ps->ladded++;
		ps->llitsadded += size;
		if (size > 2) {
			ps->lladded++;
			ps->nlclauses++;
			ps->llits += size;
		}
	} else {
		ps->oadded++;
		if (size > 2) {
			ps->loadded++;
			ps->noclauses++;
			ps->olits += size;
		}
	}

	ps->addedclauses++;
	assert(ps->addedclauses == ps->ladded + ps->oadded);

#ifdef NO_BINARY_CLAUSES
	if (size == 2)
		res = setimpl(ps, ps->added[0], ps->added[1]);
	else
#endif
	{
		sortlits(ps, ps->added, size);

		if (learned) {
			if (ps->lhead == ps->EOL) {
				ENLARGE(ps->lclauses, ps->lhead, ps->EOL);

				/* A very difficult to find bug, which only occurs if the
	       * learned clauses stack is immediately allocated before the
	       * original clauses stack without padding.  In this case, we
	       * have 'SOC == EOC', which terminates all loops using the
	       * idiom 'for (p = SOC; p != EOC; p = NXC(p))' immediately.
	       * Unfortunately this occurred in 'fix_clause_lits' after
	       * using a recent version of the memory allocator of 'Google'
	       * perftools in the context of one large benchmark for
	       * our SMT solver 'Boolector'.
	       */
				if (ps->EOL == ps->oclauses)
					ENLARGE(ps->lclauses, ps->lhead,
						ps->EOL);
			}

#if !defined(NDEBUG) && defined(TRACE)
			idx = LIDX2IDX(ps->lhead - ps->lclauses);
#endif
		} else {
			if (ps->ohead == ps->eoo) {
				ENLARGE(ps->oclauses, ps->ohead, ps->eoo);
				if (ps->EOL == ps->oclauses)
					ENLARGE(ps->oclauses, ps->ohead,
						ps->eoo); /* ditto */
			}

#if !defined(NDEBUG) && defined(TRACE)
			idx = OIDX2IDX(ps->ohead - ps->oclauses);
#endif
		}

		assert(ps->EOL != ps->oclauses); /* ditto */

		res = new_clause(ps, size, learned);

		glue = 0;

		if (learned) {
			assert(ps->dusedhead == ps->dused);

			for (p = ps->added; p < ps->ahead; p++) {
				lit = *p;
				if (lit->val) {
					litlevel = LIT2VAR(lit)->level;
					assert(litlevel <= ps->LEVEL);
					while (ps->levels + litlevel >=
					       ps->levelshead) {
						if (ps->levelshead >=
						    ps->eolevels)
							ENLARGE(ps->levels,
								ps->levelshead,
								ps->eolevels);
						assert(ps->levelshead <
						       ps->eolevels);
						*ps->levelshead++ = 0;
					}
					if (!ps->levels[litlevel]) {
						if (ps->dusedhead >=
						    ps->eodused)
							ENLARGE(ps->dused,
								ps->dusedhead,
								ps->eodused);
						assert(ps->dusedhead <
						       ps->eodused);
						*ps->dusedhead++ = litlevel;
						ps->levels[litlevel] = 1;
						glue++;
					}
				} else
					glue++;
			}

			while (ps->dusedhead > ps->dused) {
				litlevel = *--ps->dusedhead;
				assert(ps->levels + litlevel < ps->levelshead);
				assert(ps->levels[litlevel]);
				ps->levels[litlevel] = 0;
			}
		}

		assert(glue <= MAXGLUE);
		res->glue = glue;

#if !defined(NDEBUG) && defined(TRACE)
		if (ps->trace)
			assert(CLS2IDX(res) == idx);
#endif
		if (learned)
			*ps->lhead++ = res;
		else
			*ps->ohead++ = res;

#if !defined(NDEBUG) && defined(TRACE)
		if (ps->trace && learned)
			assert(ps->zhead - ps->zhains ==
			       ps->lhead - ps->lclauses);
#endif
		assert(ps->lhead != ps->oclauses); /* ditto */
	}

	if (learned && ps->rup) {
		if (!ps->rupstarted) {
			write_rup_header(ps, ps->rup);
			ps->rupstarted = 1;
		}
	}

	num_true = num_undef = num_false = 0;

	q = res->lits;
	for (p = ps->added; p < ps->ahead; p++) {
		lit = *p;
		*q++ = lit;

		if (learned && ps->rup)
			fprintf(ps->rup, "%d ", LIT2INT(lit));

		val = lit->val;

		num_true += (val == TRUE);
		num_undef += (val == UNDEF);
		num_false += (val == FALSE);
	}
	assert(num_false + num_true + num_undef == size);

	if (learned && ps->rup)
		fputs("0\n", ps->rup);

	ps->ahead = ps->added; /* reset */

	if (!reentered) // TODO merge
		if (size > 0) {
			assert(size <= 2 || !reentered); // TODO remove
			connect_head_tail(ps, res->lits[0], res);
			if (size > 1)
				connect_head_tail(ps, res->lits[1], res);
		}

	if (size == 0) {
		if (!ps->mtcls)
			ps->mtcls = res;
	}

#ifdef NO_BINARY_CLAUSES
	if (size != 2)
#endif
#ifndef NDEBUG
		res->connected = 1;
#endif

	LOG(fprintf(ps->out, "%s%s ", ps->prefix,
		    learned ? "learned" : "original");
	    dumpclsnl(ps, res));

	/* Shrink clause by resolving it against top level assignments.
   */
	if (!ps->LEVEL && num_false > 0) {
		assert(ps->ahead == ps->added);
		assert(ps->rhead == ps->resolved);

		count_resolved = 1;
		add_antecedent(ps, res);

		end = end_of_lits(res);
		for (p = res->lits; p < end; p++) {
			lit = *p;
			v = LIT2VAR(lit);
			use_var(ps, v);

			if (lit->val == FALSE) {
				add_antecedent(ps, v->reason);
				count_resolved++;
			} else
				add_lit(ps, lit);
		}

		assert(count_resolved >= 2);

		learned = 1;
#ifdef NO_BINARY_CLAUSES
		if (res == &ps->impl)
			resetimpl(ps);
#endif
		reentered = 1;
		goto REENTER; /* and return simplified clause */
	}

	if (!num_true && num_undef == 1) /* unit clause */
	{
		lit = 0;
		for (p = res->lits; p < res->lits + size; p++) {
			if ((*p)->val == UNDEF)
				lit = *p;

			v = LIT2VAR(*p);
			use_var(ps, v);
		}
		assert(lit);

		reason = res;
#ifdef NO_BINARY_CLAUSES
		if (size == 2) {
			Lit *other = res->lits[0];
			if (other == lit)
				other = res->lits[1];

			assert(other->val == FALSE);
			reason = LIT2REASON(NOTLIT(other));
		}
#endif
		assign_forced(ps, lit, reason);
		num_true++;
	}

	if (num_false == size && !ps->conflict) {
#ifdef NO_BINARY_CLAUSES
		if (res == &ps->impl)
			ps->conflict = setcimpl(ps, res->lits[0], res->lits[1]);
		else
#endif
			ps->conflict = res;
	}

	if (!learned && !num_true && num_undef)
		incjwh(ps, res);

#ifdef NO_BINARY_CLAUSES
	if (res == &ps->impl)
		resetimpl(ps);
#endif
	return res;
}

static int trivial_clause(PS *ps)
{
	Lit **p, **q, *prev;
	Var *v;

	SORT(Lit *, cmp_ptr, ps->added, ps->ahead - ps->added);

	prev = 0;
	q = ps->added;
	for (p = q; p < ps->ahead; p++) {
		Lit *this = *p;

		v = LIT2VAR(this);

		if (prev == this) /* skip repeated literals */
			continue;

		/* Top level satisfied ?
       */
		if (this->val == TRUE && !v->level)
			return 1;

		if (prev == NOTLIT(this)) /* found pair of dual literals */
			return 1;

		*q++ = prev = this;
	}

	ps->ahead = q; /* shrink */

	return 0;
}

static void simplify_and_add_original_clause(PS *ps)
{
#ifdef NO_BINARY_CLAUSES
	Cls *c;
#endif
	if (trivial_clause(ps)) {
		ps->ahead = ps->added;

		if (ps->ohead == ps->eoo)
			ENLARGE(ps->oclauses, ps->ohead, ps->eoo);

		*ps->ohead++ = 0;

		ps->addedclauses++;
		ps->oadded++;
	} else {
		if (ps->CLS != ps->clshead)
			add_lit(ps, NOTLIT(ps->clshead[-1]));

#ifdef NO_BINARY_CLAUSES
		c =
#endif
			add_simplified_clause(ps, 0);
#ifdef NO_BINARY_CLAUSES
		if (c == &ps->impl)
			assert(!ps->implvalid);
#endif
	}
}

#ifndef NADC

static void add_ado(PS *ps)
{
	unsigned len = ps->ahead - ps->added;
	Lit **ado, **p, **q, *lit;
	Var *v, *u;

#ifdef TRACE
	assert(!ps->trace);
#endif

	ABORTIF(ps->ados < ps->hados && llength(ps->ados[0]) != len,
		"internal: non matching all different constraint object lengths");

	if (ps->hados == ps->eados)
		ENLARGE(ps->ados, ps->hados, ps->eados);

	NEWN(ado, len + 1);
	*ps->hados++ = ado;

	p = ps->added;
	q = ado;
	u = 0;
	while (p < ps->ahead) {
		lit = *p++;
		v = LIT2VAR(lit);
		ABORTIF(v->inado,
			"internal: variable in multiple all different objects");
		v->inado = ado;
		if (!u && !lit->val)
			u = v;
		*q++ = lit;
	}

	assert(q == ado + len);
	*q++ = 0;

	/* TODO simply do a conflict test as in propado */

	ABORTIF(!u,
		"internal: "
		"adding fully instantiated all different object not implemented yet");

	assert(u);
	assert(u->inado == ado);
	assert(!u->ado);
	u->ado = ado;

	ps->ahead = ps->added;
}

#endif

static void hdown(PS *ps, Rnk *r)
{
	unsigned end, rpos, cpos, opos;
	Rnk *child, *other;

	assert(r->pos > 0);
	assert(ps->heap[r->pos] == r);

	end = ps->hhead - ps->heap;
	rpos = r->pos;

	for (;;) {
		cpos = 2 * rpos;
		if (cpos >= end)
			break;

		opos = cpos + 1;
		child = ps->heap[cpos];

		if (cmp_rnk(r, child) < 0) {
			if (opos < end) {
				other = ps->heap[opos];

				if (cmp_rnk(child, other) < 0) {
					child = other;
					cpos = opos;
				}
			}
		} else if (opos < end) {
			child = ps->heap[opos];

			if (cmp_rnk(r, child) >= 0)
				break;

			cpos = opos;
		} else
			break;

		ps->heap[rpos] = child;
		child->pos = rpos;
		rpos = cpos;
	}

	r->pos = rpos;
	ps->heap[rpos] = r;
}

static Rnk *htop(PS *ps)
{
	assert(ps->hhead > ps->heap + 1);
	return ps->heap[1];
}

static Rnk *hpop(PS *ps)
{
	Rnk *res, *last;
	unsigned end;

	assert(ps->hhead > ps->heap + 1);

	res = ps->heap[1];
	res->pos = 0;

	end = --ps->hhead - ps->heap;
	if (end == 1)
		return res;

	last = ps->heap[end];

	ps->heap[last->pos = 1] = last;
	hdown(ps, last);

	return res;
}

inline static void hpush(PS *ps, Rnk *r)
{
	assert(!r->pos);

	if (ps->hhead == ps->eoh)
		ENLARGE(ps->heap, ps->hhead, ps->eoh);

	r->pos = ps->hhead++ - ps->heap;
	ps->heap[r->pos] = r;
	hup(ps, r);
}

static void fix_trail_lits(PS *ps, long delta)
{
	Lit **p;
	for (p = ps->trail; p < ps->thead; p++)
		*p += delta;
}

#ifdef NO_BINARY_CLAUSES
static void fix_impl_lits(PS *ps, long delta)
{
	Ltk *s;
	Lit **p;

	for (s = ps->impls + 2; s <= ps->impls + 2 * ps->max_var + 1; s++)
		for (p = s->start; p < s->start + s->count; p++)
			*p += delta;
}
#endif

static void fix_clause_lits(PS *ps, long delta)
{
	Cls **p, *clause;
	Lit **q, *lit, **eol;

	for (p = SOC; p != EOC; p = NXC(p)) {
		clause = *p;
		if (!clause)
			continue;

		q = clause->lits;
		eol = end_of_lits(clause);
		while (q < eol) {
			assert(q - clause->lits <= (int)clause->size);
			lit = *q;
			lit += delta;
			*q++ = lit;
		}
	}
}

static void fix_added_lits(PS *ps, long delta)
{
	Lit **p;
	for (p = ps->added; p < ps->ahead; p++)
		*p += delta;
}

static void fix_assumed_lits(PS *ps, long delta)
{
	Lit **p;
	for (p = ps->als; p < ps->alshead; p++)
		*p += delta;
}

static void fix_cls_lits(PS *ps, long delta)
{
	Lit **p;
	for (p = ps->CLS; p < ps->clshead; p++)
		*p += delta;
}

static void fix_heap_rnks(PS *ps, long delta)
{
	Rnk **p;

	for (p = ps->heap + 1; p < ps->hhead; p++)
		*p += delta;
}

#ifndef NADC

static void fix_ado(long delta, Lit **ado)
{
	Lit **p;
	for (p = ado; *p; p++)
		*p += delta;
}

static void fix_ados(PS *ps, long delta)
{
	Lit ***p;

	for (p = ps->ados; p < ps->hados; p++)
		fix_ado(delta, *p);
}

#endif

static void enlarge(PS *ps, unsigned new_size_vars)
{
	long rnks_delta, lits_delta;
	Lit *old_lits = ps->lits;
	Rnk *old_rnks = ps->rnks;

	RESIZEN(ps->lits, 2 * ps->size_vars, 2 * new_size_vars);
	RESIZEN(ps->jwh, 2 * ps->size_vars, 2 * new_size_vars);
	RESIZEN(ps->htps, 2 * ps->size_vars, 2 * new_size_vars);
#ifndef NDSC
	RESIZEN(ps->dhtps, 2 * ps->size_vars, 2 * new_size_vars);
#endif
	RESIZEN(ps->impls, 2 * ps->size_vars, 2 * new_size_vars);
	RESIZEN(ps->vars, ps->size_vars, new_size_vars);
	RESIZEN(ps->rnks, ps->size_vars, new_size_vars);

	if ((lits_delta = ps->lits - old_lits)) {
		fix_trail_lits(ps, lits_delta);
		fix_clause_lits(ps, lits_delta);
		fix_added_lits(ps, lits_delta);
		fix_assumed_lits(ps, lits_delta);
		fix_cls_lits(ps, lits_delta);
#ifdef NO_BINARY_CLAUSES
		fix_impl_lits(ps, lits_delta);
#endif
#ifndef NADC
		fix_ados(ps, lits_delta);
#endif
	}

	if ((rnks_delta = ps->rnks - old_rnks)) {
		fix_heap_rnks(ps, rnks_delta);
	}

	assert(ps->mhead == ps->marked);

	ps->size_vars = new_size_vars;
}

static void unassign(PS *ps, Lit *lit)
{
	Cls *reason;
	Var *v;
	Rnk *r;

	assert(lit->val == TRUE);

	LOG(fprintf(ps->out, "%sunassign %d\n", ps->prefix, LIT2INT(lit)));

	v = LIT2VAR(lit);
	reason = v->reason;

#ifdef NO_BINARY_CLAUSES
	assert(reason != &ps->impl);
	if (ISLITREASON(reason)) {
		/* DO NOTHING */
	} else
#endif
		if (reason) {
		assert(reason->locked);
		reason->locked = 0;
		if (reason->learned && reason->size > 2) {
			assert(ps->llocked > 0);
			ps->llocked--;
		}
	}

	lit->val = UNDEF;
	NOTLIT(lit)->val = UNDEF;

	r = VAR2RNK(v);
	if (!r->pos)
		hpush(ps, r);

#ifndef NDSC
	{
		Cls *p, *next, **q;

		q = LIT2DHTPS(lit);
		p = *q;
		*q = 0;

		while (p) {
			Lit *other = p->lits[0];

			if (other == lit) {
				other = p->lits[1];
				q = p->next + 1;
			} else {
				assert(p->lits[1] == lit);
				q = p->next;
			}

			next = *q;
			*q = *LIT2HTPS(other);
			*LIT2HTPS(other) = p;
			p = next;
		}
	}
#endif

#ifndef NADC
	if (v->adotabpos) {
		assert(ps->nadotab);
		assert(*v->adotabpos == v->ado);

		*v->adotabpos = 0;
		v->adotabpos = 0;

		ps->nadotab--;
	}
#endif
}

static Cls *var2reason(PS *ps, Var *var)
{
	Cls *res = var->reason;
#ifdef NO_BINARY_CLAUSES
	Lit *this, *other;
	if (ISLITREASON(res)) {
		this = VAR2LIT(var);
		if (this->val == FALSE)
			this = NOTLIT(this);

		other = REASON2LIT(res);
		assert(other->val == TRUE);
		assert(this->val == TRUE);
		res = setimpl(ps, NOTLIT(other), this);
	}
#else
	(void)ps;
#endif
	return res;
}

static void mark_clause_to_be_collected(Cls *c)
{
	assert(!c->collect);
	c->collect = 1;
}

static void undo(PS *ps, unsigned new_level)
{
	Lit *lit;
	Var *v;

	while (ps->thead > ps->trail) {
		lit = *--ps->thead;
		v = LIT2VAR(lit);
		if (v->level == new_level) {
			ps->thead++; /* fix pre decrement */
			break;
		}

		unassign(ps, lit);
	}

	ps->LEVEL = new_level;
	ps->ttail = ps->thead;
	ps->ttail2 = ps->thead;
#ifndef NADC
	ps->ttailado = ps->thead;
#endif

#ifdef NO_BINARY_CLAUSES
	if (ps->conflict == &ps->cimpl)
		resetcimpl(ps);
#endif
#ifndef NADC
	if (ps->conflict && ps->conflict == ps->adoconflict)
		resetadoconflict(ps);
#endif
	ps->conflict = ps->mtcls;
	if (ps->LEVEL < ps->adecidelevel) {
		assert(ps->als < ps->alshead);
		ps->adecidelevel = 0;
		ps->alstail = ps->als;
	}
	LOG(fprintf(ps->out, "%sback to level %u\n", ps->prefix, ps->LEVEL));
}

#ifndef NDEBUG

static int clause_satisfied(Cls *c)
{
	Lit **p, **eol, *lit;

	eol = end_of_lits(c);
	for (p = c->lits; p < eol; p++) {
		lit = *p;
		if (lit->val == TRUE)
			return 1;
	}

	return 0;
}

static void original_clauses_satisfied(PS *ps)
{
	Cls **p, *c;

	for (p = ps->oclauses; p < ps->ohead; p++) {
		c = *p;

		if (!c)
			continue;

		if (c->learned)
			continue;

		assert(clause_satisfied(c));
	}
}

static void assumptions_satisfied(PS *ps)
{
	Lit *lit, **p;

	for (p = ps->als; p < ps->alshead; p++) {
		lit = *p;
		assert(lit->val == TRUE);
	}
}

#endif

static void sflush(PS *ps)
{
	double now = picosat_time_stamp();
	double delta = now - ps->entered;
	delta = (delta < 0) ? 0 : delta;
	ps->seconds += delta;
	ps->entered = now;
}

static double mb(PS *ps)
{
	return ps->current_bytes / (double)(1 << 20);
}

static double avglevel(PS *ps)
{
	return ps->decisions ? ps->levelsum / ps->decisions : 0.0;
}

static void rheader(PS *ps)
{
	assert(ps->lastrheader <= ps->reports);

	if (ps->lastrheader == ps->reports)
		return;

	ps->lastrheader = ps->reports;

	fprintf(ps->out, "%s\n", ps->prefix);
	fprintf(ps->out, "%s %s\n", ps->prefix, ps->rline[0]);
	fprintf(ps->out, "%s %s\n", ps->prefix, ps->rline[1]);
	fprintf(ps->out, "%s\n", ps->prefix);
}

static unsigned dynamic_flips_per_assignment_per_mille(PS *ps)
{
	assert(FFLIPPEDPREC >= 1000);
	return ps->sdflips / (FFLIPPEDPREC / 1000);
}

#ifdef NLUBY

static int high_agility(PS *ps)
{
	return dynamic_flips_per_assignment_per_mille(ps) >= 200;
}

static int very_high_agility(PS *ps)
{
	return dynamic_flips_per_assignment_per_mille(ps) >= 250;
}

#else

static int medium_agility(PS *ps)
{
	return dynamic_flips_per_assignment_per_mille(ps) >= 230;
}

#endif

static void relemdata(PS *ps)
{
	char *p;
	int x;

	if (ps->reports < 0) {
		/* strip trailing white space
       */
		for (x = 0; x <= 1; x++) {
			p = ps->rline[x] + strlen(ps->rline[x]);
			while (p-- > ps->rline[x]) {
				if (*p != ' ')
					break;

				*p = 0;
			}
		}

		rheader(ps);
	} else
		fputc('\n', ps->out);

	ps->RCOUNT = 0;
}

static void relemhead(PS *ps, const char *name, int fp, double val)
{
	int x, y, len, size;
	const char *fmt;
	unsigned tmp, e;

	if (ps->reports < 0) {
		x = ps->RCOUNT & 1;
		y = (ps->RCOUNT / 2) * 12 + x * 6;

		if (ps->RCOUNT == 1)
			sprintf(ps->rline[1], "%6s", "");

		len = strlen(name);
		while (ps->szrline <= len + y + 1) {
			size = ps->szrline ? 2 * ps->szrline : 128;
			ps->rline[0] =
				resize(ps, ps->rline[0], ps->szrline, size);
			ps->rline[1] =
				resize(ps, ps->rline[1], ps->szrline, size);
			ps->szrline = size;
		}

		fmt = (len <= 6) ? "%6s%10s" : "%-10s%4s";
		sprintf(ps->rline[x] + y, fmt, name, "");
	} else if (val < 0) {
		assert(fp);

		if (val > -100 && (tmp = val * 10.0 - 0.5) > -1000.0) {
			fprintf(ps->out, "-%4.1f ", -tmp / 10.0);
		} else {
			tmp = -val / 10.0 + 0.5;
			e = 1;
			while (tmp >= 100) {
				tmp /= 10;
				e++;
			}

			fprintf(ps->out, "-%2ue%u ", tmp, e);
		}
	} else {
		if (fp && val < 1000 && (tmp = val * 10.0 + 0.5) < 10000) {
			fprintf(ps->out, "%5.1f ", tmp / 10.0);
		} else if (!fp && (tmp = val) < 100000) {
			fprintf(ps->out, "%5u ", tmp);
		} else {
			tmp = val / 10.0 + 0.5;
			e = 1;

			while (tmp >= 1000) {
				tmp /= 10;
				e++;
			}

			fprintf(ps->out, "%3ue%u ", tmp, e);
		}
	}

	ps->RCOUNT++;
}

inline static void relem(PS *ps, const char *name, int fp, double val)
{
	if (name)
		relemhead(ps, name, fp, val);
	else
		relemdata(ps);
}

static unsigned reduce_limit_on_lclauses(PS *ps)
{
	unsigned res = ps->lreduce;
	res += ps->llocked;
	return res;
}

static void report(PS *ps, int replevel, char type)
{
	int rounds;

#ifdef RCODE
	(void)type;
#endif

	if (ps->verbosity < replevel)
		return;

	sflush(ps);

	if (!ps->reports)
		ps->reports = -1;

	for (rounds = (ps->reports < 0) ? 2 : 1; rounds; rounds--) {
		if (ps->reports >= 0)
			fprintf(ps->out, "%s%c ", ps->prefix, type);

		relem(ps, "seconds", 1, ps->seconds);
		relem(ps, "level", 1, avglevel(ps));
		assert(ps->fixed <= ps->max_var);
		relem(ps, "variables", 0, ps->max_var - ps->fixed);
		relem(ps, "used", 1, PERCENT(ps->vused, ps->max_var));
		relem(ps, "original", 0, ps->noclauses);
		relem(ps, "conflicts", 0, ps->conflicts);
		// relem (ps, "decisions", 0, ps->decisions);
		// relem (ps, "conf/dec", 1, PERCENT(ps->conflicts,ps->decisions));
		// relem (ps, "limit", 0, reduce_limit_on_lclauses (ps));
		relem(ps, "learned", 0, ps->nlclauses);
		// relem (ps, "limit", 1, PERCENT (ps->nlclauses, reduce_limit_on_lclauses (ps)));
		relem(ps, "limit", 0, ps->lreduce);
#ifdef STATS
		relem(ps, "learning", 1, PERCENT(ps->llused, ps->lladded));
#endif
		relem(ps, "agility", 1,
		      dynamic_flips_per_assignment_per_mille(ps) / 10.0);
		// relem (ps, "original", 0, ps->noclauses);
		relem(ps, "MB", 1, mb(ps));
		// relem (ps, "lladded", 0, ps->lladded);
		// relem (ps, "llused", 0, ps->llused);

		relem(ps, 0, 0, 0);

		ps->reports++;
	}

/* Adapt this to the number of rows in your terminal.
   */
#define ROWS 25

	if (ps->reports % (ROWS - 3) == (ROWS - 4))
		rheader(ps);

	fflush(ps->out);
}

static int bcp_queue_is_empty(PS *ps)
{
	if (ps->ttail != ps->thead)
		return 0;

	if (ps->ttail2 != ps->thead)
		return 0;

#ifndef NADC
	if (ps->ttailado != ps->thead)
		return 0;
#endif

	return 1;
}

static int satisfied(PS *ps)
{
	assert(!ps->mtcls);
	assert(!ps->failed_assumption);
	if (ps->alstail < ps->alshead)
		return 0;
	assert(!ps->conflict);
	assert(bcp_queue_is_empty(ps));
	return ps->thead == ps->trail + ps->max_var; /* all assigned */
}

static void vrescore(PS *ps)
{
	Rnk *p, *eor = ps->rnks + ps->max_var;
	for (p = ps->rnks + 1; p <= eor; p++)
		if (p->score != INFFLT)
			p->score = mulflt(p->score, ps->ilvinc);
	ps->vinc = mulflt(ps->vinc, ps->ilvinc);
	;
#ifdef VISCORES
	ps->nvinc = mulflt(ps->nvinc, ps->lscore);
	;
#endif
}

static void inc_score(PS *ps, Var *v)
{
	Flt score;
	Rnk *r;

#ifndef NFL
	if (ps->simplifying)
		return;
#endif

	if (!v->level)
		return;

	if (v->internal)
		return;

	r = VAR2RNK(v);
	score = r->score;

	assert(score != INFFLT);

	score = addflt(score, ps->vinc);
	assert(score < INFFLT);
	r->score = score;
	if (r->pos > 0)
		hup(ps, r);

	if (score > ps->lscore)
		vrescore(ps);
}

static void inc_activity(PS *ps, Cls *c)
{
	Act *p;

	if (!c->learned)
		return;

	if (c->size <= 2)
		return;

	p = CLS2ACT(c);
	*p = addflt(*p, ps->cinc);
}

static unsigned hashlevel(unsigned l)
{
	return 1u << (l & 31);
}

static void push(PS *ps, Var *v)
{
	if (ps->dhead == ps->eod)
		ENLARGE(ps->dfs, ps->dhead, ps->eod);

	*ps->dhead++ = v;
}

static Var *pop(PS *ps)
{
	assert(ps->dfs < ps->dhead);
	return *--ps->dhead;
}

static void analyze(PS *ps)
{
	unsigned open, minlevel, siglevels, l, old, i, orig;
	Lit *this, *other, **p, **q, **eol;
	Var *v, *u, **m, *start, *uip;
	Cls *c;

	assert(ps->conflict);

	assert(ps->ahead == ps->added);
	assert(ps->mhead == ps->marked);
	assert(ps->rhead == ps->resolved);

	/* First, search for First UIP variable and mark all resolved variables.
   * At the same time determine the minimum decision level involved.
   * Increase activities of resolved variables.
   */
	q = ps->thead;
	open = 0;
	minlevel = ps->LEVEL;
	siglevels = 0;
	uip = 0;

	c = ps->conflict;

	for (;;) {
		add_antecedent(ps, c);
		inc_activity(ps, c);
		eol = end_of_lits(c);
		for (p = c->lits; p < eol; p++) {
			other = *p;

			if (other->val == TRUE)
				continue;

			assert(other->val == FALSE);

			u = LIT2VAR(other);
			if (u->mark)
				continue;

			u->mark = 1;
			inc_score(ps, u);
			use_var(ps, u);

			if (u->level == ps->LEVEL) {
				open++;
			} else {
				push_var_as_marked(ps, u);

				if (u->level) {
					/* The statistics counter 'nonminimizedllits' sums up the
		   * number of literals that would be added if only the
		   * 'first UIP' scheme for learned clauses would be used
		   * and no clause minimization.
		   */
					ps->nonminimizedllits++;

					if (u->level < minlevel)
						minlevel = u->level;

					siglevels |= hashlevel(u->level);
				} else {
					assert(!u->level);
					assert(u->reason);
				}
			}
		}

		do {
			if (q == ps->trail) {
				uip = 0;
				goto DONE_FIRST_UIP;
			}

			this = *--q;
			uip = LIT2VAR(this);
		} while (!uip->mark);

		uip->mark = 0;

		c = var2reason(ps, uip);
#ifdef NO_BINARY_CLAUSES
		if (c == &ps->impl)
			resetimpl(ps);
#endif
		open--;
		if ((!open && ps->LEVEL) || !c)
			break;

		assert(c);
	}

DONE_FIRST_UIP:

	if (uip) {
		assert(ps->LEVEL);
		this = VAR2LIT(uip);
		this += (this->val == TRUE);
		ps->nonminimizedllits++;
		ps->minimizedllits++;
		add_lit(ps, this);
#ifdef STATS
		if (uip->reason)
			ps->uips++;
#endif
	} else
		assert(!ps->LEVEL);

	/* Second, try to mark more intermediate variables, with the goal to
   * minimize the conflict clause.  This is a DFS from already marked
   * variables backward through the implication graph.  It tries to reach
   * other marked variables.  If the search reaches an unmarked decision
   * variable or a variable assigned below the minimum level of variables in
   * the first uip learned clause or a level on which no variable has been
   * marked, then the variable from which the DFS is started is not
   * redundant.  Otherwise the start variable is redundant and will
   * eventually be removed from the learned clause in step 4.  We initially
   * implemented BFS, but then profiling revelead that this step is a bottle
   * neck for certain incremental applications.  After switching to DFS this
   * hot spot went away.
   */
	orig = ps->mhead - ps->marked;
	for (i = 0; i < orig; i++) {
		start = ps->marked[i];

		assert(start->mark);
		assert(start != uip);
		assert(start->level < ps->LEVEL);

		if (!start->reason)
			continue;

		old = ps->mhead - ps->marked;
		assert(ps->dhead == ps->dfs);
		push(ps, start);

		while (ps->dhead > ps->dfs) {
			u = pop(ps);
			assert(u->mark);

			c = var2reason(ps, u);
#ifdef NO_BINARY_CLAUSES
			if (c == &ps->impl)
				resetimpl(ps);
#endif
			if (!c ||
			    ((l = u->level) &&
			     (l < minlevel || ((hashlevel(l) & ~siglevels))))) {
				while (ps->mhead >
				       ps->marked + old) /* reset all marked */
					(*--ps->mhead)->mark = 0;

				ps->dhead = ps->dfs; /* and DFS stack */
				break;
			}

			eol = end_of_lits(c);
			for (p = c->lits; p < eol; p++) {
				v = LIT2VAR(*p);
				if (v->mark)
					continue;

				mark_var(ps, v);
				push(ps, v);
			}
		}
	}

	for (m = ps->marked; m < ps->mhead; m++) {
		v = *m;

		assert(v->mark);
		assert(!v->resolved);

		use_var(ps, v);

		c = var2reason(ps, v);
		if (!c)
			continue;

#ifdef NO_BINARY_CLAUSES
		if (c == &ps->impl)
			resetimpl(ps);
#endif
		eol = end_of_lits(c);
		for (p = c->lits; p < eol; p++) {
			other = *p;

			u = LIT2VAR(other);
			if (!u->level)
				continue;

			if (!u->mark) /* 'MARKTEST' */
				break;
		}

		if (p != eol)
			continue;

		add_antecedent(ps, c);
		v->resolved = 1;
	}

	for (m = ps->marked; m < ps->mhead; m++) {
		v = *m;

		assert(v->mark);
		v->mark = 0;

		if (v->resolved) {
			v->resolved = 0;
			continue;
		}

		this = VAR2LIT(v);
		if (this->val == TRUE)
			this ++; /* actually NOTLIT */

		add_lit(ps, this);
		ps->minimizedllits++;
	}

	assert(ps->ahead <= ps->eoa);
	assert(ps->rhead <= ps->eor);

	ps->mhead = ps->marked;
}

static void fanalyze(PS *ps)
{
	Lit **eol, **p, *lit;
	Cls *c, *reason;
	Var *v, *u;
	int next;

#ifndef RCODE
	double start = picosat_time_stamp();
#endif

	assert(ps->failed_assumption);
	assert(ps->failed_assumption->val == FALSE);

	v = LIT2VAR(ps->failed_assumption);
	reason = var2reason(ps, v);
	if (!reason)
		return;
#ifdef NO_BINARY_CLAUSES
	if (reason == &ps->impl)
		resetimpl(ps);
#endif

	eol = end_of_lits(reason);
	for (p = reason->lits; p != eol; p++) {
		lit = *p;
		u = LIT2VAR(lit);
		if (u == v)
			continue;
		if (u->reason)
			break;
	}
	if (p == eol)
		return;

	assert(ps->ahead == ps->added);
	assert(ps->mhead == ps->marked);
	assert(ps->rhead == ps->resolved);

	next = 0;
	mark_var(ps, v);
	add_lit(ps, NOTLIT(ps->failed_assumption));

	do {
		v = ps->marked[next++];
		use_var(ps, v);
		if (v->reason) {
			reason = var2reason(ps, v);
#ifdef NO_BINARY_CLAUSES
			if (reason == &ps->impl)
				resetimpl(ps);
#endif
			add_antecedent(ps, reason);
			eol = end_of_lits(reason);
			for (p = reason->lits; p != eol; p++) {
				lit = *p;
				u = LIT2VAR(lit);
				if (u == v)
					continue;
				if (u->mark)
					continue;
				mark_var(ps, u);
			}
		} else {
			lit = VAR2LIT(v);
			if (lit->val == TRUE)
				lit = NOTLIT(lit);
			add_lit(ps, lit);
		}
	} while (ps->marked + next < ps->mhead);

	c = add_simplified_clause(ps, 1);
	v = LIT2VAR(ps->failed_assumption);
	reason = v->reason;
#ifdef NO_BINARY_CLAUSES
	if (!ISLITREASON(reason))
#endif
	{
		assert(reason->locked);
		reason->locked = 0;
		if (reason->learned && reason->size > 2) {
			assert(ps->llocked > 0);
			ps->llocked--;
		}
	}

#ifdef NO_BINARY_CLAUSES
	if (c == &ps->impl) {
		c = impl2reason(ps, NOTLIT(ps->failed_assumption));
	} else
#endif
	{
		assert(c->learned);
		assert(!c->locked);
		c->locked = 1;
		if (c->size > 2) {
			ps->llocked++;
			assert(ps->llocked > 0);
		}
	}

	v->reason = c;

	while (ps->mhead > ps->marked)
		(*--ps->mhead)->mark = 0;

	if (ps->verbosity)
		fprintf(ps->out, "%sfanalyze took %.1f seconds\n", ps->prefix,
			picosat_time_stamp() - start);
}

/* Propagate assignment of 'this' to 'FALSE' by visiting all binary clauses in
 * which 'this' occurs.
 */
inline static void prop2(PS *ps, Lit *this)
{
#ifdef NO_BINARY_CLAUSES
	Lit **l, **start;
	Ltk *lstk;
#else
	Cls *c, **p;
	Cls *next;
#endif
	Lit *other;
	Val tmp;

	assert(this->val == FALSE);

#ifdef NO_BINARY_CLAUSES
	lstk = LIT2IMPLS(this);
	start = lstk->start;
	l = start + lstk->count;
	while (l != start) {
		/* The counter 'visits' is the number of clauses that are
       * visited during propagations of assignments.
       */
		ps->visits++;
#ifdef STATS
		ps->bvisits++;
#endif
		other = *--l;
		tmp = other->val;

		if (tmp == TRUE) {
#ifdef STATS
			ps->othertrue++;
			ps->othertrue2++;
			if (LIT2VAR(other)->level < ps->LEVEL)
				ps->othertrue2u++;
#endif
			continue;
		}

		if (tmp != FALSE) {
			assign_forced(ps, other, LIT2REASON(NOTLIT(this)));
			continue;
		}

		if (ps->conflict == &ps->cimpl)
			resetcimpl(ps);
		ps->conflict = setcimpl(ps, this, other);
	}
#else
	/* Traverse all binary clauses with 'this'.  Head/Tail pointers for binary
   * clauses do not have to be modified here.
   */
	p = LIT2IMPLS(this);
	for (c = *p; c; c = next) {
		ps->visits++;
#ifdef STATS
		ps->bvisits++;
#endif
		assert(!c->collect);
#ifdef TRACE
		assert(!c->collected);
#endif
		assert(c->size == 2);

		other = c->lits[0];
		if (other == this) {
			next = c->next[0];
			other = c->lits[1];
		} else
			next = c->next[1];

		tmp = other->val;

		if (tmp == TRUE) {
#ifdef STATS
			ps->othertrue++;
			ps->othertrue2++;
			if (LIT2VAR(other)->level < ps->LEVEL)
				ps->othertrue2u++;
#endif
			continue;
		}

		if (tmp == FALSE)
			ps->conflict = c;
		else
			assign_forced(ps, other, c); /* unit clause */
	}
#endif /* !defined(NO_BINARY_CLAUSES) */
}

#ifndef NDSC
static int should_disconnect_head_tail(PS *ps, Lit *lit)
{
	unsigned litlevel;
	Var *v;

	assert(lit->val == TRUE);

	v = LIT2VAR(lit);
	litlevel = v->level;

	if (!litlevel)
		return 1;

#ifndef NFL
	if (ps->simplifying)
		return 0;
#endif

	return litlevel < ps->LEVEL;
}
#endif

inline static void propl(PS *ps, Lit *this)
{
	Lit **l, *other, *prev, *new_lit, **eol;
	Cls *next, **htp_ptr, **new_htp_ptr;
	Cls *c;
#ifdef STATS
	unsigned size;
#endif

	htp_ptr = LIT2HTPS(this);
	assert(this->val == FALSE);

	/* Traverse all non binary clauses with 'this'.  Head/Tail pointers are
   * updated as well.
   */
	for (c = *htp_ptr; c; c = next) {
		ps->visits++;
#ifdef STATS
		size = c->size;
		assert(size >= 3);
		ps->traversals++; /* other is dereferenced at least */

		if (size == 3)
			ps->tvisits++;
		else if (size >= 4) {
			ps->lvisits++;
			ps->ltraversals++;
		}
#endif
#ifdef TRACE
		assert(!c->collected);
#endif
		assert(c->size > 0);

		other = c->lits[0];
		if (other != this) {
			assert(c->size != 1);
			c->lits[0] = this;
			c->lits[1] = other;
			next = c->next[1];
			c->next[1] = c->next[0];
			c->next[0] = next;
		} else if (c->size == 1) /* With assumptions we need to
	                         * traverse unit clauses as well.
			         */
		{
			assert(!ps->conflict);
			ps->conflict = c;
			break;
		} else {
			assert(other == this && c->size > 1);
			other = c->lits[1];
			next = c->next[0];
		}
		assert(other == c->lits[1]);
		assert(this == c->lits[0]);
		assert(next == c->next[0]);
		assert(!c->collect);

		if (other->val == TRUE) {
#ifdef STATS
			ps->othertrue++;
			ps->othertruel++;
#endif
#ifndef NDSC
			if (should_disconnect_head_tail(ps, other)) {
				new_htp_ptr = LIT2DHTPS(other);
				c->next[0] = *new_htp_ptr;
				*new_htp_ptr = c;
#ifdef STATS
				ps->othertruelu++;
#endif
				*htp_ptr = next;
				continue;
			}
#endif
			htp_ptr = c->next;
			continue;
		}

		l = c->lits + 1;
		eol = (Lit **)c->lits + c->size;
		prev = this;

		while (++l != eol) {
#ifdef STATS
			if (size >= 3) {
				ps->traversals++;
				if (size > 3)
					ps->ltraversals++;
			}
#endif
			new_lit = *l;
			*l = prev;
			prev = new_lit;
			if (new_lit->val != FALSE)
				break;
		}

		if (l == eol) {
			while (l > c->lits + 2) {
				new_lit = *--l;
				*l = prev;
				prev = new_lit;
			}
			assert(c->lits[0] == this);

			assert(other == c->lits[1]);
			if (other->val == FALSE) /* found conflict */
			{
				assert(!ps->conflict);
				ps->conflict = c;
				return;
			}

			assign_forced(ps, other, c); /* unit clause */
			htp_ptr = c->next;
		} else {
			assert(new_lit->val == TRUE || new_lit->val == UNDEF);
			c->lits[0] = new_lit;
			// *l = this;
			new_htp_ptr = LIT2HTPS(new_lit);
			c->next[0] = *new_htp_ptr;
			*new_htp_ptr = c;
			*htp_ptr = next;
		}
	}
}

#ifndef NADC

static unsigned primes[] = { 996293, 330643, 753947, 500873 };

#define PRIMES ((sizeof primes) / sizeof *primes)

static unsigned hash_ado(PS *ps, Lit **ado, unsigned salt)
{
	unsigned i, res, tmp;
	Lit **p, *lit;

	assert(salt < PRIMES);

	i = salt;
	res = 0;

	for (p = ado; (lit = *p); p++) {
		assert(lit->val);

		tmp = res >> 31;
		res <<= 1;

		if (lit->val > 0)
			res |= 1;

		assert(i < PRIMES);
		res *= primes[i++];
		if (i == PRIMES)
			i = 0;

		res += tmp;
	}

	return res & (ps->szadotab - 1);
}

static unsigned cmp_ado(Lit **a, Lit **b)
{
	Lit **p, **q, *l, *k;
	int res;

	for (p = a, q = b; (l = *p); p++, q++) {
		k = *q;
		assert(k);
		if ((res = (l->val - k->val)))
			return res;
	}

	assert(!*q);

	return 0;
}

static Lit ***find_ado(PS *ps, Lit **ado)
{
	Lit ***res, **other;
	unsigned pos, delta;

	pos = hash_ado(ps, ado, 0);
	assert(pos < ps->szadotab);
	res = ps->adotab + pos;

	other = *res;
	if (!other || !cmp_ado(other, ado))
		return res;

	delta = hash_ado(ps, ado, 1);
	if (!(delta & 1))
		delta++;

	assert(delta & 1);
	assert(delta < ps->szadotab);

	for (;;) {
		pos += delta;
		if (pos >= ps->szadotab)
			pos -= ps->szadotab;

		assert(pos < ps->szadotab);
		res = ps->adotab + pos;
		other = *res;
		if (!other || !cmp_ado(other, ado))
			return res;
	}
}

static void enlarge_adotab(PS *ps)
{
	/* TODO make this generic */

	ABORTIF(ps->szadotab,
		"internal: all different objects table needs larger initial size");
	assert(!ps->nadotab);
	ps->szadotab = 10000;
	NEWN(ps->adotab, ps->szadotab);
	CLRN(ps->adotab, ps->szadotab);
}

static int propado(PS *ps, Var *v)
{
	Lit **p, **q, ***adotabpos, **ado, *lit;
	Var *u;

	if (ps->LEVEL && ps->adodisabled)
		return 1;

	assert(!ps->conflict);
	assert(!ps->adoconflict);
	assert(VAR2LIT(v)->val != UNDEF);
	assert(!v->adotabpos);

	if (!v->ado)
		return 1;

	assert(v->inado);

	for (p = v->ado; (lit = *p); p++)
		if (lit->val == UNDEF) {
			u = LIT2VAR(lit);
			assert(!u->ado);
			u->ado = v->ado;
			v->ado = 0;

			return 1;
		}

	if (4 * ps->nadotab >= 3 * ps->szadotab) /* at least 75% filled */
		enlarge_adotab(ps);

	adotabpos = find_ado(ps, v->ado);
	ado = *adotabpos;

	if (!ado) {
		ps->nadotab++;
		v->adotabpos = adotabpos;
		*adotabpos = v->ado;
		return 1;
	}

	assert(ado != v->ado);

	ps->adoconflict = new_clause(ps, 2 * llength(ado), 1);
	q = ps->adoconflict->lits;

	for (p = ado; (lit = *p); p++)
		*q++ = lit->val == FALSE ? lit : NOTLIT(lit);

	for (p = v->ado; (lit = *p); p++)
		*q++ = lit->val == FALSE ? lit : NOTLIT(lit);

	assert(q == ENDOFCLS(ps->adoconflict));
	ps->conflict = ps->adoconflict;
	ps->adoconflicts++;
	return 0;
}

#endif
