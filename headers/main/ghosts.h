#ifndef GHOSTS_HEADER
#define GHOSTS_HEADER set
#include <gb/gb.h>
#include "util.h"



void GetGhostNextDirection(uint8_t ghostIndex);
void GetGhostTargetTile(uint8_t ghostIndex);
void SetupGhosts();
void DrawGhost(uint8_t i);
void UpdateSingleGhost(uint8_t i);
void UpdateAllGhosts();

#endif