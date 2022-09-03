
// snakeDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "snake.h"
#include "snakeDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CsnakeDlg 对话框



CsnakeDlg::CsnakeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SNAKE_DIALOG, pParent),m_move(MOVE_ENUM::LEFT)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CsnakeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CsnakeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


// CsnakeDlg 消息处理程序

BOOL CsnakeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// 在此添加额外的初始化代码
	InitGame();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CsnakeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CsnakeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		// 画图需要设别上下文
		CPaintDC dc(this);
		CRect rt;
		GetClientRect(rt);

		// 创建兼容dc
		CDC dcMem;
		dcMem.CreateCompatibleDC(&dc);
		//创建位图
		CBitmap bmp;
		bmp.CreateCompatibleBitmap(&dc, rt.Width(), rt.Height());
		dcMem.SelectObject(&bmp);

		DrawGameWnd(dcMem);
		DrawSnake(dcMem);

		dc.BitBlt(0, 0, rt.Width(), rt.Height(), &dcMem, 0, 0, SRCCOPY);

		dcMem.DeleteDC();
		bmp.DeleteTempMap();

		CDialogEx::OnPaint();

		
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CsnakeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CsnakeDlg::InitGame()
{
	ZeroMemory(m_bGameArr, sizeof(m_bGameArr));
	// 初始化边界
	for (int i = 0; i < GAME_ROW; i++)
	{
		for (int j = 0; j < GAME_COL; j++)
		{
			if (i == 0 || i == GAME_ROW -1 || j == 0 || j == GAME_COL -1) 
			{
				m_bGameArr[i][j] = GAMEBRODER;
			}
			else
			{
				m_bGameArr[i][j] = GAMECONTENT;
			}
		}
	}

	// 初始化蛇身
	m_listSnake.clear();
	SNAKE sTemp{};
	sTemp.nCol = 8;
	sTemp.nRow = 8;

	for (int i = 0; i < 5; i++)
	{
		m_listSnake.push_back(sTemp);
		sTemp.nCol++;
	}

	// 初始化定时器
	SetTimer(SNAKE_MOVE, 500, nullptr);

	// 初始化食物
	CreateFood();
}

void CsnakeDlg::DrawGameWnd(CDC& dc)
{
	// 游戏内容显示颜色
	CBrush brushGame(RGB(0, 0, 0));
	// 游戏墙显示颜色
	CBrush brushBorder(RGB(0, 185, 0));
	// 食物显示颜色
	CBrush brushFood(RGB(255, 201, 14));

	for (int i = 0; i < GAME_ROW; i++)
	{
		for (int j = 0; j < GAME_COL; j++)
		{
			CRect rt;
			rt.left = j * GAMEBLOCK;
			rt.top = i * GAMEBLOCK;
			rt.right = rt.left + GAMEBLOCK;
			rt.bottom = rt.top + GAMEBLOCK;

			if (m_bGameArr[i][j] == GAMEBRODER)
			{
				dc.FillRect(rt, &brushBorder);
			}
			else if (m_bGameArr[i][j] == GAMEFOOD)
			{
				dc.FillRect(rt, &brushFood);
			}
			else
			{
				dc.FillRect(rt, &brushGame);
			}
		}
	}
}

void CsnakeDlg::DrawSnake(CDC& dc)
{
	CBrush brushSnake(RGB(0, 162, 232));

	list<SNAKE>::iterator it = m_listSnake.begin();

	while (it != m_listSnake.end())
	{
		CRect rt;
		rt.left = it->nCol * GAMEBLOCK;
		rt.top = it->nRow * GAMEBLOCK;
		rt.right = rt.left + 20;
		rt.bottom = rt.top + 20;

		if (it == m_listSnake.begin())
		{
			dc.Rectangle(rt);
		}
		else
		{
			dc.FillRect(rt, &brushSnake);
		}
		
		it++;
	}
}

void CsnakeDlg::SnakeMove()
{
	// 移动算法可以优化
	list<SNAKE>::iterator itHead = m_listSnake.end();
	list<SNAKE>::iterator itTail = m_listSnake.end();
	itHead--;
	itTail--;
	
	while (itHead != m_listSnake.begin())
	{
		itHead--;
		itTail->nCol = itHead->nCol;
		itTail->nRow = itHead->nRow;
		itTail--;
	}
	itHead = m_listSnake.begin();

	if (m_bGameArr[itHead->nRow][itHead->nCol] == GAMEFOOD)
	{
		m_bGameArr[itHead->nRow][itHead->nCol] = 0;
		m_listSnake.push_back(*itHead);
		CreateFood();
	}

	if (m_bGameArr[itHead->nRow][itHead->nCol] == GAMEBRODER)
	{
		KillTimer(SNAKE_MOVE);
		if (MessageBox(_T("Game Over\r\n是否重新开始"), _T("提示"), MB_YESNO == IDYES))
		{
			InitGame();
		}
		
		return;
	}

	switch (m_move)
	{
	case LEFT:
		itHead->nCol -= 1;
		break;
	case RIGHT:
		itHead->nCol += 1;
		break;
	case UP:
		itHead->nRow -= 1;
		break;
	case DOWN:
		itHead->nRow += 1;
		break;
	default:
		break;
	}

	Invalidate();
}

void CsnakeDlg::CreateFood()
{
	// 确保数值在墙内
	int x = rand() % (GAME_ROW-2);
	x += 1;

	int y = rand() % (GAME_COL-2);
	y += 1;

	m_bGameArr[x][y] = 3;
}



void CsnakeDlg::OnTimer(UINT_PTR nIDEvent)
{
	// 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == SNAKE_MOVE) 
	{
		SnakeMove();
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CsnakeDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// 在此添加消息处理程序代码和/或调用默认值
	if (nChar == 'A')
	{
		m_move = MOVE_ENUM::LEFT;
	}
	else if (nChar == 'D')
	{
		m_move = MOVE_ENUM::RIGHT;
	} else if (nChar == 'W')
	{
		m_move = MOVE_ENUM::UP;
	}
	else if (nChar == 'S')
	{
		m_move = MOVE_ENUM::DOWN;
	}

	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}
