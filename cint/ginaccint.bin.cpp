/* ginaccint.bin.cpp:  Binary depends on CINTSYSDIR, better don't call it
 * directly.  Use the launcher (compiled from ginaccint.cpp) instead. */

#include "G__ci.h"   /* G__atpause is defined in G__ci.h */

#if (!defined(G__CINTVERSION)) || (G__CINTVERSION < 501460)
#error You need at least cint 5.14.60 to compile GiNaC-cint. Download it via http from root.cern.ch/root/Cint.html or via ftp from ftpthep.physik.uni-mainz.de/pub/cint
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
std::string ToString(const T & t)
{
	char buf[256];
	ostrstream(buf,sizeof(buf)) << t << ends;
	return buf;
}

basic * ex::last_created_or_assigned_bp = 0;
basic * ex::dummy_bp = 0;
long ex::last_created_or_assigned_exp = 0;

#endif // def OBSCURE_CINT_HACK

G__value exec_tempfile(std::string const & command);
char * process_permanentfile(std::string const & command);
void process_tempfile(std::string const & command);
void greeting(void);
void helpmessage(void);
std::string preprocess(char const * const line, bool & comment, bool & single_quote,
                       bool & double_quote, unsigned & open_braces);
void cleanup(void);
void sigterm_handler(int n);
void initialize(void);
void initialize_cint(void);
void restart(void);
bool is_command(std::string const & command, std::string & preprocessed,
                std::string const & comparevalue, bool substr=false);
bool readlines(istream * is, std::string & allcommands);
bool readfile(std::string const & filename, std::string & allcommands, bool shutup=false);
void savefile(std::string const & filename, std::string const & allcommands);

typedef list<char *> cplist;
cplist filenames;
bool redirect_output = false;
bool silent = false;

G__value exec_tempfile(std::string const & command)
{
	G__value retval;
	char *tmpfilename = tempnam(NULL,"ginac");
	std::ofstream fout;
	fout.open(tmpfilename);
	fout << "{" << std::endl << command << std::endl << "}" << std::endl;
	fout.close();
	G__store_undo_position();
	retval = G__exec_tempfile(tmpfilename);
	G__security_recover(stdout);
	remove(tmpfilename);
	free(tmpfilename);
	return retval;
}

char * process_permanentfile(std::string const & command)
{
	char *tmpfilename = tempnam(NULL,"ginac");
	if (!silent)
		std::cout << "creating file " << tmpfilename << std::endl;
	std::ofstream fout;
	fout.open(tmpfilename);
	fout << command << std::endl;
	fout.close();
	G__store_undo_position();
	G__loadfile(tmpfilename);
	G__security_recover(stdout);
	return tmpfilename;
}

void process_tempfile(std::string const & command)
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
		std::string varname = "Out"+ToString(++out_count);
		if (retval.obj.i!=ex::last_created_or_assigned_exp) {
			// an ex was returned, but this is not the ex which was created last
			// => this is not a temporary ex, but one that resides safely in memory
			
			// std::cout << "warning: using ex from retval (experimental)" << std::endl;
			ex::dummy_bp = ((ex *)(void *)(retval.obj.i))->bp;
			exec_tempfile("ex "+varname+"(*ex::dummy_bp);");
		} else if (ex::last_created_or_assigned_bp_can_be_converted_to_ex()) {
			exec_tempfile("ex "+varname+"(*ex::last_created_or_assigned_bp);");
		} else {
			std::cout << "warning: last_created_or_assigned_bp modified 0 or not evaluated or not dynallocated" << std::endl;
		}
		exec_tempfile(std::string()+"LLLAST=LLAST;\n"
		              +"LLAST=LAST;\n"
		              +"LAST="+varname+";\n"
		              +"if (ginac_cint_internal_redirect_output&&"
		              +"    ginac_cint_internal_fout.good()) {" 
		              +"    ginac_cint_internal_fout << \""+varname+" = \" << "+varname+" << endl << endl;"
		              +"} else {"
		              +"    std::cout << \""+varname+" = \" << "+varname+" << endl << endl;"
		              +"}");
	} else if (TYPES_EQUAL(retval,ref_symbol)||
	           TYPES_EQUAL(retval,ref_constant)||
	           TYPES_EQUAL(retval,ref_function)||
	           TYPES_EQUAL(retval,ref_power)||
	           TYPES_EQUAL(retval,ref_numeric)) {
		if (!basic_type_warning_already_displayed) {
			std::cout << std::endl
			          <<"WARNING: The return value of the last expression you entered was a symbol," << std::endl
			          << "constant, function, power or numeric, which cannot be safely displayed." << std::endl
			          << "To force the output, cast it explicitly to type 'ex' or use 'cout'," << std::endl
			          << "for example (assume 'x' is a symbol):" << std::endl
			          << PROMPT1 "ex(x);" << std::endl
			          << "OutX = x" << std::endl << std::endl
			          << PROMPT1 "cout << x << endl;" << std::endl
			          << "x" << std::endl << std::endl
			          << "This warning will not be shown again." << std::endl;
			basic_type_warning_already_displayed = true;
		}
	}
