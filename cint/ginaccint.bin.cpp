#include "G__ci.h"   /* G__atpause is defined in G__ci.h */

#if (!defined(G__CINTVERSION)) || (G__CINTVERSION < 501438)
#error You need at least cint 5.14.38 to compile GiNaC-cint. Download it via http from root.cern.ch/root/Cint.html or via ftp from ftpthep.physik.uni-mainz.de/pub/cint
#endif // (!defined(G__CINTVERSION)) || (G__CINTVERSION < 501438)

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "ginac/ginac.h"
#include "config.h"
#include <list>

#ifndef NO_NAMESPACE_GINAC
using namespace GiNaC;
#endif // ndef NO_NAMESPACE_GINAC

extern "C" G__value G__exec_tempfile G__P((char *file));
extern "C" void G__store_undo_position(void);

#define PROMPT1 "GiNaC> "
#define PROMPT2 "     > "

#ifdef OBSCURE_CINT_HACK

#include <strstream>

template<class T>
string ToString(const T & t)
{
    char buf[256];
    ostrstream(buf,sizeof(buf)) << t << ends;
    return buf;
}

basic * ex::last_created_or_assigned_bp = 0;
basic * ex::dummy_bp = 0;
long ex::last_created_or_assigned_exp = 0;

#endif // def OBSCURE_CINT_HACK

G__value exec_tempfile(string const & command);
char * process_permanentfile(string const & command);
void process_tempfile(string const & command);
void greeting(void);
void helpmessage(void);
string preprocess(char const * const line, bool & comment, bool & single_quote,
                  bool & double_quote, unsigned & open_braces);
void cleanup(void);
void sigterm_handler(int n);
void initialize(void);
void initialize_cint(void);
void restart(void);
bool is_command(string const & command, string & preprocessed,
                string const & comparevalue, bool substr=false);
bool readlines(istream * is, string & allcommands);
bool readfile(string const & filename, string & allcommands);
void savefile(string const & filename, string const & allcommands);

typedef list<char *> cplist;
cplist filenames;
bool redirect_output=false;

G__value exec_tempfile(string const & command)
{
    G__value retval;
    char *tmpfilename = tempnam(NULL,"ginac");
    ofstream fout;
    fout.open(tmpfilename);
    fout << "{" << endl << command << endl << "}" << endl;
    fout.close();
    G__store_undo_position();
    retval = G__exec_tempfile(tmpfilename);
    G__security_recover(stdout);
    remove(tmpfilename);
    free(tmpfilename);
    return retval;
}

char * process_permanentfile(string const & command)
{
    char *tmpfilename = tempnam(NULL,"ginac");
    cout << "creating file " << tmpfilename << endl;
    ofstream fout;
    fout.open(tmpfilename);
    fout << command << endl;
    fout.close();
    G__store_undo_position();
    G__loadfile(tmpfilename);
    G__security_recover(stdout);
    return tmpfilename;
}

