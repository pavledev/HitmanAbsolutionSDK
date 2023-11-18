#pragma once

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include <unordered_map>

#include <imgui.h>

#include <ImGuizmo.h>

#include <imgui_node_editor.h>

#include <Glacier/Entity/ZEntityRef.h>
#include <Glacier/Resource/ZRuntimeResourceID.h>

#include <ModInterface.h>

class ZTemplateEntityBlueprintFactory;
struct STemplateEntityBlueprint;
class ZResourcePending;
class ZEntitySceneContext;

void __fastcall ZTemplateEntityBlueprintFactory_ZTemplateEntityBlueprintFactoryHook(ZTemplateEntityBlueprintFactory* pThis, int edx, STemplateEntityBlueprint* pTemplateEntityBlueprint, ZResourcePending& ResourcePending);
void __fastcall ZEntitySceneContext_CreateSceneHook(ZEntitySceneContext* pThis, int edx, const ZString& sStreamingState);
void __fastcall ZEntitySceneContext_ClearSceneHook(ZEntitySceneContext* pThis, int edx, bool bFullyUnloadScene);

class Editor : public ModInterface
{
public:
    Editor();
    ~Editor();
    void Initialize() override;
    void OnEngineInitialized() override;
    void OnDrawMenu() override;
    void OnDrawUI(const bool hasFocus) override;
    void OnDraw3D() override;

    void OnTemplateEntityBlueprintFactoryCreate(STemplateEntityBlueprint* templateEntityBlueprint, ZResourcePending& resourcePending);
    void OnCreateScene(ZEntitySceneContext* entitySceneContext, const ZString& streamingState);
    void OnClearScene(ZEntitySceneContext* entitySceneContext, bool fullyUnloadScene);

private:
    struct PinConnection
    {
        std::string fromPinName;
        std::string toPinName;
        std::string fromEntityName;
        std::string toEntityName;
    };

    struct EntityTreeNode
    {
        EntityTreeNode();
        EntityTreeNode(const unsigned int entityIndex, const char* entityName, const ZEntityRef entityRef, const ZRuntimeResourceID& tbluRuntimeResourceID, const int entityTypeResourceIndex);
        const bool IsParent(std::shared_ptr<EntityTreeNode> entityTreeNode) const;

        unsigned int entityIndex;
        unsigned int entityIndexInReferencedTEMP;
        std::shared_ptr<EntityTreeNode> parentNode;
        std::string entityName;
        ZEntityRef entityRef;
        std::vector<std::shared_ptr<EntityTreeNode>> children;
        bool hasChildren;
        ZRuntimeResourceID tbluRuntimeResourceID;
        int entityTypeResourceIndex;
    };

    enum class PinType
    {
        Flow,
        Bool,
        Int,
        Float,
        String,
        Object,
        Function,
        Delegate,
    };

    enum class PinKind
    {
        Output,
        Input
    };

    struct BlueprintNode;

    struct Pin
    {
        Pin(int id, const char* name, PinType type);

        ax::NodeEditor::PinId id;
        BlueprintNode* node;
        std::string name;
        PinType type;
        PinKind kind;
    };

    struct BlueprintNode
    {
        BlueprintNode(int id, const char* name, ImColor color = ImColor(255, 255, 255));

        ax::NodeEditor::NodeId id;
        std::string name;
        std::vector<Pin> inputs;
        std::vector<Pin> outputs;
        ImColor color;
        ImVec2 size;
    };

    struct Link
    {
        Link(ax::NodeEditor::LinkId id, ax::NodeEditor::PinId startPinId, ax::NodeEditor::PinId endPinId);

        ax::NodeEditor::LinkId id;
        ax::NodeEditor::PinId startPinID;
        ax::NodeEditor::PinId endPinID;
        ImColor color;
    };

    void RenderEntityTree(const bool hasFocus);
    void RenderEntityTree(std::shared_ptr<EntityTreeNode> entityTreeNode, const bool isTreeFiltered);
    void RenderEntityProperties(const bool hasFocus);
    void RenderGizmo(const bool hasFocus);
    void RenderEntityAABB();
    void RenderBlueprintNodesAndPins(const bool hasFocus);

