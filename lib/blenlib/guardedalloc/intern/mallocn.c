/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): Brecht Van Lommel
 *                 Campbell Barton
 *
 * ***** END GPL LICENSE BLOCK *****
 */

/** \file guardedalloc/intern/mallocn.c
 *  \ingroup MEM
 *
 * Guarded memory allocation, and boundary-write detection.
 */

#include <stdlib.h>
#include <string.h> /* memcpy */
#include <stdarg.h>
#include <sys/types.h>

/* mmap exception */
#if defined(WIN32)
#  include "mmap_win.h"
#else
#  include <sys/mman.h>
#endif

#if defined(_MSC_VER)
#  define __func__ __FUNCTION__
#endif

#include "MEM_guardedalloc.h"

/* should always be defined except for experimental cases */
#ifdef WITH_GUARDEDALLOC

/* Blame Microsoft for LLP64 and no inttypes.h, quick workaround needed: */
#if defined(WIN64)
#  define SIZET_FORMAT "%I64u"
#  define SIZET_ARG(a) ((unsigned long long)(a))
#else
#  define SIZET_FORMAT "%lu"
#  define SIZET_ARG(a) ((unsigned long)(a))
#endif

/* Only for debugging:
 * store original buffer's name when doing MEM_dupallocN
 * helpful to profile issues with non-freed "dup_alloc" buffers,
 * but this introduces some overhead to memory header and makes
 * things slower a bit, so better to keep disabled by default
 */
//#define DEBUG_MEMDUPLINAME

/* Only for debugging:
 * lets you count the allocations so as to find the allocator of unfreed memory
 * in situations where the leak is predictable */

//#define DEBUG_MEMCOUNTER

/* Only for debugging:
 * defining DEBUG_THREADS will enable check whether memory manager
 * is locked with a mutex when allocation is called from non-main
 * thread.
 *
 * This helps troubleshooting memory issues caused by the fact
 * guarded allocator is not thread-safe, however this check will
 * fail to check allocations from openmp threads.
 */
//#define DEBUG_THREADS

/* Only for debugging:
 * Defining DEBUG_BACKTRACE will store a backtrace from where
 * memory block was allocated and print this trace for all
 * unfreed blocks.
 */
//#define DEBUG_BACKTRACE

#ifdef DEBUG_BACKTRACE
#  define BACKTRACE_SIZE 100
#endif

#ifdef DEBUG_MEMCOUNTER
   /* set this to the value that isn't being freed */
#  define DEBUG_MEMCOUNTER_ERROR_VAL 0
static int _mallocn_count = 0;

/* breakpoint here */
static void memcount_raise(const char *name)
{
	fprintf(stderr, "%s: memcount-leak, %d\n", name, _mallocn_count);
}
#endif

/* --------------------------------------------------------------------- */
/* Data definition                                                       */
/* --------------------------------------------------------------------- */
/* all memory chunks are put in linked lists */
typedef struct localLink {
	struct localLink *next, *prev;
} localLink;

typedef struct localListBase {
	void *first, *last;
} localListBase;

/* note: keep this struct aligned (e.g., irix/gcc) - Hos */
typedef struct MemHead {
	int tag1;
	size_t len;
	struct MemHead *next, *prev;
	const char *name;
	const char *nextname;
	int tag2;
	int mmap;  /* if true, memory was mmapped */
#ifdef DEBUG_MEMCOUNTER
	int _count;
#endif

#ifdef DEBUG_MEMDUPLINAME
	int need_free_name, pad;
#endif

#ifdef DEBUG_BACKTRACE
	void *backtrace[BACKTRACE_SIZE];
	int backtrace_size;
#endif
} MemHead;

/* for openmp threading asserts, saves time troubleshooting
 * we may need to extend this if blender code starts using MEM_
 * functions inside OpenMP correctly with omp_set_lock() */

#if 0  /* disable for now, only use to debug openmp code which doesn lock threads for malloc */
#if defined(_OPENMP) && defined(DEBUG)
#  include <assert.h>
#  include <omp.h>
#  define DEBUG_OMP_MALLOC
#endif
#endif

