#pragma once

#include "D3DRenderer.h"

namespace D3D
{
	template<class T, class... TArgs>
	std::weak_ptr<T> Renderer::AddPass(TArgs&&... Args)
	{
		try
		{
			const auto Lock = Passes[T::Type].Lock.Acquire();

			auto Pass = std::make_shared<T>(std::forward<TArgs>(Args)...);

			if (auto Iter = std::lower_bound(Passes[T::Type].begin(), Passes[T::Type].end(), Pass->Priority(), [](const auto& Pass, const size_t Prio)
			{
				return Pass->Priority() > Prio;
			}); Iter != Passes[T::Type].end())
			{
				Passes[T::Type].emplace(Iter, std::static_pointer_cast<RenderPass>(Pass));
			}
			else
			{
				Passes[T::Type].emplace_back(std::static_pointer_cast<RenderPass>(Pass));
			}

			return { Pass };
		}
		catch (const std::exception& Ex)
		{
			Logger::Error() << Ex.what() << Logger::EndLine;
			return {};
		}
	}
}