#include <IconsMaterialDesign.h>

#include <Glacier/Module/ZHitman5Module.h>
#include <Glacier/Entity/ZTemplateEntityBlueprintFactory.h>
#include <Glacier/Serializer/ZBinaryDeserializer.h>
#include <Glacier/Entity/STemplateEntityBlueprint.h>
#include <Glacier/Resource/ZResourceManager.h>
#include <Glacier/Physics/ZCollisionManager.h>
#include <Glacier/Render/ZSpatialEntity.h>
#include <Glacier/ZLevelManager.h>
#include <Glacier/Resource/ZResourcePending.h>
#include <Glacier/Resource/ZResourceReader.h>
#include <Glacier/ZCurve.h>
#include <Glacier/Entity/ZAspectEntityBlueprintFactory.h>
#include <Glacier/Engine/ZApplicationEngineWin32.h>
#include <Glacier/Render/ZRenderManager.h>

#include <Editor.h>
#include <Utility/ResourceUtility.h>
#include <Utility/MemoryUtility.h>
#include <Hooks.h>
#include <Registry/EnumRegistry.h>
#include <Registry/PropertyRegistry.h>
#include <Utility/Builders.h>
#include <Utility/Widgets.h>

#undef min
#undef max

Editor::EntityTreeNode::EntityTreeNode()
{
    entityIndex = -1;
    entityIndexInReferencedTEMP = -1;
    hasChildren = true;
    tbluRuntimeResourceID = -1;
    entityTypeResourceIndex = -1;
}

Editor::EntityTreeNode::EntityTreeNode(const unsigned int entityIndex, const char* entityName, const ZEntityRef entityRef, const ZRuntimeResourceID& tbluRuntimeResourceID, const int entityTypeResourceIndex) : entityIndex(entityIndex), entityName(entityName), entityRef(entityRef), tbluRuntimeResourceID(tbluRuntimeResourceID), entityTypeResourceIndex(entityTypeResourceIndex), hasChildren(true)
{
}

Editor::Pin::Pin(int id, const char* name, PinType type) : id(id), node(nullptr), name(name), type(type), kind(PinKind::Input)
{
}

Editor::BlueprintNode::BlueprintNode(int id, const char* name, ImColor color) : id(id), name(name), color(color), size(0, 0)
{
}

Editor::Link::Link(ax::NodeEditor::LinkId id, ax::NodeEditor::PinId startPinId, ax::NodeEditor::PinId endPinId) : id(id), startPinID(startPinId), endPinID(endPinId), color(255, 255, 255)
{
}

const bool Editor::EntityTreeNode::IsParent(std::shared_ptr<EntityTreeNode> entityTreeNode) const
{
    std::shared_ptr<EntityTreeNode> currentNode = parentNode;

    while (currentNode)
    {
        if (currentNode.get() == entityTreeNode.get())
        {
            return true;
        }

        currentNode = currentNode->parentNode;
    }

    return false;
}

Editor::Editor() : snapValue{ 1.0f, 1.0f, 1.0f }
{
    isOpen = false;

    scrollToEntity = false;

    gizmoMode = ImGuizmo::OPERATION::TRANSLATE;
    gizmoSpace = ImGuizmo::MODE::WORLD;
    isHoldingMouse = false;
    useSnap = false;

    editor = ax::NodeEditor::CreateEditor();
    nextID = 1;
    headerBackgroundTexture = nullptr;
    headerBackgroundTextureView = nullptr;
    headerBackgroundTextureWidth = 0;
    headerBackgroundTextureHeight = 0;

    ax::NodeEditor::SetCurrentEditor(editor);
}

Editor::~Editor()
{
    Hooks::ZTemplateEntityBlueprintFactory_ZTemplateEntityBlueprintFactory.DisableHook();
    Hooks::ZTemplateEntityBlueprintFactory_ZTemplateEntityBlueprintFactory.RemoveHook();

    Hooks::ZEntitySceneContext_CreateScene.DisableHook();
    Hooks::ZEntitySceneContext_CreateScene.RemoveHook();

    Hooks::ZEntitySceneContext_ClearScene.DisableHook();
    Hooks::ZEntitySceneContext_ClearScene.RemoveHook();

    if (headerBackgroundTexture)
    {
        headerBackgroundTexture->Release();

        headerBackgroundTexture = nullptr;
    }

    if (headerBackgroundTextureView)
    {
        headerBackgroundTextureView->Release();

        headerBackgroundTextureView = nullptr;
    }

    if (editor)
    {
        ax::NodeEditor::DestroyEditor(editor);

        editor = nullptr;
    }
}

void Editor::Initialize()
{
    ModInterface::Initialize();

    Hooks::ZTemplateEntityBlueprintFactory_ZTemplateEntityBlueprintFactory.CreateHook("ZTemplateEntityBlueprintFactory::ZTemplateEntityBlueprintFactory", 0xFFBF0, ZTemplateEntityBlueprintFactory_ZTemplateEntityBlueprintFactoryHook);
    Hooks::ZEntitySceneContext_CreateScene.CreateHook("ZEntitySceneContext::CreateScene", 0x4479E0, ZEntitySceneContext_CreateSceneHook);
    Hooks::ZEntitySceneContext_ClearScene.CreateHook("ZEntitySceneContext::ClearScene", 0x265A80, ZEntitySceneContext_ClearSceneHook);

    Hooks::ZTemplateEntityBlueprintFactory_ZTemplateEntityBlueprintFactory.EnableHook();
    Hooks::ZEntitySceneContext_CreateScene.EnableHook();
    Hooks::ZEntitySceneContext_ClearScene.EnableHook();
}

void Editor::OnEngineInitialized()
{
    std::shared_ptr<DirectXRenderer> directXRenderer = SDK::GetInstance().GetDirectXRenderer();

    directXRenderer->LoadTextureFromFile("assets/images/BlueprintBackground.png", &headerBackgroundTexture, &headerBackgroundTextureView, headerBackgroundTextureWidth, headerBackgroundTextureHeight);
}

void Editor::OnDrawMenu()
{
    if (ImGui::Button(ICON_MD_VIEW_LIST " Editor"))
    {
        isOpen = !isOpen;
    }
}

void Editor::OnDrawUI(const bool hasFocus)
{
    RenderEntityTree(hasFocus);
    RenderEntityProperties(hasFocus);
    RenderGizmo(hasFocus);
    RenderBlueprintNodesAndPins(hasFocus);
}

void Editor::OnDraw3D()
{
    if (selectedentityTreeNode && selectedentityTreeNode->entityRef.GetEntityTypePtrPtr())
    {
        RenderEntityAABB();
    }
}