#ifdef DEBUG_THREADS
#  include <assert.h>
#  include <pthread.h>
static pthread_t mainid;
#endif

#ifdef DEBUG_BACKTRACE
#  if defined(__linux__) || defined(__APPLE__)
#    include <execinfo.h>
// Windows is not supported yet.
//#  elif defined(_MSV_VER)
//#    include <DbgHelp.h>
#  endif
#endif

typedef struct MemTail {
	int tag3, pad;
} MemTail;


/* --------------------------------------------------------------------- */
/* local functions                                                       */
/* --------------------------------------------------------------------- */

static void addtail(volatile localListBase *listbase, void *vlink);
static void remlink(volatile localListBase *listbase, void *vlink);
static void rem_memblock(MemHead *memh);
static void MemorY_ErroR(const char *block, const char *error);
static const char *check_memlist(MemHead *memh);

/* --------------------------------------------------------------------- */
/* locally used defines                                                  */
/* --------------------------------------------------------------------- */

#ifdef __BIG_ENDIAN__
#  define MAKE_ID(a, b, c, d) ((int)(a) << 24 | (int)(b) << 16 | (c) << 8 | (d))
#else
#  define MAKE_ID(a, b, c, d) ((int)(d) << 24 | (int)(c) << 16 | (b) << 8 | (a))
#endif

#define MEMTAG1 MAKE_ID('M', 'E', 'M', 'O')
#define MEMTAG2 MAKE_ID('R', 'Y', 'B', 'L')
#define MEMTAG3 MAKE_ID('O', 'C', 'K', '!')
#define MEMFREE MAKE_ID('F', 'R', 'E', 'E')

#define MEMNEXT(x) \
	((MemHead *)(((char *) x) - ((char *) &(((MemHead *)0)->next))))
	
/* --------------------------------------------------------------------- */
/* vars                                                                  */
/* --------------------------------------------------------------------- */
	

static volatile int totblock = 0;
static volatile uintptr_t mem_in_use = 0, mmap_in_use = 0, peak_mem = 0;

static volatile struct localListBase _membase;
static volatile struct localListBase *membase = &_membase;
static void (*error_callback)(const char *) = NULL;
static void (*thread_lock_callback)(void) = NULL;
static void (*thread_unlock_callback)(void) = NULL;

static int malloc_debug_memset = 0;

#ifdef malloc
#undef malloc
#endif

#ifdef calloc
#undef calloc
#endif

#ifdef free
#undef free
#endif


/* --------------------------------------------------------------------- */
/* implementation                                                        */
/* --------------------------------------------------------------------- */

#ifdef __GNUC__
__attribute__ ((format(printf, 1, 2)))
#endif
static void print_error(const char *str, ...)
{
	char buf[512];
	va_list ap;

	va_start(ap, str);
	vsnprintf(buf, sizeof(buf), str, ap);
	va_end(ap);
	buf[sizeof(buf) - 1] = '\0';

	if (error_callback) error_callback(buf);
}

static void mem_lock_thread(void)
{
#ifdef DEBUG_THREADS
	static int initialized = 0;

	if (initialized == 0) {
		/* assume first allocation happens from main thread */
		mainid = pthread_self();
		initialized = 1;
	}

	if (!pthread_equal(pthread_self(), mainid) && thread_lock_callback == NULL) {
		assert(!"Memory function is called from non-main thread without lock");
	}
#endif

#ifdef DEBUG_OMP_MALLOC
	assert(omp_in_parallel() == 0);
#endif

	if (thread_lock_callback)
		thread_lock_callback();
}

static void mem_unlock_thread(void)
{
	if (thread_unlock_callback)
		thread_unlock_callback();
}

int MEM_check_memory_integrity(void)
{
	const char *err_val = NULL;
	MemHead *listend;
	/* check_memlist starts from the front, and runs until it finds
	 * the requested chunk. For this test, that's the last one. */
	listend = membase->last;
	
	err_val = check_memlist(listend);

	return (err_val != NULL);
}


void MEM_set_error_callback(void (*func)(const char *))
{
	error_callback = func;
}

void MEM_set_lock_callback(void (*lock)(void), void (*unlock)(void))
{
	thread_lock_callback = lock;
	thread_unlock_callback = unlock;
}

