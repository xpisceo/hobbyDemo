-- ScriptMgr一直是空表,所以调用ScriptMgr[k]和ScriptMgr[k]=v时,都会调用MetaTable_Mgr的__index和__newindex
MetaTable_ScriptMgr = {}

if( ScriptMgrProxy == nil )then
	ScriptMgrProxy = { Tables = {} }
end

--如果t中查找不到k,就会调用__index
MetaTable_ScriptMgr.__index = function( t, k )
	-- 检查k是否能作为文件名
	return ScriptMgrProxy[k]
end

-- 脚本无法添加新元素,只能由程序来添加,所以这个可以用来存储玩家数据
-- 玩家上线时,添加进表PlayerMgr
-- 玩家下线时,从PlayerMgr移除
MetaTable_ScriptMgr.__newindex = function( t, k, v )
	if( k == "Tables" )then
		return
	end
	if( type(v) == "table" )then
		-- 可以在这里对v进行setmetatable
	end
	rawset( ScriptMgrProxy, k, v )
	if( type(v) == "table" )then
		local allTables = ScriptMgr["Tables"]
		allTables[k] = 1
	end
end

if( ScriptMgr == nil )then
	ScriptMgr = {}
end
setmetatable(ScriptMgr,MetaTable_ScriptMgr)

function ScriptMgr_LoadData()
	LoadTable( ScriptMgr["Tables"], "ScriptData/Tables.tdb" )
	local allTables = ScriptMgr["Tables"]
	for k,v in pairs(allTables) do
		Log( "加载脚本数据:"..k )
		ScriptMgr[k] = {}
		LoadTable( ScriptMgr[k], "ScriptData/"..k..".tdb" )
	end
end

function ScriptMgr_SaveData()
	SaveTable( ScriptMgr["Tables"], "ScriptData/Tables.tdb" )
	local allTables = ScriptMgr["Tables"]
	for k,v in pairs(allTables) do
		Log( "保存脚本数据:"..k )
		SaveTable( ScriptMgr[k], "ScriptData/"..k..".tdb" )
	end
end

function ScriptMgr_PrintTable( t, space )
	if( space == nil ) then space = "" end
	if( t == nil )then
		Log( space.."nil" )
		return
	end
	if( type(t) ~= "table" )then
			Log( space.."NOT a table" )
		return
	end
	Log( space.."{" )
	for k,v in pairs(t) do
		if( type(v) ~= "table" )then
			if( type(v) == "string" )then
				if( type(k) == "number" )then
					Log( space.."["..k.."]='"..tostring(v).."'" )
				else
					Log( space..k.."='"..tostring(v).."'" )
				end
			else
				if( type(k) == "number" )then
					Log( space.."["..k.."]="..tostring(v) )
				else
					Log( space..k.."="..tostring(v) )
				end
			end
		else
			if( type(k) == "number" )then
				Log( space.."["..k.."]=" )
			else
				Log( space..k.."=" )
			end
			spaceNext = space.."  "
			ScriptMgr_PrintTable( v, spaceNext )
		end
	end
	Log( space.."}" )
end
