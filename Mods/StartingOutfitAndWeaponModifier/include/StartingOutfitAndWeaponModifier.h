#pragma once

#include <Glacier/STokenID.h>
#include <Glacier/Resource/ZRuntimeResourceID.h>

#include <ModInterface.h>

class ZHitman5;
class ZComponentCreateInfo;

void __fastcall ZHitman5_ZHitman5Hook(ZHitman5* pThis, int edx, ZComponentCreateInfo& pInfo);

class StartingOutfitAndWeaponModifier : public ModInterface
{
public:
    StartingOutfitAndWeaponModifier();
    void Initialize() override;
    void OnEngineInitialized() override;
    void OnDrawMenu() override;
    void OnDrawUI(const bool hasFocus) override;

    const STokenID& GetSelectedOutfitTokenID() const;
    const STokenID& GetSelectedWeaponTokenID() const;

private:
    void RenderOutfitsTabItem();
    void RenderWeaponsTabItem();

    void GetOutfitKitEntities();
    void GetFireArmKitEntities();

    void LoadDDSTexture(const ZRuntimeResourceID& hiResNotebookImage, ID3D11Resource** texture, ID3D11ShaderResourceView** textureView, float& width, float& height);

    struct OutfitKitEntity
    {
        ~OutfitKitEntity();

        const char* title;
        const char* description;
        bool isSelected;
        STokenID tokenID;
        ZRuntimeResourceID hiResNotebookImage;
        ID3D11Resource* texture;
        ID3D11ShaderResourceView* textureView;
        float width;
        float height;
    };

    struct FireArmKitEntity
    {
        ~FireArmKitEntity();

        const char* title;
        const char* description;
        bool isSelected;
        STokenID tokenID;
        ZRuntimeResourceID hiResNotebookImage;
        ID3D11Resource* texture;
        ID3D11ShaderResourceView* textureView;
        float width;
        float height;
    };

    bool isOpen;
    std::vector<OutfitKitEntity> outfitKitEntities;
    std::vector<FireArmKitEntity> fireArmKitEntities;
    int selectedOutfitIndex;
    int selectedWeaponIndex;
    int selectedItemIndex;
};

DECLARE_MOD(StartingOutfitAndWeaponModifier)
