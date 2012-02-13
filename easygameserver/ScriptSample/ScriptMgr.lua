-- ScriptMgrһֱ�ǿձ�,���Ե���ScriptMgr[k]��ScriptMgr[k]=vʱ,�������MetaTable_Mgr��__index��__newindex
MetaTable_ScriptMgr = {}

if( ScriptMgrProxy == nil )then
	ScriptMgrProxy = { Tables = {} }
end

--���t�в��Ҳ���k,�ͻ����__index
MetaTable_ScriptMgr.__index = function( t, k )
	-- ���k�Ƿ�����Ϊ�ļ���
	return ScriptMgrProxy[k]
end

-- �ű��޷������Ԫ��,ֻ���ɳ��������,����������������洢�������
-- �������ʱ,��ӽ���PlayerMgr
-- �������ʱ,��PlayerMgr�Ƴ�
MetaTable_ScriptMgr.__newindex = function( t, k, v )
	if( k == "Tables" )then
		return
	end
	if( type(v) == "table" )then
		-- �����������v����setmetatable
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
		Log( "���ؽű�����:"..k )
		ScriptMgr[k] = {}
		LoadTable( ScriptMgr[k], "ScriptData/"..k..".tdb" )
	end
end

function ScriptMgr_SaveData()
	SaveTable( ScriptMgr["Tables"], "ScriptData/Tables.tdb" )
	local allTables = ScriptMgr["Tables"]
	for k,v in pairs(allTables) do
		Log( "����ű�����:"..k )
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
