#include "stdafx.h"
#include "RenderHooks.h"
#include "RenderFunctions.h"
#include "Game.h"
#include "GameHooks.h"
#include "ClientPlayer.h"
#include "Crypto.h"
#include "D3DHooks.h"
#include "D3DRenderer.h"

#define IMPL_FN_INTERFACE(X) \
	virtual void* Apply1 X \
	virtual void* Apply2 X \
	virtual void* Apply3 X

namespace D3D
{
	static RenderFunctionHooks RfHooks{};

	struct ViewStateCapture : public RenderFunctionEntry
	{
		ViewStateCapture(const std::shared_ptr<ViewState>& Vs, size_t ViewIndex)
			: ViewIndex{ ViewIndex }
			, Vs{ Vs }
		{}

		size_t ViewIndex;
		std::weak_ptr<ViewState> Vs;
		
		std::shared_ptr<ViewState> Take()
		{
			std::weak_ptr<ViewState> Result;
			Vs.swap(Result);
			return Result.lock();
		}
	};

	class RestoreOriginal : public FunctionInterface<ViewStateCapture>
	{
	public:
		IMPL_FN_INTERFACE((ViewStateCapture* Rc, void* Ctx) override
		{
			if (auto Vs = Rc->Take()) {
				Vs->RestoreViews(DxRenderer::Instance()->Context, Rc->ViewIndex);
			}
			return Ctx;
		})
	};

	class ApplyIntermediate : public FunctionInterface<ViewStateCapture>
	{
	public:
		IMPL_FN_INTERFACE((ViewStateCapture* Rc, void* Ctx) override
		{
			if (auto Vs = Rc->Take()) {
				Vs->ApplyViews(DxRenderer::Instance()->Context, Rc->ViewIndex);
			}
			return Ctx;
		})
	};

	bool EntityRenderer::AddRenderTask(PrimitiveDescBase* Primitive, RenderObjectEntry* Entry, size_t Sz)
	{
		const auto Result = (this->*HookRefSpec<GameInterface*>::Current()->GetOriginal<decltype(&EntityRenderer::AddRenderTask)>())(Primitive, Entry, Sz);
		
		switch (Entry->Type)
		{
			case EObjectType::Obj_SoldierModel:
			{
				*Primitive = { Entry->Type, 0 };
				break;
			}

			case EObjectType::Obj_Soldier:
			{
				const auto* Entity = reinterpret_cast<const ClientSoldierEntity*>(*Entry->Obj->Entity);

				if (IsValidPointer(Entity) &&
					IsValidPointer(Entity->Owner) && Entity->Owner != Crypto::GetLocalPlayer())
				{
					*Primitive = { Entry->Type, Entity->Owner->TeamId };
				}

				break;
			}

			case EObjectType::Obj_Vehicle:
			{
				const auto* Entity = reinterpret_cast<const ClientVehicleEntity*>(*Entry->Obj->Entity);

				if (IsValidPointer(Entity))
				{
					*Primitive = { Entry->Type, Entity->TeamId };
				}

				break;
			}
		}

		return Result;
	}

	void* MeshRenderer::PrepareStatic(EntityRenderInstructions* Inst)
	{
		const size_t Num = Inst->NumMeshes();
		auto HkRef = HookRefSpec<GameInterface&>::Current();
		const auto Result = (this->*HookRefSpec<GameInterface*>::Current()->GetOriginal<decltype(&MeshRenderer::PrepareStatic)>())(Inst);

		if (Num < Inst->NumMeshes() && Primitive)
		{
			auto* Prim = reinterpret_cast<PrimitiveDescBase*>(Primitive) - 2;
			
			if (Prim->Type == EObjectType::Obj_Vehicle)
			{
				PrepareCustom(Prim, Inst, std::get<0>(HkRef.Tuple()), Num);
			}
		}

		return Result;
	}

