///////////////////////////////////////////////////////////////////////////
/// Filename:      APTDUMP.CPP                                          ///
/// Long filename: APTOR .apx dumping file                              ///
/// File type:     C ++ code                                            ///
/// Project:       APTOR                                                ///
///////////////////////////////////////////////////////////////////////////
/// Uses:  APTOR, NAMEREF2, CRYPT                                       ///
///////////////////////////////////////////////////////////////////////////
/// Synopsis: This is a simple dumping program that will test APTOR.CPP ///
///           by echoing (to a file, or to the screen) a complete read- ///
///           out of a given .APX family.                               ///
///////////////////////////////////////////////////////////////////////////
/// Author: N.A.A. Mathewson                                            ///
/// Date: 7/19/95                      Revision: 1.0                    ///
///////////////////////////////////////////////////////////////////////////

#include <fstream.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dir.h>

#include "flag.h"
#include "aptor.h"

#define FNAME_LEN 12

#define DEBUGGING 1

/////////////
// GLOBALS //
/////////////

flag verbose;

/////////////
// STRUCTS //
/////////////

struct control
{
  char family[FNAME_LEN+1];
  char apx_name[FNAME_LEN+1];
  char map_name[FNAME_LEN+1];
  char mpx_name[FNAME_LEN+1];

  flag mpx;                                 // TRUE: use .MPX has been forced.

  flag out_file;
  char out_name[FNAME_LEN+1];
};

enum types
{
  APX = 0,
  MAP = 1,
  MPX = 2
};

struct which_files
{
  flag apx;
  flag map;
  flag mpx;
};

/////////////
// PARSING //
/////////////

void build_control( control &c, int argc, char *argv[] );
  // Builds the given control structure from the command line and from the
  //   disk directory.

void check_files( control &c, which_files &w );
  // Checks to see which files of a given family name exist.

void parse_cmd_line(control &c, int argc, char *argv[]);
  // Called by build_control.

void subparse_cmd_line(control &c, char *word);
  // Called by parse_cmd_line.

int parse_filename(char *d, const char *s);
  // Copies & parsesfilename s into d, returning nonzero for error.

char *new_ext(char *f, const char *ext);
  // Tacks extention ext onto parsed fname f.

//////////
// HELP //
//////////
void show_help(void);
  // Displays the standard help screen. Exits APTDUMP.

///////////////
// DEBUGGING //
///////////////

void show_control(control &c);
  // Pretty-prints members of c.

//////////
// DUMP //
//////////

void dump(control &c);
  // Opens APTOR_file, output file, and handles all actual dumping.

void print(APTOR_file &af, int n,
           ostream &out, flag mpx);
  // Prints a given block to the given output device.

void header(control &c, APTOR_file &af, ostream &out);
  // Prints a header to the output device.


//////////
// MAIN //
//////////

int main(int argc, char *argv[])
{
  control c;

  build_control(c, argc, argv);

  if (DEBUGGING || verbose)
    show_control(c);

  dump(c);

  return 0;

};

/////////////
// PARSING //
/////////////

void build_control(control &c, int argc, char *argv[])
{
// SETUP MEMBERS
  c.family[0]   = '\0';
  c.apx_name[0] = '\0';
  c.map_name[0] = '\0';
  c.mpx_name[0] = '\0';
  c.out_name[0] = '\0';

  c.out_file = FALSE;
  c.mpx      = FALSE;

  parse_cmd_line(c, argc, argv);

  if (! c.family[0])
  {
    cout << "ERROR: No filename specified.\n";
    exit(1);
  };

  if (c.out_name[0])
    c.out_file = TRUE;

  which_files w;
  check_files(c, w);

  if (! w.apx)
  {
    cout << "ERROR: " << c.apx_name << " does not exist.\n";
    exit(1);
  };

  if (c.mpx)
  {
    if (! w.mpx)
    {
      cout << "ERROR: " << new_ext(c.family, "MPX") << " does not exist.\n";
      exit(1);
    };
  }
  else
  {
    if (! w.map)
    {
      if (w.mpx)
        c.mpx = TRUE;
      else
      {
        cout << "ERROR: Can't find " << new_ext(c.family, "MAP") <<
                " or " << new_ext(c.family, "MPX");
        exit(1);
      };
    };
  };

}

void check_files(control &c, which_files &w)
{
  ffblk ff;
  char *cp;
  int done;

  w.apx = FALSE;
  w.map = FALSE;
  w.mpx = FALSE;

  new_ext(c.family, "   ");

// FIRST, FIND A GOOD APX

  strcpy(c.apx_name, c.family);
  new_ext(c.apx_name, "APX");

  if (findfirst(c.apx_name, &ff, 0))      // no apx found?
    return;

  if (parse_filename(c.apx_name, ff.ff_name))
    return;

  w.apx = TRUE;

// THEN, SEE WHAT ELSE THERE IS!

  strcpy(c.family, c.apx_name);
  new_ext(c.family, "*  ");

  if (findfirst(c.family, &ff, 0))
    return;
  done = FALSE;

  while (! done)
  {
    cp = strchr(ff.ff_name, '.');
    if (strcmpi(cp, ".map") == 0)
    {
      parse_filename(c.map_name, ff.ff_name);
      w.map = TRUE;
    };
    if (strcmpi(cp, ".mpx") == 0)
    {
      parse_filename(c.mpx_name, ff.ff_name);
      w.mpx = TRUE;
    };
    done = findnext(&ff);
  };

}