void process_tempfile(string const & command)
{
#ifdef OBSCURE_CINT_HACK
    static G__value ref_symbol = exec_tempfile("symbol ginac_cint_internal_symbol; ginac_cint_internal_symbol;");
    static G__value ref_constant = exec_tempfile("constant ginac_cint_internal_constant; ginac_cint_internal_constant;");
    static G__value ref_function = exec_tempfile("sin(ginac_cint_internal_symbol);");
    static G__value ref_power = exec_tempfile("power(ex(ginac_cint_internal_symbol),ex(ginac_cint_internal_symbol));");
    static G__value ref_numeric = exec_tempfile("numeric ginac_cint_internal_numeric; ginac_cint_internal_numeric;");
    static G__value ref_ex = exec_tempfile("ex ginac_cint_internal_ex; ginac_cint_internal_ex;");
    static bool basic_type_warning_already_displayed = false;
#endif // def OBSCURE_CINT_HACK

    G__value retval = exec_tempfile(command);

#ifdef OBSCURE_CINT_HACK

    #define TYPES_EQUAL(A,B) (((A).type==(B).type) && ((A).tagnum==(B).tagnum))
    
    static unsigned out_count = 0;
    if (TYPES_EQUAL(retval,ref_ex)) {
        string varname = "Out"+ToString(++out_count);
        if (retval.obj.i!=ex::last_created_or_assigned_exp) {
            // an ex was returned, but this is not the ex which was created last
            // => this is not a temporary ex, but one that resides safely in memory
            
            // cout << "warning: using ex from retval (experimental)" << endl;
            ex::dummy_bp=((ex *)(void *)(retval.obj.i))->bp;
            exec_tempfile("ex "+varname+"(*ex::dummy_bp);");
        } else if (ex::last_created_or_assigned_bp_can_be_converted_to_ex()) {
            //string varfill;
            //for (int i=4-int(log10(out_count)); i>0; --i)
            //    varfill += ' ';
            exec_tempfile("ex "+varname+"(*ex::last_created_or_assigned_bp);");
        } else {
            cout << "warning: last_created_or_assigned_bp modified 0 or not evaluated or not dynallocated" << endl;
        }
        exec_tempfile(string()+"LLLAST=LLAST;\n"
                      +"LLAST=LAST;\n"
                      +"LAST="+varname+";\n"
                      +"if (ginac_cint_internal_redirect_output&&"
                      +"    ginac_cint_internal_fout.good()) {" 
                      +"    ginac_cint_internal_fout << \""+varname+" = \" << "+varname+" << endl << endl;"
                      +"} else {"
                      +"    cout << \""+varname+" = \" << "+varname+" << endl << endl;"
                      +"}");
    } else if (TYPES_EQUAL(retval,ref_symbol)||
               TYPES_EQUAL(retval,ref_constant)||
               TYPES_EQUAL(retval,ref_function)||
               TYPES_EQUAL(retval,ref_power)||
               TYPES_EQUAL(retval,ref_numeric)) {
        if (!basic_type_warning_already_displayed) {
            cout << endl
                 <<"WARNING: The return value of the last expression you entered was a symbol," << endl
                 << "constant, function, power or numeric, which cannot be safely displayed." << endl
                 << "To force the output, cast it explicitly to type 'ex' or use 'cout'," << endl
                 << "for example (assume 'x' is a symbol):" << endl
                 << PROMPT1 "ex(x);" << endl
                 << "OutX = x" << endl << endl
                 << PROMPT1 "cout << x << endl;" << endl
                 << "x" << endl << endl
                 << "This warning will not be shown again." << endl;
            basic_type_warning_already_displayed = true;
        }
    }
#endif // def OBSCURE_CINT_HACK
    return;
}

void greeting(void)
{
    cout << "Welcome to GiNaC-cint (" << PACKAGE << " V" << VERSION << ")" << endl;
    cout << "  __,  _______  GiNaC: (C) 1999-2000 Johannes Gutenberg University Mainz," << endl
         << " (__) *       | Germany.  Cint C/C++ interpreter: (C) 1995-2000 Masaharu" << endl
         << "  ._) i N a C | Goto and Agilent Technologies, Japan.  This is free software" << endl
         << "<-------------' with ABSOLUTELY NO WARRANTY.  For details, type `.warranty'" << endl
         << "Type .help for help." << endl
         << endl;
    return;
}

