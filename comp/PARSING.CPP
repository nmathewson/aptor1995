///////////////////////////////////////////////////////////////////////////
/// Filename:      PARSING.CPP                                          ///
/// Long filename: APTCOMP Parsing routines                             ///
/// File type:     C++ code                                             ///
/// Project:       APTOR                                                ///
///////////////////////////////////////////////////////////////////////////
/// For use with:  PARSING.H                                            ///
/// Uses:          common, parsing                                      ///
///////////////////////////////////////////////////////////////////////////
/// Synopsis: Routines to handle command-line parsing and sourcecode    ///
///           parsing for APTCOMP.                                      ///
///////////////////////////////////////////////////////////////////////////
/// Author: N. A. A. Mathewson                                          ///
/// Date: 6/30/95                      Revision: 0.2                    ///
///////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "parsing.h"
#include "common.h"

#define LONGEST_CMD_NAME 10             // !beginfile, in this case.
#define MAX_FLAG_LEN 3                  // They're all the same.

///////////////////////////////////////////////////////////////////////////
/// INTERNAL DATA/STRUCTURES                                            ///
///////////////////////////////////////////////////////////////////////////

struct ptable_entry                     // Entry in a parsing table
{
  int token;
  char name[LONGEST_CMD_NAME+1];
};

typedef ptable_entry ptable[];

#define N_MAKE_TOKENS 3

const ptable make_ptable =              // Table for makefile directives
{
  { BLOCKS_DRC,     "BLOCKS" },
  { MAKE_DRC,       "MAKE" },
  { USE_DRC,        "USE"  }
};

#define N_SOURCETYPE_TOKENS 2

const ptable sourcetype_ptable =        // Table for USE directives
{
  { INLINE_INF,     "INLINE" },
  { APTORCODE_INF,  "APTOR" }
};

#define N_COMMAND_TOKENS 4

const ptable command_ptable =           // Table for APTORCode commands
{
  { BEGIN_ACMD,     "!begin" },
  { END_ACMD,       "!end" },
  { BEGINFILE_ACMD, "!beginfile" },
  { ENDFILE_ACMD,   "!endfile" },
};

#define N_MAKE_FLAG_TOKENS 5

const ptable make_flag_ptable =
{
  { ALL_MF,         "ALL" },
  { APX_MF,         "APX" },
  { MAP_MF,         "MAP" },
  { DEF_MF,         "DEF" },
  { MPX_MF,         "MPX" }
};

///////////////////////////////////////////////////////////////////////////
/// INTERNAL PROTOTYPES                                                 ///
///////////////////////////////////////////////////////////////////////////

void sub_parse_cmd(command_line &cline, const char *s);
    // Parses a piece of the command-line.

int get_token(const ptable table, const char *s);
    // Looks up a member of a ptable

const char *skip_wspace(const char *line);
    // Skips whitespace, returns ptr to first non wspace character.

const char *get_next_word(const char *line);
    // Skips nonspaces, then skips spaces.

void copy_word(char *d, const char *s, int n);
    // Copies at most n nonwhitespace characters from s to d. Stops copying
    // upon encountering whitespace, or NULL.

input_file_type get_source_type(const char *f);
    // Assumes parse_filename has been called.

error_type parse_make_flags(make_flags &mf, const char *line);
    // Parses the rest of a line into a make_flags type. Returns 0 for OK,
    // != 0 for error condition.

inline int one_slash(char *str)
{
  return ((str[0] == '\\') &&
          ((isspace(str[1])) || (! str[1])) );
};

///////////////////////////////////////////////////////////////////////////
/// COMMAND LINE PARSING                                                ///
///////////////////////////////////////////////////////////////////////////

void parse_cline(command_line &cline,
                 int argc,
                 char *argv[])
{
  cline.is_verbose = 0;

  cline.source_name[0] = '\0';

  if (argc == 1)
    display_help(1);

  for(int i = 1; i < argc; i++)                 // Parse each word
    sub_parse_cmd(cline, argv[i]);

  if (cline.source_name[0] == '\0')             // Every line NEEDS a source
    error(NO_SOURCE, TERMINAL);                 //   file.
}

void sub_parse_cmd(command_line &cline,
                   const char *s)
{
  if (s[0] == 0)
    return;

  if ( (s[0] == '-') ||
       (s[0] == '/') )
  {
    if (s[2] == '\0')
      switch (toupper(s[1]))
      {
        case '?' : display_help(1);
                   break;
        case 'H' : display_help(2);
                   break;
        case 'M' : display_help(3);
                   break;
        case 'V' : cline.is_verbose = 1;
                   break;
        default  : error(BAD_SWITCH, TERMINAL, s);
                   break;
      }
    else
      error(BAD_SWITCH, TERMINAL, s);
  }
  else
    if (parse_filename(cline.source_name, s))
      error(BAD_FILENAME, TERMINAL, s, "the command line.");
}

