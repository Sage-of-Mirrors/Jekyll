#include "application/AJekyllContext.hpp"
#include "application/AJ3DContext.hpp"
#include "model/MJointData.hpp"

#include "model/MScenegraph.hpp"

#include <bstream.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuiFileDialog.h>


AJekyllContext::AJekyllContext() : bIsDockingConfigured(false), mMainDockSpaceID(UINT32_MAX), mDockNodeTopID(UINT32_MAX),
	mDockNodeRightID(UINT32_MAX), mDockNodeDownID(UINT32_MAX), mDockNodeLeftID(UINT32_MAX), mScenegraph(nullptr), mJointData(nullptr),
	mJ3DContext(nullptr), mMainViewport(nullptr), mLightsPanel(nullptr)
{

}

AJekyllContext::~AJekyllContext() {
	delete mScenegraph;
	delete mJointData;

	delete mJ3DContext;
	delete mMainViewport;

	for (UViewport* v : mOtherViewports) {
		delete v;
	}

	mOtherViewports.clear();
}

void AJekyllContext::SetUpDocking() {
	const ImGuiViewport* mainViewport = ImGui::GetMainViewport();

	ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode;
	mMainDockSpaceID = ImGui::DockSpaceOverViewport(mainViewport, dockFlags);

	if (!bIsDockingConfigured) {
		ImGui::DockBuilderRemoveNode(mMainDockSpaceID); // clear any previous layout
		ImGui::DockBuilderAddNode(mMainDockSpaceID, dockFlags | ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(mMainDockSpaceID, mainViewport->Size);

		mDockNodeTopID = ImGui::DockBuilderSplitNode(mMainDockSpaceID, ImGuiDir_Up, 0.5f, nullptr, &mMainDockSpaceID);
		mDockNodeRightID = ImGui::DockBuilderSplitNode(mMainDockSpaceID, ImGuiDir_Right, 0.5f, nullptr, &mMainDockSpaceID);
		mDockNodeDownID = ImGui::DockBuilderSplitNode(mMainDockSpaceID, ImGuiDir_Down, 0.5f, nullptr, &mMainDockSpaceID);
		mDockNodeLeftID = ImGui::DockBuilderSplitNode(mMainDockSpaceID, ImGuiDir_Left, 0.5f, nullptr, &mMainDockSpaceID);

		//ImGui::DockBuilderDockWindow("Scenegraph", mDockNodeLeftID);
		ImGui::DockBuilderDockWindow("Properties", mDockNodeRightID);
		ImGui::DockBuilderDockWindow("Main Viewport", mMainDockSpaceID);

		ImGui::DockBuilderFinish(mMainDockSpaceID);

		bIsDockingConfigured = true;
	}
}

void AJekyllContext::RenderMenuBar() {
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("Open...")) {			
			OpenModelCB();
		}
		if (ImGui::MenuItem("Save...")) {
			//SaveModelCB();
		}

		ImGui::Separator();
		ImGui::MenuItem("Close");

		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Tools")) {
		if (ImGui::MenuItem("Lights")) {
			if (mJ3DContext != nullptr && mLightsPanel == nullptr) {
				mLightsPanel = new ULightsPanel(mJ3DContext);
			}
		}

		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("About")) {
		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();
}

void AJekyllContext::Update(float deltaTime) {
	// Remove closed viewports
	for (auto it = mOtherViewports.begin(); it != mOtherViewports.end(); ) {
		if (!(*it)->IsOpen()) {
			delete (*it);
			it = mOtherViewports.erase(it);

			continue;
		}

		++it;
	}
}

void AJekyllContext::Render(float deltaTime) {
	SetUpDocking();

    RenderMenuBar();

	ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.15f, 0.15f, 0.15f, 1.0f });

    //mScenePanel.Render(mScenegraph);
	mPropertiesPanel.Render();

	if (mJ3DContext != nullptr) {
		mMainViewport->RenderUI(deltaTime);

		for (UViewport* v : mOtherViewports) {
			v->RenderUI(deltaTime);
		}

		if (mLightsPanel != nullptr) {
			mLightsPanel->Render();
		}
	}

	ImGui::PopStyleColor();

	// Render open file dialog
	if (ImGuiFileDialog::Instance()->Display("loadModelDialog")) {
		if (ImGuiFileDialog::Instance()->IsOk()) {
			LoadModel(ImGuiFileDialog::Instance()->GetFilePathName());
		}

		ImGuiFileDialog::Instance()->Close();
	}
}

void AJekyllContext::PostRender(float deltaTime) {
	if (mJ3DContext != nullptr) {
		mMainViewport->RenderScene(mJ3DContext, deltaTime);

		for (UViewport* v : mOtherViewports) {
			v->RenderScene(mJ3DContext, deltaTime);
		}
	}
}

void AJekyllContext::OpenModelCB() {
	ImGuiFileDialog::Instance()->OpenDialog("loadModelDialog", "Choose Model File", "J3D Models{.bmd,.bdl}", ".", 1, nullptr, ImGuiFileDialogFlags_Modal);
}

void AJekyllContext::LoadModel(std::filesystem::path filePath) {
	if (!std::filesystem::exists(filePath)) {
		std::cout << "File \"" << filePath << "\" does not exist!" << std::endl;
		return;
	}
	else if (!filePath.has_extension()) {
		std::cout << "File \"" << filePath << "\" has no extension!" << std::endl;
		return;
	}
	else if (filePath.extension() != ".bmd" && filePath.extension() != ".bdl") {
		std::cout << "File \"" << filePath << "\" is not a BMD or a BDL!" << std::endl;
		return;
	}

	bStream::CFileStream fileStream = bStream::CFileStream(filePath.generic_string(), bStream::Big, bStream::In);

	mJ3DContext = new AJ3DContext();
	mJ3DContext->LoadModel(fileStream);

	mMainViewport = new UViewport("Main Viewport");

	LoadSections(fileStream);
}

void AJekyllContext::LoadSections(bStream::CStream& stream) {
	stream.seek(0);

	uint32_t sectionCount = stream.peekUInt32(0x0C);
	uint8_t* headerData = new uint8_t[0x20];

	stream.readBytesTo(headerData, 0x20);
	mMiscModelData.SetHeader(headerData);

	for (uint32_t i = 0; i < sectionCount; i++) {
		uint32_t sectionId = stream.peekUInt32(stream.tell());

		switch (sectionId) {
			case 0x494E4631:
			{
				mScenegraph = new MScenegraph();
				mScenegraph->LoadScenegraphData(stream);

				break;
			}
			case 0x4A4E5431:
			{
				mJointData = new MJointData();
				mJointData->LoadJointData(stream, mScenegraph->GetRoot());

				break;
			}
			default:
			{
				uint32_t sectionSize = stream.peekUInt32(stream.tell() + 4);
				uint8_t* sectionData = new uint8_t[sectionSize];

				stream.readBytesTo(sectionData, sectionSize);
				mMiscModelData.AddSection(sectionId, sectionSize, sectionData);

				break;
			}
		}
	}
}

void AJekyllContext::OnFileDropped(std::filesystem::path filePath) {
	LoadModel(filePath);
}
