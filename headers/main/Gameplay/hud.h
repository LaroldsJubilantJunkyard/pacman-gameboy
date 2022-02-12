#include <gb/gb.h>
#include "common.h"

void DrawNumber(uint8_t x,uint8_t y, uint16_t number,uint8_t digits,uint8_t palette);

void SetupHUD();
void UpdateHUD();
void UpdateScore();
void ShowOrHideActiveBigDots(uint8_t show);
void ShowOrHideOneUpText(uint8_t show);