[+ AutoGen5 template .cpp +][+ 
COMMENT a part of GiNaC parser -- construct functions from a byte stream.
+][+
(use-modules (ice-9 format))

(define (sequence start end . step)
  (let ((step (if (null? step) 1 (car step))))
    (let loop ((n start))
      (if (> n end) '() (cons n (loop (+ step n)))))))
+]/*
[+ (dne " * " " * " ) +]
 *
 * If you want to change this file, edit either `[+ (def-file) +]' or
 * `[+ (tpl-file) +]' file, and run the following command:
 *
 * autogen -T [+ (tpl-file) +] [+ (def-file) +]
 */
#include "parse_context.h"
#include "power.h"
#include "operators.h"
#include "inifcns.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDINT_H
#include <stdint.h> // for uintptr_t
#endif

namespace GiNaC
{
[+ FOR function +]
static ex [+ (get "name") +]_reader(const exvector& ev)
{
	return GiNaC::[+ (get "name") +]([+
		(let ((nargs (if (exist? "args")
		                 (string->number (get "args")) 1)))
		  (format '#f "~{ev[~a]~^, ~}" (sequence 0 (- nargs 1)))) +]);
}[+ ENDFOR +]

// function::registered_functions() is protected, but we need to access it
class registered_functions_hack : public function
{
public:
	static const std::vector<function_options>& get_registered_functions()
	{
		return function::registered_functions();
	}
private:
	registered_functions_hack();
	registered_functions_hack(const registered_functions_hack&);
	registered_functions_hack& operator=(const registered_functions_hack&);
};

// Encode an integer into a pointer to a function. Since functions
// are aligned (the minimal alignment depends on CPU architecture)
// we can distinguish between pointers and integers.
static reader_func encode_serial_as_reader_func(unsigned serial)
{
	uintptr_t u = (uintptr_t)serial;
	u = (u << 1) | (uintptr_t)1;
	reader_func ptr = reinterpret_cast<reader_func>((void *)u);
	return ptr;
}

const prototype_table& get_default_reader()
{
	using std::make_pair;
	static bool initialized = false;
	static prototype_table reader;
	if (!initialized) {
		[+ FOR function +]
		reader[make_pair("[+ (get "name") +]", [+ 
			(if (exist? "args") (get "args") "1")
			+])] = [+ (get "name") +]_reader;[+
		ENDFOR +]
		std::vector<function_options>::const_iterator it =
			registered_functions_hack::get_registered_functions().begin();
		std::vector<function_options>::const_iterator end =
			registered_functions_hack::get_registered_functions().end();
		unsigned serial = 0;
		for (; it != end; ++it) {
			prototype proto = make_pair(it->get_name(), it->get_nparams());
			reader[proto] = encode_serial_as_reader_func(serial);
			++serial;
		}
		initialized = true;
	}
	return reader;
}

} // namespace GiNaC