#endif // def OBSCURE_CINT_HACK
	return;
}

void greeting(void)
{
	std::cout << "Welcome to GiNaC-cint (" << PACKAGE << " V" << VERSION
	          << ", Cint V" << G__CINTVERSION << ")\n";
	std::cout << "  __,  _______  GiNaC: (C) 1999-2000 Johannes Gutenberg University Mainz,\n"
	          << " (__) *       | Germany.  Cint C/C++ interpreter: (C) 1995-2000 Masaharu\n"
	          << "  ._) i N a C | Goto and Agilent Technologies, Japan.  This is free software\n"
	          << "<-------------' with ABSOLUTELY NO WARRANTY.  For details, type `.warranty'\n"
	          << "Type `.help' for help.\n\n";
	return;
}

void helpmessage(void)
{
    std::cout << "GiNaC-cint recognizes some special commands which start with a dot:\n\n"
	          << "  .cint                    switch to cint interactive mode (see cint\n"
	          << "                           documentation for further details)\n"
	          << "  .function                define the body of a function (necessary due to a\n"
	          << "                           cint limitation)\n"
	          << "  .help                    the text you are currently reading\n"
	          << "  .q, .quit, .exit, .bye   quit GiNaC-cint\n"
	          << "  .read filename           read a file from disk and execute it in GiNaC-cint\n"
	          << "                           (recursive call is possible)\n"
	          << "  .redirect [filename]     redirect 'OutXY = ...' output to a file\n"
	          << "                           (.redirect alone redirects output back to console)\n"
	          << "  .restart                 restart GiNaC-cint (does not re-read command line\n"
	          << "                           files)\n"
	          << "  .save filename           save the commands you have entered so far in a file\n"
	          << "  .silent                  suppress 'OutXY = ...' output (variables are still\n"
	          << "                           accessible)\n"
	          << "  .warranty                information on redistribution and warranty\n"
	          << "  .> [filename]            same as .redirect [filename]\n\n"
	          << "Instead of '.cmd' you can also write '//GiNaC-cint.cmd' to be compatible with\n"
	          << "programs that will be compiled later.\n"
	          << "Additionally you can exit GiNaC-cint with quit; exit; or bye;\n\n";
    return;
}

