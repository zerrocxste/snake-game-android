#include "game.h"

CGame::CGame(int field_width, int field_height, int screeen_width, int screen_height, void* p_input, void* p_onpressed)
{
	srand(time(0));

	this->m_iFieldSize.x = field_width;
	this->m_iFieldSize.y = field_height;

	this->m_DisplaySize.x = screeen_width;
	this->m_DisplaySize.y = screen_height;

	auto gs = GetGameState();

	gs->m_fSnakeHead.Set(game_entity(this->m_iFieldSize.x / 2, this->m_iFieldSize.y - 1));
	gs->m_flSnakeSpeed = 800.f;
	gs->m_fSnakeHead.m_dDirection = UP;

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
	if (!CheckGameData())
		return;

	DrawInputs();

	RunLogic();

	DrawGame();
}

bool CGame::CheckGameData()
{
	if (!this->m_iFieldSize)
		return false;

	return true;
}

void CGame::RunLogic()
{
	InputControlSnake();
	UpdateGameTick();
	UpdateSnakeMovement();
	SnakeHistoryCollector();
	SetPointPosition();

	if (FindingExitFromPlayingZone() || FindingCollisionSnakeBody())
		ClearGameState();

	if (FindIntersectionGamePoint())
		SnakeScoreUp();
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
	auto gs = GetGameState();
	auto ic = GetInputController();

	/*if (GetIsTouched())
		LOGI("[+] %s. %d, %d", __FUNCTION__, GetTouchCoords().x, GetTouchCoords().y);*/

	CleanupInput();

	auto vButtonSize = vec2(250.f, 250.f);

	auto iScreenCenterX = (this->m_DisplaySize.x / 2.f) - (vButtonSize.x / 2.f);
	auto iScreenCenterY = (this->m_DisplaySize.y / 2.f) + 80.f;

	if (AddButton(vec2(iScreenCenterX, iScreenCenterY + 200.f), vButtonSize, color(1.f, 0.f, 0.f, 1.f)))
		ic->m_bIsPressed[input_controller::GAME_KEYS::UP] = true;

	if (AddButton(vec2(iScreenCenterX, iScreenCenterY + 600.f), vButtonSize, color(1.f, 0.f, 0.f, 1.f)))
		ic->m_bIsPressed[input_controller::GAME_KEYS::DOWN] = true;

	if (AddButton(vec2(iScreenCenterX - 300.f, iScreenCenterY + 400.f), vButtonSize, color(1.f, 0.f, 0.f, 1.f)))
		ic->m_bIsPressed[input_controller::GAME_KEYS::LEFT] = true;

	if (AddButton(vec2(iScreenCenterX + 300.f, iScreenCenterY + 400.f), vButtonSize, color(1.f, 0.f, 0.f, 1.f)))
		ic->m_bIsPressed[input_controller::GAME_KEYS::RIGHT] = true;

	if (AddButton(vec2(70.f, this->m_DisplaySize.y - 250.f), vec2(200.f, 200.f), gs->m_bStopGame ? color(1.f, 1.f, 0.5f, 1.f) : color(0.5f, 0.5f, 1.f, 1.f), true))
		gs->m_bStopGame = !gs->m_bStopGame;
}

void CGame::InputControlSnake()
{
	auto gs = GetGameState();
	auto ic = GetInputController();

	for (auto i = 0; i < input_controller::GAME_KEYS::GAME_KEYS_MAX_SIZE; i++)
	{
		if (ic->m_bIsPressed[i])
		{
			auto iCorrectXDirectionNoInverse = 0;
			auto iCorrectYDirectionNoInverse = 0;

			switch (i)
			{
			case input_controller::GAME_KEYS::DOWN:
				iCorrectYDirectionNoInverse = 1;
				break;
			case input_controller::GAME_KEYS::LEFT:
				iCorrectXDirectionNoInverse = -1;
				break;
			case input_controller::GAME_KEYS::RIGHT:
				iCorrectXDirectionNoInverse = 1;
				break;
			case input_controller::GAME_KEYS::UP:
				iCorrectYDirectionNoInverse = -1;
				break;
			}

			if (!(game_entity(gs->m_fSnakeHead.x + iCorrectXDirectionNoInverse, gs->m_fSnakeHead.y + iCorrectYDirectionNoInverse) != gs->m_fvSnakeBody[1]))
				continue;

			gs->m_fSnakeHead.m_dDirection = (DIRECTION)(i + 1);
		}
	}
}

