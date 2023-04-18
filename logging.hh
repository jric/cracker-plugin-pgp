#ifndef __LOGGING_HH__
#define __LOGGING_HH__

#include <iostream>

#define OUTPUT(stuff, level) { std::cerr << __FILE__ << ':' << __LINE__ << ": " << level << ": " << stuff << std::endl;  }
#define ERR(stuff) { OUTPUT(stuff, "ERROR") }
#define WARN(stuff) { OUTPUT(stuff, "WARN") }
#define INFO(stuff) { OUTPUT(stuff, "INFO") }
#if DEBUG > 0
#define DBG1(stuff) OUTPUT(stuff, "DEBUG1")
#else
#define DBG1(stuff) { } // noop
#endif
#if DEBUG > 1
#define DBG2(stuff) OUTPUT(stuff, "DEBUG2")
#else
#define DBG2(stuff) { } // noop
#endif
#define ABORT(stuff) { ERR("FATAL: " << stuff) perror("system error"); exit(1); }

#endif