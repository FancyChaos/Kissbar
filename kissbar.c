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

// One Element (script) of the Statusbar
typedef struct {
        char *cmd;
        unsigned long interval;
        char status[SSIZE];
} Element;

// The complete Statusbar
typedef struct {
        char *text;
        size_t text_len;
        uint32_t update_cycle;
} Statusbar;

// X11 variables
Display *dpy;
Window root;

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
void  element_update(Element*);
bool  statusbar_update(Statusbar*);
void  statusbar_build(Statusbar*);
void  statusbar_output(Statusbar*);
void  setup_X(void);
int   ignore_X_error(Display*, XErrorEvent*);
void  usage(char*);


void
element_update(Element *element)
{
        // Execute statusbar script/command
        FILE *status_cmd = popen(element->cmd, "r");
        if (status_cmd == NULL) {
                fprintf(stderr, "Kissbar: Could not execute command/script: %s\n", element->cmd);
                exit(3);
        }

        // Get return value of the script/command
        if (fgets(element->status, SSIZE, status_cmd) == NULL)
                element->status[0] = '\0';

        // Remove newline if present
        element->status[strcspn(element->status, "\n")] = '\0';

        // Close the fd
        pclose(status_cmd);
}

bool
statusbar_update(Statusbar *statusbar)
{
        // Returned and set to true when an element status updated
        bool changed = false;

        // Store old status of every element to determine an update
        char prev_status[SSIZE];

        for (int i = 0; i < elements_num; ++i) {
                // Only pull new status ever n cycles
                if (elements[i].interval &&
                    statusbar->update_cycle % elements[i].interval)
                        continue;

                // Copy the current status to a buffer
                strlcpy(prev_status, elements[i].status, SSIZE);

                // Update status of current element
                element_update(elements + i);

                // If the previous status differs from the current
                // we will set the return value changed to true
                if (strncmp(elements[i].status, prev_status, SSIZE))
                        changed = true;
        }

        ++statusbar->update_cycle;

        return changed;
}

void
statusbar_build(Statusbar *statusbar)
{
        // "clear" the statusbar text
        statusbar->text[0] = '\0';

        // Build full statusbar text from all elements
        for (int i = 0; i < elements_num; ++i) {
                // Ignore empty status
                if (strnlen(elements[i].status, SSIZE) == 0)
                        continue;

                // Copy the first elements status to the beginning
                if (statusbar->text[0] == '\0') {
                        strlcpy(statusbar->text, elements[i].status, SSIZE);
                        continue;
                }
                
                // Copy delimiter and next elements status the the statusbar
                strlcat(statusbar->text, delim, statusbar->text_len);
                strlcat(statusbar->text, elements[i].status, statusbar->text_len);
        }

        // Add newline
        if (newline) strcat(statusbar->text, "\n");

        // Be sure to be nul terminated
        statusbar->text[statusbar->text_len - 1] = '\0';
}

void
statusbar_output(Statusbar *statusbar)
{
        if (tostdout) {
                printf("%s", statusbar->text);
                fflush(stdout);
        }

        if (towin) {
                XStoreName(dpy, root, statusbar->text);
                XFlush(dpy);
        }
}

int
ignore_X_error(Display *dpy, XErrorEvent *ee)
{
        // Output X errors to stderr but ignore them
        char error[256] = {0};
        XGetErrorText(dpy, ee->error_code, error, 128);
        fprintf(stderr,
                "Kissbar: Encountered unexpected X error: %s\n",
                error);
        return 0;
}

void
setup_X(void)
{
        // Connect to X to set the root windows name
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

        // Create the statusbar
        Statusbar *statusbar = (Statusbar*) malloc(sizeof(Statusbar));
        // Allocate enough space for the whole statusbar text
        statusbar->text_len = (SSIZE * elements_num) +
                              (strlen(delim) * (elements_num - 1)) + 1;
        statusbar->text = (char*) malloc(statusbar->text_len);
        statusbar->update_cycle = 0;

        while (1) {
                // Build and output statusbar if an elements status changed
                if (statusbar_update(statusbar)) {
                        statusbar_build(statusbar);
                        statusbar_output(statusbar);
                }

                // Checking for updates every second
                sleep(1);
        }

        if (towin) XCloseDisplay(dpy);

        return 0;
}