void MEM_set_memory_debug(void)
{
	malloc_debug_memset = 1;
}

size_t MEM_allocN_len(const void *vmemh)
{
	if (vmemh) {
		const MemHead *memh = vmemh;
	
		memh--;
		return memh->len;
	}
	else {
		return 0;
	}
}

void *MEM_dupallocN(const void *vmemh)
{
	void *newp = NULL;
	
	if (vmemh) {
		const MemHead *memh = vmemh;
		memh--;

#ifndef DEBUG_MEMDUPLINAME
		if (memh->mmap)
			newp = MEM_mapallocN(memh->len, "dupli_mapalloc");
		else
			newp = MEM_mallocN(memh->len, "dupli_alloc");

		if (newp == NULL) return NULL;
#else
		{
			MemHead *nmemh;
			char *name = malloc(strlen(memh->name) + 24);

			if (memh->mmap) {
				sprintf(name, "%s %s", "dupli_mapalloc", memh->name);
				newp = MEM_mapallocN(memh->len, name);
			}
			else {
				sprintf(name, "%s %s", "dupli_alloc", memh->name);
				newp = MEM_mallocN(memh->len, name);
			}

			if (newp == NULL) return NULL;

			nmemh = newp;
			nmemh--;

			nmemh->need_free_name = 1;
		}
#endif

		memcpy(newp, vmemh, memh->len);
	}

	return newp;
}

void *MEM_reallocN(void *vmemh, size_t len)
{
	void *newp = NULL;
	
	if (vmemh) {
		MemHead *memh = vmemh;
		memh--;

		newp = MEM_mallocN(len, memh->name);
		if (newp) {
			if (len < memh->len) {
				/* shrink */
				memcpy(newp, vmemh, len);
			}
			else {
				/* grow (or remain same size) */
				memcpy(newp, vmemh, memh->len);
			}
		}

		MEM_freeN(vmemh);
	}
	else {
		newp = MEM_mallocN(len, __func__);
	}

	return newp;
}

void *MEM_recallocN(void *vmemh, size_t len)
{
	void *newp = NULL;

	if (vmemh) {
		MemHead *memh = vmemh;
		memh--;

		newp = MEM_mallocN(len, memh->name);
		if (newp) {
			if (len < memh->len) {
				/* shrink */
				memcpy(newp, vmemh, len);
			}
			else {
				memcpy(newp, vmemh, memh->len);

				if (len > memh->len) {
					/* grow */
					/* zero new bytes */
					memset(((char *)newp) + memh->len, 0, len - memh->len);
				}
			}
		}

		MEM_freeN(vmemh);
	}
	else {
		newp = MEM_callocN(len, __func__);
	}

	return newp;
}

#ifdef DEBUG_BACKTRACE
#  if defined(__linux__) || defined(__APPLE__)
static void make_memhead_backtrace(MemHead *memh)
{
	memh->backtrace_size = backtrace(memh->backtrace, BACKTRACE_SIZE);
}

static void print_memhead_backtrace(MemHead *memh)
{
	char **strings;
	int i;

	strings = backtrace_symbols(memh->backtrace, memh->backtrace_size);
	for (i = 0; i < memh->backtrace_size; i++) {
		print_error("  %s\n", strings[i]);
	}

	free(strings);
}
#  else
static void make_memhead_backtrace(MemHead *memh)
{
	(void) memh;  /* Ignored. */
}

static void print_memhead_backtrace(MemHead *memh)
{
	(void) memh;  /* Ignored. */
}
#  endif  /* defined(__linux__) || defined(__APPLE__) */
#endif  /* DEBUG_BACKTRACE */

static void make_memhead_header(MemHead *memh, size_t len, const char *str)
{
	MemTail *memt;
	
	memh->tag1 = MEMTAG1;
	memh->name = str;
	memh->nextname = NULL;
	memh->len = len;
	memh->mmap = 0;
	memh->tag2 = MEMTAG2;

#ifdef DEBUG_MEMDUPLINAME
	memh->need_free_name = 0;
#endif

#ifdef DEBUG_BACKTRACE
	make_memhead_backtrace(memh);
#endif

	memt = (MemTail *)(((char *) memh) + sizeof(MemHead) + len);
	memt->tag3 = MEMTAG3;
	
	addtail(membase, &memh->next);
	if (memh->next) {
		memh->nextname = MEMNEXT(memh->next)->name;
	}
	
	totblock++;
	mem_in_use += len;

	peak_mem = mem_in_use > peak_mem ? mem_in_use : peak_mem;
}

