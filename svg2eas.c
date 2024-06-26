#include <libxml/parser.h>
#include <string.h>
#include "path.h"
#include "serial.h"
#include <termios.h>
#include <xgfx/window.h>
#include <xgfx/drawing.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

xmlNode* pathNodes[100];

Point points[1000000];

int idx = 0;

Point current = {
    .x = 0,
    .y = 0
};
int currentIndex = 0;

void findPaths(xmlNode* node) {
    if (node) {
        if (strcmp(node->name, "path") == 0) {
            pathNodes[idx] = node;
            idx++;
        }
        for (xmlNode* child = node->children; child; child = child->next) {
            findPaths(child);
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: svg2eas [svgFile] [resolution] [serialPort]\n");
        return 1;
    }
    int serial = open(argv[3], O_RDWR | O_NOCTTY | O_SYNC);
    if (serial < 0) {
        fprintf(stderr, "Error connecting to serial port: %s\n", strerror(errno));
        return 1;
    }
    set_interface_attribs(serial, B9600, 0);
    set_blocking(serial, 0);
    sleep(2);

    float resolution = strtof(argv[2], NULL);
    xmlDoc* document = xmlReadFile(argv[1], NULL, 0);
    if (document == NULL) {
        fprintf(stderr, "Error opening svg file!\n");
        return 1;
    }
    xmlNode* root = xmlDocGetRootElement(document);

    findPaths(root);
    int offset = 0;
    for (int i = 0; i < idx; i++) {
        printf("<path>: d=\"%s\"\n", xmlGetProp(pathNodes[i], "d"));
        offset += parsePath(points + offset, xmlGetProp(pathNodes[i], "d"), resolution);
    }
    initWindow(640, 480, "SVG 2 Etch-A-Sketch");
    while(1) {
        Event event;
        while (checkWindowEvent(&event)) {
            if (event.type == WINDOW_CLOSE) {
                return 0;
            }
        }
        clear();
        line(0, 0, points[0].x, 0, 0xff00ff00);
        line(points[0].x, 0, points[0].x, points[0].y, 0xff00ff00);
        for (int i = 0; i < offset-1; i++) {
            line(points[i].x, points[i].y, points[i+1].x, points[i].y, 0xff0000ff);
            line(points[i+1].x, points[i].y, points[i+1].x, points[i+1].y, 0xff0000ff);
        }
        line(points[offset-1].x, points[offset-1].y, 0, points[offset-1].y, 0xffff0000);
        line(0, points[offset-1].y, 0, 0, 0xffff0000);
        circle(points[currentIndex].x, points[currentIndex].y, 5, 0xffffffff);
        updateWindow();
        if (currentIndex < offset+1) {
            char pointbuf[100];
            int length = sprintf(pointbuf, "%d %d\n", (int)(points[currentIndex].x*13), (int)(points[currentIndex].y*13));
            fputs(pointbuf, stdout);
            write(serial, pointbuf, length);
            currentIndex++;
            char readChar;
            do {
                readChar = 0;
                read(serial, &readChar, 1);
            }
            while (readChar != '\n');
            printf("next point\n");
        }
    }
}