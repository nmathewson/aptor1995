///////////////////////////////////////////////////////////////////////////
/// Filename:      COMPILE.CPP                                          ///
/// Long filename: Main compilation implementation                      ///
/// File type:     C++ Code                                             ///
/// Project:       APTCOMP (APTOR)                                      ///
///////////////////////////////////////////////////////////////////////////
/// For use with:  COMPILE.H                                            ///
/// Uses: INTRFACE, NAMEREF, PARSING, CRYPT                             ///
///////////////////////////////////////////////////////////////////////////
/// Synopsis: Routines to handle parsing directives and generate final  ///
///           .apx, .map, and .mpx files.  Handles all fstream calls.   ///
///////////////////////////////////////////////////////////////////////////
/// Author: N.A.A. Mathewson                                            ///
/// Date: 7/10/95                      Revision: 0.1                    ///
///////////////////////////////////////////////////////////////////////////

#include <fstream.h>
#include <string.h>
#include <ctype.h>

#include "crypt.h"
#include "intrface.h"
#include "nameref.h"
#include "parsing.h"
#include "compile.h"

typedef char *char_p;

#define MAX_LINE_LEN 255

///////////////////////////////////////////////////////////////////////////
/// STRUCTS/ENUMS                                                       ///
///////////////////////////////////////////////////////////////////////////

struct pass_info_type
// Information passed from handle_make to compile_source
{
  ofstream *apx;                            // Pointer to the APX file under
                                            //   construction.
  cryptor *c;                               // Pointer to the cryptor being
                                            //   used.
  ifstream *in;                             // Pointer to the file being read.
  input_file_type inf;                      // Type of file to read.
  name_ref names;                           // Stores names of blocks.
  char family[MAX_FNAME_LEN+1];             // "Family name" of APX
  char *in_name;                            // Name of input file.
};

///////////////////////////////////////////////////////////////////////////
/// INTERNAL PROTOTYPES                                                 ///
///////////////////////////////////////////////////////////////////////////
char *new_ext(char *dest, const char *ext);
  // Tacks a new extention onto a parsed filename. Assumes *dest is parsed,
  //    and contains sufficient space.

void handle_code(pass_info_type &p);
  // Parses a complete aptorcode file, taking information from p. May call
  //   error routines.  Calls handle_block.

void handle_block(char *buf, char *line, long &line_n, pass_info_type &p);
  // Parses a block. Assumes p.in advanced past !begin directive. Handles
  //   !end when it's encountered. Calls error routines.

void check_dups(name_ref &n);
  // Warns user on duplicate names.

///////////////////////////////////////////////////////////////////////////
/// MASTER FUNCTION                                                     ///
///////////////////////////////////////////////////////////////////////////

