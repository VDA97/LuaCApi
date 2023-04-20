local TILE_EMPTY = 0
local TILE_BLOCK = 1


--calls the host application in c++ to construct the level in lua's behalf
function LoadlLevel(host,level)
	
	map = "" --string variable
	size = {w = 16, h = 15} --table or object

	--creates the level, also note that are others ways to perform this.
	--here we will graphically describe the level in text
	if level == 1 then 
		map =
		"................"..
		".....###........"..
		"................"..
		"................"..
		".........#######"..
		"............####"..
		"######.......###"..
		"##.............#"..
		"##.............#"..
		"##.............#"..
		"##.....###.....#"..
		"################"..
		"................"..
		"................"..
		"................"
	end

	--C++ host program has no notion about what is the level at this moment
	--So we will create a function in C++ and call it with the dimensions of our map
	--the prfix _ means will be calling a function in C++ host, its javidx9 convention
	_CreateLevel(host, size.w,size.h)
	--Allow lua to physically construct the level in our game engine, tile by tile, by using two nested for loops
	--iterates all the chars in map string,	
	for y = 1, size.h do
		for x = 1, size.w do
			c = string.sub(map, ((y-1) * size.w + x), ((y-1) * size.w + x))--extract the char from particular location

			if c == '.' then _SetTile(host ,x-1, y-1, TILE_EMPTY)--send Empty Tile Type, check enum class
			elseif c == '#' then _SetTile(host, x-1, y-1, TILE_BLOCK)--send Solid Tile Type
			end

		end
	end

	--calls and pass the type and the position in the GameWorld.
	-- _CreateDynamicObject will return an identifier 	lua_pushlightuserdata(L, object->createDynamicObject(type, x, y).get());
	-- This ptr will be stored to PlayerObject
	PlayerObject1 = _CreateDynamicObject(host,0,100.0,40.0)
	--_AssignPlayerControl(host,PlayerObject1)

	PlayerObject2 = _CreateDynamicObject(host,1,140.0,40.0)
	_AssignPlayerControl(host,PlayerObject2)

	return 1
end