#include "ngpch.h"

#include "SubsystemManager.h"

namespace Ngin {
	void SubsystemManager::InitAll()
	{
		for (auto& s : m_Subsystems)
		{
			s->OnInit();
		}
	}

	void SubsystemManager::ShutdownAll()
	{
		for (auto it = m_Subsystems.rbegin(); it != m_Subsystems.rend(); ++it)
		{
			(*it)->OnShutdown();
		}
	}

	void SubsystemManager::TickAll(float deltaTime)
	{
		for (auto& s : m_Subsystems)
		{
			s->Tick(deltaTime);
		}
	}

	void SubsystemManager::OnEvent(Event& event)
	{
		for (auto& s : m_Subsystems)
		{
			s->OnEvent(event);
		}
	}
}
