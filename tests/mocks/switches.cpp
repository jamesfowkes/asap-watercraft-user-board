/*
 * C standard library Includes
 */

#include <stdint.h>

#include "../../switches.h"

bool switch_pressed(SWITCHES sw) { (void)sw; return false; }
bool switch_not_pressed(SWITCHES sw) { (void)sw; return false; }
bool switch_just_pressed(SWITCHES sw) { (void)sw; return false; }
bool switch_just_released(SWITCHES sw) { (void)sw; return false; }
