///////////////////////////////////////////////////////////////////////////
/// Filename:      INTRFACE.CPP                                         ///
/// Long filename: User interface routines and objects for APTCOMP      ///
/// File type:     CPP Code                                             ///
/// Project:       APTOR                                                ///
///////////////////////////////////////////////////////////////////////////
/// For use with:  intrface.h                                           ///
/// Uses: COMMON                                                        ///
///////////////////////////////////////////////////////////////////////////
/// Synopsis: The following rountines handle command-line functions,    ///
///           and user-interfaces for APTCOMP.  Parsing routines are    ///
///           used to handle filenames.                                 ///
///////////////////////////////////////////////////////////////////////////
/// Author: N.A.A. Mathewson                                            ///
/// Date: 6/30/96                      Revision: 0.2                    ///
///////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <stdlib.h>
#include <string.h>

#include "intrface.h"
#include "common.h"

void display_intro(void)
{
  cout << "\nAPTCOMP V"
  APTCOMP_VERSION_STR ": (C) 1995, N. A. A. Mathewson\n\n";
}

void display_help(int which_help)
{
  switch (which_help)
  {
    case 1: cout <<

    "Syntax:\nAPTCOMP [switches] <filename> [switches]\n\n"
    "Options:\n"
    "/?   : This screen.\n"
    "/h   : Help with APTORCode.\n"
    "/m   : Help with APTORMake.\n"
    "/v   : Verbose messages.\n\n"
    "NOTE: <filename> must refer to an APTORMake file (extention .apm)\n";

    break;

    case 2: cout <<

    "APTORCode help:\n\n"
    "Note: inline APTORCode is identified by lines beginning //!\n"
    "Command:\n"
    "   !begin <xxx> indicates the start of a text block named <xxx>, where\n"
    "      <xxx> is a valid C identifier.\n"
    "   !end indicates the end of a text block.\n"
    "   !beginfile <family> indicates the begin of a subfile of family name\n"
    "      <family>.  If any other family is being compiled, any blocks in\n"
    "      the subfile will be ignored.\n"
    "   !endfile indicates the end of a subfile.\n\n";

    break;

    case 3: cout <<

    "APTORMake Help:\n\n"
    "APTOR Makefiles should have extention .APM\n"
    "Correct syntax:\n"
    "  MAKE <family>\n"
    "  [BLOCKS <n>]\n"
    "  USE <filename> [APTOR | INLINE]\n"
    "  [USE <filename> [APTOR | INLINE]]\n"
    "  [USE <filename> [APTOR | INLINE]]  etc.\n\n"
    "where: - <family> is the family of .apx, .map, .def, and .mpx files to\n"
    "                  generate. (See !beginfile).\n"
    "       - <n> is the number of blocks to allocate space for.\n"
    "       - <filename> is the name of a source file to read.\n"
    "       - APTOR specifies an APTORCode source file (default extention .apt)\n"
    "       - INLINE specifies an inline APTORCode source file.\n";
    
    break;

  };
  exit(0);
}

void display_switches(const command_line &cline)
{
  if ((DEBUGGING) || cline.is_verbose)
  {
    cout << "OPTIONS:\n";
    if (cline.is_verbose)
      cout << "  Verbose Messages on.\n";
    else
      cout << "  Verbose Messages off.\n";

  };

  cout << "Compiling from " << cline.source_name << "\n";

}

void error(error_type err, int terminal, const char *text,
           const char *in)
{
  cout << "\nERROR: ";
  switch (err)
  {
    case INTERNAL       : cout << "Internal error.\n";
                          break;
    case BAD_FILENAME   : cout << "Bad filename:\n";
                          break;
    case BAD_SWITCH     : cout << "Bad command-line argument:\n";
                          break;
    case NO_SOURCE      : cout << "No source file was specified.\n";
                          break;
    case WILDCARDS_USED : cout << "Illegal wildcard use.\n";
                          break;
    case UNREC_TOKEN    : cout << "Unrecognized token:\n";
                          break;
    case LINE_TOO_LONG  : cout << "Line too long:\n";
                          break;
    case VAL_OUT_OF_BOUNDS
                        : cout << "Value out of bounds:\n";
                          break;
    case TOO_MANY_ARGS  : cout << "Too many arguments:\n";
                          break;
    case TOO_FEW_ARGS   : cout << "Too few arguments:\n";
                          break;
    case NOT_INLINE     : cout << "Misrecognized inline code"
                                  " (internal error!!!)\n";
                          break;
    case OUT_OF_MEMORY  : cout << "Out of memory.\n";
                          break;
    case FILE_NOT_FOUND : cout << "File not found:\n";
                          break;
    case READ_ERROR     : cout << "Read error\n";
                          break;
    case WRITE_ERROR    : cout << "Write error\n";
                          break;
    case MULTIPLE_MAKE  : cout << "Multiple MAKE directives:\n";
                          break;
    case MAKE_EXPECTED  : cout << "MAKE directive expected:\n";
                          break;
    case USE_EXPECTED   : cout << "USE directive expected ";
                          break;
    case BLOCKS_LATE    : cout << "BLOCKS may not appear after USED:\n";
                          break;
    case MULTIPLE_BLOCKS
                        : cout << "Multiple BLOCKS directives:\n";
                          break;
    case ENDFILE_EXPECTED
                        : cout << "!endfile expected:\n";
                          break;
    case ENDFILE_WO_BF  : cout << "!endfile without !beginfile\n";
                          break;
    case END_WO_BEGIN   : cout << "!end without !begin\n";
                          break;
    case BLOCK_INTERRUPTED
                        : cout << "Block interrupted\n";
                          break;
    case END_EXPECTED   : cout << "!end expected\n";
                          break;
    case BLOCK_TOO_LONG : cout << "Block too long\n";
                          break;
    case DUPLICATE_NAMES
                        : cout << "Duplicate names:\n";
                          break;
    default             : cout << "Unknown.\n";
                          break;
  };

  if (text[0])
    cout << "  " << text << "\n";

  if (in[0])
    cout << "in " << in << "\n";

  if (terminal)
    exit((int) err);

}


void error(error_type err, int terminal, const char *text,
           long line, const char *fname)
{
  static char buf[32] = "line ";

  if (line >= 10000000L)                 // Ten million lines is enough.
    line = 9999999L;
  ltoa(line, &buf[5], 10);
  strcat(buf, " of ");
  strcat(buf, fname);

  error(err, terminal, text, buf);
}