void helpmessage(void)
{
    cout << "GiNaC-cint recognizes some special commands which start with a dot:" << endl << endl
         << "  .cint                    switch to cint interactive mode (see cint" << endl
         << "                           documentation for further details)" << endl
         << "  .function                define the body of a function (necessary due to a" << endl
         << "                           cint limitation)" << endl
         << "  .help                    the text you are currently reading" << endl
         << "  .q, .quit, .exit, .bye   quit GiNaC-cint" << endl
         << "  .read filename           read a file from disk and execute it in GiNaC-cint" << endl
         << "                           (recursive call is possible)" << endl
         << "  .redirect [filename]     redirect 'OutXY = ...' output to a file" << endl
         << "                           (.redirect alone redirects output back to console)" << endl
         << "  .restart                 restart GiNaC-cint (does not re-read command line" << endl
         << "                           files)" << endl
         << "  .save filename           save the commands you have entered so far in a file" << endl
         << "  .silent                  suppress 'OutXY = ...' output (variables are still" << endl
         << "                           accessible)" << endl
         << "  .warranty                information on redistribution and warranty" << endl
         << "  .> [filename]            same as .redirect [filename]" << endl << endl
         << "Instead of '.cmd' you can also write '//GiNaC-cint.cmd' to be compatible with" << endl
         << "programs that will be compiled later." << endl
         << "Additionally you can exit GiNaC-cint with quit; exit; or bye;" << endl
         << endl;
    return;
}

void warrantymessage(void)
{
    cout << "GiNaC is free software; you can redistribute it and/or modify it under the" << endl
         << "the terms of the GNU General Public License as published by the Free Software" << endl
         << "Foundation; either version 2 of the License, or (at your option) any later" << endl
         << "version." << endl
         << "This program is distributed in the hope that it will be useful, but WITHOUT" << endl
         << "ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS" << endl
         << "FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more" << endl
         << "details." << endl
         << "You should have received a copy of the GNU General Public License along with" << endl
         << "this program. If not, write to the Free Software Foundation, 675 Mass Ave," << endl
         << "Cambridge, MA 02139, USA." << endl << endl;
    cout << "Cint and associated tools are copyright by Agilent Technologies Japan Company" << endl
         << "and Masaharu Goto <MXJ02154@niftyserve.or.jp>." << endl
         << "Source code, binary executable or library of Cint and associated tools can be" << endl
         << "used, modified and distributed with no royalty for any purpose provided that" << endl
         << "the copyright notice appear in all copies and that both that copyright notice" << endl
         << "and this permission notice appear in supporting documentation." << endl
         << "Agilent Technologies Japan and the author make no representations about the" << endl
         << "suitability of this software for any purpose.  It is provided \"AS IS\"" << endl
         << "without express or implied warranty." << endl;
    return;
}

string preprocess(char const * const line, bool & comment, bool & single_quote,
                  bool & double_quote, unsigned & open_braces)
{
    // "preprocess" the line entered to be able to decide if the command shall be
    // executed directly or more input is needed or this is a special command

    // ALL whitespace will be removed
    // all comments (/* */ and //) will be removed
    // open and close braces ( { and } ) outside strings will be counted 

    /*
    cout << "line=" << line << endl;
    cout << "comment=" << comment << ", single_quote=" << single_quote
         << ",double_quote=" << double_quote << ", open_braces=" << open_braces
         << endl;
    */
    
    string preprocessed;
    int pos = 0;
    bool end = false;
    bool escape = false;
    bool slash = false;
    bool asterisk = false;
    while ((line[pos]!='\0')&&!end) {
        if (escape) {
            // last character was a \, ignore this one
            escape = false;
        } else if (slash) {
            // last character was a /, test if * or /
            slash = false;
            if (line[pos]=='/') {
                end = true;
            } else if (line[pos]=='*') {
                comment = true;
            } else {
                preprocessed += '/';
                preprocessed += line[pos];
            }
        } else if (asterisk) {
            // last character was a *, test if /
            asterisk = false;
            if (line[pos]=='/') {
                comment = false;
            } else if (line[pos]=='*') {
                preprocessed += '*';
                asterisk = true;
            }
        } else {
            switch (line[pos]) {
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                // whitespace: ignore
                break;
            case '\\':
                // escape character, ignore next
                escape=true;
                break;
            case '"':
                if ((!single_quote)&&(!comment)) {
                    double_quote = !double_quote;
                }
                break;
                case '\'':
                    if ((!double_quote)&&(!comment)) {
                        single_quote = !single_quote;
                    }
                    break;
            case '{':
                if ((!single_quote)&&(!double_quote)&&(!comment)) {
                    open_braces++;
                }
                break;
            case '}':
                if ((!single_quote)&&(!double_quote)&&(!comment)&&(open_braces>0)) {
                    open_braces--;
                }
                break;
            case '/':
                slash = true;
                break;
            case '*':
                asterisk = true;
                break;
            default:
                preprocessed += line[pos];
            }
        }
        pos++;
    }

    /*
    cout << "preprocessed=" << preprocessed << endl;
    cout << "comment=" << comment << ", single_quote=" << single_quote
         << ",double_quote=" << double_quote << ", open_braces=" << open_braces
         << endl;
    */
    
    return preprocessed;
}