void handle_make(const char *source_name)
{
  int make_applied,                         // Has MAKE been used?
      blocks_applied,                       // Has BLOCKS been used?
      use_applied;                          // Has USE been used?
  int n_blocks;                             // How many blocks ARE there?
  long make_line;                           // What line of make_file are
                                            //   we on?

  pass_info_type p_info;                    // All the many structured to
                                            //   be passed back & forth!

  ifstream make_file;                       // The fstream used to read the
                                            //   makefile.

  ifstream code_file;                       // The fstream used to read all
                                            //   code.

  ofstream apx;                             // Fstream used to generate .APX
                                            //   file.

  make_flags mf;                            // Which MAP files to make?

  make_directive m_dir;                     // Stores return-values from
                                            //   parse_make_line

  static char buf[MAX_LINE_LEN+1];          // Holds each make_file line as
                                            //   it is read.

  static char fname[MAX_FNAME_LEN+1];       // Holds names of files.

//SET UP VARIABLES

  make_applied = blocks_applied
               = use_applied = 0;
  n_blocks     = DEFAULT_BLOCKS;
  make_line    = 0L;
  p_info.apx   = NULL;
  p_info.in    = NULL;

  p_info.inf   = NO_INF;

//SET UP CRYPTOR

  p_info.c = new cryptor;
  if (! p_info.c)
    error(OUT_OF_MEMORY, TERMINAL);
  p_info.c->keygen();

//OPEN MAKE_FILE
  make_file.open(source_name);

  if (make_file.bad())
    error(FILE_NOT_FOUND, TERMINAL, source_name);

//MAIN LOOP
  do
  {
    ++make_line;
    make_file.getline(buf, 255);
    if (make_file.bad())
      error(READ_ERROR, TERMINAL, source_name);

    parse_make_line(m_dir, buf);

    if (m_dir.err)
      error(m_dir.err, TERMINAL, buf,                 // All make errors
            make_line, source_name);                  //  are terminal.

    switch (m_dir.dir)
    {
    case NO_DRC     : break;
    case MAKE_DRC   : if (make_applied)
                        error(MULTIPLE_MAKE, TERMINAL, buf,
                              make_line, source_name);
                      make_applied = 1;
                      mf = m_dir.mf;
                      strcpy(fname, m_dir.sarg);
                      strcpy(p_info.family, m_dir.sarg);
                      break;
    case BLOCKS_DRC : if (! make_applied)
                        error(MAKE_EXPECTED, TERMINAL, buf,
                              make_line, source_name);
                      if (use_applied)
                        error(BLOCKS_LATE, TERMINAL, buf,
                              make_line, source_name);
                      if (blocks_applied)
                        error(MULTIPLE_BLOCKS, TERMINAL, buf,
                              make_line, source_name);
                      blocks_applied = 1;
                      n_blocks = m_dir.narg;
                      break;
    case USE_DRC    : if (! make_applied)
                        error(MAKE_EXPECTED, TERMINAL, buf,
                              make_line, source_name);
                      if (! use_applied)            // Must open .apx file,
                      {                             //   allocate namerefs.
                        use_applied = 1;
                        new_ext(fname, "APX");
                        if (DEBUGGING || verbose)
                          cout << "Opening " << fname << "\n";

                        apx.open(fname);
                        if (apx.bad())
                          error(WRITE_ERROR, TERMINAL, fname);
                        apx << p_info.c->getkey();
                        apx.put('\0');
                        p_info.apx = &apx;

                        if (DEBUGGING || verbose)
                          cout << "Allocating space for " << n_blocks
                               << " blocks.\n";
                        p_info.names.set_memory(n_blocks);
                      };
                      if (DEBUGGING || verbose)
                        cout << "Compiling from " << m_dir.sarg
                             << " in format " << m_dir.inf << "\n";
                      p_info.inf = m_dir.inf;
                      p_info.in = &code_file;
                      code_file.open(m_dir.sarg);
                      if (code_file.bad())
                        error(FILE_NOT_FOUND, TERMINAL, m_dir.sarg,
                              make_line, source_name);
                      p_info.in_name = m_dir.sarg;
                      handle_code(p_info);
                      code_file.close();
                      break;
    };
  }
  while (! make_file.eof());

  if (! use_applied)
    error(USE_EXPECTED, TERMINAL, "", make_line, source_name);

//CLOSE MAKE_FILE, APX_FILE.
  make_file.close();
  apx.close();

//FINISH MAP INFO
  if (DEBUGGING || verbose)
    cout << "Sorting file map\n";

  p_info.names.sort();

  check_dups(p_info.names);

//MAKE ALL MAP, DEF, MPX FILES.
  if (mf.map)
  {
    new_ext(fname, "MAP");
    if (DEBUGGING || verbose)
      cout << "Saving information to " << fname << "\n";
    ofstream map(fname);
    save_to_map(map, p_info.names);
    map.close();
  };

  if (mf.mpx)
  {
    new_ext(fname, "MPX");
    if (DEBUGGING || verbose)
      cout << "Saving information to " << fname << "\n";
    ofstream mpx(fname, ios::binary);
    save_to_mpx(mpx, p_info.names);
    mpx.close();
  };

  if (mf.def)
  {
    char sym[MAX_FNAME_LEN+1];
    char apx_name[MAX_FNAME_LEN+1];

    new_ext(fname, "H  ");
    strcpy(sym, fname);
    *(strchr(sym, '.')) = '_';

    new_ext(fname, "APX");
    strcpy(apx_name, fname);

    new_ext(fname, "DEF");
    if (DEBUGGING || verbose)
      cout << "Saving information to " << fname << "\n";
    ofstream def(fname);
    save_to_def(def, p_info.names, sym, fname, apx_name);
    def.close();
  };

}

