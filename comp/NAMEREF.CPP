///////////////////////////////////////////////////////////////////////////
/// Filename:      NAMEREF.CPP                                          ///
/// Long filename: name_ref class implementation (for APTCOMP)          ///
/// File type:     C++ code                                             ///
/// Project:       APTOR                                                ///
///////////////////////////////////////////////////////////////////////////
/// For use with:  NAMEREF.H                                            ///
/// Uses: INTRFACE
///////////////////////////////////////////////////////////////////////////
/// Synopsis: This class is used by APTCOMP to store its 'Address-table'///
///           into files as they are being created.                     ///
///////////////////////////////////////////////////////////////////////////
/// Author: N.A.A. Mathewson                                            ///
/// Date: 7/8/95                       Revision: 0.1                    ///
///////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>

#include "intrface.h"
#include "nameref.h"

typedef char *char_p;
typedef int (* compare_fn_p)(const void *, const void *);

void name_ref::allocate(void)
{
  mem_p[mem_used] = new char[mem_block_size];

  if (! mem_p[mem_used])
    error(OUT_OF_MEMORY, TERMINAL);

  ++mem_used;
  mem_pos = 0U;
}

name_ref::name_ref(void)
{
  ref_p = mem_p = NULL;
  mem_pos = mem_block_size = 0U;
  max_refs = n_refs = mem_used = 0;
  sorted_f = 1;
}

int name_ref::set_memory(int names, unsigned block_size)
{
  if (ref_p)
    return 1;

  mem_block_size = block_size;
  max_refs = names;
  ref_p = new char_p[names];
  mem_p = new char_p[( names * (MAX_NAME_LEN + sizeof(long))
                       / mem_block_size) ];

  if (! mem_p)
    error(OUT_OF_MEMORY, TERMINAL);
  if (! ref_p)
    error(OUT_OF_MEMORY, TERMINAL);

  allocate();
  return 0;
}

const char *name_ref::name(int n)
{
  if ((n >= n_refs) || (n < 0))
    return NULL;

  return str(n);
}

// USED BY NAME_REF::SORT only!!!
int compare_refs(const char **a, const char **b)
// -------------------------SYNTAX----------------------------------
// ENGLISH                              TYPE            C++ CODE
//  Address of:                         (char *)        &((*a)[sizeof(long)])
//   longth element of array indexed by (char)          (*a)[sizeof(long)]
//     pointer referenced by            (char *)        *a
//      a.                              (const char **) a
{
  return strcmp(&( (*a)[sizeof(long)] ),
                &( (*b)[sizeof(long)] ));

}

void name_ref::sort(void)
{
  qsort( (void *) ref_p,
         n_refs,
         sizeof(ref_p[0]),
         (compare_fn_p) compare_refs);
  sorted_f = 1;
}

int name_ref::token(const char *s)
{
  int c, low, high, mid;

  if (! sorted_f)
    sort();

  low = 0;
  high = n_refs - 1;
  c = strcmp(s, str(low));

  if (c < 0)
    return -1;
  if (c == 0)
    return low;

  c = strcmp(s, str(high));
  if (c > 0)
    return -1;
  if (c == 0)
    return high;

  mid = (low + high) / 2;
  do
  {
    c = strcmp(s, str(mid));
    if (c == 0)
      return mid;
    if (c > 0)
      low = mid;
    if (c < 0)
      high = mid;
    mid = (low + high) / 2;
  }
  while ( (low != mid) && (high != mid) );

  return -1;
}

long name_ref::get_address(int n)
{
  if ((n >=0 ) && (n < n_refs))
    return *(num(n));
  else
    return -1L;
}

int name_ref::add(const char *s, long l)
{
  if ( (mem_block_size - mem_pos) < (strlen(s) + (sizeof(long) + 2)) )
    allocate();

  if (n_refs == max_refs)
    error(OUT_OF_MEMORY, TERMINAL);

  ref_p[n_refs] = & mem_p[mem_used - 1][mem_pos]; // Where to store it?
  *(num(n_refs)) = l;                             // Store l.
  strcpy(str(n_refs), s);                         // Store s.
  mem_pos += 1 + sizeof(long) + strlen(s);        // Increment mem_pos.
#ifdef TOUPPER_F
  strupr(str(n_refs));
#endif

  ++n_refs;
  sorted_f = 0;

  return 0;
}

name_ref::~name_ref(void)
{
  delete [] ref_p;
  for(int i = 0; i < mem_used; i++)
    delete [] mem_p[i];
  delete [] mem_p;
}

