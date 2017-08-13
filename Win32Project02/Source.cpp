#ifndef UNICODE
#define UNICODE
#endif 


#include <windows.h>
#include <wingdi.h>
#include <memory>
#include <vector>
#include "config.h"
#include "block.h"
#include "ball.h"

#pragma comment(lib, "msimg32.lib")

// ---- Parameter ----

// ---- Prototype ----
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static VOID setClientSize(HWND, LONG, LONG);
class BlockInterface;
class BasicBlock;
class HardBlock;
class BlockFactory;

/**
* @brief Entry Point
* @param hInstance �A�v���P�[�V�����̌��݂̃C���X�^���X
* @param hPrevInstance �ȑO�̃C���X�^���X�n���h��
* @param lpCmdLine �R�}���h���C��
* @param �\�����
* @return ����I���Ȃ�EXIT_SUCCESS
*/

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	// �E�B���h�E �N���X��o�^����
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	WNDCLASS wc = {};

	// �Ƃ肠�����ClpfnWndProc, hInstance, lpszClassName�����C��`���Ă����Α��v
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	::RegisterClass(&wc);

	// �E�B���h�E���쐬����

	HWND hWnd = ::CreateWindowEx(
		WS_EX_COMPOSITED,               // �I�v�V�����̃E�B���h�E �X�^�C��
		CLASS_NAME,                     // �E�B���h�E �N���X
		L"Learn to Program Windows",    // �E�B���h�E �e�L�X�g
		WS_OVERLAPPEDWINDOW,            // �E�B���h�E �X�^�C��
										// �T�C�Y�ƈʒu
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // �e�E�B���h�E    
		NULL,       // ���j���[
		hInstance,  // �C���X�^���X �n���h��
		NULL        // �ǉ��̃A�v���P�[�V���� �f�[�^
	);

	if (hWnd == NULL)
	{
		return EXIT_SUCCESS;
	}

	setClientSize(hWnd, SCREEN_WIDTH, SCREEN_HEIGHT);
	::ShowWindow(hWnd, nCmdShow);
	::UpdateWindow(hWnd);

	// ���b�Z�[�W ���[�v�����s����

	MSG msg = {};
	while (::GetMessage(&msg, NULL, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return EXIT_SUCCESS;
}


/**
* @brief �E�B���h�E�v���V�[�W��
* @param hwnd �E�B���h�E�n���h��
* @param uMsg ���b�Z�[�W�̎��ʎq
* @param wParam ���b�Z�[�W�̍ŏ��̃p�����[�^
* @param lParam ���b�Z�[�W��2�Ԗڂ̃p�����[�^
* @return ���b�Z�[�W�����̌���
*/
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP hBitmap; // bitmap
	static HDC hMemDC; // off screen
	PAINTSTRUCT ps;
	BITMAP bmp;

	// image handler
	static HBITMAP hBackground;
	static HBITMAP hBall;
	static HBITMAP hBar;
	static HBITMAP hPrevBitmap;
	static std::vector<HBITMAP> hBlocks;

	// const value
	const size_t kstBallSize = 8;
	const SIZE ksizeBar = SIZE{ 32, 4 };
	const size_t kstMaxCols = 9;
	const size_t kstMaxRows = 10;
	const size_t kstOffsetBlocksX = 16;
	const size_t kstOffsetBlocksY = 32;

	// press space bar, then game start.
	static BOOL isStartGame = FALSE;

	// position and velocity
	static lpPosition ball_position = lpPosition(new Position());
	static lpVelocity ball_velocity = lpVelocity(new Velocity());
	static lpPosition bar_position  = lpPosition(new Position());

	// stage
	static int nClear = 0;

	// Blocks
	static std::vector<BlockInterface*> vecBlocks;

	UINT BMP_W; // bitmap width
	UINT BMP_H; // bitmap height
	switch (uMsg)
	{
	case WM_DESTROY:
		::DeleteDC(hMemDC);
		::DeleteObject(hBitmap);
		::DeleteObject(hBackground);
		::DeleteObject(hBall);
		::DeleteObject(hBar);
		hBlocks.clear();
		vecBlocks.clear();
		PostQuitMessage(0);
		return 0;
	case WM_MOUSEMOVE:
	{
		bar_position->x = LOWORD(lParam);
		if (bar_position->x < GAMEPANE_X_OFFSET)
		{
			bar_position->x = GAMEPANE_X_OFFSET;
		}
		if (bar_position->x > GAMEPANE_X_OFFSET + GAMEPANE_WIDTH - ksizeBar.cx)
		{
			bar_position->x = GAMEPANE_X_OFFSET + GAMEPANE_WIDTH - ksizeBar.cx;
		}
		/////// Cursor //////
		HCURSOR hCursor = ::LoadCursor(NULL, IDC_CROSS);
		::SetCursor(hCursor);
		return 0;
	}
	case WM_CREATE:
	// ---- Initialize ----
		/////// Double buffer //////
		HDC hDC;
		hDC = GetDC(hWnd);
		hMemDC = CreateCompatibleDC(hDC);
		hBitmap = CreateCompatibleBitmap(hDC, SCREEN_WIDTH, SCREEN_HEIGHT);
		SelectObject(hMemDC, hBitmap);
		SelectObject(hMemDC, GetStockObject(DC_PEN));
		SelectObject(hMemDC, GetStockObject(DC_BRUSH));
		ReleaseDC(hWnd, hDC);
		/////// Load image //////
		hBackground = (HBITMAP)::LoadImage(NULL, TEXT(".\\img\\background.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hBall       = (HBITMAP)::LoadImage(NULL, TEXT(".\\img\\ball.bmp"),       IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hBar        = (HBITMAP)::LoadImage(NULL, TEXT(".\\img\\bar.bmp"),        IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		hBlocks.push_back((HBITMAP)::LoadImage(NULL, TEXT(".\\img\\BasicBlock.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
		////// Initial value //////
		ball_position->x = 104;
		ball_position->y = 200;
		ball_velocity->x = 2;
		ball_velocity->y = 2;
		POINT mouse_pos;
		::GetCursorPos(&mouse_pos);
		bar_position->x = mouse_pos.x;
		if (bar_position->x < GAMEPANE_X_OFFSET)
		{
			bar_position->x = GAMEPANE_X_OFFSET;
		}
		if (bar_position->x > GAMEPANE_X_OFFSET + GAMEPANE_WIDTH - ksizeBar.cx)
		{
			bar_position->x = GAMEPANE_X_OFFSET + GAMEPANE_WIDTH - ksizeBar.cx;
		}
		bar_position->y = 448;
		return 0;
	case WM_TIMER:
	// ---- Procedure ----
	{
		////// �{�[��: �ǂ̔��� //////
		Position tmp_pos{ ball_position->x, ball_position->y };
		tmp_pos.x += ball_velocity->x;
		if (tmp_pos.x < GAMEPANE_X_OFFSET || tmp_pos.x > GAMEPANE_X_OFFSET + GAMEPANE_WIDTH)
		{
			ball_velocity->x = -ball_velocity->x;
		}
		tmp_pos.y += ball_velocity->y;
		if (tmp_pos.y < GAMEPANE_Y_OFFSET || tmp_pos.y > GAMEPANE_Y_OFFSET + GAMEPANE_HEIGHT)
		{
			ball_velocity->y = -ball_velocity->y;
		}
		////// �{�[��: �o�[�̔��� //////
		if (tmp_pos.y + kstBallSize > bar_position->y && tmp_pos.x > bar_position->x && tmp_pos.x < bar_position->x + ksizeBar.cx)
		{
			ball_velocity->y = -ball_velocity->y;
		}
		::InvalidateRect(hWnd, NULL, TRUE);
		////// �{�[��: �u���b�N�̔��� //////
		bool isHashed = false;
		for each (auto& block in vecBlocks)
		{
			if (!block->hasCleared() && block->isHashed(ball_position))
			{
				isHashed = true;
				block->clear();
			}
		}
		// ������ɂԂ��邱�Ƃ��l������
		if (isHashed)
		{
			ball_velocity->x = -ball_velocity->y;
			ball_velocity->y = ball_velocity->x;
		}
		////// �{�[���̈ړ� //////
		ball_position->x += ball_velocity->x;
		ball_position->y += ball_velocity->y;
		return 0;
	}
	case WM_ERASEBKGND:
		return 1;
	case WM_KEYDOWN:
	{
		if (wParam == VK_SPACE && !isStartGame && nClear == 0)
		{
			////// Timer setting ///////
			isStartGame = TRUE;
			for (size_t row = 0; row < kstMaxRows; ++row)
			{
				for (size_t col = 0; col < kstMaxCols; ++col)
				{
					BlockInterface* block = BlockFactory::getBlock(BlockType::Basic, col, row);
					vecBlocks.push_back(block);
				}
			}
			::SetTimer(hWnd, 1, 1, NULL);
		}
		return 0;
	}
	case WM_PAINT:
		HDC hdc = BeginPaint(hWnd, &ps);
		// �������f�o�C�X�R���e�L�X�g���쐬����
		HDC hBuffer = CreateCompatibleDC(hdc);
		// ���[�h�����r�b�g�}�b�v��I������

		////// �w�i�摜 //////
		hPrevBitmap = (HBITMAP)SelectObject(hBuffer, hBackground);
		::GetObject(hBackground, sizeof(BITMAP), &bmp);
		BMP_W = (int)bmp.bmWidth;
		BMP_H = (int)bmp.bmHeight;
		::BitBlt(hMemDC, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, hBuffer, 0, 0, SRCCOPY);

		////// �{�[���̕\�� //////
		hPrevBitmap = (HBITMAP)SelectObject(hBuffer, hBall); // �摜��`��p�o�b�t�@�ɕϊ�?
		GetObject(hBall, sizeof(BITMAP), &bmp); // �I�u�W�F�N�g�̑傫�������߂�
		BMP_W = (int)bmp.bmWidth;
		BMP_H = (int)bmp.bmHeight;
		::TransparentBlt(hMemDC, ball_position->x, ball_position->y, BMP_W, BMP_H, hBuffer, 0, 0, BMP_W, BMP_H, RGB(255, 255, 255));

		////// �o�[�̕\�� //////
		hPrevBitmap = (HBITMAP)SelectObject(hBuffer, hBar);
		GetObject(hBar, sizeof(BITMAP), &bmp);
		BMP_W = (int)bmp.bmWidth;
		BMP_H = (int)bmp.bmHeight;
		::BitBlt(hMemDC, bar_position->x, bar_position->y, BMP_W, BMP_H, hBuffer, 0, 0, SRCCOPY); // �`��

		////// �u���b�N�̕\�� //////
		if (isStartGame)
		{
			for each (auto& block in vecBlocks)
			{
				if (!block->hasCleared())
				{
					hPrevBitmap = (HBITMAP)SelectObject(hBuffer, hBlocks.at(block->getType()));
					GetObject(hBlocks[block->getType()], sizeof(BITMAP), &bmp);
					BMP_W = (int)bmp.bmWidth;
					BMP_H = (int)bmp.bmHeight;
					POINT p = block->getPoint();
					::BitBlt(hMemDC, p.x, p.y, BMP_W, BMP_H, hBuffer, 0, 0, SRCCOPY);
				}
			}
		}

		BitBlt(hdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, hMemDC, 0, 0, SRCCOPY);
		DeleteDC(hBuffer);
		EndPaint(hWnd, &ps);
		return 0;

	}
	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

static VOID setClientSize(HWND hWnd, LONG width, LONG height)
{
	RECT rc1;
	RECT rc2;

	GetWindowRect(hWnd, &rc1);
	GetClientRect(hWnd, &rc2);
	width += ((rc1.right - rc1.left) - (rc2.right - rc2.left));
	height += ((rc1.bottom - rc1.top) - (rc2.bottom - rc2.top));
	SetWindowPos(hWnd, NULL, 0, 0, width, height, (SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE));
}