#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
// defines Element, elements, delim, elements_num, SSIZE
#include "kissbar.h"

// Function definitions
void getstatus(Element*);
int update_elements(unsigned long);
void draw();
void setupX();
void usage(char*);

// Total number of scripts
const int elements_num = sizeof(elements)/sizeof(Element);

// Determines the output destination of the statusbar
char tostdout = 0;
char towin = 0;

// X11 variables
Display *dpy;
Window root;

// Full statusbar text
char *statusbar_text;

void
getstatus(Element *element)
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

int
update_elements(unsigned long time)
{
        // Signal to redraw the bar
        int redraw = 0;

        // Store old status of every element to redraw the bar
        // if the status changed
        char prev_status[SSIZE];

        Element *element;
        for (int i = 0; i < elements_num; ++i) {
                element = elements + i;

                // Do nothing if the interval is set and
                // the interval is not reached yet
                if (element->interval && time % element->interval)
                        continue;

                // Copy the current status to a buffer
                strlcpy(prev_status, element->status, SSIZE);

                // Get and set the current status of the element
                getstatus(element);

                // If the previous status differs from the current
                // we will redraw
                if (strncmp(element->status, prev_status, SSIZE))
                        redraw = 1;
        }

        return redraw;
}

void
draw()
{
        // "clear" the statusbar text
        statusbar_text[0] = '\0';

        // Form full statusbar text from all elements
        for (int i = 0; i < elements_num; ++i) {
                // Ignore empty status
                if (strlen(elements[i].status) == 0)
                        continue;

                // Print delimeter for every element
                // except the first one
                if (strlen(statusbar_text) == 0)
                        strlcpy(statusbar_text, elements[i].status, SSIZE);
                else
                        sprintf(statusbar_text, "%s%s%s", statusbar_text, delim, elements[i].status);
        }

        // Add newline
        strcat(statusbar_text, "\n");

        if (tostdout) {
                // Final printf to "send" out the full statusbar
                printf("%s", statusbar_text);
                fflush(stdout);
        }

        if (towin) {
                XStoreName(dpy, root, statusbar_text);
                XFlush(dpy);
        }

}

void
setupX()
{
        dpy = XOpenDisplay(NULL);
        if(!dpy) {
                fprintf(stderr, "Kissbar: Could not open X11 display\n");
                exit(2);
        }
        root = RootWindow(dpy, DefaultScreen(dpy));
}

void
usage(char *name)
{
    fprintf(stderr, "Usage: %s [-ow]\n", name);
    exit(1);
}

int
main(int argc, char **argv)
{
        // Parse arguments
        int opt;
        while((opt = getopt(argc, argv, "ow")) != -1) {
                switch(opt) {
                        case 'o': tostdout = 1; break;
                        case 'w': towin = 1; break;
                        default: usage(argv[0]);
                }
        }

        // We expect at least one output mode
        if (!tostdout && !towin)
                usage(argv[0]);

        if (towin)
                setupX();

        // Allocate enough space for the whole statusbar text
        statusbar_text = (char*) malloc((SSIZE * elements_num) + (strlen(delim) * elements_num));

        // Important variables for main loop
        const unsigned int polling_rate = 1;
        unsigned long time = 0;
        int redraw = 0;

        while (1) {
                // Poll all elements of the bar
                redraw = update_elements(time);

                // Redraw if an elements status changed
                if (redraw)
                        draw();

                // Take a break;
                time += polling_rate;
                sleep(polling_rate);
        }

        if (towin)
                XCloseDisplay(dpy);

        return 0;
}
