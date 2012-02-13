MetaTable_PlayerMgr = {}

if( PlayerMgrProxy == nil )then
	PlayerMgrProxy = {}
end

--如果t中查找不到k,就会调用__index
MetaTable_PlayerMgr.__index = function( t, k )
	return PlayerMgrProxy[k]
end

-- 脚本无法添加新元素,只能由程序来添加,所以这个可以用来存储玩家数据
-- 玩家上线时,添加进表PlayerMgr
-- 玩家下线时,从PlayerMgr移除
MetaTable_PlayerMgr.__newindex = function( t, k, v )
	Log( "PlayerMgr is ReadOnly!!!!!!" )
end

-- PlayerMgr一直是空的
if( PlayerMgr == nil )then
	PlayerMgr = {}
end
setmetatable(PlayerMgr, MetaTable_PlayerMgr)

-- 获取玩家的table,在线玩家一定有
function GetPlayerTable( playerId )
	if( playerId == nil )then
		playerId = GetPlayerId() --没有参数的话,就取当前玩家的
		if( playerId == nil )then return nil end
	end
	return PlayerMgr[playerId]
end