void Editor::OnTemplateEntityBlueprintFactoryCreate(STemplateEntityBlueprint* templateEntityBlueprint, ZResourcePending& resourcePending)
{
    const unsigned int resourceDataSize = resourcePending.GetResourceReader().GetTarget()->GetResourceDataSize();
    const ZRuntimeResourceID& tbluRuntimeResourceID = resourcePending.GetResource().GetResourceStub()->GetRuntimeResourceID();

    templateEntityBlueprints.insert(std::make_pair(tbluRuntimeResourceID, STemplateEntityBlueprint()));

    STemplateEntityBlueprint& templateEntityBlueprint2 = templateEntityBlueprints[tbluRuntimeResourceID];

    templateEntityBlueprint2.entityTemplates.Resize(templateEntityBlueprint->entityTemplates.Size());
    templateEntityBlueprint2.pinConnections.Resize(templateEntityBlueprint->pinConnections.Size());
    templateEntityBlueprint2.inputPinForwardings.Resize(templateEntityBlueprint->inputPinForwardings.Size());
    templateEntityBlueprint2.outputPinForwardings.Resize(templateEntityBlueprint->outputPinForwardings.Size());

    templateEntityBlueprint2.rootEntityIndex = templateEntityBlueprint->rootEntityIndex;

    for (size_t i = 0; i < templateEntityBlueprint2.entityTemplates.Size(); ++i)
    {
        ZString entityName;

        entityName.Allocate(templateEntityBlueprint->entityTemplates[i].entityName.ToCString(), templateEntityBlueprint->entityTemplates[i].entityName.Length());

        templateEntityBlueprint2.entityTemplates[i].parentIndex = templateEntityBlueprint->entityTemplates[i].parentIndex;
        templateEntityBlueprint2.entityTemplates[i].entityTypeResourceIndex = templateEntityBlueprint->entityTemplates[i].entityTypeResourceIndex;
        templateEntityBlueprint2.entityTemplates[i].entityName = entityName;
    }

    for (size_t i = 0; i < templateEntityBlueprint2.pinConnections.Size(); ++i)
    {
        ZString fromPinName, toPinName;

        fromPinName.Allocate(templateEntityBlueprint->pinConnections[i].fromPinName.ToCString(), templateEntityBlueprint->pinConnections[i].fromPinName.Length());
        toPinName.Allocate(templateEntityBlueprint->pinConnections[i].toPinName.ToCString(), templateEntityBlueprint->pinConnections[i].toPinName.Length());

        templateEntityBlueprint2.pinConnections[i].fromID = templateEntityBlueprint->pinConnections[i].fromID;
        templateEntityBlueprint2.pinConnections[i].toID = templateEntityBlueprint->pinConnections[i].toID;
        templateEntityBlueprint2.pinConnections[i].fromPinName = fromPinName;
        templateEntityBlueprint2.pinConnections[i].toPinName = toPinName;
    }

    for (size_t i = 0; i < templateEntityBlueprint2.inputPinForwardings.Size(); ++i)
    {
        ZString fromPinName, toPinName;

        fromPinName.Allocate(templateEntityBlueprint->inputPinForwardings[i].fromPinName.ToCString(), templateEntityBlueprint->inputPinForwardings[i].fromPinName.Length());
        toPinName.Allocate(templateEntityBlueprint->inputPinForwardings[i].toPinName.ToCString(), templateEntityBlueprint->inputPinForwardings[i].toPinName.Length());

        templateEntityBlueprint2.inputPinForwardings[i].fromID = templateEntityBlueprint->inputPinForwardings[i].fromID;
        templateEntityBlueprint2.inputPinForwardings[i].toID = templateEntityBlueprint->inputPinForwardings[i].toID;
        templateEntityBlueprint2.inputPinForwardings[i].fromPinName = fromPinName;
        templateEntityBlueprint2.inputPinForwardings[i].toPinName = toPinName;
    }

    for (size_t i = 0; i < templateEntityBlueprint2.outputPinForwardings.Size(); ++i)
    {
        ZString fromPinName, toPinName;

        fromPinName.Allocate(templateEntityBlueprint->outputPinForwardings[i].fromPinName.ToCString(), templateEntityBlueprint->outputPinForwardings[i].fromPinName.Length());
        toPinName.Allocate(templateEntityBlueprint->outputPinForwardings[i].toPinName.ToCString(), templateEntityBlueprint->outputPinForwardings[i].toPinName.Length());

        templateEntityBlueprint2.outputPinForwardings[i].fromID = templateEntityBlueprint->outputPinForwardings[i].fromID;
        templateEntityBlueprint2.outputPinForwardings[i].toID = templateEntityBlueprint->outputPinForwardings[i].toID;
        templateEntityBlueprint2.outputPinForwardings[i].fromPinName = fromPinName;
        templateEntityBlueprint2.outputPinForwardings[i].toPinName = toPinName;
    }
}

void Editor::OnCreateScene(ZEntitySceneContext* entitySceneContext, const ZString& streamingState)
{
    rootNode = std::make_shared<EntityTreeNode>();

    rootNode->entityName = "Scene";
}

void Editor::OnClearScene(ZEntitySceneContext* entitySceneContext, bool fullyUnloadScene)
{
    rootNode.reset();

    for (auto it = templateEntityBlueprints.begin(); it != templateEntityBlueprints.end();)
    {
        const ZRuntimeResourceID runtimeResourceID = it->first;

        if (runtimeResourceID.IsLibraryResource())
        {
            templateEntityBlueprints.erase(it++);
        }
        else
        {
            ++it;
        }
    }
}

void Editor::RenderEntityTree(const bool hasFocus)
{
    if (!hasFocus || !isOpen)
    {
        return;
    }

    ImGui::SetNextWindowPos({ 0, 110 }, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({ 700, ImGui::GetIO().DisplaySize.y - 110 }, ImGuiCond_FirstUseEver);
    ImGui::PushFont(SDK::GetInstance().GetBoldFont());

    const bool isWindowVisible = ImGui::Begin(ICON_MD_VIEW_LIST " Entity Tree", &isOpen, ImGuiWindowFlags_HorizontalScrollbar);

    if (!isWindowVisible)
    {
        ImGui::PopFont();
        ImGui::End();

        return;
    }

    ImGui::PushFont(SDK::GetInstance().GetRegularFont());

    if (!rootNode->entityRef.GetEntityTypePtrPtr())
    {
        ZEntitySceneContext* entitySceneContext = Hitman5Module->GetSceneContext();
        TResourcePtr<IEntityBlueprintFactory>& sceneBlueprintResource = entitySceneContext->GetSceneBlueprintResource();
        ZTemplateEntityBlueprintFactory* templateEntityBlueprintFactory = static_cast<ZTemplateEntityBlueprintFactory*>(sceneBlueprintResource.GetRawPointer());

        rootNode->entityIndex = templateEntityBlueprintFactory->GetRootEntityIndex();
        rootNode->entityIndexInReferencedTEMP = -1;
        rootNode->parentNode = nullptr;
        rootNode->entityRef = entitySceneContext->GetLoadedScene();
    }

    RenderEntityTree(rootNode);

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

void Editor::RenderEntityTree(std::shared_ptr<EntityTreeNode> entityTreeNode)
{
    ImGui::PushID(entityTreeNode.get());

    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_SpanFullWidth;
    const bool isNodeSelected = selectedentityTreeNode && entityTreeNode->entityRef == selectedentityTreeNode->entityRef;

    if (isNodeSelected)
    {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;

        if (scrollToEntity)
        {
            ImGui::SetScrollHereY();

            scrollToEntity = false;
        }
    }
    else if (scrollToEntity)
    {
        if (selectedentityTreeNode->IsParent(entityTreeNode))
        {
            ImGui::SetNextItemOpen(true);
        }
    }

    if (entityTreeNode->hasChildren)
    {
        nodeFlags |= ImGuiTreeNodeFlags_OpenOnArrow;

        const bool isNodeOpen = ImGui::TreeNodeEx(entityTreeNode->entityName.c_str(), nodeFlags);

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            selectedentityTreeNode = entityTreeNode;
        }

        if (isNodeOpen)
        {
            if (rootNode.get() == entityTreeNode.get() && entityTreeNode->children.size() == 0)
            {
                ZEntitySceneContext* entitySceneContext = Hitman5Module->GetSceneContext();
                TResourcePtr<IEntityBlueprintFactory>& sceneBlueprintResource = entitySceneContext->GetSceneBlueprintResource();
                ZTemplateEntityBlueprintFactory* templateEntityBlueprintFactory = static_cast<ZTemplateEntityBlueprintFactory*>(sceneBlueprintResource.GetRawPointer());

                AddChildren(entityTreeNode, rootNode->entityRef.GetEntityTypePtrPtr(), templateEntityBlueprintFactory, templateEntityBlueprintFactory->GetRootEntityIndex());
            }

            for (std::shared_ptr<EntityTreeNode> entityTreeNode : entityTreeNode->children)
            {
                RenderEntityTree(entityTreeNode);
            }

            ImGui::TreePop();
        }
    }
    else
    {
        nodeFlags |= ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf;

        ImGui::TreeNodeEx(entityTreeNode->entityName.c_str(), nodeFlags);

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            selectedentityTreeNode = entityTreeNode;
        }
    }

    ImGui::PopID();
}