void *MEM_mallocN(size_t len, const char *str)
{
	MemHead *memh;

	mem_lock_thread();

	len = (len + 3) & ~3;   /* allocate in units of 4 */
	
	memh = (MemHead *)malloc(len + sizeof(MemHead) + sizeof(MemTail));

	if (memh) {
		make_memhead_header(memh, len, str);
		mem_unlock_thread();
		if (malloc_debug_memset && len)
			memset(memh + 1, 255, len);

#ifdef DEBUG_MEMCOUNTER
		if (_mallocn_count == DEBUG_MEMCOUNTER_ERROR_VAL)
			memcount_raise(__func__);
		memh->_count = _mallocn_count++;
#endif
		return (++memh);
	}
	mem_unlock_thread();
	print_error("Malloc returns null: len=" SIZET_FORMAT " in %s, total %u\n",
	            SIZET_ARG(len), str, (unsigned int) mem_in_use);
	return NULL;
}

void *MEM_callocN(size_t len, const char *str)
{
	MemHead *memh;

	mem_lock_thread();

	len = (len + 3) & ~3;   /* allocate in units of 4 */

	memh = (MemHead *)calloc(len + sizeof(MemHead) + sizeof(MemTail), 1);

	if (memh) {
		make_memhead_header(memh, len, str);
		mem_unlock_thread();
#ifdef DEBUG_MEMCOUNTER
		if (_mallocn_count == DEBUG_MEMCOUNTER_ERROR_VAL)
			memcount_raise(__func__);
		memh->_count = _mallocn_count++;
#endif
		return (++memh);
	}
	mem_unlock_thread();
	print_error("Calloc returns null: len=" SIZET_FORMAT " in %s, total %u\n",
	            SIZET_ARG(len), str, (unsigned int) mem_in_use);
	return NULL;
}

/* note; mmap returns zero'd memory */
void *MEM_mapallocN(size_t len, const char *str)
{
	MemHead *memh;

	mem_lock_thread();
	
	len = (len + 3) & ~3;   /* allocate in units of 4 */

	memh = mmap(NULL, len + sizeof(MemHead) + sizeof(MemTail),
	            PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);

	if (memh != (MemHead *)-1) {
		make_memhead_header(memh, len, str);
		memh->mmap = 1;
		mmap_in_use += len;
		peak_mem = mmap_in_use > peak_mem ? mmap_in_use : peak_mem;
		mem_unlock_thread();
#ifdef DEBUG_MEMCOUNTER
		if (_mallocn_count == DEBUG_MEMCOUNTER_ERROR_VAL)
			memcount_raise(__func__);
		memh->_count = _mallocn_count++;
#endif
		return (++memh);
	}
	else {
		mem_unlock_thread();
		print_error("Mapalloc returns null, fallback to regular malloc: "
		            "len=" SIZET_FORMAT " in %s, total %u\n",
		            SIZET_ARG(len), str, (unsigned int) mmap_in_use);
		return MEM_callocN(len, str);
	}
}

/* Memory statistics print */
typedef struct MemPrintBlock {
	const char *name;
	uintptr_t len;
	int items;
} MemPrintBlock;

static int compare_name(const void *p1, const void *p2)
{
	const MemPrintBlock *pb1 = (const MemPrintBlock *)p1;
	const MemPrintBlock *pb2 = (const MemPrintBlock *)p2;

	return strcmp(pb1->name, pb2->name);
}

static int compare_len(const void *p1, const void *p2)
{
	const MemPrintBlock *pb1 = (const MemPrintBlock *)p1;
	const MemPrintBlock *pb2 = (const MemPrintBlock *)p2;

	if (pb1->len < pb2->len)
		return 1;
	else if (pb1->len == pb2->len)
		return 0;
	else
		return -1;
}

