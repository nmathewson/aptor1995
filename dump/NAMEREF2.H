///////////////////////////////////////////////////////////////////////////
/// Filename:      NAMEREF.H                                            ///
/// Long filename: name_ref class header file.                          ///
/// File type:     CPP Class header                                     ///
/// Project:       APTOR                                                ///
///////////////////////////////////////////////////////////////////////////
/// For use with:  NAMEREF.CPP                                          ///
/// Uses:  INTRFACE                                                     ///
///////////////////////////////////////////////////////////////////////////
/// Synopsis: This class stores the names and addresses of various      ///
///           text blocks for APTOR.                                    ///
///////////////////////////////////////////////////////////////////////////
/// Author: N.A.A. Mathewson                                            ///
/// Date: 7/8/95                       Revision: 0.1                    ///
///////////////////////////////////////////////////////////////////////////

#ifndef NAMEREF_H
#define NAMEREF_H

#ifndef FALSE
#include "flag.h"
#endif

#define DEFAULT_MEM_BLOCK 16384   // How big is the default mem block?
#define MAX_NAME_LEN 16           // How long can a name be, anyway?

class name_ref
{
protected:
  char **ref_p;                   // For 'BIG' table.
  char **mem_p;                   //  "    "     "
  long *addresses;                // For 'SMALL' table.

  int max_refs,                   // How many names have we allocated?
      n_refs,                     // How many names have we used?
      mem_used;                   // # of memory blocks currently used.

  unsigned mem_pos,               // Position within most recent block.
           mem_block_size;        // Size of memory blocks.

  char *str(int n)
  {
    return &((ref_p[n])[sizeof(long)]);
  };
  long *num(int n)
  {
    return (long *) ref_p[n];
  };
  int allocate(void);             // Unconditionally allocates a new memory
                                  //   block.

  flag longs_only;                // Is 'SMALL' table active?

  int set_mem_big(int names,
                  unsigned block_size);
  int set_mem_small(int names);

public:
  name_ref(void);

  int set_memory(flag small,
                 int a, int b = DEFAULT_MEM_BLOCK)
  {                               // Initializes memory conditions. MUST
    return (small) ?              // BE CALLED FIRST. Flag is true ==>
             (set_mem_small(a)) : // no name storage.
             (set_mem_big(a,b));
  };

  const char *name(int n);        // Returns either str(n) or NULL.
  int n(void)
  {
    return n_refs;
  };
  int token(const char *s);
  long get_address(const char *s)
  {
    return get_address(token(s));
  };
  long get_address(int n);
  int add(const char *s, long l); // Adds a new element
  int add(long l);                // "    "  "    "
  ~name_ref(void);
};


#endif