void Editor::RenderEntityProperties(const bool hasFocus)
{
    if (!hasFocus ||
        !isOpen ||
        !selectedentityTreeNode ||
        !selectedentityTreeNode->entityRef.GetEntityTypePtrPtr() ||
        !selectedentityTreeNode->entityRef.GetProperties())
    {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowPos({ io.DisplaySize.x - 600, 110 }, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({ 600, io.DisplaySize.y - 110 }, ImGuiCond_FirstUseEver);
    ImGui::PushFont(SDK::GetInstance().GetBoldFont());

    const bool isWindowVisible = ImGui::Begin(ICON_MD_BUILD " Entity Properties", &isOpen, ImGuiWindowFlags_HorizontalScrollbar);

    if (!isWindowVisible)
    {
        ImGui::PopFont();
        ImGui::End();

        return;
    }

    ImGui::PushFont(SDK::GetInstance().GetRegularFont());

    if (ImGui::Button("Teleport Free Camera To Entity"))
    {
        ZCameraEntity* activeCamera = ZApplicationEngineWin32::GetInstance()->GetActiveCamera();
        const float aspectRatio = activeCamera->GetAspectWByH();
        const float verticalFov = activeCamera->GetFov();
        const float horizontalFOV = 2 * std::atan(std::tan(verticalFov / 2.0f) * aspectRatio);

        ZSpatialEntity* spatialEntity = selectedentityTreeNode->entityRef.QueryInterfacePtr<ZSpatialEntity>();
        float4 min, max;

        spatialEntity->CalculateBounds(min, max, 1, 0);

        DirectX::BoundingBox boundingBox;
        DirectX::BoundingSphere boundingSphere;

        DirectX::BoundingBox::CreateFromPoints(boundingBox, min.m, max.m);
        DirectX::BoundingSphere::CreateFromBoundingBox(boundingSphere, boundingBox);

        const float xzExtent = boundingSphere.Radius / tan(horizontalFOV / 2.0f);
        const float yExtent = boundingSphere.Radius / tan(verticalFov / 2.0f);
        const float distance = std::max(xzExtent, yExtent);

        const float4 cameraPosition = activeCamera->GetWorldPosition();
        const float4 targetPosition = spatialEntity->GetWorldPosition();

        const DirectX::SimpleMath::Vector3 cameraPosition2 = DirectX::SimpleMath::Vector3(cameraPosition.x, cameraPosition.y, cameraPosition.z);
        const DirectX::SimpleMath::Vector3 targetPosition2 = DirectX::SimpleMath::Vector3(targetPosition.x, targetPosition.y, targetPosition.z) + boundingSphere.Center;
        DirectX::SimpleMath::Vector3 targetDirection = targetPosition2 - cameraPosition2;

        targetDirection.Normalize();

        const DirectX::SimpleMath::Vector3 targetPosition3 = targetPosition2 - targetDirection * distance;
        const DirectX::SimpleMath::Vector3 up = DirectX::SimpleMath::Vector3(0.f, 0.f, 1.f);
        const SMatrix worldMatrix = DirectX::SimpleMath::Matrix::CreateWorld(targetPosition3, targetPosition2 - targetPosition3, up);

        activeCamera->SetObjectToWorldMatrix(worldMatrix);
    }

    TArray<SPropertyData>* properties = selectedentityTreeNode->entityRef.GetProperties();

    for (size_t i = 0; i < properties->Size(); ++i)
    {
        SPropertyData* propertyData = &(*properties)[i];
        ZVariant variant = selectedentityTreeNode->entityRef.GetProperty(propertyData->m_nPropertyID);
        const IType* typeInfo = propertyData->m_pInfo->m_Type->pTypeInfo;

        const std::string typeName = typeInfo->GetTypeName();
        const std::string inputID = std::format("##Property{}", i);
        const std::string& propertyName = PropertyRegistry::GetInstance().GetPropertyName(propertyData->m_nPropertyID);

        ImGui::AlignTextToFramePadding();
        ImGui::Text(propertyName.c_str());
        ImGui::SameLine();

        ImGui::PushItemWidth(-1);

        if (typeName == "bool")
        {
            BoolProperty(inputID, selectedentityTreeNode->entityRef, propertyData->m_nPropertyID, variant.GetData());
        }
        else if (typeName == "uint8")
        {
            Uint8Property(inputID, selectedentityTreeNode->entityRef, propertyData->m_nPropertyID, variant.GetData());
        }
        else if (typeName == "int8")
        {
            Int8Property(inputID, selectedentityTreeNode->entityRef, propertyData->m_nPropertyID, variant.GetData());
        }
        else if (typeName == "uint16")
        {
            Uint16Property(inputID, selectedentityTreeNode->entityRef, propertyData->m_nPropertyID, variant.GetData());
        }
        else if (typeName == "int16")
        {
            Int16Property(inputID, selectedentityTreeNode->entityRef, propertyData->m_nPropertyID, variant.GetData());
        }
        else if (typeName == "uint32")
        {
            Uint32Property(inputID, selectedentityTreeNode->entityRef, propertyData->m_nPropertyID, variant.GetData());
        }
        else if (typeName == "int32")
        {
            Int32Property(inputID, selectedentityTreeNode->entityRef, propertyData->m_nPropertyID, variant.GetData());
        }
        else if (typeName == "uint64")
        {
            Uint64Property(inputID, selectedentityTreeNode->entityRef, propertyData->m_nPropertyID, variant.GetData());
        }
        else if (typeName == "int64")
        {
            Int64Property(inputID, selectedentityTreeNode->entityRef, propertyData->m_nPropertyID, variant.GetData());
        }
        else if (typeName == "float32")
        {
            Float32Property(inputID, selectedentityTreeNode->entityRef, propertyData->m_nPropertyID, variant.GetData());
        }
        else if (typeName == "float64")
        {
            Float64Property(inputID, selectedentityTreeNode->entityRef, propertyData->m_nPropertyID, variant.GetData());
        }
        else if (typeName == "ZString")
        {
            StringProperty(inputID, selectedentityTreeNode->entityRef, propertyData->m_nPropertyID, variant.GetData());
        }
        else if (typeName == "SVector2")
        {
            SVector2Property(inputID, selectedentityTreeNode->entityRef, propertyData->m_nPropertyID, variant.GetData());
        }
        else if (typeName == "SVector3")
        {
            SVector3Property(inputID, selectedentityTreeNode->entityRef, propertyData->m_nPropertyID, variant.GetData());
        }
        else if (typeName == "SVector4")
        {
            SVector4Property(inputID, selectedentityTreeNode->entityRef, propertyData->m_nPropertyID, variant.GetData());
        }
        else if (typeName == "SMatrix43")
        {
            SMatrix43Property(inputID, selectedentityTreeNode->entityRef, propertyData->m_nPropertyID, variant.GetData());
        }
        else if (typeName == "ZEntityRef")
        {
            EntityRefProperty(variant.GetData());
        }
        else if (typeInfo->IsEnum())
        {
            EnumProperty(inputID, selectedentityTreeNode->entityRef, propertyData, variant.GetData());
        }
        else if (typeInfo->IsTResourcePtr())
        {
            ResourceProperty(inputID, selectedentityTreeNode->entityRef, propertyData->m_nPropertyID, variant.GetData());
        }
        else
        {
            UnsupportedProperty(inputID, selectedentityTreeNode->entityRef, propertyData, variant.GetData());
        }
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

void Editor::RenderGizmo(const bool hasFocus)
{
    ImGuizmo::BeginFrame();

    ImGuiIO& io = ImGui::GetIO();

    if (hasFocus)
    {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && !io.WantCaptureMouse)
        {
            const ImVec2 mousePosition = ImGui::GetMousePos();

            OnLeftMouseButtonDown(SVector2(mousePosition.x, mousePosition.y), !isHoldingMouse);

            isHoldingMouse = true;
        }
        else
        {
            isHoldingMouse = false;
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Tab))
        {
            if (gizmoMode == ImGuizmo::TRANSLATE)
            {
                gizmoMode = ImGuizmo::ROTATE;
            }
            else if (gizmoMode == ImGuizmo::ROTATE)
            {
                gizmoMode = ImGuizmo::SCALE;
            }
            else if (gizmoMode == ImGuizmo::SCALE)
            {
                gizmoMode = ImGuizmo::TRANSLATE;
            }
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Space))
        {
            gizmoSpace = gizmoSpace == ImGuizmo::WORLD ? ImGuizmo::LOCAL : ImGuizmo::WORLD;
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Backspace))
        {
            ZEntityRef entityRef;

            OnSelectEntity(entityRef);
        }
    }

    ImGuizmo::Enable(hasFocus);

    if (selectedentityTreeNode && selectedentityTreeNode->entityRef.GetEntityTypePtrPtr())
    {
        ZSpatialEntity* spatialEntity = selectedentityTreeNode->entityRef.QueryInterfacePtr<ZSpatialEntity>();

        if (spatialEntity)
        {
            ZCameraEntity* activeCamera = ZApplicationEngineWin32::GetInstance()->GetActiveCamera();
            SMatrix modelMatrix = spatialEntity->GetObjectToWorldMatrix();
            SMatrix viewMatrix = activeCamera->GetViewMatrix();
            const SMatrix projectionMatrix = activeCamera->GetProjectionMatrix();

            ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

            if (ImGuizmo::Manipulate(&viewMatrix.XAxis.x, &projectionMatrix.XAxis.x, gizmoMode, gizmoSpace, &modelMatrix.XAxis.x, NULL, useSnap ? &snapValue[0] : NULL))
            {
                OnEntityTransformChange(selectedentityTreeNode->entityRef, modelMatrix, false);
            }
        }
    }
}

