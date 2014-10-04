

function ReadCsv(fileName)
	local fr = io.open (fileName,"r")

	local cs_lst ={}
	local n = 0

	while true do
		local line = fr:read("*line")	-- 한 줄 읽기

		if nil == line then break end

		local v ={}
		local i = 1

		for w in string.gmatch(line, "([^,^\t]+)") do
			v[i] = w; i = i + 1
		end

		--[[
		for i=1, #v do
			print(v[i])
			print(" ")
		end

		print("\n")
		v = nil
		--]]

		n = n + 1
		cs_lst[n] = v
	end

	fr:close()

	if 0 == n then
		return nil
	end

	return cs_lst

end





function DrawButton(tex, btn, x, y, over)
	local l; local t; local r; local b

	if nil ~= over then
		l = btn.x1
		t = btn.y1
		r = btn.w + l
		b = btn.h + t
	else
		l = btn.x0
		t = btn.y0
		r = btn.w + l
		b = btn.h + t
	end

	Ltex.Draw(tex, l, t, r, b, x, y)

end


function DrawCheck(tex, btn, lst_check, lst_rect)
	local n
	local x; local y; local w; local h
	local l; local t; local r; local b

	l = btn.x0
	t = btn.y0
	r = btn.w + l
	b = btn.h + t

	for n=1, #lst_check do
	
		local idx = lst_check[n]
		--print("idx: " .. idx .. "\n")

		local st_data = lst_rect[ idx ]

		x = st_data.x
		y = st_data.y
		w = st_data.w
		h = st_data.h

		x = x + w/2 - 32 + 400
		y = y + h/2 - 32

		Ltex.Draw(tex, l, t, r, b, x, y)
	end


end




