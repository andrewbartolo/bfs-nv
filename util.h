/*
 * Centralized definition of spin.
 * TODO replace these with a more-efficient semaphore, etc. impl.
 * or some sort of sleeping, energy-efficient polling (use hardware?).
 */

#define spinWhile(fn_to_eval) while(fn_to_eval) { }


/*
 * Min macro. Currently unused.
 */
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
