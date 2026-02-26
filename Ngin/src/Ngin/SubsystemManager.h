#pragma once

#include "Core.h"
#include "Subsystem.h"

#include <typeindex>
#include <vector>
#include <unordered_map>
#include <memory>

namespace Ngin {

	class NG_API SubsystemManager
	{
	public:
		SubsystemManager() = default;
		SubsystemManager(const SubsystemManager&) = delete;
		SubsystemManager& operator=(const SubsystemManager&) = delete;
		SubsystemManager(SubsystemManager&&) = default;
		SubsystemManager& operator=(SubsystemManager&&) = default;

		template<typename T, typename... Args>
		T* Register(Args&&... args)
		{
			auto subsystem = std::make_unique<T>(std::forward<Args>(args)...);
			T* ptr = subsystem.get();
			m_SubsystemMap[typeid(T)] = ptr;
			m_Subsystems.push_back(std::move(subsystem));
			return ptr;
		}

		template<typename T>
		T* Get()
		{
			auto it = m_SubsystemMap.find(typeid(T));
			if (it != m_SubsystemMap.end())
				return static_cast<T*>(it->second);
			return nullptr;
		}

		void InitAll();
		void ShutdownAll();
		void TickAll(float deltaTime);
		void OnEvent(Event& event);

	private:
		std::vector<std::unique_ptr<Subsystem>> m_Subsystems;
		std::unordered_map<std::type_index, Subsystem*> m_SubsystemMap;
	};

}
