-------------------------------------------------------------------------------
--
-- Simple Snake Game
-- Heesung Oh(afewhee@gmail.com)
-- 2012-07-20
--


-- constant -------------------------------------------------------------------

-- direction index
DIR_L		= 1		-- left
DIR_T		= 2		-- top
DIR_R		= 3		-- right
DIR_B		= 4		-- bottom

-- direction vector
MOVE_DIR= {}
MOVE_DIR[DIR_L] = {x=-1, y= 0}
MOVE_DIR[DIR_T] = {x= 0, y=-1}
MOVE_DIR[DIR_R] = {x= 1, y= 0}
MOVE_DIR[DIR_B] = {x= 0, y= 1}

-- Key event
KEY_DOWN	= 1
KEY_UP		= 2
KEY_PRESS	= 3

-- game state and etc....
TRUE		= 1
FALSE		= 0
FAIL		= -1


-- game data -----------------------------------------------------------------

-- snake info
g_snakeI= {x= 32*7, y=32*10, dir= DIR_R, w= 32, delta = 329}
g_snake = {	}					-- snake body

g_item = {x=-1, y=-1, type= 0}	-- Item: type= 0-disable 1-orange 2-grape

g_timeCur	= 0					-- current time
g_timeBgn	= 0					-- begin time
g_score		= 0					-- game g_score
g_state		= 0					-- 0: none 1: active, -1 fail

g_timer		= nil

g_nBlcW = 14					-- 길이 방향 block 수
g_nBlcH = 20					-- 높이 방향 block 수


-- rendering and system data --------------------------------------------------

g_window ={x = -1, y = 10, w = 480, h = 800
			, cls ="GALICSOFT INC", full = 0 }

-- mouse
g_mouse_x	= 0
g_mouse_y	= 0
g_mouse_e	= 0

-- texture list
g_txSnake	= nil
g_txArraw	= nil

-- font list
g_fntScore = nil
g_fntMsg   = nil



-- setup the game system -------------------------------------------------------
function Lua_Create()

	Lsys.ScriptFile( 1, "main.lua")
	Lsys.SetClearColor("0xFF003311")
	Lsys.ShowCursor(1)
	Lsys.ShowState(0)
	Lsys.ConsoleSet(0)
	Lsys.CreateWindow(g_window.x, g_window.y, g_window.w, g_window.h
						, g_window.cls, g_window.full)

	return 0
end



-- Initialize ... --------------------------------------------------------------
function Lua_Init()


	InitGameData()


	-- game font
	-- score
	g_fntScore = Lfont.New("default", 24)
	Lfont.Setup(g_fntScore, "점수: 0", 280, 720, "0xFF00FFFF")

	-- message
	g_fntMsg = Lfont.New("default", 32)
	Lfont.Setup(g_fntMsg, "Game Over", 100, 230, "0xFFFF3300")

	-- game texture
	g_txSnake	= Ltex.New("texture/snake.png")
	g_txArraw	= Ltex.New("texture/arraw.png")


	return 0
end



-- Release ... -----------------------------------------------------------------
function Lua_Destroy()

	return 0
end



