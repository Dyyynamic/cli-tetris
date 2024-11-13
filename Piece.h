#pragma once

#include <array>

#include "Point.h"

struct Piece
{
    int type;
    Point pos;
    int color;
    std::array<std::array<Point, 4>, 4> shapes;
    int rotation;
};
