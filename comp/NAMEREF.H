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

#define TOUPPER_F                 // Yes, convert strings to uppercase as
                                  //   they are read.
#define DEFAULT_MEM_BLOCK 16384   // How big is the default mem block?

class name_ref
{
protected:
  char **ref_p;
  char **mem_p;
  int max_refs,                   // How many names have we allocated?
      n_refs,                     // How many names have we used?
      mem_used;                   // # of memory blocks currently used.
  unsigned mem_pos,               // Position within most recent block.
           mem_block_size;        // Size of memory blocks.
  int sorted_f;                   // Flag to store whether ref_p is sorted.

  char *str(int n)
  {
    return &((ref_p[n])[sizeof(long)]);
  };
  long *num(int n)
  {
    return (long *) ref_p[n];
  };
  void allocate(void);            // Unconditionally allocates a new memory
                                  //   block.

public:
  name_ref(void);
  int set_memory(int names,
                 unsigned block_size = DEFAULT_MEM_BLOCK);
                                  // Initializes memory condidtions. MUST
                                  //   BE CALLED BEFORE ALL OTHER FUNCTIONS.
  const char *name(int n);        // Returns either str(n) or NULL.
  int n(void)
  {
    return n_refs;
  };
  void sort(void);                // Calls qsort to sort ref_p alphabetically
                                  //   by (str)
  int token(const char *s);       // May call sort
  long get_address(const char *s) // May call sort
  {
    return get_address(token(s));
  };
  long get_address(int n);
  int add(const char *s, long l); // Adds a new element
  int sorted(void)
  {
    return sorted_f;
  };
  ~name_ref(void);
};

#endif
