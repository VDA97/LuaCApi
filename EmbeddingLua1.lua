--a = 7 + 22 + math.sin(23.7)
--a = 50 + a;

--Using lua as a parameters file

--PlayerTitle = "Squire"
--PlayerName = "PiraCrazy"
--PlayerFamily = "Almeida"
--PlayerLevel = 26


--Lua Global object player, also know as lua table.

players = {}
players[1] = {Title = "Squire", Name = "Victor", Family = "Almeida", Level = 26}
players[2] = {Title = "Squire", Name = "Vinicius", Family = "Almeida", Level = 16}


--let's create a function

function AddStuff(a,b)
	print("[LUA] AddStuff("..a..","..b..") called \n")
	return a*b
end

function GetPlayer(n)
 return players[n]
end

function DoAThing(a,b)
	print("[LUA] - DoAThing("..a..","..b..") called\n")

	--calling a cpp function

	c = HostFunction(a+10,b*15)

	return c
end



time = {year = "",month="",day="",hour = "",minute="",second = ""}


function EditTime(y,m,d,h,mt,s)
	print("[LUA] PutTime, year: "..time.year.." Default\n")
	print("[LUA] PutTime, year: "..time.month.." Default\n")
	print("[LUA] PutTime, year: "..time.day.." Default\n")
	print("[LUA] PutTime, year: "..time.hour.." Default\n")
	print("[LUA] PutTime, year: "..time.minute.." Default\n")
	print("[LUA] PutTime, year: "..time.second.." Default\n")

	time.year = y
	time.month = m
	time.day = d
	time.hour = h
	time.minute = mt
	time.second = s
	print("[LUA] PutTime, year: "..time.year.." Edited\n")
	print("[LUA] PutTime, month: "..time.month.." Default\n")
	print("[LUA] PutTime, day: "..time.day.." Default\n")
	print("[LUA] PutTime, hour: "..time.hour.." Default\n")
	print("[LUA] PutTime, minute: "..time.minute.." Default\n")
	print("[LUA] PutTime, second: "..time.second.." Default\n")

end