void CGame::DrawGame()
{
	auto gs = GetGameState();

	auto iScreenSpaceGameFieldX = (this->m_DisplaySize.x / 2.f) - ((this->m_iCellSize.x * this->m_iFieldSize.x) / 2.f);

	for (int x = 1; x < this->m_iFieldSize.x; ++x)
	{
		for (int y = 1; y < this->m_iFieldSize.y; ++y)
		{
			vec2 pos((m_iCellSize.x * x) + iScreenSpaceGameFieldX, (m_iCellSize.y * y) + 100.f);

			color col(0.f, 0.7f, 0.5f, 1.f);

			if (game_entity(x, y) == gs->m_fPoint)
				col = color(0.f, 0.5f, 0.7f, 1.f); //point

			AddFieldSquare(pos, col);

			if (game_entity(x, y) == gs->m_fSnakeHead)
				AddFieldSquare(pos, color(1.f, 0.5f, 0.f, 1.f));

			for (auto i = 0; i < gs->m_iSnakePoints; i++)
			{
				if (game_entity(x, y) == gs->m_fvSnakeBody.at(i + 1))
					AddFieldSquare(pos, color(1.f, 0.65f, 0.f, 1.f));
			}
		}
	}
}

bool CGame::FindIntersectionGamePoint()
{
	auto gs = GetGameState();

	auto ret = gs->m_fSnakeHead == gs->m_fPoint;

	if (ret)
		LOGI("[+] %s. Snake intersection a point", __FUNCTION__);

	return ret;
}

bool CGame::AddButton(vec2 pos, vec2 size, color col, bool activate_on_up)
{
	drawing::box(pos.x, pos.y, size.x, col);

	auto bIsTouched = GetIsTouched();
	auto tcTouchCoords = GetTouchCoords();

	bool bIsPressed = true;

	if (activate_on_up)
	{
		bIsPressed = false;

		static bool bIsDown = false;
		static GAME_TICK_COUNTER gtcUpdateButton = GetTickCount();

		if (bIsTouched)
		{
			if (!bIsDown && (GetTickCount() - gtcUpdateButton > 300))
			{
				bIsPressed = true;
				bIsDown = true;
				gtcUpdateButton = GetTickCount();
			}
		}
		else
			bIsDown = false;
	}

	return IntersectionRect(pos, size, vec2(tcTouchCoords.x, tcTouchCoords.y)) && bIsTouched && bIsPressed;
}

void CGame::AddFieldSquare(vec2 pos, color col)
{
	drawing::box(pos.x, pos.y, m_iCellSize.y - 5.f, col);
}

bool CGame::FindingExitFromPlayingZone()
{
	auto gs = GetGameState();

	auto ret = gs->m_fSnakeHead.x > (this->m_iFieldSize.x - 1) || gs->m_fSnakeHead.x < 1
		|| gs->m_fSnakeHead.y >(this->m_iFieldSize.y - 1) || gs->m_fSnakeHead.y < 1;

	if (ret)
		LOGI("[+] %s. Snake exit from game zone", __FUNCTION__);

	return ret;
}

bool CGame::FindingCollisionSnakeBody()
{
	auto gs = GetGameState();

	for (int i = 0; i < gs->m_iSnakePoints; i++)
	{
		if (gs->m_fSnakeHead == gs->m_fvSnakeBody[i + 1])
		{
			LOGI("[+] %s. Snake intersect yourself", __FUNCTION__);
			return true;
		}
	}
	return false;
}