void Editor::RenderEntityAABB()
{
    ZSpatialEntity* spatialEntity = selectedentityTreeNode->entityRef.QueryInterfacePtr<ZSpatialEntity>();

    if (!spatialEntity)
    {
        return;
    }

    SMatrix transform = spatialEntity->GetObjectToWorldMatrix();
    float4 min, max;

    spatialEntity->CalculateBounds(min, max, 1, 0);

    SDK::GetInstance().GetDirectXRenderer()->DrawOBB3D(SVector3(min.x, min.y, min.z), SVector3(max.x, max.y, max.z), transform, SVector4(0.f, 0.f, 1.f, 1.f));
}

void Editor::RenderBlueprintNodesAndPins(const bool hasFocus)
{
    if (!hasFocus ||
        !isOpen ||
        !selectedentityTreeNode ||
        !selectedentityTreeNode->entityRef.GetEntityTypePtrPtr() ||
        !selectedentityTreeNode->entityRef.GetInputPins() ||
        !selectedentityTreeNode->entityRef.GetOutputPins())
    {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();

    float bottomWindowPosX = 700;
    float bottomWindowWidth = (io.DisplaySize.x - 600) - 700;
    float bottomWindowHeight = io.DisplaySize.y / 3;

    ImGui::SetNextWindowPos({ bottomWindowPosX, 110 }, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({ bottomWindowWidth, bottomWindowHeight }, ImGuiCond_FirstUseEver);
    ImGui::PushFont(SDK::GetInstance().GetBoldFont());

    const bool isWindowVisible = ImGui::Begin(ICON_MD_BUILD " Blueprints Graph View", &isOpen, ImGuiWindowFlags_HorizontalScrollbar);

    if (!isWindowVisible)
    {
        ImGui::PopFont();
        ImGui::End();

        return;
    }

    if (nodes.size() == 0)
    {
        unsigned int rootEntityNodeIndex = -1;

        AddBlueprintNodesAndPins(selectedentityTreeNode->tbluRuntimeResourceID, rootEntityNodeIndex);

        if (selectedentityTreeNode->entityIndexInReferencedTEMP != -1)
        {
            ZResourcePtr tbluResourcePtr = ResourceManager->GetResourcePtr(selectedentityTreeNode->tbluRuntimeResourceID, 0);
            ZResourcePtr referenceResourcePtr = tbluResourcePtr.GetResourceStub()->GetInstallDependency(selectedentityTreeNode->entityTypeResourceIndex);

            if (referenceResourcePtr.GetResourceStub()->GetResourceTag() == 'TBLU')
            {
                AddBlueprintNodesAndPins(referenceResourcePtr.GetResourceStub()->GetRuntimeResourceID(), rootEntityNodeIndex);
            }
        }

        BuildNodes();
    }

    ImGui::PushFont(SDK::GetInstance().GetRegularFont());

    ax::NodeEditor::SetCurrentEditor(editor);

    static ax::NodeEditor::NodeId contextNodeId = 0;
    static ax::NodeEditor::LinkId contextLinkId = 0;
    static ax::NodeEditor::PinId  contextPinId = 0;
    static bool createNewNode = false;
    static Pin* newNodeLinkPin = nullptr;
    static Pin* newLinkPin = nullptr;

    ax::NodeEditor::Begin("Node editor");
    {
        const ImVec2 cursorTopLeft = ImGui::GetCursorScreenPos();

        ax::NodeEditor::Utilities::BlueprintNodeBuilder builder(headerBackgroundTextureView, headerBackgroundTextureWidth, headerBackgroundTextureHeight);

        for (auto& node : nodes)
        {
            builder.Begin(node.id);

            builder.Header(node.color);
            ImGui::Spring(0);
            ImGui::TextUnformatted(node.name.c_str());
            ImGui::Spring(1);
            ImGui::Dummy(ImVec2(0, 28));
            ImGui::Spring(0);
            builder.EndHeader();

            for (auto& input : node.inputs)
            {
                float alpha = ImGui::GetStyle().Alpha;

                if (newLinkPin && !CanCreateLink(newLinkPin, &input) && &input != newLinkPin)
                {
                    alpha = alpha * (48.0f / 255.0f);
                }

                builder.Input(input.id);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                DrawPinIcon(input, IsPinLinked(input.id), (int)(alpha * 255));
                ImGui::Spring(0);

                if (!input.name.empty())
                {
                    ImGui::TextUnformatted(input.name.c_str());
                    ImGui::Spring(0);
                }

                ImGui::PopStyleVar();
                builder.EndInput();
            }

            for (auto& output : node.outputs)
            {
                auto alpha = ImGui::GetStyle().Alpha;

                if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
                {
                    alpha = alpha * (48.0f / 255.0f);
                }

                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                builder.Output(output.id);

                if (!output.name.empty())
                {
                    ImGui::Spring(0);
                    ImGui::TextUnformatted(output.name.c_str());
                }

                ImGui::Spring(0);
                DrawPinIcon(output, IsPinLinked(output.id), (int)(alpha * 255));
                ImGui::PopStyleVar();
                builder.EndOutput();
            }

            builder.End();
        }

        for (auto& link : links)
        {
            ax::NodeEditor::Link(link.id, link.startPinID, link.endPinID, link.color, 2.0f);
        }

        if (!createNewNode)
        {
            if (ax::NodeEditor::BeginCreate(ImColor(255, 255, 255), 2.0f))
            {
                auto showLabel = [](const char* label, ImColor color)
                {
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
                    auto size = ImGui::CalcTextSize(label);

                    auto padding = ImGui::GetStyle().FramePadding;
                    auto spacing = ImGui::GetStyle().ItemSpacing;

                    ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

                    auto rectMin = ImGui::GetCursorScreenPos() - padding;
                    auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

                    auto drawList = ImGui::GetWindowDrawList();
                    drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
                    ImGui::TextUnformatted(label);
                };

                ax::NodeEditor::PinId startPinId = 0, endPinId = 0;

                if (ax::NodeEditor::QueryNewLink(&startPinId, &endPinId))
                {
                    auto startPin = FindPin(startPinId);
                    auto endPin = FindPin(endPinId);

                    newLinkPin = startPin ? startPin : endPin;

                    if (startPin->kind == PinKind::Input)
                    {
                        std::swap(startPin, endPin);
                        std::swap(startPinId, endPinId);
                    }

                    if (startPin && endPin)
                    {
                        if (endPin == startPin)
                        {
                            ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        }
                        else if (endPin->kind == startPin->kind)
                        {
                            showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                            ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        }
                        //else if (endPin->Node == startPin->Node)
                        //{
                        //    showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                        //    ax::NodeEditor::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                        //}
                        else if (endPin->type != startPin->type)
                        {
                            showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                            ax::NodeEditor::RejectNewItem(ImColor(255, 128, 128), 1.0f);
                        }
                        else
                        {
                            showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                            if (ax::NodeEditor::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
                            {
                                links.emplace_back(Link(GetNextID(), startPinId, endPinId));
                                links.back().color = GetIconColor(startPin->type);
                            }
                        }
                    }
                }

                ax::NodeEditor::PinId pinId = 0;

                if (ax::NodeEditor::QueryNewNode(&pinId))
                {
                    newLinkPin = FindPin(pinId);

                    if (newLinkPin)
                    {
                        showLabel("+ Create Node", ImColor(32, 45, 32, 180));
                    }

                    if (ax::NodeEditor::AcceptNewItem())
                    {
                        createNewNode = true;
                        newNodeLinkPin = FindPin(pinId);
                        newLinkPin = nullptr;
                        ax::NodeEditor::Suspend();
                        ImGui::OpenPopup("Create New Node");
                        ax::NodeEditor::Resume();
                    }
                }
            }
            else
            {
                newLinkPin = nullptr;
            }

            ax::NodeEditor::EndCreate();

            if (ax::NodeEditor::BeginDelete())
            {
                ax::NodeEditor::NodeId nodeId = 0;

                while (ax::NodeEditor::QueryDeletedNode(&nodeId))
                {
                    if (ax::NodeEditor::AcceptDeletedItem())
                    {
                        auto id = std::find_if(nodes.begin(), nodes.end(), [nodeId](auto& node) { return node.id == nodeId; });

                        if (id != nodes.end())
                        {
                            nodes.erase(id);
                        }
                    }
                }

                ax::NodeEditor::LinkId linkId = 0;

                while (ax::NodeEditor::QueryDeletedLink(&linkId))
                {
                    if (ax::NodeEditor::AcceptDeletedItem())
                    {
                        auto id = std::find_if(links.begin(), links.end(), [linkId](auto& link) { return link.id == linkId; });

                        if (id != links.end())
                        {
                            links.erase(id);
                        }
                    }
                }
            }

            ax::NodeEditor::EndDelete();
        }

        ImGui::SetCursorScreenPos(cursorTopLeft);
    }

# if 0
    auto openPopupPosition = ImGui::GetMousePos();
    ax::NodeEditor::Suspend();
    if (ax::NodeEditor::ShowNodeContextMenu(&contextNodeId))
        ImGui::OpenPopup("Node Context Menu");
    else if (ax::NodeEditor::ShowPinContextMenu(&contextPinId))
        ImGui::OpenPopup("Pin Context Menu");
    else if (ax::NodeEditor::ShowLinkContextMenu(&contextLinkId))
        ImGui::OpenPopup("Link Context Menu");
    else if (ax::NodeEditor::ShowBackgroundContextMenu())
    {
        ImGui::OpenPopup("Create New Node");
        newNodeLinkPin = nullptr;
    }
    ax::NodeEditor::Resume();

    ax::NodeEditor::Suspend();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopup("Node Context Menu"))
    {
        auto node = FindNode(contextNodeId);

        ImGui::TextUnformatted("Node Context Menu");
        ImGui::Separator();
        if (node)
        {
            ImGui::Text("ID: %p", node->id.AsPointer());
            //ImGui::Text("Type: %s", node->type == NodeType::Blueprint ? "Blueprint" : (node->Type == NodeType::Tree ? "Tree" : "Comment"));
            ImGui::Text("Inputs: %d", (int)node->inputs.size());
            ImGui::Text("Outputs: %d", (int)node->outputs.size());
        }
        else
            ImGui::Text("Unknown node: %p", contextNodeId.AsPointer());
        ImGui::Separator();
        if (ImGui::MenuItem("Delete"))
            ax::NodeEditor::DeleteNode(contextNodeId);
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Pin Context Menu"))
    {
        auto pin = FindPin(contextPinId);

        ImGui::TextUnformatted("Pin Context Menu");
        ImGui::Separator();
        if (pin)
        {
            ImGui::Text("ID: %p", pin->id.AsPointer());
            if (pin->node)
                ImGui::Text("Node: %p", pin->node->id.AsPointer());
            else
                ImGui::Text("Node: %s", "<none>");
        }
        else
            ImGui::Text("Unknown pin: %p", contextPinId.AsPointer());

        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Link Context Menu"))
    {
        auto link = FindLink(contextLinkId);

        ImGui::TextUnformatted("Link Context Menu");
        ImGui::Separator();
        if (link)
        {
            ImGui::Text("ID: %p", link->id.AsPointer());
            ImGui::Text("From: %p", link->startPinID.AsPointer());
            ImGui::Text("To: %p", link->endPinID.AsPointer());
        }
        else
            ImGui::Text("Unknown link: %p", contextLinkId.AsPointer());
        ImGui::Separator();
        if (ImGui::MenuItem("Delete"))
            ax::NodeEditor::DeleteLink(contextLinkId);
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Create New Node"))
    {
        auto newNodePostion = openPopupPosition;

        Node* node = nullptr;
        if (ImGui::MenuItem("Input Action"))
            node = SpawnInputActionNode();
        if (ImGui::MenuItem("Output Action"))
            node = SpawnOutputActionNode();
        if (ImGui::MenuItem("Branch"))
            node = SpawnBranchNode();
        if (ImGui::MenuItem("Do N"))
            node = SpawnDoNNode();
        if (ImGui::MenuItem("Set Timer"))
            node = SpawnSetTimerNode();
        if (ImGui::MenuItem("Less"))
            node = SpawnLessNode();
        if (ImGui::MenuItem("Weird"))
            node = SpawnWeirdNode();
        if (ImGui::MenuItem("Trace by Channel"))
            node = SpawnTraceByChannelNode();
        if (ImGui::MenuItem("Print String"))
            node = SpawnPrintStringNode();
        ImGui::Separator();
        if (ImGui::MenuItem("Comment"))
            node = SpawnComment();
        ImGui::Separator();
        if (ImGui::MenuItem("Sequence"))
            node = SpawnTreeSequenceNode();
        if (ImGui::MenuItem("Move To"))
            node = SpawnTreeTaskNode();
        if (ImGui::MenuItem("Random Wait"))
            node = SpawnTreeTask2Node();
        ImGui::Separator();
        if (ImGui::MenuItem("Message"))
            node = SpawnMessageNode();
        ImGui::Separator();
        if (ImGui::MenuItem("Transform"))
            node = SpawnHoudiniTransformNode();
        if (ImGui::MenuItem("Group"))
            node = SpawnHoudiniGroupNode();

        if (node)
        {
            BuildNodes();

            createNewNode = false;

            ax::NodeEditor::SetNodePosition(node->ID, newNodePostion);

            if (auto startPin = newNodeLinkPin)
            {
                auto& pins = startPin->Kind == PinKind::Input ? node->Outputs : node->Inputs;

                for (auto& pin : pins)
                {
                    if (CanCreateLink(startPin, &pin))
                    {
                        auto endPin = &pin;
                        if (startPin->Kind == PinKind::Input)
                            std::swap(startPin, endPin);

                        m_Links.emplace_back(Link(GetNextId(), startPin->ID, endPin->ID));
                        m_Links.back().Color = GetIconColor(startPin->Type);

                        break;
                    }
                }
            }
        }

        ImGui::EndPopup();
    }
    else
        createNewNode = false;
    ImGui::PopStyleVar();
    ax::NodeEditor::Resume();
# endif

    ax::NodeEditor::End();

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

void Editor::AddChildren(std::shared_ptr<EntityTreeNode> entityTreeNode, ZEntityRef rootEntity, ZTemplateEntityBlueprintFactory* templateEntityBlueprintFactory, const int parentIndex, STemplateEntityBlueprint* templateEntityBlueprint, bool deleteTemplateEntityBlueprint)
{
    ZEntitySceneContext* entitySceneContext = Hitman5Module->GetSceneContext();
    const ZRuntimeResourceID& tbluRuntimeResourceID = templateEntityBlueprintFactory->GetRuntimeResourceID();
    const ZRuntimeResourceID& headerLibraryRuntimeResourceID = entitySceneContext->GetSceneHeaderLibrary().GetResourceStub()->GetRuntimeResourceID();
    void* tbluResourceData = nullptr;
    unsigned char tbluResourceAlignment = 0;

    if (!templateEntityBlueprints.contains(tbluRuntimeResourceID))
    {
        unsigned int tbluResourceDataSize = 0;

        if (tbluRuntimeResourceID.IsLibraryResource())
        {
            ResourceUtility::LoadResource(tbluRuntimeResourceID, headerLibraryRuntimeResourceID, tbluResourceData, tbluResourceDataSize);
        }
        else
        {
            ResourceUtility::LoadResource(tbluRuntimeResourceID, 0x00A6B9335EB4770D, tbluResourceData, tbluResourceDataSize);
        }

        ZBinaryDeserializer binaryDeserializer;
        templateEntityBlueprint = static_cast<STemplateEntityBlueprint*>(binaryDeserializer.Deserialize(tbluResourceData, tbluResourceDataSize));
        tbluResourceAlignment = binaryDeserializer.GetAlignment();
    }
    else
    {
        templateEntityBlueprint = &templateEntityBlueprints[tbluRuntimeResourceID];
    }

    for (size_t i = 0; i < templateEntityBlueprint->entityTemplates.Size(); ++i)
    {
        if (templateEntityBlueprint->entityTemplates[i].parentIndex == parentIndex)
        {
            ZEntityRef entityRef = templateEntityBlueprintFactory->GetSubEntity(rootEntity.GetEntityTypePtrPtr(), i);
            IEntityBlueprintFactory* entityBlueprintFactory2 = templateEntityBlueprintFactory->GetBlueprintResource(i);
            std::shared_ptr<EntityTreeNode> childNode = std::make_shared<EntityTreeNode>(i, templateEntityBlueprint->entityTemplates[i].entityName.ToCString(), entityRef, tbluRuntimeResourceID, templateEntityBlueprint->entityTemplates[i].entityTypeResourceIndex);

            childNode->parentNode = entityTreeNode;

            AddChildren(childNode, rootEntity, templateEntityBlueprintFactory, childNode->entityIndex, templateEntityBlueprint, false);

            if (*reinterpret_cast<void**>(entityBlueprintFactory2) == ZTemplateEntityBlueprintFactoryVFTbl)
            {
                ZTemplateEntityBlueprintFactory* templateEntityBlueprintFactory2 = static_cast<ZTemplateEntityBlueprintFactory*>(entityBlueprintFactory2);

                childNode->entityIndexInReferencedTEMP = templateEntityBlueprintFactory2->GetRootEntityIndex();

                AddChildren(childNode, childNode->entityRef, templateEntityBlueprintFactory2, childNode->entityIndexInReferencedTEMP);

                //Case when referenced TBLU only has root entity
                if (childNode->children.size() == 0)
                {
                    ZEntityRef entityRef2 = templateEntityBlueprintFactory2->GetSubEntity(childNode->entityRef.GetEntityTypePtrPtr(), childNode->entityIndexInReferencedTEMP);
                    IEntityBlueprintFactory* entityBlueprintFactory3 = templateEntityBlueprintFactory2->GetBlueprintResource(childNode->entityIndexInReferencedTEMP);

                    if (*reinterpret_cast<void**>(entityBlueprintFactory3) == ZTemplateEntityBlueprintFactoryVFTbl)
                    {
                        ZTemplateEntityBlueprintFactory* templateEntityBlueprintFactory3 = static_cast<ZTemplateEntityBlueprintFactory*>(entityBlueprintFactory3);

                        childNode->entityIndexInReferencedTEMP = templateEntityBlueprintFactory3->GetRootEntityIndex();

                        AddChildren(childNode, entityRef2, templateEntityBlueprintFactory3, childNode->entityIndexInReferencedTEMP);
                    }
                    else if (*reinterpret_cast<void**>(entityBlueprintFactory3) == ZAspectEntityBlueprintFactoryVFTbl)
                    {
                        ZAspectEntityBlueprintFactory* aspectEntityBlueprintFactory = static_cast<ZAspectEntityBlueprintFactory*>(entityBlueprintFactory3);
                        IEntityBlueprintFactory* aspectFactory = aspectEntityBlueprintFactory->GetAspectFactory(0);

                        if (*reinterpret_cast<void**>(aspectFactory) == ZTemplateEntityBlueprintFactoryVFTbl)
                        {
                            ZTemplateEntityBlueprintFactory* templateEntityBlueprintFactory3 = static_cast<ZTemplateEntityBlueprintFactory*>(aspectFactory);

                            childNode->entityIndexInReferencedTEMP = templateEntityBlueprintFactory3->GetRootEntityIndex();

                            AddChildren(childNode, childNode->entityRef, templateEntityBlueprintFactory3, childNode->entityIndexInReferencedTEMP);
                        }
                    }
                }
            }
            else if (*reinterpret_cast<void**>(entityBlueprintFactory2) == ZAspectEntityBlueprintFactoryVFTbl)
            {
                ZAspectEntityBlueprintFactory* aspectEntityBlueprintFactory = static_cast<ZAspectEntityBlueprintFactory*>(entityBlueprintFactory2);
                IEntityBlueprintFactory* aspectFactory = aspectEntityBlueprintFactory->GetAspectFactory(0);

                if (*reinterpret_cast<void**>(aspectFactory) == ZTemplateEntityBlueprintFactoryVFTbl)
                {
                    ZTemplateEntityBlueprintFactory* templateEntityBlueprintFactory2 = static_cast<ZTemplateEntityBlueprintFactory*>(aspectFactory);

                    childNode->entityIndexInReferencedTEMP = templateEntityBlueprintFactory2->GetRootEntityIndex();

                    AddChildren(childNode, childNode->entityRef, templateEntityBlueprintFactory2, childNode->entityIndexInReferencedTEMP);
                }
            }

            if (childNode->children.size() > 0)
            {
                childNode->hasChildren = true;
            }

            entityTreeNode->children.push_back(childNode);
        }
    }

    if (entityTreeNode->children.size() > 0)
    {
        std::sort(entityTreeNode->children.begin(), entityTreeNode->children.end(), [](const std::shared_ptr<EntityTreeNode> a, const std::shared_ptr<EntityTreeNode> b)
        {
            return a->entityName < b->entityName;
        });
    }
    else
    {
        entityTreeNode->hasChildren = false;
    }
}

std::shared_ptr<Editor::EntityTreeNode> Editor::FindNode(const ZEntityRef& entityRef, std::shared_ptr<EntityTreeNode> entityTreeNode) const
{
    if (entityTreeNode->entityRef == entityRef)
    {
        return entityTreeNode;
    }

    for (size_t i = 0; i < entityTreeNode->children.size(); ++i)
    {
        if (entityTreeNode->children[i]->entityRef == entityRef)
        {
            return entityTreeNode->children[i];
        }

        std::shared_ptr<EntityTreeNode> entityTreeNode2 = FindNode(entityRef, entityTreeNode->children[i]);

        if (entityTreeNode2)
        {
            return entityTreeNode2;
        }
    }

    return nullptr;
}

void Editor::OnSelectEntity(ZEntityRef entityRef)
{
    if (entityRef.GetEntityTypePtrPtr())
    {
        selectedentityTreeNode = FindNode(entityRef, rootNode);
        scrollToEntity = true;
    }
    else
    {
        selectedentityTreeNode.reset();
        scrollToEntity = false;
    }
}

void Editor::OnLeftMouseButtonDown(const SVector2& mousePosition, const bool isFirstClick)
{
    SVector3 worldPosition;
    SVector3 direction;

    SDK::GetInstance().GetDirectXRenderer()->ScreenToWorld(mousePosition, worldPosition, direction);

    const float4 direction2 = float4(direction.x, direction.y, direction.z, 1.f);
    const float4 from = float4(worldPosition.x, worldPosition.y, worldPosition.z, 1.f);
    const float4 to = from + direction2 * 200.f;

    if (!CollisionManager)
    {
        return;
    }

    ZRayQueryInput rayQueryInput = ZRayQueryInput(from, to, RAYDETAILS_MESH);
    ZRayQueryOutput rayQueryOutput{};

    if (!CollisionManager->RayCastClosestHit(rayQueryInput, &rayQueryOutput))
    {
        return;
    }

    if (isFirstClick && rayQueryOutput.GetBlockingEntity().GetEntityTypePtrPtr())
    {
        const ZEntityRef selectedEntity = rayQueryOutput.GetBlockingEntity();

        OnSelectEntity(selectedEntity);

        nodes.clear();
        links.clear();
    }
}

void Editor::OnEntityTransformChange(ZEntityRef entityRef, const SMatrix& transform, bool relative)
{
    ZSpatialEntity* spatialEntity = selectedentityTreeNode->entityRef.QueryInterfacePtr<ZSpatialEntity>();

    if (!spatialEntity)
    {
        return;
    }

    if (!relative)
    {
        spatialEntity->SetObjectToWorldMatrix(transform);
    }
    else
    {
        ZSpatialEntity* transformParent = spatialEntity->GetTransformParent();
        SMatrix parentTransform;

        if (transformParent)
        {
            parentTransform = transformParent->GetObjectToWorldMatrix();
        }

        SMatrix worldTransform = transform * parentTransform;

        worldTransform.Trans = parentTransform.Trans + transform.Trans;
        worldTransform.Trans.w = 1.f;

        spatialEntity->SetObjectToWorldMatrix(worldTransform);
    }
}

void Editor::OnSetPropertyValue(ZEntityRef entityRef, const unsigned int propertyID, const ZVariant& value)
{
    entityRef.SetProperty(propertyID, value);
}

void Editor::BoolProperty(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data)
{
    bool value = *static_cast<bool*>(data);

    if (ImGui::Checkbox(id.c_str(), &value))
    {
        ZVariant variant;

        variant.Set<bool>(value);

        OnSetPropertyValue(entityRef, propertyID, variant);
    }
}

void Editor::Uint8Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data)
{
    unsigned char value = *static_cast<unsigned char*>(data);

    if (ImGui::InputScalar(id.c_str(), ImGuiDataType_U8, &value))
    {
        ZVariant variant;

        variant.Set<unsigned char>(value);

        OnSetPropertyValue(entityRef, propertyID, variant);
    }
}

void Editor::Uint16Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data)
{
    unsigned short value = *static_cast<unsigned short*>(data);

    if (ImGui::InputScalar(id.c_str(), ImGuiDataType_U16, &value))
    {
        ZVariant variant;

        variant.Set<unsigned short>(value);

        OnSetPropertyValue(entityRef, propertyID, variant);
    }
}

void Editor::Uint32Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data)
{
    unsigned int value = *static_cast<unsigned int*>(data);

    if (ImGui::InputScalar(id.c_str(), ImGuiDataType_U32, &value))
    {
        ZVariant variant;

        variant.Set<unsigned int>(value);

        OnSetPropertyValue(entityRef, propertyID, variant);
    }
}

void Editor::Uint64Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data)
{
    unsigned long long value = *static_cast<unsigned long long*>(data);

    if (ImGui::InputScalar(id.c_str(), ImGuiDataType_U64, &value))
    {
        ZVariant variant;

        variant.Set<unsigned long long>(value);

        OnSetPropertyValue(entityRef, propertyID, variant);
    }
}

void Editor::Int8Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data)
{
    char value = *static_cast<char*>(data);

    if (ImGui::InputScalar(id.c_str(), ImGuiDataType_S8, &value))
    {
        ZVariant variant;

        variant.Set<char>(value);

        OnSetPropertyValue(entityRef, propertyID, variant);
    }
}

void Editor::Int16Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data)
{
    short value = *static_cast<short*>(data);

    if (ImGui::InputScalar(id.c_str(), ImGuiDataType_S16, &value))
    {
        ZVariant variant;

        variant.Set<short>(value);

        OnSetPropertyValue(entityRef, propertyID, variant);
    }
}

