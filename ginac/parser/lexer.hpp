#ifndef GINAC_LEXER_HPP_
#define GINAC_LEXER_HPP_
#include <iosfwd>
#include <string>
#include <cstddef>
namespace GiNaC
{

class lexer
{
	std::istream* input;
	std::ostream* output;
	std::ostream* error;
	/// last character read from stream
	int c;
	/// identifier and number tokens are stored here
	std::string str;
	std::size_t line_num;
	std::size_t column;
	friend class parser;
public:

	lexer(std::istream* in = 0, std::ostream* out = 0, std::ostream* err = 0);
	~lexer();

	int gettok();
	void switch_input(std::istream* in);

	struct token_type
	{
		enum
		{
			eof		= -1,
			identifier	= -4,
			number		= -5,
			literal		= -6

		};
	};
};

} // namespace GiNaC

#endif // GINAC_LEXER_HPP_

