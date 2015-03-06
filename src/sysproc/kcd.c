#include <LPC17xx.h>

#include "../rtx.h"
#include "../sys/uart.h"
#include "../sys/uart_polling.h"
#include "../sysproc/crt.h"
#include "../util/map.h"
#include "../util/string.h"

void kcd_proc(void) {
    int pid_from = -1;
    Map recipient_map;
    msgbuf *buf;
    void *hash;
    char *recipients;
    int i, j;
    int message_ack;

    while(1) {
        buf = receive_message(&pid_from);
        if (buf->mtype == DEFAULT) {
            message_ack = 0;
            // Assumption: start of message should always be the first character
            // Assumption: all messages are less than 51 characters
            if (buf->mtext[0] == '%') {
                for (i = 1; !!buf->mtext[i]; ++i) {
                    hash = hash_string(&buf->mtext[1], i - 1);
                    if (map_is_in(&recipient_map, hash)) {
                        recipients = map_get(&recipient_map, hash);
                        message_ack = 1;
                        for (j = 0; !!recipients[j] && j < MAX_MAP_ELEMENTS; ++j) {
                            send_message(recipients[j], buf);
                        }
                    }
                }
            }
            if (!message_ack) {
                release_memory_block(buf);
            }
        } else if (buf->mtype == KCD_REG) {
            hash = hash_string(buf->mtext, -1);
            recipients = map_reserve(&recipient_map, hash);
            for (i = 0; i < MAX_MAP_ELEMENTS && !!recipients[i]; ++i) {
                // pass
            }
            // In the case of a full mailbox, we discard the last unread message.
            recipients[i] = pid_from;
            release_memory_block(buf);
        }
    }
}
