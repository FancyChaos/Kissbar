#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <X11/Xlib.h>
#ifdef __linux__
#include <bsd/string.h>
#endif

// Maximum size of the status message from one element (script)
enum { SSIZE = 64 };

// Datastructure for one element (script)
typedef struct {
        char *cmd;
        unsigned long interval;
        char status[SSIZE];
} Element;

// X11 variables
Display *dpy;
Window root;

// Full statusbar text
char *statusbar_text;
// Largest possible size of statusbar (init in main)
size_t statusbar_len = 0;

// Options
// Add newline to the output
bool newline = 0;
// Determines the output destination of the statusbar
bool tostdout = 0;
bool towin = 0;

// Defines custom elements
#include "kissbar.h"
// Total number of elements (scripts)
const size_t elements_num = sizeof(elements)/sizeof(Element);

// Function definitions
void  update_status(Element*);
bool  update_elements(uint64_t);
void  output();
void  setup_X();
int   ignore_X_error(Display*, XErrorEvent*);
void  usage(char*);


void
update_status(Element *element)
{
        // Execute statusbar script/command
        FILE *status_cmd = popen(element->cmd, "r");
        if (status_cmd == NULL) {
                fprintf(stderr, "Kissbar: Could not execute command/script: %s\n", element->cmd);
                exit(3);
        }

        // Get return value of the script/command
        if(fgets(element->status, SSIZE, status_cmd) == NULL)
                element->status[0] = '\0';

        // Remove newline if present
        element->status[strcspn(element->status, "\n")] = '\0';

        // Close the fd
        pclose(status_cmd);
}

bool
update_elements(uint64_t cycle)
{
        // Signal to output the changed statusbar
        bool changed = false;

        // Store old status of every element to redraw the bar
        // if the status changed
        char prev_status[SSIZE];

        for (int i = 0; i < elements_num; ++i) {
                // Only pull new status ever n cycles
                if (elements[i].interval && cycle % elements[i].interval)
                        continue;

                // Copy the current status to a buffer
                strlcpy(prev_status, elements[i].status, SSIZE);

                // Update status of current element
                update_status(elements + i);

                // If the previous status differs from the current
                // we will re-output the statusbar
                if (strncmp(elements[i].status, prev_status, SSIZE))
                        changed = true;
        }

        return changed;
}

void
output()
{
        // "clear" the statusbar text
        statusbar_text[0] = '\0';

        // Form full statusbar text from all elements
        for (int i = 0; i < elements_num; ++i) {
                // Ignore empty status
                if (strnlen(elements[i].status, SSIZE) == 0)
                        continue;

                // Copy the first elements status straight to the statusbar
                if (statusbar_text[0] == '\0') {
                        strlcpy(statusbar_text, elements[i].status, SSIZE);
                        continue;
                }
                
                // Copy delimiter and next elements status the the statusbar
                strlcat(statusbar_text, delim, statusbar_len);
                strlcat(statusbar_text, elements[i].status, statusbar_len);
        }

        // Add newline
        if (newline) strcat(statusbar_text, "\n");

        if (tostdout) {
                printf("%s", statusbar_text);
                fflush(stdout);
        }

        if (towin) {
                XStoreName(dpy, root, statusbar_text);
                XFlush(dpy);
        }
}

int
ignore_X_error(Display *dpy, XErrorEvent *ee)
{
        char error[256] = {0};
        XGetErrorText(dpy, ee->error_code, error, 128);
        fprintf(stderr,
                "Kissbar: Encountered unexpected X error: %s\n",
                error);
        return 0;
}

void
setup_X()
{
        if (!(dpy = XOpenDisplay(NULL))) {
                fprintf(stderr, "Kissbar: Could not open X11 display\n");
                exit(2);
        }
        root = DefaultRootWindow(dpy);
        XSetErrorHandler(ignore_X_error);
        XSync(dpy, 0);
}

void
usage(char *name)
{
        fprintf(stderr, "Usage: %s [-own]\n", name);
        exit(1);
}

int
main(int argc, char **argv)
{
        // Parse arguments
        int opt;
        while ((opt = getopt(argc, argv, "own")) != -1) {
                switch(opt) {
                        case 'o': tostdout = true; break;
                        case 'w': towin = true; break;
                        case 'n': newline = true; break;
                        default: usage(argv[0]);
                }
        }

        // Print to stdout by default except only -w (xsetroot) option is given
        if (!towin) tostdout = 1;
        else setup_X();

        // Allocate enough space for the whole statusbar text
        statusbar_len = (SSIZE * elements_num) +
                        (strlen(delim) * (elements_num - 1)) + 1;
        statusbar_text = (char*) malloc(statusbar_len);

        uint64_t cycle = 0;
        while (1) {
                // Output statusbar if an elements status changed
                if (update_elements(cycle++)) output();

                // Checking for updates every second
                sleep(1);
        }

        if (towin) XCloseDisplay(dpy);

        return 0;
}
