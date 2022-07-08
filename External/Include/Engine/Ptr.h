#pragma once

template<typename T>
class Ptr
{
private:
	T* m_pTarget;

public:
	T* Get()
	{
		return m_pTarget;
	}

public:
	T* operator ->()
	{
		return m_pTarget;
	}

	T* operator ->() const
	{
		return m_pTarget;
	}

	Ptr& operator = (T* _pOtherTarget)
	{
		if (nullptr != m_pTarget)
		{
			m_pTarget->SubRef();
		}

		m_pTarget = _pOtherTarget;

		if (nullptr != m_pTarget)
		{
			m_pTarget->AddRef();
		}

		return *this;
	}

	Ptr& operator = (Ptr<T> _pOther)
	{
		if (nullptr != m_pTarget)
		{
			m_pTarget->SubRef();
		}

		m_pTarget = _pOther.Get();

		if (nullptr != m_pTarget)
		{
			m_pTarget->AddRef();
		}

		return *this;
	}

	bool operator == (T* _pTarget)
	{
		if (m_pTarget == _pTarget)
			return true;
		return false;
	}

	bool operator != (T* _pTarget)
	{
		return !(*this) == _pTarget;
	}

	bool operator == (Ptr<T> _pTarget)
	{
		if (m_pTarget == _pTarget.Get())
			return true;
		return false;
	}

	bool operator != (Ptr<T> _pTarget)
	{
		return !(*this) == _pTarget;
	}
	

	/*operator T* ()
	{
		return m_pTarget;
	}*/

public:
	Ptr()
		: m_pTarget(nullptr)
	{
	}

	Ptr(T* _pTarget)
		: m_pTarget(_pTarget)
	{
		if(nullptr != m_pTarget)
			m_pTarget->AddRef();
	}

	Ptr(const Ptr& _origin)
		: m_pTarget(_origin.m_pTarget)
	{
		if (nullptr != m_pTarget)
			m_pTarget->AddRef();
	}


	~Ptr()
	{
		if (nullptr != m_pTarget)
			m_pTarget->SubRef();
	}
};


template<typename T>
bool operator == (void* _pTarget1, Ptr<T> _pTarget2)
{
	if (_pTarget1 == _pTarget2.Get())
		return true;
	return false;
}

template<typename T>
bool operator != (void* _pTarget1, Ptr<T> _pTarget2)
{
	if (_pTarget1 != _pTarget2.Get())
		return true;
	return false;
}