void handle_code(pass_info_type &p)
{
  int in_a_file;                          // Inside a !beginfile pair?
  int in_correct_file;                    // Inside matching !beginfile pair?
  APTORCode_command cmd;                  // Command parsed.
  long line_n;
  int read_line;                          // Read more lines?

  static char line[MAX_LINE_LEN+1];       // Holds lines under consideration.

  char *buf;                              // Holds text-block under
                                          //   construction.

  in_a_file = 0;
  in_correct_file = 1;
  read_line = 1;
  line[0] = '\0';
  line_n = 0L;
  buf = new char[MAX_BLOCK_SIZE];

  do
  {
    if (read_line)
    {
      (p.in)->getline(line, MAX_LINE_LEN);
      ++line_n;
    }
    else
      read_line = 1;

    if ((p.inf == INLINE_INF) &&
        (! is_APTORCode_line(line)) )
      continue;

    parse_APTORCode_line(cmd, line, p.inf);

    if (cmd.err)
      error(cmd.err, NONTERMINAL, line, line_n, p.in_name);

    if (! cmd.cmd)
      continue;

    switch (cmd.cmd)
    {
    case BEGINFILE_ACMD : if (in_a_file)
                            error(ENDFILE_EXPECTED, NONTERMINAL, "", line_n,
                                  p.in_name);
                          in_a_file = 1;
                          if (strcmp(cmd.sarg, p.family) == 0)
                            in_correct_file = 1;
                          else
                            in_correct_file = 0;
                          if (DEBUGGING || verbose)
                          {
                            cout << "    Entering file " << cmd.sarg;
                            if (! in_correct_file)
                              cout << "; ignoring text blocks.";
                            cout << "\n";
                          };
                          break;
    case ENDFILE_ACMD   : if (! in_a_file)
                            error(ENDFILE_WO_BF, NONTERMINAL, "", line_n,
                                  p.in_name);
                          in_a_file = 0;
                          in_correct_file = 1;
                          if (DEBUGGING || verbose)
                            cout << "    Exiting file.\n";
                          break;
    case BEGIN_ACMD     : if (! in_correct_file)
                            break;
                          if (DEBUGGING || verbose)
                            cout << "    Reading block " << cmd.sarg
                                 << " at " << (p.apx)->tellp() << "\n";
                          p.names.add(cmd.sarg, (p.apx)->tellp());
                          handle_block(buf, line, line_n, p);
                          parse_APTORCode_line(cmd, line, p.inf);
                          if (cmd.err)
                            error(cmd.err, NONTERMINAL, "", line_n, p.in_name);
                          if (cmd.cmd != END_ACMD)
                            read_line = 0;
                          break;
    case END_ACMD       : if (in_correct_file)
                            error(END_WO_BEGIN, NONTERMINAL, "", line_n,
                                  p.in_name);
                          break;
    };

    if ((p.in)->bad())
      error(READ_ERROR, TERMINAL);
  }
  while (! (p.in)->eof());

}

void handle_block(char *buf, char *l, long &line_n, pass_info_type &p)
{
  int buf_length;
  int line_length;
  char *line;
  int over;                                 // Finished?
  int overflow;                             // Overflow checking.

  line = l;
  buf_length = 0;
  line_length = 0;
  over = 0;
  overflow = 0;
  buf[0] = '\0';

  do
  {
    line = l;
    (p.in)->getline(line, MAX_LINE_LEN);
    ++line_n;
    if (p.inf == INLINE_INF)
    {
      if (dbl_slash(line))
        line += 2;
      else
      {
        error(BLOCK_INTERRUPTED, NONTERMINAL, line, line_n, p.in_name);
        continue;
      };
    };

    if (*line == '!')
    {
      if ( (strcmp(line, "!end") == 0) &&
           (isspace(line[4]) || ! line[4]) )
        ;
      else
        error(END_EXPECTED, NONTERMINAL, "", line_n, p.in_name);
      over = 1;
      continue;
    };

    if (overflow)
      continue;

    line_length = tighten_line(line);

    if ( (line_length + buf_length) >= MAX_BLOCK_SIZE )
    {
      overflow = 1;
      error(BLOCK_TOO_LONG, NONTERMINAL, "", line_n, p.in_name);
      continue;
    };

    buf_length += line_length;
    strcat(buf, line);

    if ((p.in)->bad())
      error(READ_ERROR, TERMINAL);
  }
  while ((! (p.in)->eof()) && (! over));

  (p.c)->send(buf, *(p.apx));
  (p.apx)->put('\0');

}

///////////////////////////////////////////////////////////////////////////
/// MAP GENERATION FUNCTIONS                                            ///
///////////////////////////////////////////////////////////////////////////

// .MAP file format:
// Line 0: MAP
// Line 1: <number of records>
// Line 2..Line N: <wspace><Token><wspace><Name><wspace><Address>

// ****  .MAP files are designed to be read both by the user and by
// NOTE:      APTOR_file::open, where as .MPX files are intended for
// ****       APTOR_file::open only. .MPX files are smaller, but require
//            the calling code to use token-numbers only.

