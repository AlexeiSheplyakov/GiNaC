#include "G__ci.h"   /* G__atpause is defined in G__ci.h */
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "ginac/ginac.h"
#include "config.h"
#include <list>

extern "C" G__value G__exec_tempfile G__P((char *file));
extern "C" void G__store_undo_position(void);

#ifdef OBSCURE_CINT_HACK

#include <strstream>

template<class T>
string ToString(const T & t)
{
    char buf[256];
    ostrstream(buf,sizeof(buf)) << t << ends;
    return buf;
}

basic * ex::last_created_or_assigned_bp=0;

#endif // def OBSCURE_CINT_HACK

typedef list<char *> cplist;

cplist filenames;

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

bool is_whitespace_char(char c)
{
    return ((c==' ') || (c=='\t') || (c=='\n') || (c=='\r')); 
}

char first_non_whitespace_char(char const * s)
{
    int l = strlen(s);
    int pos = 0;
    while ((pos<l)&&is_whitespace_char(s[pos])) pos++;
    return s[pos];
}    

char last_non_whitespace_char(char const * s)
{
    int pos = strlen(s)-1;
    while ((pos>=0) && is_whitespace_char(s[pos])) pos--;
    return s[pos];
}    

string strip_whitespace(string const & s)
{
    string s2;
    int l = s.length();
    for (int pos=0; pos<l; ++pos) {
        if (!is_whitespace_char(s[pos])) s2 += s[pos];
    }
    return s2;
}

G__value exec_tempfile(string const & command)
{
    G__value retval;
    char *tmpfilename = tempnam(NULL,"ginac");
    ofstream fout;
    fout.open(tmpfilename);
    fout << "{" << endl << command << endl << "}" << endl;
    fout.close();
    G__store_undo_position();
    retval=G__exec_tempfile(tmpfilename);
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
    static G__value ref_ex = exec_tempfile("ex ginac_cint_internal_ex; ginac_cint_internal_ex;");
    static G__value ref_function = exec_tempfile("sin(ginac_cint_internal_symbol);");
    static G__value ref_power = exec_tempfile("power(ginac_cint_internal_symbol,ginac_cint_internal_symbol);");
#endif // def OBSCURE_CINT_HACK

    G__value retval = exec_tempfile(command);

#ifdef OBSCURE_CINT_HACK

    #define TYPES_EQUAL(A,B) (((A).type==(B).type) && ((A).tagnum==(B).tagnum))
    
    static unsigned out_count = 0;
    if (TYPES_EQUAL(retval,ref_ex)) {
        if (ex::last_created_or_assigned_bp_can_be_converted_to_ex()) {
            string varname = "Out"+ToString(++out_count);
            //string varfill;
            //for (int i=4-int(log10(out_count)); i>0; --i)
            //    varfill += ' ';
            exec_tempfile("ex "+varname+"(*ex::last_created_or_assigned_bp);\n"
                          +"LLLAST=LLAST;\n"
                          +"LLAST=LAST;\n"
                          +"LAST="+varname+";\n"
                          +"cout << \""+varname+" = \" << "+varname+" << endl << endl;");
        } else {
            cout << "warning: last_created_or_assigned_bp modified 0 or not evaluated or not dynallocated" << endl;
        }
    }
#endif // def OBSCURE_CINT_HACK
}

void greeting(void)
{
    cout << "Welcome to GiNaC-cint (" << PACKAGE << " V" << VERSION << ")" << endl;
    cout << "This software is provided \"as is\" without any warranty.  Copyright of Cint is" << endl
         << "owned by Agilent Technologies Japan and Masaharu Goto.  Registration is" << endl
         << "  __,  _______  requested, at this moment, for commercial use.  Send e-mail to" << endl
         << " (__) *       | <MXJ02154@niftyserve.or.jp>.  The registration is free." << endl
         << "  ._) i N a C | The GiNaC framework is Copyright by Johannes Gutenberg Univ.," << endl
         << "<-------------' Germany and licensed under the terms and conditions of the GPL." << endl << endl;
}

int main(void) 
{
    char *line;
    char prompt[G__ONELINE];

    greeting();

    atexit(cleanup);
    signal(SIGTERM,sigterm_handler);
    
    G__init_cint("cint");    /* initialize cint */

    exec_tempfile("#include <string>\n");
    exec_tempfile("ex LAST,LLAST,LLLAST;\n");
    
    bool quit = false;
    bool next_command_is_function=false;    
    while (!quit) {
        strcpy(prompt,"GiNaC> ");
        int open_braces = 0;
        bool end_of_command=false;
        string command;
        while (!end_of_command) {
            line = G__input(prompt);
        
            int pos = 0;
            bool double_quote=false;
            bool single_quote=false;
            while(line[pos]!='\0') {
                switch(line[pos]) {
                case '"':
                    if (!single_quote) double_quote = !double_quote;
                    break;
                case '\'':
                    if (!double_quote) single_quote = !single_quote;
                    break;
                case '{':
                    if ((!single_quote) && (!double_quote)) open_braces++;
                    break;
                case '}':
                    if ((!single_quote) && (!double_quote)) open_braces--;
                    break;
                }
                pos++;
            }
            command += line;
            command += '\n';
            if (open_braces==0) {
                if ((first_non_whitespace_char(command.c_str())=='#')||
                    (first_non_whitespace_char(command.c_str())=='.')||
                    (last_non_whitespace_char(command.c_str())==';')||
                    (last_non_whitespace_char(command.c_str())=='}')) {
                    end_of_command=true;
                }
            }
            strcpy(prompt,"     > ");
        }
        string stripped_command=strip_whitespace(command);
        if ((stripped_command=="quit;")||
            (stripped_command=="exit;")||
            (stripped_command=="bye;")||
            (stripped_command==".q")||
            (stripped_command==".quit")||
            (stripped_command==".exit")||
            (stripped_command==".bye")) {
            quit = true;
        } else if (stripped_command==".function") {
            cout << "next expression can be a function definition" << endl;
            next_command_is_function=true;
        } else if (stripped_command==".cint") {
            cout << endl << "switching to cint interactive mode" << endl;
            cout << "'h' for help, 'q' to quit, '{ statements }' or 'p [expression]' to evaluate" << endl;
            G__pause();
            cout << "back from cint" << endl;
        } else if (command[0]=='.') {
            cout << "special command (TBD): " << command << endl;
        } else {
            // cout << "now processing: " << command << endl;
            if (next_command_is_function) {
                next_command_is_function = false;
                filenames.push_back(process_permanentfile(command));
            } else {
                process_tempfile(command);
            }
        }
    }

    return 0;
}