    void AddChildren(std::shared_ptr<EntityTreeNode> entityTreeNode, ZEntityRef rootEntity, ZTemplateEntityBlueprintFactory* templateEntityBlueprintFactory, const int parentIndex = -1, STemplateEntityBlueprint* templateEntityBlueprint = nullptr, bool deleteTemplateEntityBlueprint = true);
    std::shared_ptr<EntityTreeNode> FindNode(const ZEntityRef& entityRef, std::shared_ptr<EntityTreeNode> entityTreeNode) const;
    void SearchEntityNameInTree(std::shared_ptr<EntityTreeNode> node, const std::string& entityName, std::unordered_map<std::shared_ptr<EntityTreeNode>, std::shared_ptr<EntityTreeNode>>& parentMap);
    void SearchTypeNameInTree(std::shared_ptr<EntityTreeNode> node, const std::string& typeName, std::unordered_map<std::shared_ptr<EntityTreeNode>, std::shared_ptr<EntityTreeNode>>& parentMap);
    std::shared_ptr<EntityTreeNode> GeneratedFilteredEntityTree(const std::unordered_map<std::shared_ptr<EntityTreeNode>, std::shared_ptr<EntityTreeNode>>& parentMap);

    void OnSelectEntity(ZEntityRef entityRef);
    void OnLeftMouseButtonDown(const SVector2& mousePosition, const bool isFirstClick);
    void OnEntityTransformChange(ZEntityRef entityRef, const SMatrix& transform, bool relative);
    void OnSetPropertyValue(ZEntityRef entityRef, const unsigned int propertyID, const ZVariant& value);
    void OnSignalEntityPin(ZEntityRef entityRef, const std::string& pinName, const bool isInputPin);

    void BoolProperty(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data);
    void Uint8Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data);
    void Uint16Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data);
    void Uint32Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data);
    void Uint64Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data);
    void Int8Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data);
    void Int16Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data);
    void Int32Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data);
    void Int64Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data);
    void Float32Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data);
    void Float64Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data);
    void EnumProperty(const std::string& id, const ZEntityRef entityRef, const SPropertyData* propertyData, void* data);
    void StringProperty(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data);

    void SVector2Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data);
    void SVector3Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data);
    void SVector4Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data);

    void SMatrix43Property(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data);

    void ResourceProperty(const std::string& id, const ZEntityRef entityRef, const unsigned int propertyID, void* data);

    void EntityRefProperty(void* data);

    void UnsupportedProperty(const std::string& id, const ZEntityRef entityRef, const SPropertyData* propertyData, void* data);

    int GetNextID();
    ax::NodeEditor::LinkId GetNextLinkID();
    BlueprintNode* FindNode(const ax::NodeEditor::NodeId id);
    Link* FindLink(const ax::NodeEditor::LinkId id);
    Pin* FindPin(const ax::NodeEditor::PinId id);
    bool IsPinLinked(const ax::NodeEditor::PinId id) const;
    bool CanCreateLink(const Pin* a, const Pin* b) const;
    void BuildNode(BlueprintNode* node);
    void BuildNodes();
    ImColor GetIconColor(const PinType type);
    void DrawPinIcon(const Pin& pin, const bool connected, const int alpha);

    void AddBlueprintNodesAndPins(const ZRuntimeResourceID& tbluRuntimeResourceID, unsigned int& rootEntityNodeIndex);

    bool isOpen;
    std::shared_ptr<EntityTreeNode> rootNode;
    std::shared_ptr<EntityTreeNode> selectedentityTreeNode;
    std::shared_ptr<EntityTreeNode> filteredTreeRootNode;
    bool scrollToEntity;
    std::set<unsigned int> filteredProperties;

    ImGuizmo::OPERATION gizmoMode;
    ImGuizmo::MODE gizmoSpace;
    bool isHoldingMouse;
    bool useSnap;
    float snapValue[3];

    std::unordered_map<unsigned long long, STemplateEntityBlueprint> templateEntityBlueprints;

    ax::NodeEditor::EditorContext* editor;
    int nextID;
    const int pinIconSize = 24;
    std::vector<BlueprintNode> nodes;
    std::vector<Link> links;
    ID3D11Texture2D* headerBackgroundTexture;
    ID3D11ShaderResourceView* headerBackgroundTextureView;
    unsigned int headerBackgroundTextureWidth;
    unsigned int headerBackgroundTextureHeight;
};

DECLARE_MOD(Editor)