void Editor::Int32Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data)
{
    int value = *static_cast<int*>(data);

    if (ImGui::InputScalar(id.c_str(), ImGuiDataType_S32, &value))
    {
        ZVariant variant;

        variant.Set<int>(value);

        OnSetPropertyValue(entityRef, propertyID, variant);
    }
}

void Editor::Int64Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data)
{
    long long value = *static_cast<long long*>(data);

    if (ImGui::InputScalar(id.c_str(), ImGuiDataType_S64, &value))
    {
        ZVariant variant;

        variant.Set<long long>(value);

        OnSetPropertyValue(entityRef, propertyID, variant);
    }
}

void Editor::Float32Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data)
{
    float value = *static_cast<float*>(data);

    if (ImGui::InputScalar(id.c_str(), ImGuiDataType_Float, &value))
    {
        ZVariant variant;

        variant.Set<float>(value);

        OnSetPropertyValue(entityRef, propertyID, variant);
    }
}

void Editor::Float64Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data)
{
    double value = *static_cast<double*>(data);

    if (ImGui::InputScalar(id.c_str(), ImGuiDataType_Double, &value))
    {
        ZVariant variant;

        variant.Set<double>(value);

        OnSetPropertyValue(entityRef, propertyID, variant);
    }
}

void Editor::EnumProperty(const std::string& id, const ZEntityRef entityRef, const SPropertyData* propertyData, void* data)
{
    int value = *static_cast<int*>(data);
    STypeID* typeID = propertyData->m_pInfo->m_Type;
    const char* typeName = typeID->pTypeInfo->GetTypeName();
    std::map<int, std::string>* enumItems = EnumRegistry::GetInstance().GetEnum(typeName);

    std::string currentValue;

    for (auto it = enumItems->begin(); it != enumItems->end(); ++it)
    {
        if (it->first == value)
        {
            currentValue = it->second;

            break;
        }
    }

    if (ImGui::BeginCombo(id.c_str(), currentValue.c_str()))
    {
        for (auto it = enumItems->begin(); it != enumItems->end(); ++it)
        {
            if (ImGui::Selectable(it->second.c_str(), it->first == value))
            {
                OnSetPropertyValue(entityRef, propertyData->m_nPropertyID, ZVariant(typeID, &it->first));
            }
        }

        ImGui::EndCombo();
    }
}

