///////////////////////////////////////////////////////////////////////////
/// Filename:       APTCOMP.CPP                                         ///
/// Long filename:  APTOR Compiler                                      ///
/// File type:      CPP Code                                            ///
/// Project:        APTOR                                               ///
///////////////////////////////////////////////////////////////////////////
/// For use with:   APTOR                                               ///
/// Uses:           INTRFACE, PARSING, COMPILE, COMMON                  ///
///////////////////////////////////////////////////////////////////////////
/// Synopsis: Compiles .apx files to accompany other files. If file     ///
///           extention is .apt, assumes entire file consists of APTOR  ///
///           directives. Otherwise, inline code is assumed, beginning  ///
///           "//!". See APTOR documentation for more information.      ///
///////////////////////////////////////////////////////////////////////////
/// Author: N. A. A. Mathewson                                          ///
/// Date: 6/26/95                      Revision: 0.2                    ///
///////////////////////////////////////////////////////////////////////////

#include <fstream.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "common.h"
#include "parsing.h"
#include "intrface.h"
#include "compile.h"

///////////////////////////////////////////////////////////////////////////
/// MAIN                                                                ///
///////////////////////////////////////////////////////////////////////////

int verbose;

int main(int argc, char *argv[])
{
  display_intro();

  command_line cmnd;
  parse_cline(cmnd, argc, argv);

  display_switches(cmnd);
  verbose = cmnd.is_verbose;

  handle_make(cmnd.source_name);

  return 0;
}