void MEM_printmemlist_stats(void)
{
	MemHead *membl;
	MemPrintBlock *pb, *printblock;
	int totpb, a, b;

	mem_lock_thread();

	/* put memory blocks into array */
	printblock = malloc(sizeof(MemPrintBlock) * totblock);

	pb = printblock;
	totpb = 0;

	membl = membase->first;
	if (membl) membl = MEMNEXT(membl);

	while (membl) {
		pb->name = membl->name;
		pb->len = membl->len;
		pb->items = 1;

		totpb++;
		pb++;

		if (membl->next)
			membl = MEMNEXT(membl->next);
		else break;
	}

	/* sort by name and add together blocks with the same name */
	qsort(printblock, totpb, sizeof(MemPrintBlock), compare_name);
	for (a = 0, b = 0; a < totpb; a++) {
		if (a == b) {
			continue;
		}
		else if (strcmp(printblock[a].name, printblock[b].name) == 0) {
			printblock[b].len += printblock[a].len;
			printblock[b].items++;
		}
		else {
			b++;
			memcpy(&printblock[b], &printblock[a], sizeof(MemPrintBlock));
		}
	}
	totpb = b + 1;

	/* sort by length and print */
	qsort(printblock, totpb, sizeof(MemPrintBlock), compare_len);
	printf("\ntotal memory len: %.3f MB\n",
	       (double)mem_in_use / (double)(1024 * 1024));
	printf("peak memory len: %.3f MB\n",
	       (double)peak_mem / (double)(1024 * 1024));
	printf(" ITEMS TOTAL-MiB AVERAGE-KiB TYPE\n");
	for (a = 0, pb = printblock; a < totpb; a++, pb++) {
		printf("%6d (%8.3f  %8.3f) %s\n",
		       pb->items, (double)pb->len / (double)(1024 * 1024),
		       (double)pb->len / 1024.0 / (double)pb->items, pb->name);
	}
	free(printblock);
	
	mem_unlock_thread();

#if 0 /* GLIBC only */
	malloc_stats();
#endif
}

static const char mem_printmemlist_pydict_script[] =
"mb_userinfo = {}\n"
"totmem = 0\n"
"for mb_item in membase:\n"
"    mb_item_user_size = mb_userinfo.setdefault(mb_item['name'], [0,0])\n"
"    mb_item_user_size[0] += 1 # Add a user\n"
"    mb_item_user_size[1] += mb_item['len'] # Increment the size\n"
"    totmem += mb_item['len']\n"
"print('(membase) items:', len(membase), '| unique-names:',\n"
"      len(mb_userinfo), '| total-mem:', totmem)\n"
"mb_userinfo_sort = list(mb_userinfo.items())\n"
"for sort_name, sort_func in (('size', lambda a: -a[1][1]),\n"
"                             ('users', lambda a: -a[1][0]),\n"
"                             ('name', lambda a: a[0])):\n"
"    print('\\nSorting by:', sort_name)\n"
"    mb_userinfo_sort.sort(key = sort_func)\n"
"    for item in mb_userinfo_sort:\n"
"        print('name:%%s, users:%%i, len:%%i' %%\n"
"              (item[0], item[1][0], item[1][1]))\n";

/* Prints in python syntax for easy */
static void MEM_printmemlist_internal(int pydict)
{
	MemHead *membl;

	mem_lock_thread();

	membl = membase->first;
	if (membl) membl = MEMNEXT(membl);
	
	if (pydict) {
		print_error("# membase_debug.py\n");
		print_error("membase = [\n");
	}
	while (membl) {
		if (pydict) {
			fprintf(stderr,
			        "    {'len':" SIZET_FORMAT ", "
			        "'name':'''%s''', "
			        "'pointer':'%p'},\n",
			        SIZET_ARG(membl->len), membl->name, (void *)(membl + 1));
		}
		else {
#ifdef DEBUG_MEMCOUNTER
			print_error("%s len: " SIZET_FORMAT " %p, count: %d\n",
			            membl->name, SIZET_ARG(membl->len), membl + 1,
			            membl->_count);
#else
			print_error("%s len: " SIZET_FORMAT " %p\n",
			            membl->name, SIZET_ARG(membl->len), membl + 1);
#endif
#ifdef DEBUG_BACKTRACE
			print_memhead_backtrace(membl);
#endif
		}
		if (membl->next)
			membl = MEMNEXT(membl->next);
		else break;
	}
	if (pydict) {
		fprintf(stderr, "]\n\n");
		fprintf(stderr, mem_printmemlist_pydict_script);
	}
	
	mem_unlock_thread();
}