void Editor::StringProperty(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data)
{
    ZString* string = static_cast<ZString*>(data);
    static char stringBuffer[2048] = {};

    memcpy(stringBuffer, string->ToCString(), string->Length());

    stringBuffer[string->Length() + 1] = '\0';

    if (ImGui::InputText(id.c_str(), stringBuffer, sizeof(stringBuffer)))
    {
        ZVariant variant;

        variant.Set<ZString>(stringBuffer);

        OnSetPropertyValue(entityRef, propertyID, variant);
    }
}

void Editor::SVector2Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data)
{
    SVector2 vector = *static_cast<SVector2*>(data);

    if (ImGui::InputFloat2(id.c_str(), &vector.x))
    {
        ZVariant variant;

        variant.Set<SVector2>(vector);

        OnSetPropertyValue(entityRef, propertyID, variant);
    }
}

void Editor::SVector3Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data)
{
    SVector3 vector = *static_cast<SVector3*>(data);

    if (ImGui::InputFloat3(id.c_str(), &vector.x))
    {
        ZVariant variant;

        variant.Set<SVector3>(vector);

        OnSetPropertyValue(entityRef, propertyID, variant);
    }
}

void Editor::SVector4Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data)
{
    SVector4 vector = *static_cast<SVector4*>(data);

    if (ImGui::InputFloat4(id.c_str(), &vector.x))
    {
        ZVariant variant;

        variant.Set<SVector4>(vector);

        OnSetPropertyValue(entityRef, propertyID, variant);
    }
}

