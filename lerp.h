typedef struct {
    int x;
    int y;
} Point;

Point lerp(Point a, Point b, float t);
Point quadBezier(Point a, Point b, Point c, float t);
Point cubicBezier(Point a, Point b, Point c, Point d, float t);