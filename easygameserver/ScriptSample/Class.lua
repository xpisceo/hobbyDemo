local _Core_ClassMgr = {} --������ļ���

--����:����һ������
--����baseClass:����
function NewClass( baseClass )
	local class_type={}
	class_type.ctor=false
	class_type.baseClass=baseClass
	--new
	class_type.new=function(...) 
			local obj={}
			--�ݹ���û���Ĺ��캯��
			do
				local create
				create = function(c,...)
					if c.baseClass then
						create(c.baseClass,...)
					end
					if c.ctor then
						c.ctor(obj,...)
					end
				end
				create(class_type,...)
			end
			setmetatable(obj,{ __index=_Core_ClassMgr[class_type] })
			return obj
		end -- end of class_type.new

	local proxyTable={}
	_Core_ClassMgr[class_type]=proxyTable

	--���������Ԫ��,���k�ڻ�����Ҳ��,����"����"
	setmetatable(class_type,{__newindex=
		function(t,k,v)
			proxyTable[k]=v
		end
	})

	if baseClass then
		--���Լ��ı����Ҳ���,�͵�����ı�����,����"�̳�"
		setmetatable(proxyTable,{__index=
			function(t,k)
				local ret=_Core_ClassMgr[baseClass][k] --����������
				proxyTable[k]=ret
				return ret
			end
		})
	end

	return class_type
end

--ʵ��
--[[
base_type=NewClass()		-- ����һ������ base_type

function base_type:ctor(x)	-- ���� base_type �Ĺ��캯��
	print("base_type ctor")
	self.x=x
end

function base_type:print_x()	-- ����һ����Ա���� base_type:print_x
	print(self.x)
end

function base_type:hello()	-- ������һ����Ա���� base_type:hello
	print("hello base_type")
end

test=NewClass(base_type)	-- ����һ���� test �̳��� base_type

function test:ctor()	-- ���� test �Ĺ��캯��
	print("test ctor")
end

function test:hello()	-- ���� base_type:hello Ϊ test:hello
	print("hello test")
end

Log( "New base_type:" )
a = base_type:new()
a:hello()

Log( "New test:" )
b = test:new()
b:hello()
]]--