	void* MeshRenderer::PrepareDynamic(EntityRenderInstructions* Inst)
	{
		const size_t Num = Inst->NumMeshes();
		auto HkRef = HookRefSpec<GameInterface&>::Current();
		const auto Result = (this->*HkRef->GetOriginal<decltype(&MeshRenderer::PrepareDynamic)>())(Inst);

		if (Num < Inst->NumMeshes() && Primitive)
		{
			PrepareCustom(reinterpret_cast<PrimitiveDescBase*>(Primitive) - 1, Inst, std::get<0>(HkRef.Tuple()), Num);
		}
		
		return Result;
	}

	std::optional<Alliance> MeshRenderer::ShouldRender(const ClientPlayer* Local, const PrimitiveDescBase* Prim)
	{
		if (!IsValidPointer(Local))
		{
			return {};
		}

		if (Prim->Index > 2)
		{
			return {};
		}

		return std::make_optional(Prim->Index != Local->TeamId ? Alliance::Alliance_Enemy : Alliance::Alliance_Friendly);
	}

	RenderFunctions* MeshRenderer::PrepareCustomImpl(RenderFunctionEntry& Fn, Alliance RenderAlliance, std::shared_ptr<ViewState> Vs)
	{
		auto* Rf = Fn.Func();

		return RfHooks.HookRenderFunctions(Rf, [RenderAlliance, &Fn, Rf, Vs](FunctionInterface<void*>* Fib, size_t Index) mutable
		{
			Fn.FuncPtr = reinterpret_cast<size_t>(Rf);

			auto Original = Rf[1];

			for (auto* Current = Original.First; Original.Size; Current = Current->Next())
			{
				if (*reinterpret_cast<size_t*>(Current->FuncPtr) == static_cast<size_t>(EVariableOffsets::Offset_SetupDrawCall))
				{
					Fib->RFn.Allocate<ApplyIntermediate, ViewStateCapture>(Vs, RenderAlliance);

					Current->Execute(Index, Fib, Current);

					Fib->RFn.Allocate<RestoreOriginal, ViewStateCapture>(Vs, RenderAlliance);
				}
				else
				{
					Current->Execute(Index, Fib, Current);
				}

				Original.Size -= Current->NextOffset;
			}

			RfHooks.ClearRenderFunction(Rf);
		});
	}

	void MeshRenderer::PrepareCustom(PrimitiveDescBase* Prim, EntityRenderInstructions* Inst, GameInterface& Gi, size_t First)
	{
		auto RenderAlliance = Match(Prim->Type,
			case EObjectType::Obj_SoldierModel:
				return std::make_optional(Alliance::Alliance_Friendly);

			case EObjectType::Obj_Vehicle:
			case EObjectType::Obj_Soldier:
				return ShouldRender(Crypto::GetLocalPlayer(), Prim);

			default:
				return std::optional<Alliance> {};
		);

		const auto Vs = Gi.GetViewState();

		if (Vs && RenderAlliance)
		{
			if (auto* Desc = Inst->ReDesc[RpGeometry])
			{
				for (size_t N = First; N < Desc->NumGeometries; ++N)
				{
					auto* Detail = Desc->GDesc[N].Detail;

					if (Detail && !RfHooks.IsProxy(Detail->Entry.Func()))
					{
						Detail->Entry.FuncPtr = reinterpret_cast<size_t>(PrepareCustomImpl(Detail->Entry, *RenderAlliance, Vs));
					}
				}
			}
		}
	}

	PrimitiveDesc* SoldierRenderer::SoldierEntityRenderPrimitive(void* A0, ClientSoldierEntity* A1, void* A2, RenderObjectEntry* A3,
		void* A4, void* A5, void* A6, void* A7, bool A8, void* A9, uint32_t A10, void* A11)
	{
		auto Hk = HookRefSpec<GameInterface*>::Current();

		auto Result = (this->*Hk->GetOriginal<decltype(&SoldierRenderer::SoldierEntityRenderPrimitive)>())(
			A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11);

		// Unsafe 
		if (A3->Obj)
		{
			Result->Entry = A3;
		}

		return Result;
	}

}