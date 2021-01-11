/*! 
 * @file Error.hh
 * The header file containing error enums and debug reoprting macros 
 *
 * File is part of the Cubes project 
 */

#ifndef A_Error_h
#define A_Error_h 1

#include <iostream>


#ifndef DBGLEV
#define DBGLEV 1 ///< default debug level
#endif

/*! DEBUG MACRO
* for 4 levels defined at compile time
* 0 : NO MESSAGES
* 1 : IMPORTANT PRINT MESSAGES (ERRORS and such) - default setting
* 2 : INFO MESSAGES - enabled by setting DBGLEV => 2
* 3 : DEBUGGING STATUS MESSAGES - enabled by setting DBGLEV => 3
*/  
#define DBGMSG(lev,msg) \
    do { \
        if (DBGLEV >= lev) \
        std::cerr << __FILE__ << "(" << __LINE__ << ")" \
        << ": " << msg << std::endl; \
    } while(0)

enum stat_e {SUCCESS=0,FAILURE}; // easy to use enum now. 
// If reqd. Can define full fledged class for error
// handling later. See https://codereview.stackexchange.com/questions/157448/error-handling-class 

#endif
