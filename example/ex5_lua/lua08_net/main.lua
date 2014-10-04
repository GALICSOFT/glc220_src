--------------------------------------------------------------------------------
-- constant --------------------------------------------------------------------

-- Net event
NET_ERR			= -1		-- 0xFFFFFFFF: error or close
NET_OK			= 0			-- 0x00000000: Initialize completed or there is no progblem on network
NET_NONE		= 65535		-- 0x0000FFFF:
NET_PENDDING	= 1			-- 0x00000001: io processsing

NETIO_CONNECTED	= 16+1		-- 0x00000011: Connected to server message
NETIO_CLOSED	= 16+2		-- 0x00000012: socket closed message
NETIO_SEND		= 16+3		-- 0x00000013: sending completed message
NETIO_RECV		= 16+4		-- 0x00000014: receive message


-- game state and etc.
TRUE		= 1
FALSE		= 0
OK			= 0
FAIL		= -1


-- network
g_net =
{
	socket		= nil
	, proto		= "tcp"
	, cs_act	= "client"
	, io_mdl	= "select"
	, ip		= "172.17.11.161"
	, port		= "20000"
	, state		= NET_NONE
}


-- Window System
g_window ={
			x = -1, y = -1						-- center position -1, -1
			, w = 800, h = 480					-- dc size 800 * 480
			, cls ="GALICSOFT INC"				-- class name
			, full=0							-- screen window mode
			}

-- input
g_mouse		= {x=0, y=0, el=0, er=0, em=0}		-- mouse position(0,0), event(0,1,2)

-- font
g_font		= nil								-- test font instance


--------------------------------------------------------------------------------
-- setup the game system -------------------------------------------------------
function Lua_Create()

	Lsys.ScriptFile(1, "main.lua")				-- setup the lua script file

	Lsys.SetClearColor("0xFF006699")			-- clear color A, R, G, B
	Lsys.ShowCursor(1)							-- show? mouse cursor
	Lsys.ShowState(1)							-- show? system state
	Lsys.ConsoleSet(0)							-- console mode for d3d

	Lsys.CreateWindow(g_window.x, g_window.y	-- window position for pc
					, g_window.w, g_window.h	-- window dc size
					, g_window.cls				-- window class and title
					, g_window.full				-- full mode for d3d
					)

	return 0
end -- Lua_Create



--------------------------------------------------------------------------------
-- Initialize ... --------------------------------------------------------------
function Lua_Init()

	-- game font
	g_font = Lfont.New("default", 18)

	Lfont.Setup(g_font, " ", 10, 130, "0xFFFFFF00")


	local socket = Lnet.New(g_net.protocol, g_net.cs_act, g_net.io_mdl, g_net.ip, g_net.port)
	if 0 > socket then
		return -1
	end

	g_net.socket = socket
	g_net.state = Lnet.Connect(g_net)

	return 0
end -- Lua_Init



--------------------------------------------------------------------------------
-- Release ... -----------------------------------------------------------------
function Lua_Destroy()

	return 0
end -- Lua_Destroy



--------------------------------------------------------------------------------
-- Update data ... -------------------------------------------------------------
function Lua_FrameMove()

	local hr
	local val

	hr, val = Lnet.Recv()

	if     NETIO_CLOSED		== hr then
		print("socket closed ------------------------------------------\n")

	elseif NETIO_CONNECTED	== hr then
		print("Connection success -------------------------------------\n")

	elseif NETIO_SEND		== hr then
		print("Send message -------------------------------------------\n")

	elseif NETIO_RECV		== hr then
		print("Recv message -------------------------------------------\n")

	else
		print("Etc Message---------------------------------------------\n")

	end

	-- update the input
	g_mouse.x, g_mouse.y = Lin.MousePos()
	g_mouse.el, g_mouse.er, g_mouse.em = Lin.MouseEvnt()



	return 0
end -- Lua_FrameMove



--------------------------------------------------------------------------------
-- Rendering ... ---------------------------------------------------------------
function Lua_Render()

	-- draw font
	Lfont.Draw(g_font)

	return 0
end -- Lua_Render

