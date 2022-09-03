
// snakeDlg.h: 头文件
//

#pragma once
#define GAME_ROW 34
#define GAME_COL 34
#define GAMEBLOCK 20
#define GAMEBRODER 1
#define GAMECONTENT 0
#define GAMEFOOD 3
#define SNAKE_MOVE 1000
#include <list>

using namespace std;

struct SNAKE 
{
	int nRow;
	int nCol;
};

enum MOVE_ENUM
{
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// CsnakeDlg 对话框
class CsnakeDlg : public CDialogEx
{
// 构造
public:
	CsnakeDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SNAKE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

// 自定义游戏部分
public:
	// 初始化游戏
	void InitGame();
	// 画游戏墙
	void DrawGameWnd(CDC& dc);
	// 画蛇身部分
	void DrawSnake(CDC& dc);
	// 移动蛇
	void SnakeMove();
	// 随机食物
	void CreateFood();
private:
	byte m_bGameArr[GAME_ROW][GAME_COL];
	list<SNAKE> m_listSnake;
	MOVE_ENUM m_move;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};