void cleanup(void)
{
    for (cplist::iterator it=filenames.begin(); it!=filenames.end(); ++it) {
        cout << "removing file " << *it << endl;
        remove(*it);
        free(*it);
    }
}

void sigterm_handler(int n)
{
    exit(1);
}

void initialize(void)
{
    if (isatty(0))
        greeting();

    atexit(cleanup);
    signal(SIGTERM,sigterm_handler);
    initialize_cint();
}    

void initialize_cint(void)
{
    G__init_cint("cint");    /* initialize cint */

    // no longer needed as of cint 5.14.31:
    // exec_tempfile("#include <string>\n");

#ifndef NO_NAMESPACE_GINAC
    exec_tempfile("using namespace GiNaC;");
#endif // ndef NO_NAMESPACE_GINAC
    
    exec_tempfile("ex LAST,LLAST,LLLAST;\n");
    exec_tempfile("bool ginac_cint_internal_redirect_output=false;\n");
    exec_tempfile("ofstream ginac_cint_internal_fout;\n");
}    

void restart(void)
{
    cout << "Restarting GiNaC-cint." << endl;
    G__scratch_all();
    initialize_cint();
}

void redirect(string const & filename)
{
    if (filename=="") {
        cout << "Redirecting output back to console..." << endl;
        exec_tempfile( string()
                      +"ginac_cint_internal_redirect_output=false;\n"
                      +"ginac_cint_internal_fout.close();");
    } else {
        cout << "Redirecting output to " << filename << "..." << endl;
        exec_tempfile( string()
                      +"ginac_cint_internal_redirect_output=true;\n"
                      +"ginac_cint_internal_fout.open(\""+filename+"\");\n");
    }
}

bool is_command(string const & command, string & preprocessed,
                string const & comparevalue, bool substr)
{
    bool single_quote = false;
    bool double_quote = false;
    bool comment = false;
    unsigned open_braces = 0;
    if ((preprocessed=="."+comparevalue)||
        substr&&(preprocessed.substr(0,comparevalue.length()+1)==
                 "."+comparevalue)) {
        return true;
    }
    if ((command=="//GiNaC-cint."+comparevalue+"\n")||
        substr&&(command.substr(0,comparevalue.length()+13)==
                 "//GiNaC-cint."+comparevalue)) {
        preprocessed = preprocess(command.substr(12).c_str(),comment,
                                  single_quote,double_quote,open_braces);
        return true;
    }
    return false;
}       

