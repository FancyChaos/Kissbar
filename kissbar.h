/*
 * ATTENTION:
 * Your statusbar scripts/commands must only print one line.
*/

/* Add your statusbar scripts/commands here.
 *
 * The first entry will be displayed as the far most left.
 * The interval determines every n cycle the script should get called.
 * One cycle equals roughly to one second plus the time needed to
 * execute the scripts in that cycle.
 * The lowest interval possible is '1' = The script gets called every cycle.
*/
Element elements[] = {
        { .cmd = "/etc/fos/statusbar/sb-cpu",           .interval = 2},
        { .cmd = "/etc/fos/statusbar/sb-mem",           .interval = 3},
        { .cmd = "/etc/fos/statusbar/sb-cpuspeed",      .interval = 5},
        { .cmd = "/etc/fos/statusbar/sb-battery",       .interval = 5},
        { .cmd = "/etc/fos/statusbar/sb-clock",         .interval = 10}
};

// The delimiter between each element
const char *delim = " | ";
