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
		initialized = true;
	}
	return reader;
}
} // namespace GiNaC