void Editor::SMatrix43Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data)
{
    ImGui::NewLine();

    SMatrix43 matrix = *static_cast<SMatrix43*>(data);
    ZVariant variant;

    variant.Set<SMatrix43>(matrix);

    if (ImGui::InputFloat3((id + "x").c_str(), &matrix.XAxis.x))
    {
        OnSetPropertyValue(entityRef, propertyID, variant);
    }

    if (ImGui::InputFloat3((id + "y").c_str(), &matrix.YAxis.x))
    {
        OnSetPropertyValue(entityRef, propertyID, variant);
    }

    if (ImGui::InputFloat3((id + "z").c_str(), &matrix.ZAxis.x))
    {
        OnSetPropertyValue(entityRef, propertyID, variant);
    }

    if (ImGui::InputFloat3((id + "t").c_str(), &matrix.Trans.x))
    {
        OnSetPropertyValue(entityRef, propertyID, variant);
    }
}

void Editor::ResourceProperty(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data)
{
    const ZResourcePtr* resourcePtr = static_cast<ZResourcePtr*>(data);
    static char stringBuffer[2048] = {};

    if (resourcePtr->GetResourceStub())
    {
        const ZResourceID resourceID = ZRuntimeResourceID::QueryResourceID(resourcePtr->GetResourceStub()->GetRuntimeResourceID());

        memcpy(stringBuffer, resourceID.GetURI().ToCString(), resourceID.GetURI().Length() + 1);
    }
    else
    {
        stringBuffer[0] = '\0';
    }

    if (ImGui::InputText(id.c_str(), stringBuffer, sizeof(stringBuffer)))
    {
        const ZRuntimeResourceID runtimeResourceID = ZRuntimeResourceID::QueryRuntimeResourceID(stringBuffer);
        const ZResourcePtr resourcePtr2 = ResourceManager->GetResourcePtr(runtimeResourceID, 0);
        ZVariant variant;

        variant.Set<ZResourcePtr>(resourcePtr2);

        OnSetPropertyValue(entityRef, propertyID, variant);
    }
}

void Editor::EntityRefProperty(void* data)
{
    const ZEntityRef* entityRef = static_cast<ZEntityRef*>(data);

    if (entityRef->GetEntityTypePtrPtr())
    {
        const std::shared_ptr<EntityTreeNode> entityTreeNode = FindNode(*entityRef, rootNode);

        if (entityTreeNode)
        {
            ImGui::Text(entityTreeNode->entityName.c_str());
        }
    }
    else
    {
        ImGui::Text("");
    }
}

void Editor::UnsupportedProperty(const std::string& id, const ZEntityRef entityRef, const SPropertyData* propertyData, void* data)
{
    const std::string typeName = propertyData->m_pInfo->m_Type->pTypeInfo->GetTypeName();

    ImGui::Text(" %s (Unsupported)", typeName.c_str());
}

int Editor::GetNextID()
{
    return nextID++;
}

ax::NodeEditor::LinkId Editor::GetNextLinkID()
{
    return ax::NodeEditor::LinkId(GetNextID());
}

Editor::BlueprintNode* Editor::FindNode(const ax::NodeEditor::NodeId id)
{
    for (auto& node : nodes)
    {
        if (node.id == id)
        {
            return &node;
        }
    }

    return nullptr;
}

Editor::Link* Editor::FindLink(const ax::NodeEditor::LinkId id)
{
    for (auto& link : links)
    {
        if (link.id == id)
        {
            return &link;
        }
    }

    return nullptr;
}

Editor::Pin* Editor::FindPin(const ax::NodeEditor::PinId id)
{
    if (!id)
    {
        return nullptr;
    }

    for (auto& node : nodes)
    {
        for (auto& pin : node.inputs)
        {
            if (pin.id == id)
            {
                return &pin;
            }
        }

        for (auto& pin : node.outputs)
        {
            if (pin.id == id)
            {
                return &pin;
            }
        }
    }

    return nullptr;
}

