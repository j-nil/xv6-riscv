#ifndef _XV6_PTRACE_H_
#define _XV6_PTRACE_H_

#define PTRACE_TRACEME              0
#define PTRACE_CONT                 7
#define PTRACE_SYSCALL             24

#define PTRACE_GET_SYSCALL_INFO     0x420e
#define PTRACE_SYSCALL_INFO_NONE    0
#define PTRACE_SYSCALL_INFO_ENTRY   1
#define PTRACE_SYSCALL_INFO_EXIT    2

struct ptrace_syscall_info {
    uint8 op;
    uint8 pad[3];
    uint64 instruction_pointer;
    uint64 stack_pointer;
    union {
        struct {
            uint64 nr;
            uint64 args[6];
        } entry;
        struct {
            long rval;
            uint8 is_error;
        } exit;
    };
};

#endif /* _XV6_PTRACE_H_ */
