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

#define PROMPT "GiNaC> "

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
basic * ex::dummy_bp=0;
long ex::last_created_or_assigned_exp=0;

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
    static bool basic_type_warning_already_displayed=false;
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
                      +"cout << \""+varname+" = \" << "+varname+" << endl << endl;");
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
                 << PROMPT "ex(x);" << endl
                 << "OutX = x" << endl << endl
                 << PROMPT "cout << x << endl;" << endl
                 << "x" << endl << endl
                 << "This warning will not be shown again." << endl;
            basic_type_warning_already_displayed=true;
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
         << "<-------------' Germany and licensed under the terms and conditions of the GPL." << endl
         << endl;
}

string preprocess(char const * line, bool & comment, bool & single_quote,
                  bool & double_quote, unsigned & open_braces)
{
    // "preprocess" the line entered to be able to decide if the command shall be
    // executed directly or more input is needed or this is a special command

    // all whitespace will be removed
    // all comments (/* */ and //) will be removed
    // open and close braces ( { and } ) outside strings will be counted 

    /*
    cout << "line=" << line << endl;
    cout << "comment=" << comment << ", single_quote=" << single_quote
         << ",double_quote=" << double_quote << ", open_braces=" << open_braces
         << endl;
    */
    
    string preprocessed;
    int pos=0;
    bool end=false;
    bool escape=false;
    bool slash=false;
    bool asterisk=false;
    while ((line[pos]!='\0')&&!end) {
        if (escape) {
            // last character was a \, ignore this one
            escape=false;
        } else if (slash) {
            // last character was a /, test if * or /
            slash=false;
            if (line[pos]=='/') {
                end=true;
            } else if (line[pos]=='*') {
                comment=true;
            } else {
                preprocessed += '/';
            }
        } else if (asterisk) {
            // last character was a *, test if /
            asterisk=false;
            if (line[pos]=='/') {
                comment=false;
            } else if (line[pos]=='*') {
                preprocessed += '*';
                asterisk=true;
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
                slash=true;
                break;
            case '*':
                asterisk=true;
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
                
int main(void) 
{
    char *line;
    char prompt[G__ONELINE];

    if (isatty(0))
        greeting();

    atexit(cleanup);
    signal(SIGTERM,sigterm_handler);
    
    G__init_cint("cint");    /* initialize cint */

    // no longer needed as of cint 5.14.31
    // exec_tempfile("#include <string>\n");

    exec_tempfile("ex LAST,LLAST,LLLAST;\n");
    
    bool quit = false;
    bool next_command_is_function=false;
    bool single_quote=false;
    bool double_quote=false;
    bool comment=false;
    unsigned open_braces=0;

    while (!quit) {
        strcpy(prompt,PROMPT);
        bool end_of_command = false;
        string command;
        string preprocessed;
        while (!end_of_command) {
            line = G__input(prompt);
            command += line;
            command += "\n";
            preprocessed += preprocess(line,comment,single_quote,double_quote,open_braces);
            if ((open_braces==0)&&(!single_quote)&&(!double_quote)&&(!comment)) {
                unsigned l=preprocessed.length();
                if ((l==0)||
                    (preprocessed[0]=='#')||
                    (preprocessed[0]=='.')||
                    (preprocessed[l-1]==';')||
                    (preprocessed[l-1]=='}')) {
                    end_of_command=true;
                }
            }
            strcpy(prompt,"     > ");
        }
        if ((preprocessed=="quit;")||
            (preprocessed=="exit;")||
            (preprocessed=="bye;")||
            (preprocessed==".q")||
            (preprocessed==".quit")||
            (preprocessed==".exit")||
            (preprocessed==".bye")) {
            quit = true;
        } else if (preprocessed==".function") {
            cout << "next expression can be a function definition" << endl;
            next_command_is_function=true;
        } else if (preprocessed==".cint") {
            cout << endl << "switching to cint interactive mode" << endl;
            cout << "'h' for help, 'q' to quit, '{ statements }' or 'p [expression]' to evaluate" << endl;
            G__pause();
            cout << "back from cint" << endl;
        /* test for more special commands
        } else if (preprocessed==".xyz") {
            cout << "special command (TBD): " << command << endl;
        */
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