bool readlines(istream * is, string & allcommands)
{
    char const * line;
    char prompt[G__ONELINE];
    string linebuffer;
    
    bool quit = false;
    bool eof = false;
    bool next_command_is_function = false;
    bool single_quote = false;
    bool double_quote = false;
    bool comment = false;
    unsigned open_braces = 0;

    while ((!quit)&&(!eof)) {
        strcpy(prompt,PROMPT1);
        bool end_of_command = false;
        string command;
        string preprocessed;
        while (!end_of_command) {
            if (is==NULL) {
                line = G__input(prompt);
            } else {
                getline(*is,linebuffer);
                line = linebuffer.c_str();
            }
            command += line;
            command += "\n";
            preprocessed += preprocess(line,comment,single_quote,double_quote,open_braces);
            if ((open_braces==0)&&(!single_quote)&&(!double_quote)&&(!comment)) {
                unsigned l = preprocessed.length();
                if ((l==0)||
                    (preprocessed[0]=='#')||
                    (preprocessed[0]=='.')||
                    (preprocessed[l-1]==';')||
                    (preprocessed[l-1]=='}')) {
                    end_of_command = true;
                }
            }
            strcpy(prompt,PROMPT2);
        }
        if ((preprocessed=="quit;")||
            (preprocessed=="exit;")||
            (preprocessed=="bye;")||
            (is_command(command,preprocessed,"quit"))||
            (is_command(command,preprocessed,"exit"))||
            (is_command(command,preprocessed,"bye"))||
            (is_command(command,preprocessed,"q"))) {
            quit = true;
        } else if (is_command(command,preprocessed,"function")) {
            cout << "next expression can be a function definition" << endl;
            next_command_is_function=true;
        } else if (is_command(command,preprocessed,"cint")) {
            cout << endl << "switching to cint interactive mode" << endl;
            cout << "'h' for help, 'q' to quit, '{ statements }' or 'p [expression]' to evaluate" << endl;
            G__pause();
            cout << "back from cint" << endl;
        } else if (is_command(command,preprocessed,"help")) {
            helpmessage();
        } else if (is_command(command,preprocessed,"read",true)) {
            quit=readfile(preprocessed.substr(5),allcommands);
        } else if (is_command(command,preprocessed,"save",true)) {
            command = "/* "+command+" */"; // we do not want the .save command itself in saved files
            savefile(preprocessed.substr(5),allcommands);
        } else if (is_command(command,preprocessed,"restart")) {
            restart();
        } else if (is_command(command,preprocessed,"redirect",true)) {
            redirect(preprocessed.substr(9));
        } else if (is_command(command,preprocessed,">",true)) {
            redirect(preprocessed.substr(2));
        } else if (is_command(command,preprocessed,"silent")) {
            redirect("/dev/null");
        } else if (is_command(command,preprocessed,"warranty")) {
            warrantymessage();
        /* test for more special commands
        } else if (preprocessed==".xyz") {
            cout << "special command (TBD): " << command << endl;
        */
        } else if (command.substr(0,2)=="#!") {
            // ignore lines which indicate that this file is executed as a script
        } else {
            // cout << "now processing: " << command << endl;
            if (next_command_is_function) {
                next_command_is_function = false;
                filenames.push_back(process_permanentfile(command));
            } else {
                process_tempfile(command);
            }
        }
        if (is!=NULL) {
            // test for end of file if reading from a stream
            eof=is->eof();
        } else {
            // save commands only when reading from keyboard
            allcommands += command;
        }

    }
    return quit;
}    

bool readfile(string const & filename, string & allcommands)
{
    cout << "Reading commands from file " << filename << "." << endl;
    bool quit = false;
    ifstream fin;
    fin.open(filename.c_str());
    if (fin.good()) {
        quit = readlines(&fin,allcommands);
    } else {
        cout << "Cannot open " << filename << " for reading." << endl;
    }
    fin.close();
    return quit;
}

void savefile(string const & filename, string const & allcommands)
{
    cout << "Saving commands to file " << filename << "." << endl;
    ofstream fout;
    fout.open(filename.c_str());
    if (fout.good()) {
        fout << allcommands;
        if (!fout.good()) {
            cout << "Cannot save commands to " << filename << "." << endl;
        }
    } else {
        cout << "Cannot open " << filename << " for writing." << endl;
    }
    fout.close();
}

int main(int argc, char ** argv) 
{
    string allcommands;
    initialize();

    bool quit = false;
    bool argsexist = argc>1;

    if (argsexist) {
        allcommands = "/* Files given as command line arguments:\n";
    }
    
    argc--; argv++;
    while (argc && !quit) {
        allcommands += *argv;
        allcommands += "\n";
        quit=readfile(*argv,allcommands);
        argc--; argv++;
    }

    if (argsexist) {
        allcommands += "*/\n";
    }

    if (!quit) {
        readlines(NULL,allcommands);
    }

    return 0;
}