void MEM_callbackmemlist(void (*func)(void *))
{
	MemHead *membl;

	mem_lock_thread();

	membl = membase->first;
	if (membl) membl = MEMNEXT(membl);

	while (membl) {
		func(membl + 1);
		if (membl->next)
			membl = MEMNEXT(membl->next);
		else break;
	}

	mem_unlock_thread();
}

#if 0
short MEM_testN(void *vmemh)
{
	MemHead *membl;

	mem_lock_thread();

	membl = membase->first;
	if (membl) membl = MEMNEXT(membl);

	while (membl) {
		if (vmemh == membl + 1) {
			mem_unlock_thread();
			return 1;
		}

		if (membl->next)
			membl = MEMNEXT(membl->next);
		else break;
	}

	mem_unlock_thread();

	print_error("Memoryblock %p: pointer not in memlist\n", vmemh);
	return 0;
}
#endif

void MEM_printmemlist(void)
{
	MEM_printmemlist_internal(0);
}
void MEM_printmemlist_pydict(void)
{
	MEM_printmemlist_internal(1);
}

void MEM_freeN(void *vmemh)
{
	MemTail *memt;
	MemHead *memh = vmemh;
	const char *name;

	if (memh == NULL) {
		MemorY_ErroR("free", "attempt to free NULL pointer");
		/* print_error(err_stream, "%d\n", (memh+4000)->tag1); */
		return;
	}

	if (sizeof(intptr_t) == 8) {
		if (((intptr_t) memh) & 0x7) {
			MemorY_ErroR("free", "attempt to free illegal pointer");
			return;
		}
	}
	else {
		if (((intptr_t) memh) & 0x3) {
			MemorY_ErroR("free", "attempt to free illegal pointer");
			return;
		}
	}
	
	memh--;
	if (memh->tag1 == MEMFREE && memh->tag2 == MEMFREE) {
		MemorY_ErroR(memh->name, "double free");
		return;
	}

	mem_lock_thread();
	if ((memh->tag1 == MEMTAG1) &&
	    (memh->tag2 == MEMTAG2) &&
	    ((memh->len & 0x3) == 0))
	{
		memt = (MemTail *)(((char *) memh) + sizeof(MemHead) + memh->len);
		if (memt->tag3 == MEMTAG3) {
			
			memh->tag1 = MEMFREE;
			memh->tag2 = MEMFREE;
			memt->tag3 = MEMFREE;
			/* after tags !!! */
			rem_memblock(memh);

			mem_unlock_thread();

			return;
		}
		MemorY_ErroR(memh->name, "end corrupt");
		name = check_memlist(memh);
		if (name != NULL) {
			if (name != memh->name) MemorY_ErroR(name, "is also corrupt");
		}
	}
	else {
		name = check_memlist(memh);
		if (name == NULL)
			MemorY_ErroR("free", "pointer not in memlist");
		else
			MemorY_ErroR(name, "error in header");
	}

	totblock--;
	/* here a DUMP should happen */

	mem_unlock_thread();

	return;
}

/* --------------------------------------------------------------------- */
/* local functions                                                       */
/* --------------------------------------------------------------------- */

static void addtail(volatile localListBase *listbase, void *vlink)
{
	struct localLink *link = vlink;

	/* for a generic API error checks here is fine but
	 * the limited use here they will never be NULL */
#if 0
	if (link == NULL) return;
	if (listbase == NULL) return;
#endif

	link->next = NULL;
	link->prev = listbase->last;

	if (listbase->last) ((struct localLink *)listbase->last)->next = link;
	if (listbase->first == NULL) listbase->first = link;
	listbase->last = link;
}

