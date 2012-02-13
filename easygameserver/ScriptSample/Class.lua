local _Core_ClassMgr = {} --所有类的集合

--功能:生成一个新类
--参数baseClass:基类
function NewClass( baseClass )
	local class_type={}
	class_type.ctor=false
	class_type.baseClass=baseClass
	--new
	class_type.new=function(...) 
			local obj={}
			--递归调用基类的构造函数
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

	--往表里加新元素,如果k在基类里也有,就是"重载"
	setmetatable(class_type,{__newindex=
		function(t,k,v)
			proxyTable[k]=v
		end
	})

	if baseClass then
		--在自己的表里找不到,就到基类的表里找,就是"继承"
		setmetatable(proxyTable,{__index=
			function(t,k)
				local ret=_Core_ClassMgr[baseClass][k] --到基类里找
				proxyTable[k]=ret
				return ret
			end
		})
	end

	return class_type
end

--实例
--[[
base_type=NewClass()		-- 定义一个基类 base_type

function base_type:ctor(x)	-- 定义 base_type 的构造函数
	print("base_type ctor")
	self.x=x
end

function base_type:print_x()	-- 定义一个成员函数 base_type:print_x
	print(self.x)
end

function base_type:hello()	-- 定义另一个成员函数 base_type:hello
	print("hello base_type")
end

test=NewClass(base_type)	-- 定义一个类 test 继承于 base_type

function test:ctor()	-- 定义 test 的构造函数
	print("test ctor")
end

function test:hello()	-- 重载 base_type:hello 为 test:hello
	print("hello test")
end

Log( "New base_type:" )
a = base_type:new()
a:hello()

Log( "New test:" )
b = test:new()
b:hello()
]]--
