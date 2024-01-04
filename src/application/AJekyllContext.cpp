#include "application/AJekyllContext.hpp"
#include "application/AJ3DContext.hpp"

#include "model/MScenegraph.hpp"
#include "model/MJointData.hpp"
#include "model/MShapeData.hpp"
#include "model/MTextureData.hpp"
#include "model/MMaterialData.hpp"

#include "ui/properties/UJointPropertiesTab.hpp"
#include "ui/properties/UShapePropertiesTab.hpp"
#include "ui/properties/UTexturePropertiesTab.hpp"
#include "ui/properties/UMaterialPropertiesTab.hpp"

#include <bstream.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuiFileDialog.h>


AJekyllContext::AJekyllContext() : bIsDockingConfigured(false), mMainDockSpaceID(UINT32_MAX), mDockNodeTopID(UINT32_MAX),
	mDockNodeRightID(UINT32_MAX), mDockNodeDownID(UINT32_MAX), mDockNodeLeftID(UINT32_MAX), mScenegraph(nullptr),
	mJointData(nullptr), mShapeData(nullptr), mTextureData(nullptr), mMaterialData(nullptr), mJ3DContext(nullptr),
	mMainViewport(nullptr), mLightsPanel(nullptr)
{

}

AJekyllContext::~AJekyllContext() {
	delete mScenegraph;
	delete mJointData;
	delete mShapeData;
	delete mTextureData;
	delete mMaterialData;

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
			SaveModelCB();
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

	// Render save file dialog
	if (ImGuiFileDialog::Instance()->Display("saveModelDialog")) {
		if (ImGuiFileDialog::Instance()->IsOk()) {
			SaveModel(ImGuiFileDialog::Instance()->GetFilePathName());
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
	ImGuiFileDialog::Instance()->OpenDialog("loadModelDialog", "Open Model File", "J3D Models{.bmd,.bdl}", ".", 1, nullptr, ImGuiFileDialogFlags_Modal);
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

	mPropertiesPanel.AddTab(new UJointPropertiesTab(mJointData));
	mPropertiesPanel.AddTab(new UShapePropertiesTab(mShapeData));
	mPropertiesPanel.AddTab(new UTexturePropertiesTab(mTextureData));
	mPropertiesPanel.AddTab(new UMaterialPropertiesTab(mMaterialData));
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
			case 0x53485031:
			{
				mShapeData = new MShapeData();
				mShapeData->LoadShapeData(stream);

				break;
			}
			case 0x54455831:
			{
				mTextureData = new MTextureData();
				mTextureData->SetTextureData(mJ3DContext->GetTextures());

				break;
			}
			case 0x4D415433:
			{
				mMaterialData = new MMaterialData();
				mMaterialData->SetMaterialData(mJ3DContext->GetMaterials());
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

void AJekyllContext::LoadMaterialTable(std::filesystem::path filePath) {
	mJ3DContext->LoadMaterialTable(bStream::CFileStream(filePath.generic_string(), bStream::Big, bStream::In));
}

void AJekyllContext::LoadAnimation(std::filesystem::path filePath) {
	mJ3DContext->LoadAnimation(bStream::CFileStream(filePath.generic_string(), bStream::Big, bStream::In), filePath.extension().generic_string());
}

void AJekyllContext::SaveModelCB() {
	ImGuiFileDialog::Instance()->OpenDialog("saveModelDialog", "Save Model File", ".bmd,.bdl", ".", 1, nullptr,
		ImGuiFileDialogFlags_Modal | ImGuiFileDialogFlags_ConfirmOverwrite);
}

void AJekyllContext::SaveModel(std::filesystem::path filePath) {
	bool bGenerateMdl3 = false;

	if (filePath.extension() == ".bdl") {
		bGenerateMdl3 = true;
	}

	bStream::CFileStream stream(filePath.generic_string(), bStream::Big, bStream::Out);

	uint32_t sectionSize = 0;
	uint8_t* sectionData = nullptr;

	// Write header
	stream.writeBytes(mMiscModelData.GetHeader(), 0x20);

	mScenegraph->SaveScenegraph(stream);

	// Write VTX1
	mMiscModelData.GetSection(0x56545831, sectionSize, sectionData);
	stream.writeBytes(sectionData, sectionSize);

	// Write EVP1
	mMiscModelData.GetSection(0x45565031, sectionSize, sectionData);
	stream.writeBytes(sectionData, sectionSize);

	// Write DRW1
	mMiscModelData.GetSection(0x44525731, sectionSize, sectionData);
	stream.writeBytes(sectionData, sectionSize);

	mJointData->SaveJointData(stream);
	mShapeData->SaveShapeData(stream);

	// TEMPORARY: Write MAT3
	mMiscModelData.GetSection(0x4D415433, sectionSize, sectionData);
	stream.writeBytes(sectionData, sectionSize);

	if (bGenerateMdl3) {
		// TEMPORARY: Write MDL3
		mMiscModelData.GetSection(0x4D444C33, sectionSize, sectionData);
		stream.writeBytes(sectionData, sectionSize);
	}

	mTextureData->SaveTextureData(stream);

	size_t finalSize = stream.tell();
	stream.seek(0x04);

	if (bGenerateMdl3) {
		stream.writeUInt32(0x62646C34);
		stream.seek(0x0C);
		stream.writeUInt32(0x09);
	}
	else {
		stream.writeUInt32(0x626D6433);
		stream.seek(0x0C);
		stream.writeUInt32(0x08);
	}

	stream.seek(0x08);
	stream.writeUInt32(finalSize);

	// Write watermark - might remove this later, idk
	stream.seek(0x10);
	stream.writeString("Made with Jekyll");
}

void AJekyllContext::OnFileDropped(std::filesystem::path filePath) {
	if (!filePath.has_extension()) {
		return;
	}

	if (filePath.extension() == ".bmd" || filePath.extension() == ".bdl") {
		LoadModel(filePath);
	}
	if (filePath.extension() == ".bmt") {
		LoadMaterialTable(filePath);
	}
	else {
		LoadAnimation(filePath);
	}
}
