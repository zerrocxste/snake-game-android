#pragma once

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "SnakeGame.NativeActivity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "SnakeGame.NativeActivity", __VA_ARGS__))

typedef unsigned long GAME_TICK_COUNTER;

class CGame
{
public:
	CGame(int width, int height, void* p_input, void* p_onpressed);
	~CGame();

	void Run();
private:
	const int m_iFieldSize[2]{ 27, 40 };
	const vec2 m_iCellSize{ 30.f, 30.f };

	vec2 m_DisplaySize;

	void* m_pTouchCoords;
	void* m_pIsTouched;

	void CalculateFieldSize();
	void RunLogic();
	bool GetIsTouched();
	void DrawInputs();
	void InputControlSnake();
	void DrawGame();
	bool FindIntersectionGamePoint();
	bool AddButton(vec2 pos, vec2 size, color col, bool activate_on_up = false);
	void SetPointPosition();
	void SnakeLevelUp();
	void CleanupInput();
	GAME_TICK_COUNTER GetTickCount();
	bool IntersectionRect(vec2 pos, vec2 size, vec2 point);
	void AddFieldSquare(vec2 pos, color col);
	bool FindingExitFromPlayingZone();
	bool FindingCollisionSnakeBody();
	void UpdateGameTick();
	void UpdateSnakeMovement();
	void SnakeHistoryCollector();
	void ClearGameState();

	struct touch_coords
	{
		int x, y;
	};

	touch_coords GetTouchCoords();

	enum DIRECTION
	{
		NONE,
		DOWN,
		LEFT,
		RIGHT,
		UP
	};

	struct field
	{
		field() { x = y = 0; }
		field(int x, int y) : x(x), y(y) {};
		void Set(field a)
		{
			x = a.x, y = a.y;
		}
		void Set(int x1, int y1)
		{
			x = x1, x = y1;
		}
		bool operator==(int* a)
		{
			return x == a[0] && y == a[1];
		}
		bool operator==(field a)
		{
			return x == a.x && y == a.y;
		}
		bool operator!=(int* a)
		{
			return !operator==(a);
		}
		bool operator!=(field a)
		{
			return !operator==(a);
		}
		bool operator!()
		{
			return x == 0 && y == 0;
		}

		DIRECTION m_dDirection;
		int x, y;
	};

	struct game_state
	{
		bool m_bStopGame;
		field m_fSnakeHead;
		std::vector<field> m_fvSnakeBody;
		int m_iSnakeArrayLengh;
		int m_iSnakePoints;
		int m_iSnakeHistory;
		float m_flSnakeSpeed;
		GAME_TICK_COUNTER m_gtcGameTick;
		field m_fPoint;
		bool m_bUpdateSnake;
	};

	game_state m_GameState;

	struct input_controller
	{
		enum GAME_KEYS
		{
			DOWN,
			LEFT,
			RIGHT,
			UP,
			GAME_KEYS_MAX_SIZE
		};
		bool m_bIsPressed[GAME_KEYS_MAX_SIZE];
	};

	input_controller m_InputController;
};