//////////////////////////////////////////////////////////////////////////
/// SHARED PARSING                                                     ///
//////////////////////////////////////////////////////////////////////////

int parse_filename(char *d, const char *s)
{
  if (d[0])                             // d already occupied?
    return 1;

  if (strlen(s) > MAX_FNAME_LEN)        // s too long?
    return 2;

  if (s[0] == 0)                        // s too short?
    return 3;

  strcpy(d, s);                         // copy s into d.

  for(int i = 0; d[i]; i++)             // make d uppercase.
    d[i] = toupper(d[i]);

  i = 0;
  int period_pos = -1;
  while(d[i])                           // make sure fname in bounds...
  {
    if ((d[i] == '*') &&                // no wildcards.
        (d[i] == '?'))
    {
      error(WILDCARDS_USED, NONTERMINAL);

      return 4;
    };

    if (d[i] == ' ')                    // This should never happen.
      return 5;

    if ((d[i] == '.') &&
        (period_pos > -1) )
      period_pos = 50;

    if ((d[i] == '.') &&
        (period_pos == -1))
      period_pos = i;

    i++;
  };

  if (period_pos > 8)                   // If more than 8 characters read
    return 6;                           //   before the '.', error.

  if (period_pos == 0)                  // At least 1 char in fname.
    return 7;

  // At this point, we know that FFF.XXX has 1<= FFF <=8. Maybe no '.',
  // and maybe XXX too long.

  if ((period_pos == -1) &&             // prevent FFFF > 8, no '.'
      (i > 8))
    return 8;

  if ((period_pos > -1) &&              // prevent .XXXX
      ((i - period_pos) > 4) )
    return 9;

  if (period_pos == -1)                 // There must be a period!!!
  {
    d[i] = '.';
    d[i+1] = '\x00';
  };

  return 0;                             // Everything's OK!!
}

input_file_type get_source_type(const char *f)
{
  while(*(f++) != '.')                          // parse_filename guarantees
    ;                                           //   the existance of a '.'

  if ( strcmp(f,"APT") == 0 )
    return APTORCODE_INF;
  if ( strcmp(f,"APM") == 0 )
    return MULTIPLE_INF;

  return INLINE_INF;
}

///////////////////////////////////////////////////////////////////////////
/// SOURCE PARSING                                                      ///
///////////////////////////////////////////////////////////////////////////

int is_APTORCode_line(const char *line)
{
  line = skip_wspace(line);

  return ((line[0] == '/') &&
          (line[1] == '/') &&
          (line[2] == '!'));
};

const char *skip_wspace(const char *line)
{
  while (isspace(*line))
    ++line;
  return line;
}

const char *get_next_word(const char *line)
{
  while ((*line) && (! isspace(*line)))
    ++line;
  return skip_wspace(line);
}

int get_token(const ptable table, const char *s, int ntokens)
{
  for(int i = 0; i < ntokens; ++i)
    if (strcmp(s, table[i].name) == 0)
      return (table[i].token);

  return -1;
}

void copy_word(char *d, const char *s, int n)
{
  int i = 0;
  while ((i < n) && (! isspace(s[i])) && (s[i]))
  {
    d[i] = s[i];
    ++i;
  };

  d[i] = '\0';
}

void parse_make_line(make_directive &m, const char *line)
{
  static char buf[MAX_ARG_LEN + 1];

  m.dir = NO_DRC;
  m.err = NO_ERROR;
  m.narg = 0;
  m.inf = (input_file_type) 0;
  m.sarg[0] = '\0';
  line = skip_wspace(line);

  if ((*line == '\0') || (*line == '#'))      // Line is empty
    return;

  copy_word(buf, line, MAX_ARG_LEN);
  strupr(buf);
  m.dir = (make_directive_type)
          get_token(make_ptable, buf, N_MAKE_TOKENS);

  if (m.dir == BAD_DRC)
  {
    m.err = UNREC_TOKEN;
    return;
  };

  line = get_next_word(line);

  if ((*line == '\0') || (*line == '#'))
  {
    m.err = TOO_FEW_ARGS;
    return;
  };

  copy_word(buf, line, MAX_ARG_LEN);

  switch(m.dir)
  {
    case BLOCKS_DRC : m.narg = atoi(buf);
                      if (! ORDERED3(MIN_BLOCKS, m.narg, MAX_BLOCKS) )
                        m.err = VAL_OUT_OF_BOUNDS;
                      break;
    case MAKE_DRC   : strupr(buf);
                      if ((strlen(buf) > 8)  ||
                          strchr(buf, '.') )
                      {
                        m.err = BAD_FILENAME;
                        break;
                      }
                      if (parse_filename(m.sarg, buf))
                      {
                        m.err = BAD_FILENAME;
                        break;
                      };
                      line = get_next_word(line);
                      if ((*line == '\0') || (*line == '#'))
                      {
                        m.err = TOO_FEW_ARGS;
                        break;
                      };
                      m.err = parse_make_flags(m.mf, line);
                      return;                     // Everything is handled.
    case USE_DRC    : if (parse_filename(m.sarg, buf))
                        m.err = BAD_FILENAME;
                      else
                      {
                        line = get_next_word(line);
                        copy_word(buf, line, MAX_ARG_LEN);
                        if ((*buf == '\0') || (*buf == '#'))
                        {
                          m.inf = get_source_type(m.sarg);
                          if (m.inf == MULTIPLE_INF)
                            m.inf = INLINE_INF;
                          return;
                        }
                        else
                        {
                          m.inf = (input_file_type)
                                  get_token(sourcetype_ptable, buf,
                                            N_SOURCETYPE_TOKENS);
                          if (m.inf == BAD_DRC)
                            m.err = UNREC_TOKEN;
                        };
                      };
                      break;
  };

  if (m.err)
    return;

  line = get_next_word(line);
  if (! ((*line == '\0') || (*line == '#')) )
    m.err = TOO_MANY_ARGS;

}

