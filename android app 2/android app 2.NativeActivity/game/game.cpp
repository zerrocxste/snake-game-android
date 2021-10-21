#include "game.h"

CGame::CGame(int width, int height, void* p_input, void* p_onpressed)
{
	srand(time(0));

	this->m_DisplaySize.x = width;
	this->m_DisplaySize.y = height;

	CalculateFieldSize();

	this->m_GameState.m_fSnakeHead.Set(field(this->m_iFieldSize[0] / 2, this->m_iFieldSize[1] - 1));
	this->m_GameState.m_flSnakeSpeed = 800.f;
	this->m_GameState.m_fSnakeHead.m_dDirection = UP;

	this->m_pTouchCoords = p_input;
	this->m_pIsTouched = p_onpressed;

	LOGI("[+] %s. Constructor called", __FUNCTION__);
}

CGame::~CGame()
{
	LOGI("[+] %s. Destructor called", __FUNCTION__);
}

void CGame::Run()
{
	DrawInputs();

	Logic();

	DrawGame();
}

void CGame::Logic()
{
	InputControlSnake();
	UpdateGameTick();
	UpdateSnakeMovement();
	SnakeHistoryCollector();
	SetPointPosition();

	if (FindingExitFromPlayingZone() || FindingCollisionSnakeBody())
		ClearGameState();

	if (FindIntersectionGamePoint())
		SnakeLevelUp();
}

CGame::touch_coords CGame::GetTouchCoords()
{
	return *(CGame::touch_coords*)this->m_pTouchCoords;
}

bool CGame::GetIsTouched()
{
	return *(bool*)this->m_pIsTouched;
}

void CGame::DrawInputs()
{
	CleanupInput();

	/*if (GetIsTouched())
		LOGI("[+] %s. %d, %d", __FUNCTION__, GetTouchCoords().x, GetTouchCoords().y);*/

	auto vButtonSize = vec2(300.f, 300.f);

	auto iScreenCenterX = (this->m_DisplaySize.x / 2.f) - (vButtonSize.x / 2.f);
	auto iScreenCenterY = (this->m_DisplaySize.y / 2.f);

	if (AddButton(vec2(iScreenCenterX, iScreenCenterY + 200.f), vButtonSize, color(1.f, 0.f, 0.f, 1.f)))
		this->m_InputController.m_bIsPressed[input_controller::GAME_KEYS::UP] = true;

	if (AddButton(vec2(iScreenCenterX, iScreenCenterY + 600.f), vButtonSize, color(1.f, 0.f, 0.f, 1.f)))
		this->m_InputController.m_bIsPressed[input_controller::GAME_KEYS::DOWN] = true;

	if (AddButton(vec2(iScreenCenterX - 350.f, iScreenCenterY + 400.f), vButtonSize, color(1.f, 0.f, 0.f, 1.f)))
		this->m_InputController.m_bIsPressed[input_controller::GAME_KEYS::LEFT] = true;

	if (AddButton(vec2(iScreenCenterX + 350.f, iScreenCenterY + 400.f), vButtonSize, color(1.f, 0.f, 0.f, 1.f)))
		this->m_InputController.m_bIsPressed[input_controller::GAME_KEYS::RIGHT] = true;
}

