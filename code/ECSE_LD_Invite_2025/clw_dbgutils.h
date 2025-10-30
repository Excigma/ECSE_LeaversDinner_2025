/*--------------------------------------------------
 * CLW_dbgutils.h 
 * Author: Campbell Wright 
 * This is an expanding collection of Macros and other 
 * utilities I've employed across various projects - 
 * Feel free to use as you wish.
 */

#include <stdio.h>


#ifndef CLW_DBGUTILS
#define CLW_DBGUTILS
#include <string.h>
#include "pico/error.h"
//Colour:
// from https://stackoverflow.com/questions/4842424/list-of-ansi-color-escape-sequences

#define COLOUR_NONE  "\033[0m"
#define BLACK       "\033[30m"
#define RED	        "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE	    "\033[34m"
#define MAGENTA     "\033[35m"
#define CYAN	    "\033[36m"
#define WHITE	    "\033[37m"
#define BR_BLACK    "\033[90m"
#define BR_RED      "\033[91m"
#define BR_GREEN    "\033[92m"
#define BR_YELLOW   "\033[93m"
#define BR_BLUE	    "\033[94m"
#define BR_MAGENTA	"\033[95m"
#define BR_CYAN	    "\033[96m"
#define BR_WHITE    "\033[97m"

#define BG_BLACK        "\033[40m"
#define BG_RED	        "\033[41m"
#define BG_GREEN        "\033[42m"
#define BG_YELLOW       "\033[43m"
#define BG_BLUE         "\033[44m"
#define BG_MAGENTA      "\033[45m"
#define BG_CYAN	        "\033[46m"
#define BG_WHITE	    "\033[47m"
#define BG_BR_BLACK     "\033[100m"
#define BG_BR_RED       "\033[101m"
#define BG_BR_GREEN     "\033[102m"
#define BG_BR_YELLOW    "\033[103m"
#define BG_BR_BLUE	    "\033[104m"
#define BG_BR_MAGENTA	"\033[105m"
#define BG_BR_CYAN	    "\033[106m"
#define BG_BR_WHITE     "\033[107m"




#define ERROR(__err, __fmt, ...) \
    printf(BR_RED "ERROR!" BR_WHITE " %s (%d) in func %s at line %d. Info: "__fmt, strerror(__err), __err, __func__, __LINE__ __VA_OPT__(,) __VA_ARGS__, "\n");


                                                                      
#define CLW_LOGO "                &&&&&&&&&&&&&&&&&               \n           X&&&&&&&&&&&&&&&&&&&&&&&&&            \n         &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&         \n       &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&       \n     &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&     \n    &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&    \n   &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&   \n  &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&  \n &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& \n+&               &&     &&&&&&&&&&&&&&&&&&&&&&&&&\n&&&&              &&      &&&&&&&&&&&&&&&&&&&&&&&\n&&&&&      &&&&&&&&&&      &&&&&&&&&&&&&&&&&&&&&&\n&&&&&&      &&&&&&&&&&      &&&&&&&&&&&&&&&&&&&&&\n&&&&&&&      &&&&&&&&&&      &&&&&&&&&&&&&&&+ &&&\n&&&&&&&&+     &&&&&&&&&&&     X&&&&&&&&&&&&    &&\n&&&&&&&&&X     &&&&&&&&&&&      &&&&&&&&+      &&\n+&&&&&&&&&&      &&&&&   &&      &&&&&X      &&&X\n &&&&&&&&&&&      &&      &&      &&:      &&&&& \n  &&&&&&&&&&&           &&&&&:           &&&&&&  \n   &&&&&&&&&&&        X&&&&&&&X        &&&&&&&   \n    &&&&&&&&&&&X    &&&&&&&&&&&&     &&&&&&&&    \n     &&&&&&&&&&&X &&&&&&&&&&&&&&& &&&&&&&&&+     \n       &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&       \n         &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&         \n            &&&&&&&&&&&&&&&&&&&&&&&&&            \n                &&&&&&&&&&&&&&&&&                \n"
                                                                             
                                                                             
                                                                             
                                                                             
                                                                             
#endif