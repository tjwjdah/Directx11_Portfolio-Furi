#pragma once


template<typename T>
class CSingleton
{
private:
	static T* m_pMgr;

public:
	static T* GetInst()
	{
		if (nullptr == m_pMgr)
		{
			m_pMgr = new T;
		}

		return m_pMgr;
	}

	static void Destroy()
	{
		if (nullptr != m_pMgr)
		{
			delete m_pMgr;
			m_pMgr = nullptr;
		}
	}

protected:
	CSingleton()
	{
		void(*pFunc)(void) = &Destroy;
		atexit(pFunc);
	}

	~CSingleton()
	{
	}
};


template<typename T>
T* CSingleton<T>::m_pMgr = nullptr;
