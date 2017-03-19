/*
 * util,cpp
 *
 * James Fowkes (jamesfowkes@gmail.com)
 * for ASAP Watercrafts
 *
 * Created March 2017
 * 
 */

/*
 * C standard library Includes
 */

#include <stdint.h>

/*
 * Public Functions
 */

uint32_t subtract_not_below_zero(uint32_t a, uint32_t b)
{
	if (a > b) { return a - b;}
	else { return 0;}
}
