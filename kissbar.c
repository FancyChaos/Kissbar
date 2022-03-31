#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// defines Element, elements, delim, elements_num, SSIZE
#include "kissbar.h"

// Total number of scripts
const int elements_num = sizeof(elements)/sizeof(Element);

void getstatus(Element *element)
{
        // Execute statusbar script/command
        FILE *status_cmd = popen(element->cmd, "r");
        if (status_cmd == NULL) {
                strlcpy(element->status, "Could not execute command", SSIZE);
                return;
        }

        // Get return value of the script/command
        fgets(element->status, SSIZE, status_cmd);

        // Close the fd
        pclose(status_cmd);

        // Remove newline if present
        elements->status[strcspn(elements->status, "\n")] = '\0';
}

int poll(unsigned long time)
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

void draw()
{
        // Draw the bar with pure printf's
        for (int i = 0; i < elements_num; ++i) {
                printf("%s", elements[i].status);

                // Don't draw delimiter on last element
                if (i + 1 == elements_num)
                        break;
                printf("%s", delim);
        }

        // Final printf to "send" out the full statusbar
        printf("\n");
        fflush(stdout);
}

int main()
{
        const unsigned int polling_rate = 1;
        unsigned long time = 0;
        int redraw = 0;

        while (1) {
                // Poll all elements of the bar
                redraw = poll(time);

                // Redraw if an elements status changed
                if (redraw)
                        draw();

                // Take a break;
                time += polling_rate;
                sleep(polling_rate);
        }

        return 0;
}
