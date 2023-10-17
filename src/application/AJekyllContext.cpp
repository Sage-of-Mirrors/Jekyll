#include "application/AJekyllContext.hpp"

#include "model/MScenegraph.hpp"

#include <bstream.h>

#include <imgui.h>
#include <imgui_internal.h>


AJekyllContext::AJekyllContext() : bIsDockingConfigured(false), mMainDockSpaceID(UINT32_MAX), mDockNodeTopID(UINT32_MAX),
	mDockNodeRightID(UINT32_MAX), mDockNodeDownID(UINT32_MAX), mDockNodeLeftID(UINT32_MAX), mScenegraph(nullptr)
{

}

AJekyllContext::~AJekyllContext() {

}

void AJekyllContext::SetUpDocking() {
	const ImGuiViewport* mainViewport = ImGui::GetMainViewport();

	ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode;
	mMainDockSpaceID = ImGui::DockSpaceOverViewport(mainViewport, dockFlags);

	if (!bIsDockingConfigured) {
		ImGui::DockBuilderRemoveNode(mMainDockSpaceID); // clear any previous layout
		ImGui::DockBuilderAddNode(mMainDockSpaceID, dockFlags | ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(mMainDockSpaceID, mainViewport->Size);

		mDockNodeTopID = ImGui::DockBuilderSplitNode(mMainDockSpaceID, ImGuiDir_Up, 0.5f, nullptr, &mMainDockSpaceID);
		mDockNodeRightID = ImGui::DockBuilderSplitNode(mMainDockSpaceID, ImGuiDir_Right, 0.5f, nullptr, &mMainDockSpaceID);
		mDockNodeDownID = ImGui::DockBuilderSplitNode(mMainDockSpaceID, ImGuiDir_Down, 0.5f, nullptr, &mMainDockSpaceID);
		mDockNodeLeftID = ImGui::DockBuilderSplitNode(mMainDockSpaceID, ImGuiDir_Left, 0.5f, nullptr, &mMainDockSpaceID);

		ImGui::DockBuilderDockWindow("Scenegraph", mDockNodeLeftID);
		ImGui::DockBuilderDockWindow("Properties", mDockNodeRightID);

		ImGui::DockBuilderFinish(mMainDockSpaceID);

		bIsDockingConfigured = true;
	}
}

void AJekyllContext::RenderMenuBar() {
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("Open...")) {
			bStream::CFileStream test = bStream::CFileStream("D:\\SZS Tools\\cl_real.bdl", bStream::Big, bStream::In);
			test.seek(0x20);
			
			mScenegraph = new MScenegraph();
			mScenegraph->LoadScenegraphNodes(test);

			//OpenModelCB();
		}
		if (ImGui::MenuItem("Save...")) {
			//SaveModelCB();
		}

		ImGui::Separator();
		ImGui::MenuItem("Close");

		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("About")) {
		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
}

void AJekyllContext::Update(float deltaTime) {
}

void AJekyllContext::Render(float deltaTime) {
	SetUpDocking();

    RenderMenuBar();

	ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.15f, 0.15f, 0.15f, 1.0f });

    mScenePanel.Render(mScenegraph);
	mPropertiesPanel.Render();

	ImGui::PopStyleColor();
}

void AJekyllContext::OnFileDropped(std::string fileName) {

}
