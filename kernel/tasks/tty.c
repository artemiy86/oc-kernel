#include <arch/reg.h>
#include <arch/idt.h>
#include <dev/video.h>
#include <dev/keyboard.h>
#include <sched/task.h>
#include <tasks/tty.h>
#include <ipc/ipc.h>
#include <utils/kprint.h>
#include <lib/time.h>
#include <lib/string.h>
#include <lib/stdtypes.h>
#include <lib/assert.h>
#include <lib/stdio.h>
#include <messages.h>

static char const tty_output_buff[SYSLOG_SIZE];      /* tty output buffer */
char *tty_output_buff_pos = (char *)tty_output_buff; /* tty output position */

static u_short tid_stdin = TID_TTY; /* target tid to send incomming chars */

static void handle_getc(u_char keycode);
static void handle_putc(char ch);
static void handle_puts(char *str);
static void handle_vprintf(const char *format, va_list list);

/*
 * Api - Teletype task
 */
extern void task_tty_main()
{
    struct message_t msg;

    printf(MSG_TASK_TTY_LAUNCHED, (size_t *)asm_get_esp());
    clear();
    kmode(false); /* detach syslog from screen */
    tty_output_buff_pos = video_clear(tty_output_buff);
    handle_vprintf(MSG_KERNEL_NAME, null);
    video_flush(tty_output_buff);

    while (1)
    {
        kreceive(TID_TTY, &msg);

        /* decode message */
        switch (msg.type)
        {
        case TTY_MSG_TYPE_GETC:
        {
            /* key pressed */
            assert(msg.len == 1);
            handle_getc(msg.data[0]);
            break;
        }
        case TTY_MSG_TYPE_PUTC:
        {
            /* put character to screen */
            assert(msg.len == 1);
            handle_putc(msg.data[0]);
            video_flush(tty_output_buff);
            break;
        }
        case TTY_MSG_TYPE_PUTS:
        {
            /* put string to screen */
            assert(msg.data[msg.len - 1] == 0);
            handle_puts((char *)msg.data);
            video_flush(tty_output_buff);
            break;
        }
        case TTY_MSG_TYPE_CLEAR:
        {
            /* clear screen */
            tty_output_buff_pos = video_clear(tty_output_buff);
            video_flush(tty_output_buff);
            break;
        }
        case TTY_MSG_TYPE_STDIN:
        {
            /* redirect stdin */
            assert(msg.len == 2);
            tid_stdin = *(u16 *)msg.data;
            break;
        }
        case TTY_MSG_TYPE_PRINTF: {
            /* print to screen */
            assert(msg.len == 8);
            size_t addr = (size_t)msg.data;
            handle_vprintf(*((const char **)addr), *((va_list *)(addr + 4)));
            video_flush(tty_output_buff);
            break;
        }
        }
    }
}

/*
 * Handle key pressed
 */
static void handle_getc(u_char keycode)
{
    struct message_t msg;
    u_char ch;

    /* send to stdin */
    ch = keyboard_map[keycode];
    msg.type = tid_stdin == TID_TTY ? TTY_MSG_TYPE_PUTC : TTY_MSG_TYPE_GETC;
    msg.len = 1;
    msg.data[0] = ch;
    ksend(tid_stdin, &msg);
}

/*
 * Handle put character to screen
 */
static void handle_putc(char ch)
{
    if ((size_t)tty_output_buff_pos - (size_t)tty_output_buff + 1 < VIDEO_SCREEN_SIZE)
    {
        if (ch != '\n')
        {
            *tty_output_buff_pos++ = ch;
        }
        else
        {
            int line_pos = (tty_output_buff_pos - tty_output_buff) % VIDEO_SCREEN_WIDTH;
            for (int j = 0; j < VIDEO_SCREEN_WIDTH - line_pos; ++j)
            {
                *tty_output_buff_pos++ = ' ';
            }
        }
    }
    else
    {
        tty_output_buff_pos = video_scroll(tty_output_buff, tty_output_buff_pos);
        handle_putc(ch);
    }
}

/*
 * Handle put string to screen
 */
static void handle_puts(char *str)
{
    while (*str != 0)
    {
        handle_putc(*str++);
    }
}

/*
 * Handle put formatted string to screen
 */
static void handle_vprintf(const char *format, va_list list)
{
    char buff[VIDEO_SCREEN_WIDTH];
    int len = vsprintf(buff, format, list);

    for (int i = 0; i < len; ++i)
    {
        handle_putc(buff[i]);
    }
}
