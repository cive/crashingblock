#ifndef UNICODE
#define UNICODE
#endif 


#include <windows.h>
#include <wingdi.h>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>
#include "config.h"
#include "block.h"
#include "ball.h"

#pragma comment(lib, "msimg32.lib")

// ---- Define    ----
#define TM_GAME_NOW 1

// ---- Parameter ----

// ---- Prototype ----
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static VOID setClientSize(HWND, LONG, LONG);
const HFONT createFontStandard(const WCHAR*, int, int);
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
* @param hWnd �E�B���h�E�n���h��
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

	// If the value is true, game is paused
	static BOOL isPaused = FALSE;

	// The remaining lives
	static int nRemain;
	static BOOL isGameover = FALSE;

	// position and velocity
	static lpPosition ball_position = lpPosition(new Position());
	static lpVelocity ball_velocity = lpVelocity(new Velocity());
	static lpPosition bar_position  = lpPosition(new Position());

	// stage
	static int nStageClear = 0;

	// Blocks
	static std::vector<BlockInterface*> vecBlocks;

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
		nRemain = 5;
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
		switch (wParam)
		{
		case TM_GAME_NOW:
		{
			////// �{�[��: �ǂ̔��� //////
			Position tmp_pos{ ball_position->x, ball_position->y };
			tmp_pos.x += ball_velocity->x;
			if (tmp_pos.x < GAMEPANE_X_OFFSET || tmp_pos.x > GAMEPANE_X_OFFSET + GAMEPANE_WIDTH)
			{
				ball_velocity->x = -ball_velocity->x;
			}
			tmp_pos.y += ball_velocity->y;
			if (tmp_pos.y < GAMEPANE_Y_OFFSET)
			{
				ball_velocity->y = -ball_velocity->y;
			}
			////// �{�[��: �o�[�̔��� //////
			if (tmp_pos.y + kstBallSize > bar_position->y && bar_position->y + ksizeBar.cy > tmp_pos.y + kstBallSize &&
				tmp_pos.x > bar_position->x && tmp_pos.x < bar_position->x + ksizeBar.cx)
			{
				ball_velocity->y = -ball_velocity->y;
			}
			////// �{�[��: �u���b�N�̔��� //////
			bool isHashed = false;
			for each (auto& block in vecBlocks)
			{
				if (!block->hasCleared() && block->isHashed(tmp_pos))
				{
					isHashed = true;
					block->clear();
				}
			}
			// �u���b�N�̍폜
			for (auto it = vecBlocks.begin(); it != vecBlocks.end(); )
			{
				if ((*it)->hasCleared())
				{
					delete *it;
					it = vecBlocks.erase(it);
				}
				else
				{
					++it;
				}
			}
			// ������ɂԂ��邱�Ƃ��l������
			if (isHashed)
			{
				ball_velocity->x = -ball_velocity->y;
				ball_velocity->y = ball_velocity->x;
			}
			if (vecBlocks.size() == 0)
			{
				isPaused = TRUE;
				nRemain++;
				nStageClear++;
				// TODO: ���ʏ������܂Ƃ߂�ׂ�
				ball_position->x = 104;
				ball_position->y = 200;
				ball_velocity->x = 2;
				ball_velocity->y = 2;
				KillTimer(hWnd, TM_GAME_NOW);

			}
			////// �{�[���̈ړ� //////
			ball_position->x += ball_velocity->x;
			ball_position->y += ball_velocity->y;
			////// �{�[������O�� //////
			if (ball_position->y > 550)
			{
				isPaused = TRUE;
				if (nRemain == 0)
				{
					isGameover = TRUE;
				}
				KillTimer(hWnd, TM_GAME_NOW);
			}
			break;
		}
		default:
			break;
		}
		////// �ĕ`�� //////
		::InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	}
	case WM_ERASEBKGND:
		return 1;
	case WM_KEYDOWN:
	{
		////// ���������Ă��܂�����: �X�y�[�X�L�[�ŕ��� ///////
		if (wParam == VK_SPACE && isPaused && !isGameover)
		{
			// TODO: ���ʏ������܂Ƃ߂�ׂ�
			ball_position->x = 104;
			ball_position->y = 200;
			ball_velocity->x = 2;
			ball_velocity->y = 2;

			isPaused = FALSE;
			--nRemain;
			SetTimer(hWnd, TM_GAME_NOW, 1, NULL);
			return 0;
		}
		////// �Q�[���I�[�o�[�ɂȂ��Ă��܂�����: Esc�L�[�ōŏ������蒼�� //////
		if (wParam == VK_ESCAPE && isGameover)
		{
			// TODO: ���ʏ������܂Ƃ߂�ׂ�
			ball_position->x = 104;
			ball_position->y = 200;
			ball_velocity->x = 2;
			ball_velocity->y = 2;

			isGameover = FALSE;
			isPaused = FALSE;
			nStageClear = 0;
		}


		if (wParam == VK_SPACE && !isStartGame && nStageClear == BasicStage::one)
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
			SetTimer(hWnd, TM_GAME_NOW, 1, NULL);
		}
		return 0;
	}
	case WM_PAINT:
		HDC hdc = BeginPaint(hWnd, &ps);
		HBITMAP hPrevBitmap;
		// �������f�o�C�X�R���e�L�X�g���쐬����
		HDC hBuffer = CreateCompatibleDC(hdc);
		UINT BMP_W; // bitmap width
		UINT BMP_H; // bitmap height

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
		////// �����̕\�� //////
		HFONT hFont, hPrevFont;
		const WCHAR* font_name     = L"Meiryo UI";
		const WCHAR* plsPressSpace = L"Space key, game start";
		const size_t kstPlsPressSpaceX = 400;
		const size_t kstPlsPressSpaceY =  20;
		const WCHAR* kwcLife = L"LIFE: ";
		const size_t kstLifeX = 400;
		const size_t kstLifeY = 60;
		const WCHAR* kwcStageClear = L"SAGE CLEAR!!!";
		const size_t kstStageClearX = 400;
		const size_t kstStageClearY = 300;
		hFont = createFontStandard(font_name, 22, FW_REGULAR);
		hPrevFont = (HFONT)SelectObject(hMemDC, hFont);
		SetTextColor(hMemDC, RGB(10, 10, 10));

		// for debug...
		// std::wstring wstrForDebug;
		// const size_t kstForDebugX = 400;
		// const size_t kstForDebugY = 400;
		// wstrForDebug.append(std::to_wstring(ball_position->x)).append(L" ,").append(std::to_wstring(ball_position->y));
		/// for debug...

		if (isPaused || !isStartGame)
		{
			TextOut(hMemDC, kstPlsPressSpaceX, kstPlsPressSpaceY, plsPressSpace, wcslen(plsPressSpace));
		}
		TextOut(hMemDC, kstLifeX,          kstLifeY,          kwcLife,       wcslen(kwcLife));
		TextOut(hMemDC, kstLifeX+100,      kstLifeY,          (LPCWSTR)std::to_wstring(nRemain).c_str(), 1);
		if (vecBlocks.size() == 0 && isPaused)
		{
			TextOut(hMemDC, kstStageClearX, kstStageClearY, kwcStageClear, wcslen(kwcStageClear));
		}
		// for debug...
		// TextOut(hMemDC, kstForDebugX, kstForDebugY, (LPCWSTR)wstrForDebug.c_str(), wstrForDebug.size());
		// for debug...
		SelectObject(hMemDC, hPrevFont);
		DeleteObject(hFont);

		BitBlt(hdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, hMemDC, 0, 0, SRCCOPY);
		DeleteDC(hBuffer);
		EndPaint(hWnd, &ps);
		return 0;

	}
	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

/**
 * @brief �E�B���h�E�̃T�C�Y�����T�C�Y����֐�
 * @param hWnd �E�B���h�E�n���h��
 * @param width ��
 * @param height ����
 */
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

/**
 * @brief �ȈՓI��Font��ݒ肷��
 * @param name ���̖�
 * @param size �傫��
 * @param weight ����
 */
const HFONT createFontStandard(const WCHAR* name, int size, int weight)
{
	return (HFONT)CreateFont(size, 0, 0, 0, weight, FALSE, FALSE, FALSE, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, (LPCWSTR)name);
}