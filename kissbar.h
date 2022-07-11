/*
 * Your statusbar scripts/commands must only print one line.
*/

// Maximum size of the status message from one element (script)
enum { SSIZE = 128 };

// Datastructure for one element (script)
// DO NOT TOUCH
typedef struct {
        char *cmd;
        unsigned long interval;
        char status[SSIZE];
} Element;

// Add your statusbar scripts/commands here.
// The first entry will be displayed as the far most left.
// The interval determines every n seconds the script should get called.
// The lowest interval possible is '1' = The script gets called every second.
Element elements[] = {
        { .cmd = "/etc/fos/statusbar/sb-cpu",           .interval = 2},
        { .cmd = "/etc/fos/statusbar/sb-mem",           .interval = 3},
        { .cmd = "/etc/fos/statusbar/sb-cpuspeed",      .interval = 5},
        { .cmd = "/etc/fos/statusbar/sb-battery",       .interval = 5},
        { .cmd = "/etc/fos/statusbar/sb-clock",         .interval = 10}
};

// The delimiter between each element
const char *delim = " | ";
