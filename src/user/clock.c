#include "../rtx.h"
#include "../sysproc/crt.h"
#include "../sys/timer.h"
#include "../util/string.h"

#define ONE_SECOND 1000

static int enabled = 0;
static int seconds = 0;
static int mins = 0;
static int hours = 0;

#define CLOCK_BUFFER_SIZE 10

static char display_buffer[CLOCK_BUFFER_SIZE];

static void cls() {
    strncpy(display_buffer, "\r\n", 3);
    crt_send_string(display_buffer);
}


static void display() {
    cls();

    strncpy(display_buffer, "hh:mm:ss", 8);
    display_buffer[0] = (hours/10) + '0';
    display_buffer[1] = (hours%10) + '0';
    display_buffer[3] = (mins/10) + '0';
    display_buffer[4] = (mins%10) + '0';
    display_buffer[6] = (seconds/10) + '0';
    display_buffer[7] = (seconds%10) + '0';

    crt_send_string(display_buffer);
}

static void reset() {
    enabled = 1;
    seconds = mins = hours = 0;
    display();
}

static void terminate() {
    enabled = 0;
    
    cls();
    strncpy(display_buffer, "        ", 9);
    crt_send_string(display_buffer);
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
	  // 0xffffffff is a number that is unlikely to be used as a message
	  // and out of range and will hard-fault if written to
    void *clock_msg = (void *) 0xffffffff; 
    int recipient = -1;
    msgbuf *kcd_reg = request_memory_block();
    msgbuf *received = NULL;
    int len = -1;

    kcd_reg->mtype = KCD_REG;
    strncpy(kcd_reg->mtext, "W", 2);
    send_message(PID_KCD, kcd_reg);

    delayed_send(PID_A, clock_msg, ONE_SECOND);
    while (1) {
        received = receive_message(&recipient);
        if (received == clock_msg) {
            if (enabled) {
                tick();
            }
            delayed_send(PID_A, clock_msg, ONE_SECOND);
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
