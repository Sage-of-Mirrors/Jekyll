#include "application/AJ3DContext.hpp"
#include "application/ACamera.hpp"

//#include "img/toon.h"
#include "img/toonex.h"
#include "img/i_kumo.h"
#include "img/i_kusa_a.h"
#include "img/h_ma_rak.h"

#include <bstream.h>
#include <algorithm>

#include <J3D/J3DModelLoader.hpp>
#include <J3D/Data/J3DModelData.hpp>
#include <J3D/Data/J3DModelInstance.hpp>

#include <J3D/Animation/J3DAnimationLoader.hpp>
#include <J3D/Animation/J3DTexMatrixAnimationInstance.hpp>
#include <J3D/Animation/J3DColorAnimationInstance.hpp>
#include <J3D/Animation/J3DTexIndexAnimationInstance.hpp>
#include <J3D/Animation/J3DJointAnimationInstance.hpp>
#include <J3D/Animation/J3DJointFullAnimationInstance.hpp>
#include <J3D/Animation/J3DVisibilityAnimationInstance.hpp>

#include <J3D/Rendering/J3DRendering.hpp>

#include <J3D/Material/J3DMaterial.hpp>
#include <J3D/Material/J3DUniformBufferObject.hpp>
#include <J3D/Material/J3DMaterialTableLoader.hpp>

#include <J3D/Picking/J3DPicking.hpp>


void SortFuncTest(J3D::Rendering::RenderPacketVector& packets) {
	std::vector<J3DRenderPacket> opaquePackets;
	std::vector<J3DRenderPacket> xluPackets;

	for (J3DRenderPacket packet : packets) {
		if ((packet.SortKey & 0x00800000) != 0) {
			opaquePackets.push_back(packet);
		}
		else {
			xluPackets.push_back(packet);
		}
	}

	std::sort(
		opaquePackets.begin(),
		opaquePackets.end(),
		[](const J3DRenderPacket& a, const J3DRenderPacket& b) -> bool {
			return a.Material->Name < b.Material->Name;
		}
	);
	std::sort(
		xluPackets.begin(),
		xluPackets.end(),
		[](const J3DRenderPacket& a, const J3DRenderPacket& b) -> bool {
			return a.Material->Name < b.Material->Name;
		}
	);

	packets.clear();

	for (J3DRenderPacket packet : opaquePackets) {
		packets.push_back(packet);
	}
	for (J3DRenderPacket packet : xluPackets) {
		packets.push_back(packet);
	}
}

void PacketSort(J3D::Rendering::RenderPacketVector packets) {
	std::sort(packets.begin(), packets.end(),
		[](const J3DRenderPacket& a, const J3DRenderPacket& b) -> bool {
			// Sort bias
			{
				uint8_t sort_bias_a = static_cast<uint8_t>((a.SortKey & 0xFF000000) >> 24);
				uint8_t sort_bias_b = static_cast<uint8_t>((b.SortKey & 0xFF000000) >> 24);
				if (sort_bias_a != sort_bias_b) {
					return sort_bias_a > sort_bias_b;
				}
			}

			// Opaque or alpha test
			{
				uint8_t sort_alpha_a = static_cast<uint8_t>((a.SortKey & 0x800000) >> 23);
				uint8_t sort_alpha_b = static_cast<uint8_t>((b.SortKey & 0x800000) >> 23);
				if (sort_alpha_a != sort_alpha_b) {
					return sort_alpha_a < sort_alpha_b;
				}
			}

			return a.Material->Name < b.Material->Name;
		});
}

AJ3DContext::AJ3DContext() {
	J3D::Picking::InitFramebuffer(560, 745);
	J3D::Rendering::SetSortFunction(SortFuncTest);
}

AJ3DContext::~AJ3DContext() {
	J3D::Picking::DestroyFramebuffer();
}

void AJ3DContext::LoadModel(bStream::CStream& stream) {
	stream.seek(0);

    J3DModelLoader loader;
    
    std::shared_ptr<J3DModelData> data = loader.Load(&stream, 0);
	std::shared_ptr<AJ3DModelEntry> entry = std::make_shared<AJ3DModelEntry>();

	entry->data = data;
	entry->instance = data->CreateInstance();

	mLoadedModels.push_back(entry);

	mLights[1].Position = { 0.0f, 1000.0f, 1000.0f, 1.0f };
	mLights[1].DistAtten = { 1.0f, 0.0f, 0.0f, 1.0f };

    mLights[0].Color = { 0.0f, 0.0f, 0.0f, 1.0f };
    mLights[1].Color = { 1.0f, 0.0f, 0.0f, 1.0f };
    mLights[2].Color = { 0.0f, 0.0f, 0.0f, 1.0f };

	//mModelData->SetTexture("ZAtoon", 256, 8, (uint8_t*)toon, 0);
	//mModelData->SetTexture("ZBtoonEX", 256, 256, (uint8_t*)toonex, 0);
}

void AJ3DContext::LoadMaterialTable(bStream::CStream& stream) {
	if (!HasModels() || mSelectedModel.expired()) {
		return;
	}

	std::shared_ptr<AJ3DModelEntry> selectedLocked = mSelectedModel.lock();
	J3DMaterialTableLoader matTblLoader;

	std::shared_ptr<J3DMaterialTable> tbl = matTblLoader.Load(&stream, selectedLocked->data);
	tbl->SetTexture("ZBtoonEX", 256, 256, (uint8_t*)toonex, 0);

	selectedLocked->instance->SetInstanceMaterialTable(tbl);
}

