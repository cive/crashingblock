#pragma once
#define SCREEN_WIDTH        (640)       // スクリーンの横幅(32ドット×20個)
#define SCREEN_HEIGHT       (480)       // スクリーンの高さ(32ドット×15個)

#define GAMEPANE_X_OFFSET 16
#define GAMEPANE_Y_OFFSET 16
#define GAMEPANE_WIDTH   320
#define GAMEPANE_HEIGHT  448
#define BALL_SIZE          8
#define BLOCK_X_OFFSET GAMEPANE_X_OFFSET + 16
#define BLOCK_Y_OFFSET GAMEPANE_Y_OFFSET + 16
#define GAMEOVER_LINE    550

/*! ゲームステージ, まだ一面のみ */
enum BasicStage
{
	one
};