-- Update data ... -------------------------------------------------------------
function Lua_FrameMove()

	if FAIL == g_state then
		return 0
	end

	local i
	local timerEvent = FAIL

	-- update the input
	g_mouse_x, g_mouse_y = Lin.MousePos()
	g_mouse_e = Lin.MouseEvnt()


	-- update timer
	timerEvent = UpdateTimer(g_timer)


	-- update the current time
	g_timeCur = Lsys.GetTime()


	-- event notify the mouse
	if KEY_DOWN == g_mouse_e then

		local dir = 0

		if 0 < IsInRect(g_mouse_x, g_mouse_y, 64, 736, 64, 64) then dir = DIR_L end
		if 0 < IsInRect(g_mouse_x, g_mouse_y,128, 672, 64, 64) then dir = DIR_T end
		if 0 < IsInRect(g_mouse_x, g_mouse_y,128, 736, 64, 64) then	dir = DIR_B	end
		if 0 < IsInRect(g_mouse_x, g_mouse_y,192, 736, 64, 64) then	dir = DIR_R	end

		if 0 < dir then
			print("before: " .. g_snakeI.dir .. "\t")

			-- direction test
			local tmp_x = g_snake[1].x + MOVE_DIR[dir].x * g_snakeI.w
			local tmp_y = g_snake[1].y + MOVE_DIR[dir].y * g_snakeI.w

			if  not(math.abs(g_snake[2].x - tmp_x) < 4 and
					math.abs(g_snake[2].y - tmp_y) < 4      ) then
				g_snakeI.dir = dir
			end
			print("pos: " .. tmp_x .. ", " .. tmp_y .. "  :: " .. g_snake[2].x .. ", " .. g_snake[2].y .. "\n")
			print("new: " .. g_snakeI.dir .. "\n")

		end
		
	end



	if g_timeCur - g_timeBgn > g_snakeI.delta then
		g_timeBgn  = g_timeCur

		-- new position of head
		local tmp_x = g_snake[1].x + MOVE_DIR[g_snakeI.dir].x * g_snakeI.w
		local tmp_y = g_snake[1].y + MOVE_DIR[g_snakeI.dir].y * g_snakeI.w


		-- test the collsion to area
		if	g_snakeI.w > tmp_x  or tmp_x > g_nBlcW * (g_snakeI.w - 0.99) or
			g_snakeI.w > tmp_y  or tmp_y > g_nBlcH * (g_snakeI.w - 0.99) then
			g_state = FAIL
			return 0
		end


		-- test the collision to body
		for i=2, #g_snake do

			if  math.abs(g_snake[i].x - tmp_x) < 4 and
				math.abs(g_snake[i].y - tmp_y) < 4 then
				g_state = FAIL

				print("collision Idx: " .. i .. "\n")
				break
			end
		end


		-- get the item
		if 0 < g_item.type and
			math.abs(tmp_x - g_item.x) < 4 and
			math.abs(tmp_y - g_item.y) < 4 then

			-- increase the snake tail
			g_snake[#g_snake+1] ={x=0, y=0}

			-- item type to none
			g_item.type = 0

			g_score = g_score + 30
			Lfont.String(g_fntScore, "점수: " .. g_score)
		end


		-- copy the position from tail to head
		for i = #g_snake, 2, -1 do
			g_snake[i].x = g_snake[i-1].x
			g_snake[i].y = g_snake[i-1].y
		end

		-- update the snake head
		g_snake[1].x = tmp_x
		g_snake[1].y = tmp_y
	end


	-- generate the item
	if TRUE == timerEvent then
		GenerateItem()
	end

	return 0
end



-- Rendering ... ---------------------------------------------------------------
function Lua_Render()

	local i

	-- draw the back ground
	for i = 0, g_nBlcW do
		Ltex.Draw( g_txSnake,  96, 32, 128, 64, i* 32, 0)
		Ltex.Draw( g_txSnake,  96, 32, 128, 64, i* 32, 32*20)
	end

	for i = 0, g_nBlcH do
		Ltex.Draw( g_txSnake,  96, 32, 128, 64, 0, i* 32)
		Ltex.Draw( g_txSnake,  96, 32, 128, 64, g_nBlcW* 32, i* 32)
	end


	-- draw the snake body
	for i = 2, #g_snake do	Ltex.Draw( g_txSnake,  0,  32, 32, 64, g_snake[i].x, g_snake[i].y)	end

	-- draw the snake head
	if DIR_L == g_snakeI.dir then	Ltex.Draw( g_txSnake,  0, 0, 32, 32, g_snake[1].x, g_snake[1].y)	end
	if DIR_T == g_snakeI.dir then	Ltex.Draw( g_txSnake, 32, 0, 64, 32, g_snake[1].x, g_snake[1].y)	end
	if DIR_R == g_snakeI.dir then	Ltex.Draw( g_txSnake, 64, 0, 96, 32, g_snake[1].x, g_snake[1].y)	end
	if DIR_B == g_snakeI.dir then	Ltex.Draw( g_txSnake, 96, 0,128, 32, g_snake[1].x, g_snake[1].y)	end

	-- draw the item
	if 0 < g_item.type then
		Ltex.Draw( g_txSnake,  g_item.type*32, 32, (g_item.type+1)*32, 64, g_item.x, g_item.y)
	end
	
	
	--draw the arraw (left top right bottom)
	Ltex.Draw(g_txArraw, 64, 0,128, 64, 128, 672)
	Ltex.Draw(g_txArraw,  0, 0, 64, 64,  64, 736)
	Ltex.Draw(g_txArraw,192, 0,256, 64, 128, 736)
	Ltex.Draw(g_txArraw,128, 0,192, 64, 192, 736)


	-- draw score
	Lfont.Draw(g_fntScore)


	if FAIL == g_state then
		Lfont.Draw(g_fntMsg)
	end
	
	return 0
end



-- Generate Item --------------------------------------------------------------

function GenerateItem()

	if 0 < g_item.type then
		return
	end

	local x = 0
	local y = 0
	local i = 1

	local bcol = 1

	while true do

		bcol = false
		x = Lutil.Rand(2, 9) * 32
		y = Lutil.Rand(2, 9) * 32

		for i=1, #g_snake do

			if  math.abs(g_snake[i].x - x) < 4 and
				math.abs(g_snake[i].y - y) < 4 then
				bcol = true
				break
			end
		end

		if false == bcol then break end

	end


	g_item.x = x; g_item.y = y

	g_item.type = Lutil.Rand(1, 2)

	print("Generate item:" .. g_item.type .. "  pos(" .. g_item.x .. "," .. g_item.type .. ")\n")
end


-- Initialize the game data ---------------------------------------------------

function InitGameData()
	g_score		= 0		-- game g_score
	g_state		= 1

	-- initialize the snake head and tail
	g_snake[1] = {x = g_snakeI.x               , y = g_snakeI.y  }
	g_snake[2] = {x = g_snake[1].x - g_snakeI.w, y = g_snake[1].y}

	-- initialize the time variables to current time
	g_timeCur  = Lsys.GetTime()
	g_timeBgn  = g_timeCur

	g_timer = NewTimer(3000)

	StartTimer(g_timer)

end

-- check the mouse position in rect -------------------------------------------

function IsInRect(x, y, left, top, width, height)
	if left < x and x < (left+width) and top < y and y < (top+height) then
		return 1
	end

	return -1
end


-- simple timer ---------------------------------------------------------------

function NewTimer(_elapse, _count, _delay, _begin)

	local _tm ={elapse= 0, count = -1, delay = 0, begin = 0 }

	if nil ~= _elapse then _tm.elapse = _elapse end
	if nil ~= _count  then _tm.count  = _count end
	if nil ~= _delay  then _tm.delay  = _delay end
	if nil ~= _begin  then _tm.begin  = _begin end

	print( "elapse:" .. _tm.elapse
		.. " count:" .. _tm.count
		.. " delay:" .. _tm.delay
		.. " begin:" .. _tm.begin .. "\n")

	return _tm
end


function StartTimer(_tm)
	_tm.begin = Lsys.GetTime() + _tm.delay
end


function SetTimer(_tm, _count)
	_tm.count = _count
end


function UpdateTimer(_tm)

	if 0 == _tm.count then
		return FAIL
	end

	local curTime = Lsys.GetTime()

	if curTime > (_tm.begin + _tm.elapse) then
		_tm.begin = curTime

		if -1 < _tm.count then
			_count = _count -1
		end

		return TRUE
	end

	return FALSE
end