void AJ3DContext::LoadAnimation(bStream::CStream& stream, const std::string& extension) {
	if (!HasModels() || mSelectedModel.expired()) {
		return;
	}

	std::shared_ptr<J3DModelInstance> selectedLocked = mSelectedModel.lock()->instance;
	J3DAnimation::J3DAnimationLoader anmLoader;

	if (extension == ".btk") {
		selectedLocked->SetTexMatrixAnimation(anmLoader.LoadAnimation<J3DAnimation::J3DTexMatrixAnimationInstance>(stream));
	}
	else if (extension == ".brk") {
		selectedLocked->SetRegisterColorAnimation(anmLoader.LoadAnimation<J3DAnimation::J3DColorAnimationInstance>(stream));
	}
	else if (extension == ".btp") {
		selectedLocked->SetTexIndexAnimation(anmLoader.LoadAnimation<J3DAnimation::J3DTexIndexAnimationInstance>(stream));
	}
	else if (extension == ".bck") {
		selectedLocked->SetJointAnimation(anmLoader.LoadAnimation<J3DAnimation::J3DJointAnimationInstance>(stream));
	}
	else if (extension == ".bca") {
		selectedLocked->SetJointFullAnimation(anmLoader.LoadAnimation<J3DAnimation::J3DJointFullAnimationInstance>(stream));
	}
	else if (extension == ".bva") {
		selectedLocked->SetVisibilityAnimation(anmLoader.LoadAnimation<J3DAnimation::J3DVisibilityAnimationInstance>(stream));
	}
}

void AJ3DContext::Render(ASceneCamera& camera, float deltaTime) {
    if (!HasModels()) {
        return;
    }

	shared_vector<J3DModelInstance> instances;
	for (std::shared_ptr<AJ3DModelEntry> e : mLoadedModels) {
		e->instance->UpdateAnimations(deltaTime);

		e->instance->SetLight(mLights[0], 0);
		e->instance->SetLight(mLights[1], 1);
		e->instance->SetLight(mLights[2], 2);

		instances.push_back(e->instance);

		for (std::shared_ptr<J3DTexture> t : e->data->GetTextures())
		{
			
		}
	}

	J3D::Rendering::RenderPacketVector sortedPackets = J3D::Rendering::SortPackets(instances, camera.GetPosition());

    glm::mat4 projection = camera.GetProjectionMatrix();
    glm::mat4 view = camera.GetViewMatrix();

    //J3DUniformBufferObject::SetLights(mLights);
    J3DUniformBufferObject::SetProjAndViewMatrices(projection, view);

    J3D::Rendering::Render(deltaTime, view, projection, sortedPackets);
	J3D::Picking::RenderPickingScene(view, projection, sortedPackets);
}

void AJ3DContext::UpdateTextureLods(float bias) {

}

void AJ3DContext::ToggleBmt() {
	if (!HasModels() || mSelectedModel.expired()) {
		return;
	}

	std::shared_ptr<J3DModelInstance> selectedLocked = mSelectedModel.lock()->instance;
	selectedLocked->SetUseInstanceMaterialTable(!selectedLocked->GetUseInstanceMaterialTable());
}

std::vector<std::weak_ptr<J3DTexture>> AJ3DContext::GetTextures() {
	if (!HasModels() || mSelectedModel.expired()) {
		return std::vector<std::weak_ptr<J3DTexture>>();
	}

	std::vector<std::weak_ptr<J3DTexture>> textures;
	for (std::shared_ptr<J3DTexture> mat : mSelectedModel.lock()->data->GetTextures()) {
		textures.push_back(mat);
	}

	return textures;
}

std::vector<std::weak_ptr<J3DMaterial>> AJ3DContext::GetMaterials() {
	if (!HasModels() || mSelectedModel.expired()) {
		return std::vector<std::weak_ptr<J3DMaterial>>();
	}

	std::vector<std::weak_ptr<J3DMaterial>> materials;
	for (std::shared_ptr<J3DMaterial> mat : mSelectedModel.lock()->data->GetMaterials()) {
		materials.push_back(mat);
	}

	return materials;
}

void AJ3DContext::PickQuery(uint32_t x, uint32_t y) {
	if (!HasModels()) {
		return;
	}

	J3D::Picking::ModelMaterialIdPair p = J3D::Picking::Query(x, y);
	uint16_t modelId = std::get<0>(p);
	uint16_t matId = std::get<1>(p);

	if (modelId == 0) {
		mSelectedModel.reset();
		return;
	}

	for (std::shared_ptr<AJ3DModelEntry> e : mLoadedModels) {
		if (e->instance->GetModelId() == modelId) {
			mSelectedModel = e;
			break;
		}
	}
}

void AJ3DContext::HoverQuery(glm::vec2 mousePos) {
	if (!HasModels()) {
		return;
	}

	for (std::shared_ptr<AJ3DModelEntry> e : mLoadedModels) {
		shared_vector<J3DMaterial> materials = e->data->GetMaterials();

		for (std::shared_ptr<J3DMaterial> m : materials) {
			if (m->IsSelected()) {
				m->SetSelected(false);
			}
		}
	}

	J3D::Picking::ModelMaterialIdPair p = J3D::Picking::Query(mousePos.x, mousePos.y);

	uint16_t modelId = std::get<0>(p);
	uint16_t matId = std::get<1>(p);

	if (modelId == 0) {
		return;
	}

	for (std::shared_ptr<AJ3DModelEntry> e : mLoadedModels) {
		if (e->instance->GetModelId() != modelId) {
			continue;
		}

		shared_vector<J3DMaterial> materials = e->data->GetMaterials();

		for (std::shared_ptr<J3DMaterial> m : materials) {
			if (m->GetMaterialId() == matId) {
				m->SetSelected(true);
				break;
			}
		}
	}
}

void AJ3DContext::ResizePickingBuffer(glm::vec2 size) {
	J3D::Picking::ResizeFramebuffer(size.x, size.y);
}