void parse_cmd_line(control &c, int argc, char *argv[])
{
  if (argc == 1)
    show_help();

  for(int i = 1; i < argc; ++i)
    subparse_cmd_line(c, argv[i]);
}

void subparse_cmd_line(control &c, char *word)
{
  strlwr(word);

  if ((word[0] == '-') ||
      (word[0] == '/'))
  {
    if ( (strcmp(&word[1], "?") == 0) ||
         (strcmp(&word[1], "h") == 0) )
      show_help();
    else if (strcmp(&word[1], "mpx") == 0)
      c.mpx = TRUE;
    else if (strcmp(&word[1], "v") == 0)
      verbose = TRUE;
    else if (word[1] == 'f')
    {
      if (c.out_name[0] != '\0')
      {
        cout << "ERROR: Multiple output files.\n";
        exit(1);
      };
      if (parse_filename(c.out_name, &word[2]))
      {
        cout << "Bad output filename: " << (&word[2])
             << "\n";
        exit(1);
      };
    }
    else
    {
      cout << "Bad switch: " << word << "\n";
      exit(1);
    };
  }
  else
  {
    if (c.family[0] != '\0')
    {
      cout << "ERROR: Multiple input files.\n";
      exit(1);
    };
    if (parse_filename(c.family, word))
    {
      cout << "Bad filename: " << word << "\n";
      exit(1);
    };
  };
}

int parse_filename(char *d, const char *s)
{
  if (strlen(s) > FNAME_LEN)            // s too long?
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

char *new_ext(char *f, const char *ext)
{
  char *d;
  d = f;
  while ((*d) && (*d != '.'))
    ++d;

  ++d;

  strcpy(d, ext);

  return f;
}

//////////
// HELP //
//////////

void show_help(void)
{
  cout <<
"SYNTAX: \n"
"   APTDUMP [switches] <filename> [switches]\n\n"
"-mpx    : use .mpx file only.\n"
"-f!!!!  : echo dump to file !!!!\n"
"-v      : verbose messages (for debugging)\n"
"-?      : display this screeen.\n";

  exit(0);
}

///////////////
// DEBUGGING //
///////////////

void show_control(control &c)
{
  cout << "Family name: " << c.family << "\n"
          ".apx   name: " << c.apx_name << "\n"
          ".map   name: " << c.map_name << "\n"
          ".mpx   name: " << c.mpx_name << "\n";

  if (c.mpx)
    cout << "Reading map from " << c.mpx_name << "\n";
  else
    cout << "Reading map from " << c.map_name << "\n";

  if (c.out_file)
    cout << "Writing info to  " << c.out_name << "\n\n";
  else
    cout << "Writing info to the screen.\n\n";
}

//////////
// DUMP //
//////////

void dump(control &c)
{
  ofstream *out;
// OPEN OUTPUT FILE
  if (c.out_file)
  {
    out = new ofstream(c.out_name);
    if (! out->good())
    {
      cout << "ERROR: Unable to open " << c.out_name << "\n";
      exit(1);
    };
  };

// OPEN APTOR FILE
  APTOR_file af;

  if (c.mpx)
    af.open(c.apx_name, c.mpx_name);
  else
    af.open(c.apx_name, c.map_name);

  if (! af.ok())
  {
    cout << "ERROR: Unable to open APTOR_file\n";
    exit(1);
  };

// DUMP!

  header(c, af, cout);
  if (c.out_file)
    header(c, af, *out);

  int i, n;
  n = af.n();

  for(i = 0; i < n; ++i)
  {
    print(af, i, cout, c.mpx);
    if (c.out_file)
      print(af, i, *out, c.mpx);
  };

  if (c.out_file)
    out->close();

  af.close();
};

void print(APTOR_file &af, int n, ostream &out, flag mpx)
{
  static char line[80];
  static char word[80];

  for(int i = 0; i < 79; ++i)
    line[i] = '-';
  line[i] = '\0';

  itoa(n, word, 10);

  if (! mpx)
  {
    strcat(word, " ");
    strcat(word, af.name(n));
  };

  int pos, len;
  len = strlen(word);
  pos = (79 - len) / 2;

  strcpy(&line[pos], word);
  line[pos-1] = ' ';
  line[pos+len] = ' ';

  out << line << "\n";
  out << af[n] << "\n";
};

void header(control &c, APTOR_file &af, ostream &out)
{
  static char line[] = "\
-------------------------------------------------------------------------------\
";

  out << line << "\n";
  out << "TRANSCRIPT OF " << c.apx_name;
  if (c.mpx)
    out << "\n";
  else
    out << ", WITH BLOCK NAMES\n";

  out << "TOTAL OF " << af.n() << " BLOCKS\n";

  out << line << "\n";
}

  
