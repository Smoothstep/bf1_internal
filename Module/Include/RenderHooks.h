#pragma once
#include "RenderFunctions.h"
#include "ClassDecl.h"

namespace D3D
{
	enum class EObjectType : size_t
	{
		Obj_SoldierModel = 0x1C5,
		Obj_Soldier = 0x3A2,
		Obj_Vehicle = 0x3A3
	};

	struct RenderObject
	{
		uint32_t Type;
		uint32_t Unk;
		const void* Unk1;
		const void* EntityData;
		const void** Entity;
	};

	struct RenderObjectEntry
	{
		RenderObject* Obj;
		size_t Count;
		EObjectType Type;
		size_t Part;
	};

	struct PrimitiveDescBase
	{
		EObjectType Type;
		uint8_t Index;
	};

	struct PrimitiveDesc
	{
		uint8_t Pad[0xB8];
		RenderObjectEntry* Entry;
	};

	class EntityRenderer
	{
	public:
		bool AddRenderTask(PrimitiveDescBase* Primitive, RenderObjectEntry* Obj, size_t Sz);
	};

	class MeshRenderer
	{
	public:
		void* PrepareDynamic(EntityRenderInstructions* Inst);
		void* PrepareStatic(EntityRenderInstructions* Inst);

	private:	
		std::optional<Alliance> ShouldRender(const ClientPlayer* Local, const PrimitiveDescBase* Prim);

		void PrepareCustom(PrimitiveDescBase* Prim, EntityRenderInstructions* Inst, GameInterface& Gi, size_t First);
		RenderFunctions* PrepareCustomImpl(RenderFunctionEntry& Fn, Alliance RenderAlliance, std::shared_ptr<ViewState> Vs);

		MeshSet* Mesh;
		uint8_t Pad[0x80];
		PrimitiveDesc* Primitive;
	};

	class SoldierRenderer
	{
	public:
		PrimitiveDesc* SoldierEntityRenderPrimitive(void*, class ClientSoldierEntity*, void*,
			RenderObjectEntry*, void*, void*, void*, void*, bool, void*, uint32_t, void*);
	};
}