#include "../rtx.h"
#include "../sysproc/crt.h"
#include "../sys/timer.h"
#include "../util/string.h"

static int enabled = 0;
static int seconds = 0;
static int mins = 0;
static int hours = 0;

static void backup(int n) {
    static unsigned char buffer[80];
    int i = 0;
    if (!n) {
        return;
    }
    for (i = 0; i < n && i < 79; ++i) {
        buffer[i] = 0x08; // Backspace
    }
    buffer[i] = 0;
    crt_send_string(buffer);
}

static void display() {
    static unsigned char buffer[80];
    backup(strlen((const char*) buffer) + 2); // TODO: Why is there a SPACE?

    strncpy((char *) buffer, "hh:mm:ss", 80);
    buffer[0] = (hours/10) + '0';
    buffer[1] = (hours%10) + '0';
    buffer[3] = (mins/10) + '0';
    buffer[4] = (mins%10) + '0';
    buffer[6] = (seconds/10) + '0';
    buffer[7] = (seconds%10) + '0';

    crt_send_string(buffer);
}

static void reset() {
    enabled = 1;
    seconds = mins = hours = 0;
}

static void terminate() {
    enabled = 0;
}

static void tick() {
    ++seconds;
    if (seconds/60) {
        ++mins;
        seconds = 0;
    }
    if (mins/60) {
        ++hours;
        mins = 0;
    }
    display();
}

void clock_proc() {
    void *clock_msg = (void *) 0x1337c10c;
    int recipient = -1;
    msgbuf *kcd_reg = request_memory_block();
    msgbuf *received = NULL;
    int len = -1;

    kcd_reg->mtype = KCD_REG;
    strncpy(kcd_reg->mtext, "W", 64);
    send_message(PID_KCD, kcd_reg);

    delayed_send(PID_A, clock_msg, 100);
    while (1) {
        received = receive_message(&recipient);
        if (received == clock_msg) {
            if (enabled) {
                tick();
            }
            // messages are supposed to be unique.
            clock_msg = (void *) (((int) clock_msg) + 1);
            delayed_send(PID_A, clock_msg, 100);
        } else {
            if (!strncmp(received->mtext, "%WR", 3)) {
                reset();
            } else if (!strncmp(received->mtext, "%WT", 3)) {
                terminate();
            } else if (!strncmp(received->mtext, "%WS", 3)) {
                len = strlen(received->mtext);
                if (len != 12) {
                    crt_send_string("%WS: invalid format.\r\n");
                } else {
                    hours = (received->mtext[4] - '0')*10;
                    hours += (received->mtext[5] - '0');
                    mins = (received->mtext[7] - '0')*10;
                    mins += (received->mtext[8] - '0');
                    seconds = (received->mtext[10] - '0')*10;
                    seconds += (received->mtext[11] - '0');
                }
                enabled = 1;
            }
        }
    }
}