void CGame::InputControlSnake()
{
	if (this->m_InputController.m_bIsPressed[input_controller::GAME_KEYS::UP]
		&& field(this->m_GameState.m_fSnakeHead.x, this->m_GameState.m_fSnakeHead.y - 1) != this->m_GameState.m_fvSnakeBody[1])
		this->m_GameState.m_fSnakeHead.m_dDirection = DIRECTION::UP;

	if (this->m_InputController.m_bIsPressed[input_controller::GAME_KEYS::DOWN]
		&& field(this->m_GameState.m_fSnakeHead.x, this->m_GameState.m_fSnakeHead.y + 1) != this->m_GameState.m_fvSnakeBody[1])
		this->m_GameState.m_fSnakeHead.m_dDirection = DIRECTION::DOWN;

	if (this->m_InputController.m_bIsPressed[input_controller::GAME_KEYS::LEFT]
		&& field(this->m_GameState.m_fSnakeHead.x - 1, this->m_GameState.m_fSnakeHead.y) != this->m_GameState.m_fvSnakeBody[1])
		this->m_GameState.m_fSnakeHead.m_dDirection = DIRECTION::LEFT;

	if (this->m_InputController.m_bIsPressed[input_controller::GAME_KEYS::RIGHT]
		&& field(this->m_GameState.m_fSnakeHead.x + 1, this->m_GameState.m_fSnakeHead.y) != this->m_GameState.m_fvSnakeBody[1])
		this->m_GameState.m_fSnakeHead.m_dDirection = DIRECTION::RIGHT;
}

void CGame::DrawGame()
{
	auto iScreenSpaceGameFieldX = (this->m_DisplaySize.x / 2.f) - ((this->m_iCellSize.x * this->m_iFieldSize[0]) / 2.f);

	for (int x = 1; x < this->m_iFieldSize[0]; ++x)
	{
		for (int y = 1; y < this->m_iFieldSize[1]; ++y)
		{
			vec2 pos((m_iCellSize.x * x) + iScreenSpaceGameFieldX, (m_iCellSize.y * y) + 100.f);

			color col(0.f, 0.7f, 0.5f, 1.f);

			if (field(x, y) == this->m_GameState.m_fPoint)
				col = color(0.f, 0.5f, 0.7f, 1.f); //point

			AddFieldSquare(pos, col);

			if (field(x, y) == this->m_GameState.m_fSnakeHead)
				AddFieldSquare(pos, color(1.f, 0.5f, 0.f, 1.f));

			for (auto i = 0; i < this->m_GameState.m_iSnakePoints; i++)
			{
				if (field(x, y) == this->m_GameState.m_fvSnakeBody.at(i + 1))
					AddFieldSquare(pos, color(1.f, 0.65f, 0.f, 1.f));
			}
		}
	}
}

bool CGame::FindIntersectionGamePoint()
{
	auto ret = this->m_GameState.m_fSnakeHead == this->m_GameState.m_fPoint;

	if (ret)
		LOGI("[+] %s. Snake intersection a point", __FUNCTION__);

	return ret;
}

bool CGame::AddButton(vec2 pos, vec2 size, color col)
{
	drawing::box(pos.x, pos.y, size.x, col);
	auto coords = GetTouchCoords();
	return GetIsTouched() && IntersectionRect(pos, size, vec2(coords.x, coords.y));
}

void CGame::AddFieldSquare(vec2 pos, color col)
{
	drawing::box(pos.x, pos.y, m_iCellSize.y - 5.f, col);
}

bool CGame::FindingExitFromPlayingZone()
{
	auto ret = this->m_GameState.m_fSnakeHead.x > (this->m_iFieldSize[0] - 1) || this->m_GameState.m_fSnakeHead.x < 1
		|| this->m_GameState.m_fSnakeHead.y >(this->m_iFieldSize[1] - 1) || this->m_GameState.m_fSnakeHead.y < 1;

	if (ret)
		LOGI("[+] %s. Snake exit from game zone", __FUNCTION__);

	return ret;
}

bool CGame::FindingCollisionSnakeBody()
{
	for (int i = 0; i < this->m_GameState.m_iSnakePoints; i++)
	{
		if (this->m_GameState.m_fSnakeHead == this->m_GameState.m_fvSnakeBody[i + 1])
		{
			LOGI("[+] %s. Snake intersect yourself", __FUNCTION__);
			return true;
		}
	}
	return false;
}