void parse_APTORCode_line(APTORCode_command &c, const char *line,
                          input_file_type inf)
{
  static char buf[MAX_ARG_LEN + 1];
  int over;

  c.cmd = NO_ACMD;
  c.err = NO_ERROR;
  c.sarg[0] = '\0';
  line = skip_wspace(line);

  if (inf == APTORCODE_INF)
    if (dbl_slash(line) || (*line == '\0'))
      return;

  if (inf == INLINE_INF)
  {
    if (! dbl_slash(line))
    {
      c.err = NOT_INLINE;
      return;
    }
    else
      line = &(line[2]);
  };

  copy_word(buf, line, MAX_ARG_LEN);
  c.cmd = (APTORCode_command_type)
          get_token(command_ptable, buf, N_COMMAND_TOKENS);

  if (c.cmd == BAD_ACMD)
  {
    c.err = UNREC_TOKEN;
    return;
  };

  line = get_next_word(line);
  over = (*line == '\0') || dbl_slash(line);
  if (! over)
  {
    copy_word(buf, line, MAX_ARG_LEN);
    line = get_next_word(line);
  };

  switch (c.cmd)
  {
    case BEGIN_ACMD     : if (over)
                            c.err = TOO_FEW_ARGS;
                           else
                             strcpy(c.sarg, buf);
                           break;
    case END_ACMD       :
    case ENDFILE_ACMD   : if (! over)
                            c.err = TOO_MANY_ARGS;
                          break;
    case BEGINFILE_ACMD : if (over)
                            c.err = TOO_FEW_ARGS;
                          else if ((strlen(buf) > 8) ||
                                    strchr(buf, '#') )
                            c.err = BAD_FILENAME;
                          else if (parse_filename(c.sarg, buf))
                            c.err = BAD_FILENAME;
                          break;
  };

  if (c.err)
    return;

  if (! ((*line == '\0') || dbl_slash(line)) )
    c.err = TOO_MANY_ARGS;

  return;
}

error_type parse_make_flags(make_flags &mf, const char *line)
{
  char buf[MAX_FLAG_LEN+2];
  make_flag_type flag;
  mf.apx = 1;
  mf.map = mf.def = mf.mpx = mf.all = 0;

  do
  {
    copy_word(buf, line, MAX_FLAG_LEN+1);
    strupr(buf);
    flag = (make_flag_type)
           get_token(make_flag_ptable, buf, N_MAKE_FLAG_TOKENS);
    switch (flag)
    {
      case NO_MF  : break;                 // This shouldn't happen.
      case APX_MF : break;                 // Does nothing; apx always true.
      case BAD_MF : return UNREC_TOKEN;
      case ALL_MF : mf.map = 1;
                    mf.def = 1;
                    mf.mpx = 1;
                    mf.all = 1;
                    break;
      case MAP_MF : mf.map = 1;
                    break;
      case DEF_MF : mf.def = 1;
                    break;
      case MPX_MF : mf.mpx = 1;
                    break;
    };
    line = get_next_word(line);
  }
  while ((*line != '\0') && (*line != '#'));

  return NO_ERROR;
};

int tighten_line(char *line)
{
  char *wspace;
  char *prev_wspace;
  int slash;

  wspace = line;
  slash = 0;

  do
  {
    prev_wspace = wspace;
    slash = one_slash(line);                 // If lone slash...
    while ( (! isspace(*line)) && (*line))   // Skip Until wspace..
      ++line;
    wspace = line;                           // Store beginning wspace.
    while (isspace(*line))                   // Skip wspace.
      ++line;
  }
  while (*line);

  if (slash)
    *prev_wspace = '\0';
  else
  {
    wspace[0] = '\n';
    wspace[1] = '\0';
  };

  return strlen(line);
}