void warrantymessage(void)
{
	std::cout << "GiNaC is free software; you can redistribute it and/or modify it under the\n"
	          << "the terms of the GNU General Public License as published by the Free Software\n"
	          << "Foundation; either version 2 of the License, or (at your option) any later\n"
	          << "version.\n"
	          << "This program is distributed in the hope that it will be useful, but WITHOUT\n"
	          << "ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS\n"
	          << "FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more\n"
	          << "details.\n"
	          << "You should have received a copy of the GNU General Public License along with\n"
	          << "this program. If not, write to the Free Software Foundation, 675 Mass Ave,\n"
	          << "Cambridge, MA 02139, USA.\n\n";
	std::cout << "Cint and associated tools are copyright by Agilent Technologies Japan Company\n"
	          << "and Masaharu Goto <MXJ02154@niftyserve.or.jp>.\n"
	          << "Source code, binary executable or library of Cint and associated tools can be\n"
	          << "used, modified and distributed with no royalty for any purpose provided that\n"
	          << "the copyright notice appear in all copies and that both that copyright notice\n"
	          << "and this permission notice appear in supporting documentation.\n"
	          << "Agilent Technologies Japan and the author make no representations about the\n"
	          << "suitability of this software for any purpose.  It is provided \"AS IS\"\n"
	          << "without express or implied warranty.\n";
    return;
}

/** "preprocess" the line entered to be able to decide if the command shall be
 *  executed directly or more input is needed or this is a special command.
 *  All whitespace will be removed.  All comments will be removed.  Open and
 *  close braces ( { and } ) outside strings will be counted. */
std::string preprocess(char const * const line, bool & comment, bool & single_quote,
                       bool & double_quote, unsigned & open_braces)
{
	std::string preprocessed;
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
				escape = true;
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
					++open_braces;
				}
				break;
			case '}':
				if ((!single_quote)&&(!double_quote)&&(!comment)&&(open_braces>0)) {
					--open_braces;
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
		++pos;
	}
	
	return preprocessed;
}

void cleanup(void)
{
	for (cplist::iterator it=filenames.begin(); it!=filenames.end(); ++it) {
		if (!silent)
			std::cout << "removing file " << *it << std::endl;
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
	atexit(cleanup);
	signal(SIGTERM,sigterm_handler);
	initialize_cint();
}    

void initialize_cint(void)
{
	G__init_cint("cint");    /* initialize cint */
	
#ifndef NO_NAMESPACE_GINAC
	exec_tempfile("using namespace GiNaC;");
#endif // ndef NO_NAMESPACE_GINAC
	
	exec_tempfile("ex LAST,LLAST,LLLAST;\n");
	exec_tempfile("bool ginac_cint_internal_redirect_output=false;\n");
	exec_tempfile("ofstream ginac_cint_internal_fout;\n");
}    

void restart(void)
{
	std::cout << "Restarting GiNaC-cint." << std::endl;
	G__scratch_all();
	initialize_cint();
}

void redirect(std::string const & filename,
              bool shutup=false)
{
	if (filename=="") {
		if (!shutup)
			std::cout << "Redirecting output back to console..." << std::endl;
		exec_tempfile(std::string()
		              +"ginac_cint_internal_redirect_output=false;\n"
		              +"ginac_cint_internal_fout.close();");
	} else {
		if (!shutup)
			std::cout << "Redirecting output to " << filename << "..." << std::endl;
		exec_tempfile(std::string()
		              +"ginac_cint_internal_redirect_output=true;\n"
		              +"ginac_cint_internal_fout.open(\""+filename+"\");\n");
	}
}

/** Sort out command line options and evaluate them.  Returns true if it
 *  succeeds and false otherwise. */
bool evaloption(const std::string & option)
{
	if (option=="--version") {
		std::cout << "GiNaC-cint (" << PACKAGE << " V" << VERSION
		          << ", Cint V" << G__CINTVERSION << ")\n";
		exit(0);
	}
	if (option=="--help") {
		std::cout << "GiNaC-cint (" << PACKAGE << " V" << VERSION
		          << ", Cint V" << G__CINTVERSION << ")\n";
		std::cout << "usage: ginaccint [option] [file ...]\n";
		std::cout << " --help           print this help message and exit\n"
		          << " --silent         invoke ginaccint in silent mode\n"
		          << " --version        print GiNaC version and Cint version and exit\n";
		exit(0);
	}
	if (option=="--silent") {
		redirect("/dev/null",true);
		silent = true;
		return true;
	}
	return false;
}

bool is_command(std::string const & command,
                std::string & preprocessed,
                std::string const & comparevalue,
                bool substr)
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
	if ((command=="//GiNaC-cint."+comparevalue+"\n") ||
		substr &&
		(command.substr(0,comparevalue.length()+13)=="//GiNaC-cint."+comparevalue)) {
		preprocessed = preprocess(command.substr(12).c_str(),comment,
		                          single_quote,double_quote,open_braces);
		return true;
	}
	return false;
}