int save_to_map(ostream &map, name_ref &names)
{
  int i,                                      // Index into names
      n,                                      // Number of names
      w;                                      // Max width of tokens.

  if (! map.good())
    return WRITE_ERROR;

  if (! names.sorted())
    names.sort();

  n = names.n();

  if (n >= 10000)
    w = 5;
  else if (n >= 1000)
    w = 4;
  else if (n >= 100)
    w = 3;
  else if (n >= 10)
    w = 2;
  else
    w = 1;

  map << "MAP\n";
  map << n << "\n";

  for(i = 0; i < n; i++)
  {
    map.width(w);
    map.setf(ios::right);
    map << i << " ";
    map.width(MAX_ARG_LEN+2);
    map.setf(ios::left);
    map << names.name(i) << " ";
    map << names.get_address(i) << "\n";
  };

  if (! map.good())
    return WRITE_ERROR;

  return NO_ERROR;
}

// .DEF file format:
// Line 0..1        : Comments
// Line 2           :
// Line 3..4        : prevent multiple includes
// Line 5           :
// Line 6           : Case APTOR_TOKENS
// Line 7           :
// Line 8..N+8      : #define T_<block name> <token>
// Line N+9         :
// Line N+10        : Case ELSE
// Line N+11        :
// Line N+12..2N+12 : #define T_<block name> "<block name>"
// Line 2N+13       :
// Line 2N+14       : End cases.
// Line 2N+15       :
// Line 2N+16       : End mult-include-prevention

int save_to_def(ostream &def, name_ref &names, const char *sym,
                const char *def_name, const char *apx_name)
{
  int i, n;

  if (! def.good())
    return WRITE_ERROR;

  if (! names.sorted())
    names.sort();

  n = names.n();

  def << "// " << def_name <<
               " (Generated by APTCOMP V" APTCOMP_VERSION_STR ")\n" // Line 0
         "// For use with " << apx_name << "\n"                     // Line 1
         "\n"                                                       // Line 2
         "#ifndef " << sym << " // Prevent multiple #includes\n"    // Line 3
         "#define " << sym << "\n"                                  // Line 4
         "\n"                                                       // Line 5
         "#ifdef APTOR_TOKENS\n"                                    // Line 6
         "\n";                                                      // Line 7

  def.setf(ios::left);
  for(i = 0; i < n; i++)                                            // Line 8
  {                                                                 // .....
    def << "#define T_";                                            //  8+N
    def.width(MAX_ARG_LEN+2);
    def << names.name(i) << "  " << i << "\n";
  };

  def << "\n#else\n\n";                                             // 9+N...

  for(i = 0; i < n; i++)                                            // N+12
  {                                                                 // .....
    def << "#define T_";                                            // 2N+12
    def.width(MAX_ARG_LEN+2);
    def << names.name(i)
        << "  \"" << names.name(i) << "\"\n";
  };

  def << "\n#endif\n\n#endif // prevent multiple #includes\n\n";

  if (! def.good())
    return WRITE_ERROR;

  return 0;
}


// .MPX file format:
// Sizeof(char) : '~'
// Sizeof(int)  : number of records.
// Record format: sizeof(long) : address.

int save_to_mpx(ostream &mpx, name_ref &names)
{
  int i, n;
  long address;

  if (! mpx.good())
    return WRITE_ERROR;

  if (! names.sorted())
    names.sort();

  mpx.put('~');
  n = names.n();
  mpx.write((char_p) &n, sizeof(n));

  for(i = 0; i < n; i++)
  {
    address = names.get_address(i);
    mpx.write((char_p) &address, sizeof(address));
  };

  if (! mpx.good())
    return WRITE_ERROR;

  return NO_ERROR;
}

///////////////////////////////////////////////////////////////////////////
/// SMALL INTERNAL FUNCTIONS                                            ///
///////////////////////////////////////////////////////////////////////////

char *new_ext(char *dest, const char *ext)
{
  char *p;

  p = strchr(dest, '.');

  if (! p)
    error(INTERNAL, TERMINAL);

  strcpy(++p, ext);
  return dest;
}

void check_dups(name_ref &n)
{
  int i, max;
  if (! n.sorted())
    n.sort();

  max = n.n() - 1;
  for(i = 0; i < max; ++i)
    if (strcmp(n.name(i), n.name(i+1)) == 0)
      error(DUPLICATE_NAMES, NONTERMINAL, n.name(i));

}
