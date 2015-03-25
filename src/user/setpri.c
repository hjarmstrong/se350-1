#include "../rtx.h"
#include "../sysproc/crt.h"
#include "../util/string.h"
#include "setpri.h"

extern void crt_caller_managed_send_string(const char* input);

void setpri_proc() {
    msgbuf *kcd_reg = request_memory_block();
    msgbuf *received = NULL;
    const char *str;
    int pid = -1;
    int priority = -1;
    int recipient;

    kcd_reg->mtype = KCD_REG;
    strncpy(kcd_reg->mtext, "C", 2);
    send_message(PID_KCD, kcd_reg);
    while (1) {
        received = receive_message(&recipient);
        if (!strncmp(received->mtext, "C ", 2)) {
            str = received->mtext + 2;
            pid = read_num(&str);
            read_whitespace(&str);
            priority = read_num(&str);
            read_whitespace(&str); // Accept spaces after command
            if (pid == INVALID_NUMBER || priority == INVALID_NUMBER || str[0] != '\0') {
                crt_caller_managed_send_string("%C: could not parse command.\r\nUsage: %C <pid> <priority>\r\n");
            } else if (pid > MAX_PID) {
                crt_caller_managed_send_string("%C: Invalid PID.\r\nUsage: %C <pid> <priority>\r\n");
            } else if (priority >= PNULL) {
                crt_caller_managed_send_string("%C: Invalid priority.\r\nUsage: %C <pid> <priority>\r\n");
            } else {
                set_process_priority(pid, priority);
            }
        } else {
            crt_caller_managed_send_string("%C: unknown command\r\n");
        }
    }
}
