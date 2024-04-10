#pragma once

#include "global_includes.h"
#include "move.h"
#include "board.h"

Move think(Board board, int softTimeLimit, int hardTimeLimit);