void CGame::UpdateGameTick()
{
	auto gs = GetGameState();

	if (!gs->m_bStopGame)
	{
		if (gs->m_bUpdateSnake && GetTickCount() - gs->m_gtcGameTick > gs->m_flSnakeSpeed)
			gs->m_bUpdateSnake = false;
		else if (!gs->m_bUpdateSnake)
			gs->m_bUpdateSnake = true;
	}
}

void CGame::UpdateSnakeMovement()
{
	auto gs = GetGameState();

	if (!gs->m_bStopGame)
	{
		if (!gs->m_bUpdateSnake)
		{
			if (gs->m_fSnakeHead.m_dDirection == UP) 
				gs->m_fSnakeHead.y -= 1;
			else if (gs->m_fSnakeHead.m_dDirection == DOWN)
				gs->m_fSnakeHead.y += 1;
			else if (gs->m_fSnakeHead.m_dDirection == LEFT)
				gs->m_fSnakeHead.x -= 1;
			else if (gs->m_fSnakeHead.m_dDirection == RIGHT)
				gs->m_fSnakeHead.x += 1;

			gs->m_iSnakeHistory++;
			gs->m_gtcGameTick = GetTickCount();
		}
	}
}

void CGame::SnakeHistoryCollector()
{
	auto gs = GetGameState();

	if (!gs->m_bUpdateSnake)
	{
		std::vector<game_entity>temp;
		temp = gs->m_fvSnakeBody;
		std::reverse(std::begin(temp), std::end(temp));
		temp.push_back(gs->m_fSnakeHead);
		std::reverse(std::begin(temp), std::end(temp));
		gs->m_fvSnakeBody = temp;
	}
}

void CGame::SetPointPosition()
{
	auto gs = GetGameState();

	if (!gs->m_fPoint)
	{
		int iXResult = rand() % (this->m_iFieldSize.x - 1) + 1;
		int iYResult = rand() % (this->m_iFieldSize.y - 1) + 1;
		if (gs->m_iSnakePoints > 0)
		{
			for (int i = 0; i < gs->m_iSnakePoints; i++)
			{
				if (game_entity(iXResult, iYResult) != gs->m_fvSnakeBody[i])
				{
					gs->m_fPoint.x = iXResult;
					gs->m_fPoint.y = iYResult;
					LOGI("[+] %s. New point position x: %d, y: %d", __FUNCTION__, gs->m_fPoint.x, gs->m_fPoint.y);
				}
			}
		}
		else
		{
			gs->m_fPoint.x = iXResult;
			gs->m_fPoint.y = iYResult;
		}
	}
}

void CGame::SnakeScoreUp()
{
	auto gs = GetGameState();

	gs->m_flSnakeSpeed -= gs->m_flSnakeSpeed / 40.f;
	gs->m_fPoint.Set();
	gs->m_iSnakePoints++;
}

void CGame::CleanupInput()
{
	memset(GetInputController(), 0, sizeof(input_controller));
}

bool CGame::IntersectionRect(vec2 pos, vec2 size, vec2 point)
{
	return point.x >= pos.x && point.x <= (pos.x + size.x) && point.y >= pos.y && point.y <= (pos.y + size.y);
}

void CGame::ClearGameState()
{
	auto gs = GetGameState();

	LOGI("[+] %s. Clearing game state...", __FUNCTION__);

	gs->m_fPoint.Set();
	gs->m_fSnakeHead.Set(game_entity(this->m_iFieldSize.x / 2, this->m_iFieldSize.y - 2));
	gs->m_flSnakeSpeed = 800.f;
	gs->m_iSnakePoints = 0;
	gs->m_fvSnakeBody.clear();
	gs->m_fSnakeHead.m_dDirection = UP;
}

CGame::game_state* CGame::GetGameState()
{
	return &this->m_GameState;
}

CGame::input_controller* CGame::GetInputController()
{
	return &this->m_InputController;
}

GAME_TICK_COUNTER CGame::GetTickCount()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}
