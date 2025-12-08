#include "types.h"
#include "console.h"
#include "time.h"
#include "string.h"
#include "m328Pdef.h"
#include "traps.h"

#include <nanofs/nanofs.h>

#define CMD_BUF_SZ  (4)

int ls(char *argc, char **argv)
{
    printf("its ls\n");
    return 0;
}

static int (*cmds[])(char *, char **) = { \
    ls,
    0
};

struct shell_cmd {
    int index;
    char name[CMD_BUF_SZ];
    char arg1[CMD_BUF_SZ];
    char arg2[CMD_BUF_SZ];
};

static inline char* read_cmd_set_position(struct shell_cmd *cmd,
                                            int stage, int pos)
{
    switch (stage)
    {
    case 1: return cmd->name + pos;
    case 2: return cmd->arg1 + pos;
    case 3: return cmd->arg2 + pos;
    }
    return 0;
}

static int parse_cmd(struct shell_cmd *cmd)
{
    char *cmd_names[] = {"ls", 0};

    cmd->index = -1;

    for (int i = 0; i < sizeof(cmd_names)/sizeof(char *); i++)
        if (memcmp(cmd_names[i], cmd->name, 
                            strlen(cmd->name)) == 0) {
            cmd->index = i;
            return 1;
        }
    return 0;
}

static int read_cmd(struct shell_cmd *cmd)
{
    char c, *dst;
    int i = 0, stage = 1;

    printf("@> ");

    while (i < CMD_BUF_SZ - 1 && stage <= 3) {
        dst = read_cmd_set_position(cmd, stage, i);
        if (dst == 0)
            return 0;

        c = usart_getc();
        printf("%c", c);

        if (c == '\r' || c == '\n') {
            *dst = 0;
            break;
        }
        if (c == ' ') {
            *dst = 0;
            i = 0;
            stage += 1;
            continue;
        }
        *dst = c;
        i++;
    }
    return parse_cmd(cmd);
}

static void eval_cmd(struct shell_cmd *cmd)
{
    char **argv = {cmd->arg1, cmd->arg2};
    if (cmd->index >= 0)
        cmds[cmd->index](cmd->name, argv);
}

void run_shell(void)
{
    struct shell_cmd cmd;

    while (1) {
        memset(&cmd, 0, sizeof(cmd));

        if (read_cmd(&cmd) > 0) {
            eval_cmd(&cmd);
        }
    }
}
