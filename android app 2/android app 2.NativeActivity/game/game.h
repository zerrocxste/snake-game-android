#pragma once

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "SnakeGame.NativeActivity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "SnakeGame.NativeActivity", __VA_ARGS__))

typedef unsigned long GAME_TICK_COUNTER;

class CGame
{
public:
	CGame(int field_width, int field_height, int screeen_width, int screen_height, void* p_input, void* p_onpressed);
	~CGame();

	void Run();
private:
	struct touch_coords
	{
		int x, y;
	};

	enum DIRECTION
	{
		NONE,
		DOWN,
		LEFT,
		RIGHT,
		UP
	};

	struct game_entity
	{
		game_entity() { x = y = 0; }
		game_entity(int x, int y) : x(x), y(y) {};
		void Set(game_entity a = game_entity())
		{
			x = a.x, y = a.y;
		}
		void Set(int X, int Y)
		{
			x = X, x = Y;
		}
		bool operator==(int* a)
		{
			return x == a[0] && y == a[1];
		}
		bool operator==(game_entity a)
		{
			return x == a.x && y == a.y;
		}
		bool operator!=(int* a)
		{
			return !operator==(a);
		}
		bool operator!=(game_entity a)
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

	struct field
	{
		void SetSize(int X, int Y)
		{
			x = X, y = Y;
		}
		bool operator!()
		{
			return !x && !y;
		}
		int x, y;
	};

	struct game_state
	{
		bool m_bStopGame;
		game_entity m_fSnakeHead;
		std::vector<game_entity> m_fvSnakeBody;
		game_entity m_fPoint;
		int m_iSnakeArrayLengh;
		int m_iSnakePoints;
		int m_iSnakeHistory;
		float m_flSnakeSpeed;
		GAME_TICK_COUNTER m_gtcGameTick;
		bool m_bUpdateSnake;
	};

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

	const vec2 m_iCellSize{ 30.f, 30.f };
	field m_iFieldSize;
	vec2 m_DisplaySize;
	void* m_pTouchCoords;
	void* m_pIsTouched;
	game_state m_GameState;
	input_controller m_InputController;

	void RunLogic();
	bool CheckGameData();
	void DrawInputs();
	void InputControlSnake();
	void DrawGame();
	bool FindIntersectionGamePoint();
	bool AddButton(vec2 pos, vec2 size, color col, bool activate_on_up = false);
	void SetPointPosition();
	void SnakeScoreUp();
	void CleanupInput();
	bool IntersectionRect(vec2 pos, vec2 size, vec2 point);
	void AddFieldSquare(vec2 pos, color col);
	bool FindingExitFromPlayingZone();
	bool FindingCollisionSnakeBody();
	void UpdateGameTick();
	void UpdateSnakeMovement();
	void SnakeHistoryCollector();
	void ClearGameState();
	bool GetIsTouched();
	touch_coords GetTouchCoords();
	game_state* GetGameState();
	input_controller* GetInputController();
	GAME_TICK_COUNTER GetTickCount();
};