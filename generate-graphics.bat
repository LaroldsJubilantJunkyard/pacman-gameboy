
rmdir /s/q  "source\gen"
rmdir /s/q  "headers\gen"

mkdir "source\gen\default\graphics"
mkdir "headers\gen\graphics"


SET GBDK_HOME=C:/gbdk
SET PNG2ASSET=C:/gbdk/bin/png2asset

%PNG2ASSET% graphics\Pacman.png -c source\gen\default\graphics\Pacman.c -sw 16 -sh 16 -spr8x16 -keep_palette_order -noflip -group sprites
%PNG2ASSET% graphics\PacmanEatenDown.png -c source\gen\default\graphics\PacmanEatenDown.c -sw 16 -sh 16 -spr8x16 -keep_palette_order -noflip -group sprites
%PNG2ASSET% graphics\PacmanEatenLeft.png -c source\gen\default\graphics\PacmanEatenLeft.c -sw 16 -sh 16 -spr8x16 -keep_palette_order -noflip -group sprites
%PNG2ASSET% graphics\PacmanEatenRight.png -c source\gen\default\graphics\PacmanEatenRight.c -sw 16 -sh 16 -spr8x16 -keep_palette_order -noflip -group sprites
%PNG2ASSET% graphics\PacmanEatenUp.png -c source\gen\default\graphics\PacmanEatenUp.c -sw 16 -sh 16 -spr8x16 -keep_palette_order -noflip -group sprites
%PNG2ASSET% graphics\Ghosts.png -c source\gen\default\graphics\Ghosts.c -sw 16 -sh 16 -spr8x16 -keep_palette_order -noflip -group sprites
%PNG2ASSET% graphics\GhostsScared.png -c source\gen\default\graphics\GhostsScared.c -sw 16 -sh 16 -spr8x16 -keep_palette_order -noflip -group sprites
%PNG2ASSET% graphics\GhostsEaten.png -c source\gen\default\graphics\GhostsEaten.c -sw 16 -sh 16 -spr8x16 -keep_palette_order -noflip -group sprites
%PNG2ASSET% graphics\TargetTiles.png -c source\gen\default\graphics\TargetTiles.c -sw 8 -sh 16 -spr8x16 -keep_palette_order -noflip -group sprites


::%PNG2ASSET% graphics\Map-source.png -c source\gen\default\graphics\MapSource.c -map -use_map_attributes -is_source_tileset
%PNG2ASSET% graphics\Map.png -c source\gen\default\graphics\Map.c -map -use_map_attributes -noflip  -keep_palette_order -group backgrounds
%PNG2ASSET% graphics\Pellets.png -c source\gen\default\graphics\Pellets.c -map -use_map_attributes -noflip  -keep_palette_order -group background
%PNG2ASSET% graphics\Numbers.png -c source\gen\default\graphics\Numbers.c -map -use_map_attributes -noflip -keep_palette_order -group backgrounds
%PNG2ASSET% graphics\OneUpText.png -c source\gen\default\graphics\OneUpText.c -map -use_map_attributes -noflip -keep_palette_order -group backgrounds
%PNG2ASSET% graphics\ReadyText.png -c source\gen\default\graphics\ReadyText.c -map -use_map_attributes -noflip -keep_palette_order -group backgrounds
%PNG2ASSET% graphics\HighText.png -c source\gen\default\graphics\HighText.c -map -use_map_attributes -noflip -keep_palette_order -group backgrounds


:: move .h files to their proper location
FOR /R "source/gen/bank1/graphics/" %%X IN (*.h) DO (
    move "%%X" "%cd%/headers/gen/graphics/"
)

:: move .h files to their proper location
FOR /R "source/gen/default/graphics/" %%X IN (*.h) DO (
    move "%%X" "%cd%/headers/gen/graphics/"
)