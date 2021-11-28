#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/ptrace.h"
#include "kernel/proc_macros.h"
#include "kernel/syscall.h"
#include "user/user.h"

const char USAGE[] = "Trace system calls in a process.\n"
                     "usage: trace <program> [arguments]\n";

static void print_syscall_invocation(const struct ptrace_syscall_info *info)
{
    const uint64 *a = &info->entry.args[0];
    switch (info->entry.nr) {
    case SYS_fork:
        printf("fork()");
        break;
    case SYS_exit:
        printf("exit()\n");
        break;
    case SYS_wait:
        printf("wait(%p)", a[0]);
        break;
    case SYS_pipe:
        printf("pipe(%p)", a[0]);
        break;
    case SYS_read:
        printf("read(%d, %p, %d)", a[0], a[1], a[2]);
        break;
    case SYS_kill:
        printf("kill(%d)", a[0]);
        break;
    case SYS_exec:
        printf("exec(%p, %p)", a[0], a[1]);
        break;
    case SYS_fstat:
        printf("fstat(%d, %p)", a[0], a[1]);
        break;
    case SYS_chdir:
        printf("chdir(%p)", a[0]);
        break;
    case SYS_dup:
        printf("dup(%d)", a[0]);
        break;
    case SYS_getpid:
        printf("getpid()");
        break;
    case SYS_sbrk:
        printf("sbrk()");
        break;
    case SYS_sleep:
        printf("sleep(%d)", a[0]);
        break;
    case SYS_uptime:
        printf("uptime()");
        break;
    case SYS_open:
        printf("open(%p, %d)", a[0], a[1]);
        break;
    case SYS_write:
        printf("write(%d, %p, %d)", a[0], a[1], a[2]);
        break;
    case SYS_mknod:
        printf("mknod(%p, %d, %d)", a[0], a[1], a[2]);
        break;
    case SYS_unlink:
        printf("unlink(%p)", a[0]);
        break;
    case SYS_link:
        printf("link(%p, %p)", a[0], a[1]);
        break;
    case SYS_mkdir:
        printf("mkdir(%p)", a[0]);
        break;
    case SYS_close:
        printf("close(%d)", a[0]);
        break;
    case SYS_ptrace:
        printf("ptrace(%d, %d, %p, %p)", a[0], a[1], a[2], a[3]);
        break;
    case SYS_waitpid:
        printf("waitpid(%d, %p, %d)", a[0], a[1], a[2]);
        break;
    default:
        printf("<unknown syscall %d>()", info->entry.nr);
        break;
    }
}

static void print_syscall_exit(const struct ptrace_syscall_info *info)
{
    printf(" = %d\n", info->exit.rval);
}

int main(int argc, char **argv)
{
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        printf(USAGE);
        exit(1);
    }

    int rc;
    int pid = fork();
    if (pid == 0) {
        // Execute tracee
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        rc = exec(argv[1], &argv[2]);  /* is argv NULL-terminated already? */
        exit(rc);
    }

    int child_status;
    int in_syscall = 0;
    struct ptrace_syscall_info syscall_info;

    waitpid(pid, &child_status, WUNTRACED);
    if (!WIFSTOPPED(child_status)) {
        printf("Error: child was not stopped\n");
        exit(-1);
    }

    while (!WIFEXITED(child_status)) {
        if ((rc = ptrace(PTRACE_SYSCALL, pid, 0, 0))) {
            printf("Error: PTRACE_SYSCALL returned %d\n", rc);
            kill(pid);
            exit(rc);
        }

        waitpid(pid, &child_status, WUNTRACED);
        if (WIFEXITED(child_status)) {
            break;
        }
        if (!WIFSTOPPED(child_status)) {
            printf("Error: child was not stopped\n");
            exit(-1);
        }

        if ((rc = ptrace(PTRACE_GET_SYSCALL_INFO, pid,
            (void *) sizeof(syscall_info), (void *) &syscall_info))) {
            printf("Error: PTRACE_GET_SYSCALL_INFO returned %d\n", rc);
            kill(pid);
            exit(rc);
        }

        switch (syscall_info.op)
        {
        case PTRACE_SYSCALL_INFO_ENTRY:
            print_syscall_invocation(&syscall_info);
            break;
        case PTRACE_SYSCALL_INFO_EXIT:
            // The tracee's initial PTRACE_TRACEME will only exit; ignore it here
            if (in_syscall) {
                print_syscall_exit(&syscall_info);
            }
            break;
        default:
            break;
        }
        in_syscall = syscall_info.op == PTRACE_SYSCALL_INFO_ENTRY;
    }

    printf("Child exited with %d\n", WEXITSTATUS(child_status));
    exit(0);
}
