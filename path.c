#include "path.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int notCommand(char* input, int* goingAgain, float* againX, float* againY, int* gotCommand, char** cmd) {
    int retval = (strlen(input) != 1);
    *goingAgain = retval;
    if (retval) {
        char* endptr;
        *againX = strtof(strtok_r(input, ",", &endptr), NULL);
        *againY = strtof(strtok_r(NULL, " ", &endptr), NULL);
        input[strlen(input)] = ' ';
    }
    else {
        *gotCommand = 1;
        *cmd = input;
    }
    return retval;
}

int parsePath(Point* pointBuffer, char* commands, float resolution) {
    Point drawHead = {
        .x = 0,
        .y = 0
    };
    int pointIndex = 0;
    int closeIndex = 0;
    char* command = strtok(commands, " ");
    while (command) {
        int gotCommand = 0;
        if (strcmp(command, "M") == 0) {
            float x;
            float y;
            int goingAgain = 0;
            do {
                if (!goingAgain) {
                    x = strtof(strtok(NULL, ","), NULL);
                    y = strtof(strtok(NULL, " "), NULL);
                    printf("moveToAbs (%f, %f)\n", x, y);
                    drawHead.x = x;
                    drawHead.y = y;
                    pointBuffer[pointIndex] = drawHead;
                    pointIndex++;            
                }
                else {
                    Point b = {
                        .x = x,
                        .y = y
                    };
                    printf("lineToAbs (%f, %f)\n", x, y);
                    for (float t = 0; t <= 1; t += resolution) {
                        pointBuffer[pointIndex] = lerp(drawHead, b, t);
                        pointIndex++;
                    }
                    drawHead = b;
                }
            }
            while(notCommand(strtok(NULL, " "), &goingAgain, &x, &y, &gotCommand, &command));
        }
        else if (strcmp(command, "m") == 0) {
            float x;
            float y;
            int goingAgain = 0;
            do {
                if (!goingAgain) {
                    x = strtof(strtok(NULL, ","), NULL);
                    y = strtof(strtok(NULL, " "), NULL);
                    printf("moveToRel (%f, %f)\n", x, y);
                    drawHead.x += x;
                    drawHead.y += y;
                    pointBuffer[pointIndex] = drawHead;
                    pointIndex++;            
                }
                else {
                    Point b = {
                        .x = drawHead.x + x,
                        .y = drawHead.y + y
                    };
                    printf("lineToRel (%f, %f)\n", x, y);
                    for (float t = 0; t <= 1; t += resolution) {
                        pointBuffer[pointIndex] = lerp(drawHead, b, t);
                        pointIndex++;
                    }
                    drawHead = b;
                }
            }
            while(notCommand(strtok(NULL, " "), &goingAgain, &x, &y, &gotCommand, &command));
        }
        else if (strcmp(command, "L") == 0) {
            float x;
            float y;
            int goingAgain = 0;
            do {
                if (!goingAgain) {
                    x = strtof(strtok(NULL, ","), NULL);
                    y = strtof(strtok(NULL, " "), NULL);            
                }
                Point b = {
                    .x = x,
                    .y = y
                };
                printf("lineToAbs (%f, %f)\n", x, y);
                for (float t = 0; t <= 1; t += resolution) {
                    pointBuffer[pointIndex] = lerp(drawHead, b, t);
                    pointIndex++;
                }
                drawHead = b;
            }
            while(notCommand(strtok(NULL, " "), &goingAgain, &x, &y, &gotCommand, &command));
        }
        else if (strcmp(command, "l") == 0) {
            float x;
            float y;
            int goingAgain = 0;
            do {
                if (!goingAgain) {
                    x = strtof(strtok(NULL, ","), NULL);
                    y = strtof(strtok(NULL, " "), NULL);            
                }
                Point b = {
                    .x = drawHead.x + x,
                    .y = drawHead.y + y
                };
                printf("lineToRel (%f, %f)\n", x, y);
                for (float t = 0; t <= 1; t += resolution) {
                    pointBuffer[pointIndex] = lerp(drawHead, b, t);
                    pointIndex++;
                }
                drawHead = b;
            }
            while(notCommand(strtok(NULL, " "), &goingAgain, &x, &y, &gotCommand, &command));
        }
        else if (strcmp(command, "H") == 0) {
            float x = strtof(strtok(NULL, " "), NULL);
            printf("horizonalAbs %f\n", x);
            drawHead.x = x;
            pointBuffer[pointIndex] = drawHead;
            pointIndex++;
        }
        else if (strcmp(command, "h") == 0) {
            float x = strtof(strtok(NULL, " "), NULL);
            printf("horizonalRel %f\n", x);
            drawHead.x += x;
            pointBuffer[pointIndex] = drawHead;
            pointIndex++;
        }
        else if (strcmp(command, "V") == 0) {
            float y = strtof(strtok(NULL, " "), NULL);
            printf("verticalAbs %f\n", y);
            drawHead.y = y;
            pointBuffer[pointIndex] = drawHead;
            pointIndex++;
        }
        else if (strcmp(command, "v") == 0) {
            float y = strtof(strtok(NULL, " "), NULL);
            printf("verticalRel %f\n", y);
            drawHead.y += y;
            pointBuffer[pointIndex] = drawHead;
            pointIndex++;
        }
        else if (strcmp(command, "z") == 0 || strcmp(command, "Z") == 0) {
            printf("closePath\n");
            if (abs(drawHead.x-pointBuffer[closeIndex].x) < 2 || abs(drawHead.y-pointBuffer[closeIndex].y) < 2) { // if the closing point is close to vertical or horizonal, just use a single point
                pointBuffer[pointIndex] = pointBuffer[closeIndex];
                pointIndex++;
            }
            else { // otherwise, we have to use a lerp
                for (float t = 0; t <= 1; t += resolution) {
                        pointBuffer[pointIndex] = lerp(drawHead, pointBuffer[closeIndex], t);
                        pointIndex++;
                }
            }
            drawHead = pointBuffer[closeIndex];
            closeIndex = pointIndex;
        }
        else if (strcmp(command, "Q") == 0) {
            int goingAgain = 0;
            float x1;
            float y1;
            do {
                if (!goingAgain) {
                    x1 = strtof(strtok(NULL, ","), NULL);
                    y1 = strtof(strtok(NULL, " "), NULL);            
                }
                Point b = {
                    .x = x1,
                    .y = y1
                };
                float x = strtof(strtok(NULL, ","), NULL);
                float y = strtof(strtok(NULL, " "), NULL);
                Point c = {
                    .x = x,
                    .y = y
                };
                printf("quadraticBezierAbs (%f, %f), (%f, %f)\n", x1, y1, x, y);
                for (float t = 0; t <= 1; t += resolution) {
                    pointBuffer[pointIndex] = quadBezier(drawHead, b, c, t);
                    pointIndex++;
                }
                drawHead = c;
            }
            while(notCommand(strtok(NULL, " "), &goingAgain, &x1, &y1, &gotCommand, &command));
        }
        else if (strcmp(command, "q") == 0) {
            int goingAgain = 0;
            float x1;
            float y1;
            do {
                if (!goingAgain) {
                    x1 = strtof(strtok(NULL, ","), NULL);
                    y1 = strtof(strtok(NULL, " "), NULL);            
                }
                Point b = {
                    .x = drawHead.x + x1,
                    .y = drawHead.y + y1
                };
                float x = strtof(strtok(NULL, ","), NULL);
                float y = strtof(strtok(NULL, " "), NULL);
                Point c = {
                    .x = drawHead.x + x,
                    .y = drawHead.y + y
                };
                printf("quadraticBezierRel (%f, %f), (%f, %f)\n", x1, y1, x, y);
                for (float t = 0; t <= 1; t += resolution) {
                    pointBuffer[pointIndex] = quadBezier(drawHead, b, c, t);
                    pointIndex++;
                }
                drawHead = c;
            }
            while(notCommand(strtok(NULL, " "), &goingAgain, &x1, &y1, &gotCommand, &command));
        }
        else if (strcmp(command, "c") == 0) {
            int goingAgain = 0;
            float x1;
            float y1;
            do {
                if (!goingAgain) {
                    x1 = strtof(strtok(NULL, ","), NULL);
                    y1 = strtof(strtok(NULL, " "), NULL);
                }
                Point b = {
                    .x = drawHead.x + x1,
                    .y = drawHead.y + y1
                };
                float x2 = strtof(strtok(NULL, ","), NULL);
                float y2 = strtof(strtok(NULL, " "), NULL);
                Point c = {
                    .x = drawHead.x + x2,
                    .y = drawHead.y + y2
                };
                float x = strtof(strtok(NULL, ","), NULL);
                float y = strtof(strtok(NULL, " "), NULL);
                Point d = {
                    .x = drawHead.x + x,
                    .y = drawHead.y + y
                };
                printf("cubicBezierRel (%f, %f), (%f, %f), (%f, %f)\n", x1, y1, x2, y2, x, y);
                for (float t = 0; t <= 1; t += resolution) {
                    pointBuffer[pointIndex] = cubicBezier(drawHead, b, c, d, t);
                    pointIndex++;
                }
                drawHead = d;
            }
            while(notCommand(strtok(NULL, " "), &goingAgain, &x1, &y1, &gotCommand, &command));
        }
        if (!gotCommand) {
            command = strtok(NULL, " ");
        }
    }
    return pointIndex;
}