


Lsys.DoFile("glc_csv.lua")

function Lua_Create()

	Lsys.ScriptFile( 1, "main.lua")
	Lsys.SetClearColor("0x0")
	Lsys.ShowCursor(1)
	Lsys.ShowState(0)
	Lsys.ConsoleSet(0)
	Lsys.CreateWindow(-1, -1, 480, 320, "GALICSOFT INC", 0)

	return 0
end



function Lua_Init()


	-- open the csv db
	g_tbl = CreateCsv()

	if nil == g_tbl then
		return 0
	end


	if nil == g_tbl:Read("tb_player_base_data.csv") then
		print("Read Error\n")
		return 0
	end


	local fld_cnt = g_tbl:FieldCount()
	local rec_cnt = g_tbl:RecordCount()


	g_tbl:PrintLn("record, field: " .. fld_cnt .. "  " .. rec_cnt)


	local data = g_tbl.data


	-- select test
	for i=1, rec_cnt do
		for j=1, fld_cnt do

			local f
			f= g_tbl:FindFieldName(j)		-- get the field name with index
			--f = j

			local v = g_tbl:Select(i, f)	-- get the data from table

			print(v .."\t")
		end

		g_tbl:PrintLn("")

	end

	-- update test
	g_tbl:Update(5, "player_forward_point", 10000000)
	g_tbl:Update(6, "player_forward_point", 10000000)
	g_tbl:Update(7, "player_forward_point", 10000000)
	g_tbl:Update(8, "player_forward_point", 10000000)
	g_tbl:Update(9, "player_forward_point", 10000000)

	g_tbl:Update( 9, 2, 10000000)
	g_tbl:Update(10, 2, 20000000)
	g_tbl:Update(11, 2, 30000000)
	g_tbl:Update(12, 2, 40000000)
	g_tbl:Update(13, 2, 50000000)
	g_tbl:Update(14, 2, 60000000)


	-- save test
	if nil == g_tbl:Save("save_tex.csv") then
		print("Save Error\n")
		return 0
	end


	--release
	g_tbl = nil

	return 0
end




function Lua_Destroy()
	return 0
end



function Lua_FrameMove()

	return 0
end


function Lua_Render()


	return 0
end
