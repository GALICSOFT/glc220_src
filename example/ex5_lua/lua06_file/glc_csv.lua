

function CreateCsv()

	-- return csv table
	local csv =
	{
		field={}, types={}, data={},
	}


	function csv.FieldCount(self)
		if nil ~=  self.field then
			return #self.field
		end

		return 0
	end

	function csv.RecordCount(self)
		if nil ~=  self.data then
			return #self.data
		end

		return 0
	end



	function csv.Read(self, fileName, delimiter)

		local fp = Lfile.New(fileName, "r")

		if nil == fp then
			self.PrintLn(fileName .. ": file open Error--------------")
			return nil
		end


		--local pattern = "([^,^\t]+)"
		local pattern = "([^,]+)"

		local n=0
		local i=1

		local line = nil

		if nil ~= delimiter then
			pattern = "([" .. delimiter .. "]+)"
		end


		--self.PrintLn(pattern)

		-- read field name
		i=1
		line = Lfile.ReadLine(fp)
		for w in string.gmatch(line, pattern) do
			self.field[i] = tostring(w);	i = i + 1
		end

		--[[
		for i=1, #self.field do
			self.PrintLn(self.field[i])
		end
		self.PrintLn("--------------------------------------------")
		--]]



		-- read type list
		i=1
		line = Lfile.ReadLine(fp)
		for w in string.gmatch(line, pattern) do
			self.types[i] = tostring(w);	i = i + 1
		end

		--[[
		for i=1, #self.types do
			self.PrintLn(self.types[i])
		end
		self.PrintLn("--------------------------------------------")
		--]]


		-- read data
		n=0
		while true do
			rec = nil

			line = Lfile.ReadLine(fp)	-- 한 줄 읽기

			if nil == line then break end


			n = n + 1
			self.data[n] = {}

			i=1
			for w in string.gmatch(line, pattern) do

				local f		-- field name
				local t		-- type
				local v		-- final value

				f = self.field[i]
				t = self.types[i]

				if "small" == t or "big" == t  then
					v = tonumber(w)

				elseif "bool" == t then
					if w == "true" or w == "TRUE" then
						v = true
					else
						v = false
					end
				else
					v = w
				end


				self.data[n][f] = v
				i = i + 1

				--print(f .. "==>" ..  self.data[n][f] .. "\n")
			end

			--[[
			for i=1, #self.field do
				local f = self.field[i]
				print(f .. "==>" ..  self.data[n][f] .. "\n")
			end

			print("\n")

			--]]
		end

		Lfile.Release(fp)

		if 0 == n then
			return nil
		end


		return self
	end



	function csv.Save(self, fileName, delimiter)

		local fp = Lfile.New(fileName, "w")

		if nil == fp then
			return nil
		end
		
		local n=0
		local i=1

		local line = nil
		local pattern = ","

		if nil ~= delimiter then
			pattern = delimiter
		end


		-- write the field name
		local row = #self.data
		local col = #self.field

		for i=1, col-1 do
			Lfile.Write(fp, self.field[i] .. pattern)
		end

		Lfile.Write(fp, self.field[col] .. "\n")


		-- write the type
		for i=1, col-1 do
			Lfile.Write(fp, self.types[i] .. pattern)
		end

		Lfile.Write(fp, self.types[col] .. "\n")


		-- write data
		local f
		local v

		for n=1, row do
			for i=1, col-1 do

				f = self.field[i]
				v = self.data[n][f]

				Lfile.Write(fp, v .. pattern)
			end

			f = self.field[col]
			v = self.data[n][f]

			Lfile.Write(fp, v .. "\n")
		end

		Lfile.Release(fp)

		return self
	end


	-- data update
	function csv.Update(self, rec, _f, v)

		if #self.data < rec then
			self.PrintLn("Cann't find record-------------------------")
			return nil
		end

		local f = self.FindFieldName(self,_f)	--;self.PrintLn("Index: " .. f)

		if nil == f then return nil	end

		self.data[rec][f] = v					--;self.PrintLn("Updated data: " .. self.data[rec][f])

		return 0
	end


	-- data select
	function csv.Select(self, rec, _f)

		if #self.data < rec then
			self.PrintLn("Cann't find record")
			return nil
		end

		local f = self.FindFieldName(self,_f)	--;self.PrintLn("Field Name: " .. f)

		if nil == f then return nil	end

		return self.data[rec][f]
	end


	-- find field name
	function csv.FindFieldName(self, _f)

		if "string" == type(_f) then
			return _f
		end

		if #self.field < _f then
			self.PrintLn("Cann't find field Name")
			return nil
		end

		-- return the field name
		return self.field[_f]
	end


	function csv.PrintLn(self, str)
		print(str .. "\n")
	end


	return csv
end