bool Editor::IsPinLinked(const ax::NodeEditor::PinId id) const
{
    if (!id)
        return false;

    for (auto& link : links)
    {
        if (link.startPinID == id || link.endPinID == id)
        {
            return true;
        }
    }

    return false;
}

bool Editor::CanCreateLink(const Pin* a, const Pin* b) const
{
    if (!a || !b || a == b || a->kind == b->kind || a->type != b->type || a->node == b->node)
    {
        return false;
    }

    return true;
}

void Editor::BuildNode(BlueprintNode* node)
{
    for (auto& input : node->inputs)
    {
        input.node = node;
        input.kind = PinKind::Input;
    }

    for (auto& output : node->outputs)
    {
        output.node = node;
        output.kind = PinKind::Output;
    }
}

void Editor::BuildNodes()
{
    for (auto& node : nodes)
    {
        BuildNode(&node);
    }
}

ImColor Editor::GetIconColor(const PinType type)
{
    switch (type)
    {
        default:
        case PinType::Flow:     return ImColor(255, 255, 255);
        case PinType::Bool:     return ImColor(220, 48, 48);
        case PinType::Int:      return ImColor(68, 201, 156);
        case PinType::Float:    return ImColor(147, 226, 74);
        case PinType::String:   return ImColor(124, 21, 153);
        case PinType::Object:   return ImColor(51, 150, 215);
        case PinType::Function: return ImColor(218, 0, 183);
        case PinType::Delegate: return ImColor(255, 48, 48);
    }
}

void Editor::DrawPinIcon(const Pin& pin, const bool connected, const int alpha)
{
    ax::Drawing::IconType iconType;
    ImColor  color = GetIconColor(pin.type);
    color.Value.w = alpha / 255.0f;
    switch (pin.type)
    {
        case PinType::Flow:     iconType = ax::Drawing::IconType::Flow;   break;
        case PinType::Bool:     iconType = ax::Drawing::IconType::Circle; break;
        case PinType::Int:      iconType = ax::Drawing::IconType::Circle; break;
        case PinType::Float:    iconType = ax::Drawing::IconType::Circle; break;
        case PinType::String:   iconType = ax::Drawing::IconType::Circle; break;
        case PinType::Object:   iconType = ax::Drawing::IconType::Circle; break;
        case PinType::Function: iconType = ax::Drawing::IconType::Circle; break;
        case PinType::Delegate: iconType = ax::Drawing::IconType::Square; break;
        default:
            return;
    }

    ax::Widgets::Icon(ImVec2(static_cast<float>(pinIconSize), static_cast<float>(pinIconSize)), iconType, connected, color, ImColor(32, 32, 32, alpha));
}

void Editor::AddBlueprintNodesAndPins(const ZRuntimeResourceID& tbluRuntimeResourceID, unsigned int& rootEntityNodeIndex)
{
    STemplateEntityBlueprint* templateEntityBlueprint = &templateEntityBlueprints[tbluRuntimeResourceID];
    std::unordered_map<unsigned int, unsigned int> entityIndicesToNodeIndices;
    std::unordered_map<unsigned int, std::unordered_map<std::string, ax::NodeEditor::PinId>> nodeIndicesToPins;

    unsigned int selectedEntityIndex = selectedentityTreeNode->entityIndex;

    if (rootEntityNodeIndex != -1)
    {
        selectedEntityIndex = selectedentityTreeNode->entityIndexInReferencedTEMP;
    }

    for (size_t i = 0; i < templateEntityBlueprint->pinConnections.Size(); ++i)
    {
        const unsigned int fromEntityIndex = templateEntityBlueprint->pinConnections[i].fromID;
        const unsigned int toEntityIndex = templateEntityBlueprint->pinConnections[i].toID;

        if (fromEntityIndex == selectedEntityIndex || toEntityIndex == selectedEntityIndex)
        {
            const char* fromEntityName = templateEntityBlueprint->entityTemplates[fromEntityIndex].entityName.ToCString();
            const char* toEntityName = templateEntityBlueprint->entityTemplates[toEntityIndex].entityName.ToCString();
            const char* fromPinName = templateEntityBlueprint->pinConnections[i].fromPinName.ToCString();
            const char* toPinName = templateEntityBlueprint->pinConnections[i].toPinName.ToCString();

            auto entityIndicesToNodeIndicesIterator = entityIndicesToNodeIndices.find(fromEntityIndex);
            auto entityIndicesToNodeIndicesIterator2 = entityIndicesToNodeIndices.find(toEntityIndex);
            unsigned int fromNodeIndex = 0;
            unsigned int toNodeIndex = 0;

            if (rootEntityNodeIndex != -1)
            {
                if (fromEntityIndex == selectedEntityIndex)
                {
                    fromNodeIndex = rootEntityNodeIndex;
                }
            }
            else
            {
                if (entityIndicesToNodeIndicesIterator != entityIndicesToNodeIndices.end())
                {
                    fromNodeIndex = entityIndicesToNodeIndicesIterator->second;
                }
                else
                {
                    nodes.emplace_back(GetNextID(), fromEntityName, ImColor(128, 195, 248));

                    fromNodeIndex = nodes.size() - 1;
                    entityIndicesToNodeIndices[fromEntityIndex] = fromNodeIndex;
                }
            }

            if (rootEntityNodeIndex != -1)
            {
                if (fromEntityIndex == selectedEntityIndex)
                {
                    toNodeIndex = rootEntityNodeIndex;
                }
            }
            else
            {
                if (entityIndicesToNodeIndicesIterator2 != entityIndicesToNodeIndices.end())
                {
                    toNodeIndex = entityIndicesToNodeIndicesIterator2->second;
                }
                else
                {
                    nodes.emplace_back(GetNextID(), toEntityName, ImColor(128, 195, 248));

                    toNodeIndex = nodes.size() - 1;
                    entityIndicesToNodeIndices[toEntityIndex] = toNodeIndex;
                }
            }

            std::unordered_map<std::string, ax::NodeEditor::PinId>& pins = nodeIndicesToPins[fromNodeIndex];
            std::unordered_map<std::string, ax::NodeEditor::PinId>& pins2 = nodeIndicesToPins[toNodeIndex];
            auto pinsIterator = pins.find(fromPinName);
            auto pinsIterator2 = pins2.find(toPinName);
            ax::NodeEditor::PinId fromPinID;
            ax::NodeEditor::PinId toPinID;

            if (pinsIterator != pins.end())
            {
                fromPinID = pinsIterator->second;
            }
            else
            {
                nodes[fromNodeIndex].outputs.emplace_back(GetNextID(), fromPinName, PinType::Flow);

                fromPinID = nodes[fromNodeIndex].outputs.back().id;
                pins[fromPinName] = fromPinID;
            }

            if (pinsIterator2 != pins2.end())
            {
                toPinID = pinsIterator2->second;
            }
            else
            {
                nodes[toNodeIndex].inputs.emplace_back(GetNextID(), toPinName, PinType::Flow);

                toPinID = nodes[toNodeIndex].inputs.back().id;
                pins2[toPinName] = toPinID;
            }

            links.push_back(Link(GetNextLinkID(), fromPinID, toPinID));
        }
    }

    if (rootEntityNodeIndex == -1)
    {
        rootEntityNodeIndex = entityIndicesToNodeIndices[selectedEntityIndex];
    }
}

void __fastcall ZTemplateEntityBlueprintFactory_ZTemplateEntityBlueprintFactoryHook(ZTemplateEntityBlueprintFactory* pThis, int edx, STemplateEntityBlueprint* pTemplateEntityBlueprint, ZResourcePending& ResourcePending)
{
    GetModInstance()->OnTemplateEntityBlueprintFactoryCreate(pTemplateEntityBlueprint, ResourcePending);

    Hooks::ZTemplateEntityBlueprintFactory_ZTemplateEntityBlueprintFactory.CallOriginalFunction(pThis, pTemplateEntityBlueprint, ResourcePending);
}

void __fastcall ZEntitySceneContext_CreateSceneHook(ZEntitySceneContext* pThis, int edx, const ZString& sStreamingState)
{
    GetModInstance()->OnCreateScene(pThis, sStreamingState);

    Hooks::ZEntitySceneContext_CreateScene.CallOriginalFunction(pThis, sStreamingState);
}

void __fastcall ZEntitySceneContext_ClearSceneHook(ZEntitySceneContext* pThis, int edx, bool bFullyUnloadScene)
{
    GetModInstance()->OnClearScene(pThis, bFullyUnloadScene);

    Hooks::ZEntitySceneContext_ClearScene.CallOriginalFunction(pThis, bFullyUnloadScene);
}

DEFINE_MOD(Editor);
