#ifndef _XV6_PROC_MACROS_H_
#define _XV6_PROC_MACROS_H_

#define WNOHANG     1
#define WUNTRACED   2

#define WIFEXITED(_status)      (!(((_status) >> 8) & 0xFF))
#define WIFSTOPPED(_status)     ((((_status) >> 8) & 0xFF))
#define WEXITSTATUS(_status)    ((_status) & 0xFF)

#endif /* _XV6_PROC_MACROS_H_ */