void CGame::UpdateGameTick()
{
	if (!this->m_GameState.m_bStopGame)
	{
		if (this->m_GameState.m_bUpdateSnake && GetTickCount() - this->m_GameState.m_gtcGameTick > this->m_GameState.m_flSnakeSpeed)
		{
			this->m_GameState.m_bUpdateSnake = false;
		}
		else if (!this->m_GameState.m_bUpdateSnake)
		{
			this->m_GameState.m_bUpdateSnake = true;
		}
	}
}

void CGame::UpdateSnakeMovement()
{
	if (!this->m_GameState.m_bStopGame)
	{
		if (!this->m_GameState.m_bUpdateSnake)
		{
			if (this->m_GameState.m_fSnakeHead.m_dDirection == UP) 
				this->m_GameState.m_fSnakeHead.y -= 1;
			else if (this->m_GameState.m_fSnakeHead.m_dDirection == DOWN) 
				this->m_GameState.m_fSnakeHead.y += 1;
			else if (this->m_GameState.m_fSnakeHead.m_dDirection == LEFT) 
				this->m_GameState.m_fSnakeHead.x -= 1;
			else if (this->m_GameState.m_fSnakeHead.m_dDirection == RIGHT) 
				this->m_GameState.m_fSnakeHead.x += 1;

			this->m_GameState.m_iSnakeHistory++;
			this->m_GameState.m_gtcGameTick = GetTickCount();
		}
	}
}

void CGame::SnakeHistoryCollector()
{
	if (!this->m_GameState.m_bUpdateSnake)
	{
		std::vector<field>temp;
		temp = this->m_GameState.m_fvSnakeBody;
		std::reverse(std::begin(temp), std::end(temp));
		temp.push_back(this->m_GameState.m_fSnakeHead);
		std::reverse(std::begin(temp), std::end(temp));
		this->m_GameState.m_fvSnakeBody = temp;
	}
}

void CGame::SetPointPosition()
{
	if (!this->m_GameState.m_fPoint)
	{
		int iXResult = rand() % (this->m_iFieldSize[0] - 1) + 1;
		int iYResult = rand() % (this->m_iFieldSize[1] - 1) + 1;
		if (this->m_GameState.m_iSnakePoints > 0)
		{
			for (int i = 0; i < this->m_GameState.m_iSnakePoints; i++)
			{
				if (field(iXResult, iYResult) != this->m_GameState.m_fvSnakeBody[i])
				{
					this->m_GameState.m_fPoint.x = iXResult;
					this->m_GameState.m_fPoint.y = iYResult;
					LOGI("[+] %s. New point position x: %d, y: %d", __FUNCTION__, this->m_GameState.m_fPoint.x, this->m_GameState.m_fPoint.y);
				}
			}
		}
		else
		{
			this->m_GameState.m_fPoint.x = iXResult;
			this->m_GameState.m_fPoint.y = iYResult;
		}
	}
}

void CGame::SnakeLevelUp()
{
	this->m_GameState.m_flSnakeSpeed -= this->m_GameState.m_flSnakeSpeed / 40.f;
	this->m_GameState.m_fPoint.Set(field());
	this->m_GameState.m_iSnakePoints++;
}

void CGame::CleanupInput()
{
	memset(&this->m_InputController, 0, sizeof(input_controller));
}

GAME_TICK_COUNTER CGame::GetTickCount()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

bool CGame::IntersectionRect(vec2 pos, vec2 size, vec2 point)
{
	return point.x >= pos.x && point.x <= (pos.x + size.x) && point.y >= pos.y && point.y <= (pos.y + size.y);
}

void CGame::ClearGameState()
{
	LOGI("[+] %s. Clearing game state...", __FUNCTION__);
	this->m_GameState.m_fPoint.Set(field());
	this->m_GameState.m_fSnakeHead.Set(field(this->m_iFieldSize[0] / 2, this->m_iFieldSize[1] - 2));
	this->m_GameState.m_flSnakeSpeed = 800.f;
	this->m_GameState.m_iSnakePoints = 0;
	this->m_GameState.m_fSnakeHead.m_dDirection = UP;
}

void CGame::CalculateFieldSize()
{

}