static void remlink(volatile localListBase *listbase, void *vlink)
{
	struct localLink *link = vlink;

	/* for a generic API error checks here is fine but
	 * the limited use here they will never be NULL */
#if 0
	if (link == NULL) return;
	if (listbase == NULL) return;
#endif

	if (link->next) link->next->prev = link->prev;
	if (link->prev) link->prev->next = link->next;

	if (listbase->last == link) listbase->last = link->prev;
	if (listbase->first == link) listbase->first = link->next;
}

static void rem_memblock(MemHead *memh)
{
	remlink(membase, &memh->next);
	if (memh->prev) {
		if (memh->next)
			MEMNEXT(memh->prev)->nextname = MEMNEXT(memh->next)->name;
		else
			MEMNEXT(memh->prev)->nextname = NULL;
	}

	totblock--;
	mem_in_use -= memh->len;

#ifdef DEBUG_MEMDUPLINAME
	if (memh->need_free_name)
		free((char *) memh->name);
#endif

	if (memh->mmap) {
		mmap_in_use -= memh->len;
		if (munmap(memh, memh->len + sizeof(MemHead) + sizeof(MemTail)))
			printf("Couldn't unmap memory %s\n", memh->name);
	}
	else {
		if (malloc_debug_memset && memh->len)
			memset(memh + 1, 255, memh->len);
		free(memh);
	}
}

static void MemorY_ErroR(const char *block, const char *error)
{
	print_error("Memoryblock %s: %s\n", block, error);

#ifdef WITH_ASSERT_ABORT
	abort();
#endif
}

static const char *check_memlist(MemHead *memh)
{
	MemHead *forw, *back, *forwok, *backok;
	const char *name;

	forw = membase->first;
	if (forw) forw = MEMNEXT(forw);
	forwok = NULL;
	while (forw) {
		if (forw->tag1 != MEMTAG1 || forw->tag2 != MEMTAG2) break;
		forwok = forw;
		if (forw->next) forw = MEMNEXT(forw->next);
		else forw = NULL;
	}

	back = (MemHead *) membase->last;
	if (back) back = MEMNEXT(back);
	backok = NULL;
	while (back) {
		if (back->tag1 != MEMTAG1 || back->tag2 != MEMTAG2) break;
		backok = back;
		if (back->prev) back = MEMNEXT(back->prev);
		else back = NULL;
	}

	if (forw != back) return ("MORE THAN 1 MEMORYBLOCK CORRUPT");

	if (forw == NULL && back == NULL) {
		/* no wrong headers found then but in search of memblock */

		forw = membase->first;
		if (forw) forw = MEMNEXT(forw);
		forwok = NULL;
		while (forw) {
			if (forw == memh) break;
			if (forw->tag1 != MEMTAG1 || forw->tag2 != MEMTAG2) break;
			forwok = forw;
			if (forw->next) forw = MEMNEXT(forw->next);
			else forw = NULL;
		}
		if (forw == NULL) return NULL;

		back = (MemHead *) membase->last;
		if (back) back = MEMNEXT(back);
		backok = NULL;
		while (back) {
			if (back == memh) break;
			if (back->tag1 != MEMTAG1 || back->tag2 != MEMTAG2) break;
			backok = back;
			if (back->prev) back = MEMNEXT(back->prev);
			else back = NULL;
		}
	}

	if (forwok) name = forwok->nextname;
	else name = "No name found";

	if (forw == memh) {
		/* to be sure but this block is removed from the list */
		if (forwok) {
			if (backok) {
				forwok->next = (MemHead *)&backok->next;
				backok->prev = (MemHead *)&forwok->next;
				forwok->nextname = backok->name;
			}
			else {
				forwok->next = NULL;
				membase->last = (struct localLink *) &forwok->next;
			}
		}
		else {
			if (backok) {
				backok->prev = NULL;
				membase->first = &backok->next;
			}
			else {
				membase->first = membase->last = NULL;
			}
		}
	}
	else {
		MemorY_ErroR(name, "Additional error in header");
		return("Additional error in header");
	}

	return(name);
}

uintptr_t MEM_get_peak_memory(void)
{
	uintptr_t _peak_mem;

	mem_lock_thread();
	_peak_mem = peak_mem;
	mem_unlock_thread();

	return _peak_mem;
}

