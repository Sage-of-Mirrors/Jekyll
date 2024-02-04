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


void SortFuncTest(J3DRendering::SortFunctionArgs packets) {
	std::vector<J3DRenderPacket> opaquePackets;
	std::vector<J3DRenderPacket> xluPackets;

	for (J3DRenderPacket packet : packets) {
		if ((packet.SortKey & 0x01000000) != 0) {
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

AJ3DContext::AJ3DContext() {
	J3D::Picking::InitFramebuffer(560, 745);
}

AJ3DContext::~AJ3DContext() {
	J3D::Picking::DestroyFramebuffer();
}

void AJ3DContext::LoadModel(bStream::CStream& stream) {
	stream.seek(0);

    J3DModelLoader loader;
    
    mModelData = loader.Load(&stream, 0);
    mModelInstance = mModelData->CreateInstance();

	mLights[1].Position = { 0.0f, 1000.0f, 1000.0f, 1.0f };
	mLights[1].DistAtten = { 1.0f, 0.0f, 0.0f, 1.0f };

    mLights[0].Color = { 0.0f, 0.0f, 0.0f, 1.0f };
    mLights[1].Color = { 1.0f, 0.0f, 0.0f, 1.0f };
    mLights[2].Color = { 0.0f, 0.0f, 0.0f, 1.0f };

	J3DRendering::SetSortFunction(SortFuncTest);

	//mModelData->SetTexture("ZAtoon", 256, 8, (uint8_t*)toon, 0);
	mModelData->SetTexture("ZBtoonEX", 256, 256, (uint8_t*)toonex, 0);
}

void AJ3DContext::LoadMaterialTable(bStream::CStream& stream) {
	if (mModelData == nullptr || mModelInstance == nullptr) {
		return;
	}

	J3DMaterialTableLoader matTblLoader;

	std::shared_ptr<J3DMaterialTable> tbl = matTblLoader.Load(&stream, mModelData);
	tbl->SetTexture("ZBtoonEX", 256, 256, (uint8_t*)toonex, 0);

	mModelInstance->SetInstanceMaterialTable(tbl);
}

void AJ3DContext::LoadAnimation(bStream::CStream& stream, const std::string& extension) {
	if (mModelInstance == nullptr) {
		return;
	}

	J3DAnimation::J3DAnimationLoader anmLoader;

	if (extension == ".btk") {
		mModelInstance->SetTexMatrixAnimation(anmLoader.LoadAnimation<J3DAnimation::J3DTexMatrixAnimationInstance>(stream));
	}
	else if (extension == ".brk") {
		mModelInstance->SetRegisterColorAnimation(anmLoader.LoadAnimation<J3DAnimation::J3DColorAnimationInstance>(stream));
	}
	else if (extension == ".btp") {
		mModelInstance->SetTexIndexAnimation(anmLoader.LoadAnimation<J3DAnimation::J3DTexIndexAnimationInstance>(stream));
	}
	else if (extension == ".bck") {
		mModelInstance->SetJointAnimation(anmLoader.LoadAnimation<J3DAnimation::J3DJointAnimationInstance>(stream));
	}
	else if (extension == ".bca") {
		mModelInstance->SetJointFullAnimation(anmLoader.LoadAnimation<J3DAnimation::J3DJointFullAnimationInstance>(stream));
	}
	else if (extension == ".bva") {
		mModelInstance->SetVisibilityAnimation(anmLoader.LoadAnimation<J3DAnimation::J3DVisibilityAnimationInstance>(stream));
	}
}

void AJ3DContext::Render(ASceneCamera& camera, float deltaTime) {
    if (mModelInstance.get() == nullptr) {
        return;
    }

	mModelInstance->UpdateAnimations(deltaTime);

    glm::mat4 projection = camera.GetProjectionMatrix();
    glm::mat4 view = camera.GetViewMatrix();

    //J3DUniformBufferObject::SetLights(mLights);
    J3DUniformBufferObject::SetProjAndViewMatrices(projection, view);

	mModelInstance->SetLight(mLights[0], 0);
	mModelInstance->SetLight(mLights[1], 1);
	mModelInstance->SetLight(mLights[2], 2);

    J3DRendering::Render(deltaTime, camera.GetPosition(), view, projection, { mModelInstance });
	J3D::Picking::RenderPickingScene(view, projection, { mModelInstance });

	//J3DUniformBufferObject::ClearUBO();
}

void AJ3DContext::ToggleBmt() {
	mModelInstance->SetUseInstanceMaterialTable(!mModelInstance->GetUseInstanceMaterialTable());
}

std::vector<std::shared_ptr<J3DTexture>> AJ3DContext::GetTextures() {
	if (mModelData == nullptr) {
		return std::vector<std::shared_ptr<J3DTexture>>();
	}

	return mModelData->GetTextures();
}

std::vector<std::shared_ptr<J3DMaterial>> AJ3DContext::GetMaterials() {
	if (mModelData == nullptr) {
		return std::vector<std::shared_ptr<J3DMaterial>>();
	}

	return mModelData->GetMaterials();
}

void AJ3DContext::PickQuery(uint32_t x, uint32_t y) {
	J3D::Picking::ModelMaterialIdPair p = J3D::Picking::Query(x, y);
	std::cout << "Picked Model Id " << std::get<0>(p)  << ", mat Id " << std::get<1>(p) << ".\n";
}

void AJ3DContext::HoverQuery(glm::vec2 mousePos) {
	std::vector<std::shared_ptr<J3DMaterial>> materials = GetMaterials();

	for (std::shared_ptr<J3DMaterial> m : materials) {
		if (m->IsSelected()) {
			m->SetSelected(false);
		}
	}

	J3D::Picking::ModelMaterialIdPair p = J3D::Picking::Query(mousePos.x, mousePos.y);

	uint16_t modelId = std::get<0>(p);
	uint16_t matId = std::get<1>(p);

	if (modelId == 0) {
		return;
	}

	for (std::shared_ptr<J3DMaterial> m : materials) {
		if (m->GetMaterialId() == matId) {
			m->SetSelected(true);
			break;
		}
	}
}

void AJ3DContext::ResizePickingBuffer(glm::vec2 size) {
	J3D::Picking::ResizeFramebuffer(size.x, size.y);
}
