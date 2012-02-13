MetaTable_PlayerMgr = {}

if( PlayerMgrProxy == nil )then
	PlayerMgrProxy = {}
end

--���t�в��Ҳ���k,�ͻ����__index
MetaTable_PlayerMgr.__index = function( t, k )
	return PlayerMgrProxy[k]
end

-- �ű��޷������Ԫ��,ֻ���ɳ��������,����������������洢�������
-- �������ʱ,��ӽ���PlayerMgr
-- �������ʱ,��PlayerMgr�Ƴ�
MetaTable_PlayerMgr.__newindex = function( t, k, v )
	Log( "PlayerMgr is ReadOnly!!!!!!" )
end

-- PlayerMgrһֱ�ǿյ�
if( PlayerMgr == nil )then
	PlayerMgr = {}
end
setmetatable(PlayerMgr, MetaTable_PlayerMgr)

-- ��ȡ��ҵ�table,�������һ����
function GetPlayerTable( playerId )
	if( playerId == nil )then
		playerId = GetPlayerId() --û�в����Ļ�,��ȡ��ǰ��ҵ�
		if( playerId == nil )then return nil end
	end
	return PlayerMgr[playerId]
end