bool readlines(istream * is,
               std::string & allcommands)
{
	char const * line;
	char prompt[G__ONELINE];
	std::string linebuffer;
	
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
		std::string command;
		std::string preprocessed;
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
			if (!silent)
				std::cout << "next expression can be a function definition" << std::endl;
			next_command_is_function = true;
		} else if (is_command(command,preprocessed,"cint")) {
			std::cout << std::endl << "switching to cint interactive mode" << std::endl;
			std::cout << "'h' for help, 'q' to quit, '{ statements }' or 'p [expression]' to evaluate" << std::endl;
			G__pause();
			std::cout << "back from cint" << std::endl;
		} else if (is_command(command,preprocessed,"help")) {
			helpmessage();
		} else if (is_command(command,preprocessed,"read",true)) {
			quit = readfile(preprocessed.substr(5),allcommands);
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
			if (!silent) {
				redirect("/dev/null");
				silent = true;
			} else {
				redirect("");
				silent = false;
			}
		} else if (is_command(command,preprocessed,"warranty")) {
			warrantymessage();
		/* insert tests for more special commands here */
		} else if (command.substr(0,2)=="#!") {
			// ignore lines which indicate that this file is executed as a script
		} else {
			// std::cout << "now processing: " << command << std::endl;
			if (next_command_is_function) {
				next_command_is_function = false;
				filenames.push_back(process_permanentfile(command));
			} else {
				process_tempfile(command);
			}
		}
		if (is!=NULL) {
			// test for end of file if reading from a stream
			eof = is->eof();
		} else {
			// save commands only when reading from keyboard
			allcommands += command;
		}

	}
	return quit;
} 

bool readfile(std::string const & filename,
              std::string & allcommands,
              bool shutup = false)
{
	if (!shutup)
		std::cout << "Reading commands from file " << filename << "." << std::endl;
	bool quit = false;
	std::ifstream fin;
	fin.open(filename.c_str());
	if (fin.good())
		quit = readlines(&fin,allcommands);
	else
		std::cout << "Cannot open " << filename << " for reading." << std::endl;
	fin.close();
	return quit;
}

void savefile(std::string const & filename, std::string const & allcommands)
{
	std::cout << "Saving commands to file " << filename << "." << std::endl;
	std::ofstream fout;
	fout.open(filename.c_str());
	if (fout.good()) {
		fout << allcommands;
		if (!fout.good()) {
			std::cout << "Cannot save commands to " << filename << "." << std::endl;
		}
	} else {
		std::cout << "Cannot open " << filename << " for writing." << std::endl;
	}
	fout.close();
}

int main(int argc, char * *argv) 
{
	std::string allcommands;
	initialize();
	
	bool quit = false;
	// sort out and evaluate recognized options from the argument list
	for (int i=1; i<argc; ++i)
		if (evaloption(argv[i])) {
			for (int j=i; j<argc-1; ++j)
				argv[j] = argv[j+1];
			--argc;
		}
	bool argsexist = argc>1;
	
	// greet the user if it makes sense
	if (isatty(0) && !silent)
		greeting();
	
	// evaluate files given as command line arguments
	if (argsexist) {
		allcommands = "/* Files given as command line arguments:\n";
		--argc;
		while (argc && !quit) {
			allcommands += std::string(argv[argc])+'\n';
			quit = readfile(argv[argc], allcommands, silent);
			--argc;
		}
		allcommands += "*/\n";
	}
	
	// evaluate input from command line or script
	if (!quit)
		readlines(NULL, allcommands);
	
	return 0;
}