void MEM_reset_peak_memory(void)
{
	mem_lock_thread();
	peak_mem = 0;
	mem_unlock_thread();
}

uintptr_t MEM_get_memory_in_use(void)
{
	uintptr_t _mem_in_use;

	mem_lock_thread();
	_mem_in_use = mem_in_use;
	mem_unlock_thread();

	return _mem_in_use;
}

uintptr_t MEM_get_mapped_memory_in_use(void)
{
	uintptr_t _mmap_in_use;

	mem_lock_thread();
	_mmap_in_use = mmap_in_use;
	mem_unlock_thread();

	return _mmap_in_use;
}

int MEM_get_memory_blocks_in_use(void)
{
	int _totblock;

	mem_lock_thread();
	_totblock = totblock;
	mem_unlock_thread();

	return _totblock;
}

#ifndef NDEBUG
const char *MEM_name_ptr(void *vmemh)
{
	if (vmemh) {
		MemHead *memh = vmemh;
		memh--;
		return memh->name;
	}
	else {
		return "MEM_name_ptr(NULL)";
	}
}
#endif  /* NDEBUG */

#else  /* !WITH_GUARDEDALLOC */

#ifdef __GNUC__
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif

#include <malloc.h>

size_t MEM_allocN_len(const void *vmemh)
{
	return malloc_usable_size((void *)vmemh);
}

void MEM_freeN(void *vmemh)
{
	free(vmemh);
}

void *MEM_dupallocN(const void *vmemh)
{
	void *newp = NULL;
	if (vmemh) {
		const size_t prev_size = MEM_allocN_len(vmemh);
		newp = malloc(prev_size);
		memcpy(newp, vmemh, prev_size);
	}
	return newp;
}

void *MEM_reallocN(void *vmemh, size_t len)
{
	return realloc(vmemh, len);
}

void *MEM_recallocN(void *vmemh, size_t len)
{
	void *newp = NULL;

	if (vmemh) {
		size_t vmemh_len = MEM_allocN_len(vmemh);
		newp = malloc(len);
		if (newp) {
			if (len < vmemh_len) {
				/* shrink */
				memcpy(newp, vmemh, len);
			}
			else {
				memcpy(newp, vmemh, vmemh_len);

				if (len > vmemh_len) {
					/* grow */
					/* zero new bytes */
					memset(((char *)newp) + vmemh_len, 0, len - vmemh_len);
				}
			}
		}

		free(vmemh);
	}
	else {
		newp = calloc(1, len);
	}

	return newp;
}

void *MEM_callocN(size_t len, const char *UNUSED(str))
{
	return calloc(1, len);
}

void *MEM_mallocN(size_t len, const char *UNUSED(str))
{
	return malloc(len);
}

void *MEM_mapallocN(size_t len, const char *UNUSED(str))
{
	/* could us mmap */
	return calloc(1, len);
}

void MEM_printmemlist_pydict(void) {}
void MEM_printmemlist(void) {}

/* unused */
void MEM_callbackmemlist(void (*func)(void *))
{
	(void)func;
}

void MEM_printmemlist_stats(void) {}

void MEM_set_error_callback(void (*func)(const char *))
{
	(void)func;
}

int MEM_check_memory_integrity(void)
{
	return 1;
}

void MEM_set_lock_callback(void (*lock)(void), void (*unlock)(void))
{
	(void)lock;
	(void)unlock;
}

void MEM_set_memory_debug(void) {}

uintptr_t MEM_get_memory_in_use(void)
{
	struct mallinfo mi;
	mi = mallinfo();
	return mi.uordblks;
}

uintptr_t MEM_get_mapped_memory_in_use(void)
{
	return MEM_get_memory_in_use();
}

int MEM_get_memory_blocks_in_use(void)
{
	struct mallinfo mi;
	mi = mallinfo();
	return mi.smblks + mi.hblks;
}

/* dummy */
void MEM_reset_peak_memory(void) {}

uintptr_t MEM_get_peak_memory(void)
{
	return MEM_get_memory_in_use();
}

#endif  /* WITH_GUARDEDALLOC */
