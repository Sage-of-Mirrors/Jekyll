#include "application/AJ3DContext.hpp"
#include "application/ACamera.hpp"

#include <bstream.h>
#include <algorithm>

#include <J3D/J3DModelLoader.hpp>
#include <J3D/J3DModelData.hpp>
#include <J3D/J3DModelInstance.hpp>
#include <J3D/J3DRendering.hpp>
#include <J3D/J3DUniformBufferObject.hpp>


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

void AJ3DContext::LoadModel(bStream::CStream& stream) {
    J3DModelLoader loader;
    
    mModelData = loader.Load(&stream, 0);
    mModelInstance = mModelData->GetInstance();

    mLights[0].Color = { 1.0f, 1.0f, 1.0f, 1.0f };
    mLights[1].Color = { 1.0f, 1.0f, 1.0f, 1.0f };
    mLights[2].Color = { 1.0f, 1.0f, 1.0f, 1.0f };

	J3DRendering::SetSortFunction(SortFuncTest);
}

void AJ3DContext::Render(ASceneCamera& camera, float deltaTime) {
    if (mModelInstance.get() == nullptr) {
        return;
    }

    glm::mat4 projection = camera.GetProjectionMatrix();
    glm::mat4 view = camera.GetViewMatrix();

    J3DUniformBufferObject::SetLights(mLights);
    J3DUniformBufferObject::SetProjAndViewMatrices(&projection, &view);

    J3DRendering::Render(deltaTime, camera.GetPosition(), projection, view, {mModelInstance});
}
