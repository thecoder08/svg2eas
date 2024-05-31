#include "lerp.h"

Point lerp(Point a, Point b, float t) {
    Point c = {
        .x = a.x + (b.x-a.x) * t,
        .y = a.y + (b.y-a.y) * t
    };
    return c;
}

Point quadBezier(Point a, Point b, Point c, float t) {
    return lerp(lerp(a, b, t), lerp(b, c, t), t);
}

Point cubicBezier(Point a, Point b, Point c, Point d, float t) {
    return lerp(quadBezier(a, b, c, t), quadBezier(b, c, d, t), t);
}