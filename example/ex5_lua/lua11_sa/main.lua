
function Lua_Create()

	Lsys.ScriptFile( 1, "main.lua")

	Lsys.SetClearColor("0xFFFFFFFF")
	Lsys.ShowCursor(1)
	Lsys.ShowState(1)
	Lsys.ConsoleSet(0)

	return Lsys.CreateWindow(-1, -1, 480, 320, "GalicSoft. Inc / 2011-1", 0)
end


function Lua_Init()
	g_Font = {}
	g_Font[1] = Lfont.New("seoul_hangang_l08.ttf", 24)

	Lfont.Scale(g_Font[1], 1.0, 1.0);


	return 0
end


function Lua_Destroy()

	return 0
end


function Lua_FrameMove()
	return 0
end



function Lua_Render()

-- 영역(Rect)
--	(x,y)
--	+------+
--	|      |
--	|      |
--	|      |
--	+------+ (r,b)

	local x	-- 시작 x
	local y	-- 시작 y
	local r	-- 끝 r
	local b	-- 끝 b

	Lfont.Setup(g_Font[1], "안녕하세요 반갑습니다", 5, 50, "0xFF00FFFF")

	-- 영역은 4개의 값을 반환
	-- 인덱스가 -1이면 전체 영역을 반환하며
	-- 각 인덱스는 해당 영역을 반환
	-- 아주 큰 값 대략 10000을 넣으면 가장 끝에 있는 영역을 반환함

	local index = 2
	x, y, r, b = Lfont.Rect(g_Font[1], index)

	count = Lfont.Count(g_Font[1])
	idx = -1
	Lfont.Draw(g_Font[1])
	Lfont.Draw(g_Font[1], idx, x, y + 15, "0XFF0000FF")
	Lfont.Draw(g_Font[1], idx, r, y + 45, "0XFF0000FF")

	return 0
end

