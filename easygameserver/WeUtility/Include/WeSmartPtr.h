/********************************************************************
	created:	2010-1-4
	author:		Fish (于国平)
	svn:		https://easygameserver.svn.sourceforge.net/svnroot/easygameserver/
	purpose:	智能指针
*********************************************************************/
#pragma once

#include "WeConfig.h"

namespace We
{
	template <class T>
	class /*We_EXPORTS*/ SmartPtr
	{
	public:
		SmartPtr(T* pObject = (T*) 0);
		SmartPtr(const SmartPtr& ptr);
		~SmartPtr();

		// 隐式转换
		operator T*() const;
		T& operator*() const;
		T* operator->() const;

		// 赋值
		SmartPtr& operator=(const SmartPtr& ptr);
		SmartPtr& operator=(T* pObject);

		// 比较
		bool operator==(T* pObject) const;
		bool operator!=(T* pObject) const;
		bool operator==(const SmartPtr& ptr) const;
		bool operator!=(const SmartPtr& ptr) const;

	protected:
		T* m_pObject;
	};

	//---------------------------------------------------------------------------
	template <class T>
	inline SmartPtr<T>::SmartPtr(T* pObject)
	{
		m_pObject = pObject;
		if (m_pObject)
			m_pObject->IncRefCount();
	}
	//---------------------------------------------------------------------------
	template <class T>
	inline SmartPtr<T>::SmartPtr(const SmartPtr& ptr)
	{
		m_pObject = ptr.m_pObject;
		if (m_pObject)
			m_pObject->IncRefCount();
	}
	//---------------------------------------------------------------------------
	template <class T>
	inline SmartPtr<T>::~SmartPtr()
	{
		if (m_pObject)
			m_pObject->DecRefCount();
	}
	//---------------------------------------------------------------------------
	template <class T>
	inline SmartPtr<T>::operator T*() const
	{
		return m_pObject;
	}
	//---------------------------------------------------------------------------
	template <class T>
	inline T& SmartPtr<T>::operator*() const
	{
		return *m_pObject;
	}
	//---------------------------------------------------------------------------
	template <class T>
	inline T* SmartPtr<T>::operator->() const
	{
		return m_pObject;
	}
	//---------------------------------------------------------------------------
	template <class T>
	inline SmartPtr<T>& SmartPtr<T>::operator=(const SmartPtr& ptr)
	{
		if (m_pObject != ptr.m_pObject)
		{
			if (m_pObject)
				m_pObject->DecRefCount();
			m_pObject = ptr.m_pObject;
			if (m_pObject)
				m_pObject->IncRefCount();
		}
		return *this;
	}
	//---------------------------------------------------------------------------
	template <class T>
	inline SmartPtr<T>& SmartPtr<T>::operator=(T* pObject)
	{
		if (m_pObject != pObject)
		{
			if (m_pObject)
				m_pObject->DecRefCount();
			m_pObject = pObject;
			if (m_pObject)
				m_pObject->IncRefCount();
		}
		return *this;
	}
	//---------------------------------------------------------------------------
	template <class T>
	inline bool SmartPtr<T>::operator==(T* pObject) const
	{
		return (m_pObject == pObject);
	}
	//---------------------------------------------------------------------------
	template <class T>
	inline bool SmartPtr<T>::operator!=(T* pObject) const
	{
		return (m_pObject != pObject);
	}
	//---------------------------------------------------------------------------
	template <class T>
	inline bool SmartPtr<T>::operator==(const SmartPtr& ptr) const
	{
		return (m_pObject == ptr.m_pObject);
	}
	//---------------------------------------------------------------------------
	template <class T>
	inline bool SmartPtr<T>::operator!=(const SmartPtr& ptr) const
	{
		return (m_pObject != ptr.m_pObject);
	}
	//---------------------------------------------------------------------------
}
