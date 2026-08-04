#include <algorithm>
#include <array>
#include <cfloat>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#ifdef _WIN32
#include <windows.h>
#include <wincodec.h>
#include <mmsystem.h>
#endif

namespace {

GLuint floorTexture = 0;
GLuint wallTexture = 0;
GLuint stairTexture = 0;
GLuint playerSpriteTexture = 0;
GLuint enemySpriteTexture = 0;
GLuint centipedeSpriteTexture = 0;
GLuint gyokukenMapSpriteTexture = 0;
GLuint gyokukenInspectionSpriteTexture = 0;
GLuint gyokukenTotalityMapTexture = 0;
GLuint gyokukenTotalityInspectionTexture = 0;
GLuint obsidianGoldUiTexture = 0;
GLuint mapUnitSpriteTexture = 0;
GLuint cursedPachinkoTexture = 0;
GLuint cursedObjectsTexture = 0;
GLuint cursedToolSpritesTexture = 0;
GLuint cursedGateSpritesTexture = 0;
GLuint bossGateSpritesTexture = 0;
GLuint equipmentIconsTexture = 0;
GLuint spiderGhoulSpritesTexture = 0;
GLuint samuraiSpritesTexture = 0;
GLuint emberInsectSpritesTexture = 0;
GLuint emberInsectMapTexture = 0;
GLuint nueMapTexture = 0;
GLuint nueInspectionTexture = 0;

#ifdef _WIN32
std::filesystem::path gameplayMusicPath;
std::filesystem::path menuMusicPath;
bool gameplayMusicOpen = false;
bool gameplayMusicPlaying = false;
bool menuMusicOpen = false;
bool menuMusicPlaying = false;

void setGameplayMusicVolume(int percent) {
    const int mciVolume = std::clamp(percent, 0, 100) * 10;
    if (gameplayMusicOpen) {
        const std::wstring command = L"setaudio cursedrl_music volume to " +
                                     std::to_wstring(mciVolume);
        mciSendStringW(command.c_str(), nullptr, 0, nullptr);
    }
    if (menuMusicOpen) {
        const std::wstring command = L"setaudio cursedrl_menu_music volume to " +
                                     std::to_wstring(mciVolume);
        mciSendStringW(command.c_str(), nullptr, 0, nullptr);
    }
}

void startGameplayMusic(int volumePercent) {
    if (!gameplayMusicOpen && !gameplayMusicPath.empty()) {
        const std::wstring command = L"open \"" + gameplayMusicPath.wstring() +
                                     L"\" type mpegvideo alias cursedrl_music";
        gameplayMusicOpen =
            mciSendStringW(command.c_str(), nullptr, 0, nullptr) == 0;
        if (gameplayMusicOpen) setGameplayMusicVolume(volumePercent);
    }
    if (gameplayMusicOpen && !gameplayMusicPlaying) {
        gameplayMusicPlaying =
            mciSendStringW(L"play cursedrl_music repeat", nullptr, 0, nullptr) == 0;
    }
}

void stopGameplayMusic() {
    if (gameplayMusicOpen)
        mciSendStringW(L"close cursedrl_music", nullptr, 0, nullptr);
    gameplayMusicOpen = false;
    gameplayMusicPlaying = false;
}

void startMenuMusic(int volumePercent) {
    if (!menuMusicOpen && !menuMusicPath.empty()) {
        const std::wstring command = L"open \"" + menuMusicPath.wstring() +
                                     L"\" type mpegvideo alias cursedrl_menu_music";
        menuMusicOpen =
            mciSendStringW(command.c_str(), nullptr, 0, nullptr) == 0;
        if (menuMusicOpen) setGameplayMusicVolume(volumePercent);
    }
    if (menuMusicOpen && !menuMusicPlaying) {
        menuMusicPlaying =
            mciSendStringW(L"play cursedrl_menu_music repeat", nullptr, 0,
                           nullptr) == 0;
    }
}

void stopMenuMusic() {
    if (menuMusicOpen)
        mciSendStringW(L"close cursedrl_menu_music", nullptr, 0, nullptr);
    menuMusicOpen = false;
    menuMusicPlaying = false;
}
#endif

// Draw the large frame from obsidian_gold_ui_atlas.png as nine independently
// stretched regions. Corners retain their shape while edges and the obsidian
// center expand to fit any ImGui window or child panel.
void drawObsidianGoldFrame(bool belowTitleBar = false, float opacity = 1.0f) {
    if (obsidianGoldUiTexture == 0) return;

    ImVec2 panelMin = ImGui::GetWindowPos();
    ImVec2 panelSize = ImGui::GetWindowSize();
    if (belowTitleBar) {
        const float titleHeight = ImGui::GetFrameHeight();
        panelMin.y += titleHeight;
        panelSize.y -= titleHeight;
    }
    if (panelSize.x < 24.0f || panelSize.y < 24.0f) return;

    const ImVec2 panelMax(panelMin.x + panelSize.x, panelMin.y + panelSize.y);
    const float border = std::min(46.0f,
                                  std::min(panelSize.x, panelSize.y) * 0.22f);
    constexpr float textureWidth = 1024.0f;
    constexpr float textureHeight = 1536.0f;
    constexpr float sourceLeft = 38.0f;
    constexpr float sourceTop = 62.0f;
    constexpr float sourceRight = 987.0f;
    constexpr float sourceBottom = 729.0f;
    constexpr float sourceBorderX = 105.0f;
    constexpr float sourceBorderY = 105.0f;

    const float sx[4] = {sourceLeft, sourceLeft + sourceBorderX,
                         sourceRight - sourceBorderX, sourceRight};
    const float sy[4] = {sourceTop, sourceTop + sourceBorderY,
                         sourceBottom - sourceBorderY, sourceBottom};
    const float dx[4] = {panelMin.x, panelMin.x + border,
                         panelMax.x - border, panelMax.x};
    const float dy[4] = {panelMin.y, panelMin.y + border,
                         panelMax.y - border, panelMax.y};
    ImDrawList* draw = ImGui::GetWindowDrawList();
    const ImU32 tint = IM_COL32(255, 255, 255,
                                static_cast<int>(std::clamp(opacity, 0.0f, 1.0f) *
                                                 255.0f));
    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            draw->AddImage(
                static_cast<ImTextureID>(obsidianGoldUiTexture),
                ImVec2(dx[x], dy[y]), ImVec2(dx[x + 1], dy[y + 1]),
                ImVec2(sx[x] / textureWidth, sy[y] / textureHeight),
                ImVec2(sx[x + 1] / textureWidth,
                       sy[y + 1] / textureHeight), tint);
        }
    }
}

bool beginObsidianChild(const char* id, const ImVec2& size,
                        ImGuiChildFlags childFlags = ImGuiChildFlags_Borders,
                        ImGuiWindowFlags windowFlags = 0) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(54.0f, 54.0f));
    const bool visible = ImGui::BeginChild(id, size, childFlags, windowFlags);
    ImGui::PopStyleVar();
    if (visible) drawObsidianGoldFrame();
    return visible;
}

bool beginObsidianWindow(const char* name, bool* open,
                         ImGuiWindowFlags flags, float opacity = 1.0f) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(54.0f, 54.0f));
    const bool visible = ImGui::Begin(name, open, flags);
    ImGui::PopStyleVar();
    if (visible) drawObsidianGoldFrame(true, opacity);
    return visible;
}

#ifdef _WIN32
bool loadTexture(const std::filesystem::path& path, GLuint& texture) {
    const HRESULT comResult =
        CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const bool uninitializeCom = SUCCEEDED(comResult);

    IWICImagingFactory* factory = nullptr;
    IWICBitmapDecoder* decoder = nullptr;
    IWICBitmapFrameDecode* frame = nullptr;
    IWICFormatConverter* converter = nullptr;
    UINT width = 0;
    UINT height = 0;
    std::vector<std::uint8_t> pixels;

    HRESULT result = CoCreateInstance(
        CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&factory));
    if (SUCCEEDED(result))
        result = factory->CreateDecoderFromFilename(
            path.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad,
            &decoder);
    if (SUCCEEDED(result)) result = decoder->GetFrame(0, &frame);
    if (SUCCEEDED(result))
        result = factory->CreateFormatConverter(&converter);
    if (SUCCEEDED(result))
        result = converter->Initialize(
            frame, GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone,
            nullptr, 0.0, WICBitmapPaletteTypeCustom);
    if (SUCCEEDED(result)) result = converter->GetSize(&width, &height);
    if (SUCCEEDED(result) && width > 0 && height > 0) {
        pixels.resize(static_cast<std::size_t>(width) * height * 4);
        result = converter->CopyPixels(
            nullptr, width * 4, static_cast<UINT>(pixels.size()),
            pixels.data());
    }

    if (SUCCEEDED(result)) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<GLsizei>(width),
                     static_cast<GLsizei>(height), 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, pixels.data());
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (converter) converter->Release();
    if (frame) frame->Release();
    if (decoder) decoder->Release();
    if (factory) factory->Release();
    if (uninitializeCom) CoUninitialize();
    return SUCCEEDED(result);
}
#endif

constexpr int MapWidth = 45;
constexpr int MapHeight = 29;
constexpr int SightRadius = 8;
constexpr int IndividualAttractionCost = 25;
constexpr int GyokukenUpkeepCost = 5;
constexpr int TotalityUpkeepCost = 10;
constexpr int VolcanicBlastCost = 10;
constexpr int VolcanicBlastDamage = 10;
constexpr int RatioCriticalCost = 20;
constexpr int CollapseCost = 30;
constexpr int EmberInsectCost = 30;
constexpr int EmberInsectExplosionRadius = 3;
constexpr int NueCost = 30;
constexpr int NueExplosionRadius = 3;
constexpr int CursedBandageHealing = 10;
constexpr int SimpleDomainCost = 10;
constexpr int NeutralBarrierUpkeepCost = 30;
constexpr int DomainAmplificationUpkeepCost = 20;
constexpr int HollowWickerBasketUpkeepCost = 5;
constexpr int ReversalRedCost = 40;
constexpr int ReversalRedBaseDamage = 10;
constexpr int DomainExpansionActivationCost = 40;
constexpr int DomainExpansionUpkeepCost = 20;
constexpr int DomainExpansionRadius = 5;
constexpr int GossamerSnareCost = 20;
constexpr int GossamerSnareDamage = 5;
constexpr int GossamerSnareRange = 5;
constexpr int GossamerSnareRadius = 2;
constexpr int RestrainedDuration = 3;
constexpr std::array<const char*, 4> ClanNames{{"Gojo", "Zenin", "Jogo", "Nanami"}};
constexpr std::array<const char*, 29> BossGateNames{{
    "Resentment", "Obsession", "Glory", "Consumption", "Passion",
    "Despair", "Degradation", "Indifference", "Arrogance", "Pride",
    "Lust", "Sloth", "Fear", "Superiority", "Inferiority", "Filth",
    "Arrogance", "Suffering", "Power", "Decay", "Grace", "Disgrace",
    "Shame", "Corruption", "Depravity", "Decadence", "Kindness",
    "Hate", "Love"
}};
constexpr std::array<const char*, 4> ClanTechniques{{
    "Limitless", "Shadow Master", "Disaster Flames", "Ratio"
}};
constexpr std::array<const char*, 4> ClanTechniqueDescriptions{{
    "Lapse: 10 base damage to a target and every adjacent tile. Costs 25 CE.",
    "Lapse: Summon a 15 HP Gyokuken shikigami that deals 5 base damage. Costs 5 CE per turn per wolf. Maximum 2; Totality has 30 HP and costs 10 CE per turn.",
    "Lapse: Fire a line blast that deals 10 base damage. Costs 10 CE.",
    "Lapse: Mark every visible enemy; each stack makes the next hit critical. Costs 20 CE."
}};

struct Point {
    int x{};
    int y{};
    friend bool operator==(Point, Point) = default;
};

enum class Tile : std::uint8_t { Wall, Floor };
enum class FloorRegion : std::uint8_t { None, Room, Hallway };
enum class CurseGrade : std::uint8_t {
    None, Grade4, Grade3, Grade2, Grade1, SpecialGrade
};
enum class SpiritAttackType : std::uint8_t { None, Bite, Claws, Stinger };
enum class ActorClassification : std::uint8_t {
    Unclassified,
    Human,
    CursedSpirit,
    Shikigami
};
enum class EquipmentSlot : std::uint8_t {
    Clothing, Weapon, CursedTool, Accessory, Count
};
enum class PlayerGrade : std::uint8_t { Grade4, Grade3, Grade2, Grade1, Special };

struct Item {
    std::string name;
    EquipmentSlot slot = EquipmentSlot::Clothing;
    std::string description;
    int attackBonus = 0;
    int defenseBonus = 0;
    bool cursedDamage = false;
    int baseAttackMultiplierPercent = 100;
    bool reducesDamageByQuarter = false;
};

struct GroundItem {
    Point pos;
    Item item;
};

struct Cell {
    Tile tile = Tile::Wall;
    FloorRegion region = FloorRegion::None;
    bool visible = false;
    bool explored = false;
};

struct Actor {
    Point pos;
    int hp{};
    int maxHp{};
    int attack{};
    std::string name;
    char glyph{};
    bool knowledge = false;
    ActorClassification classification = ActorClassification::Unclassified;
    bool sorcerer = false;
    CurseGrade curseGrade = CurseGrade::None;
    SpiritAttackType spiritAttack = SpiritAttackType::None;
    int cursedEnergy = 0;
    int maxCursedEnergy = 0;
    int cursedEnergyGeneration = 1;
    bool cursedHost = false;
    SpiritAttackType embeddedTechnique = SpiritAttackType::None;
    int ratioStacks = 0;
    bool centipedeCurse = false;
    bool centipedeHost = false;
    bool spiderGhoul = false;
    bool spiderHost = false;
    bool samurai = false;
    bool samuraiHost = false;
    bool phantomWhiteGyokuken = false;
    bool gyokukenTotality = false;
    bool domainSummon = false;
    bool emberInsect = false;
    bool nue = false;
    int stunnedTurns = 0;
    Point domainSummonAnchor{-1, -1};
    bool bossSorcerer = false;
    int bossPowerLevel = 0;
    bool bossPowerGranted = false;
    bool bossCanUseDomain = false;
    bool bossReversalHealing = false;
    bool bossSimpleDomain = false;
    bool bossSimpleDomainReady = false;
    bool bossDomainAmplification = false;
    bool bossDomainAmplificationActive = false;
    bool bossHollowWickerBasket = false;
    bool bossHollowWickerBasketActive = false;
    bool bossReversalRed = false;
    bool dropsCursedKey = false;
    bool neutralBarrierDefense = false;
    bool hostileDomainActive = false;
    bool domainClashAnnounced = false;
    int playerDomainDamageNegated = 0;
    int hostileDomainDamageNegated = 0;
    int sorcererClan = -1;
    int closeCombatAbility = 1;
    std::vector<Item> inventory{};
    std::array<int, static_cast<std::size_t>(EquipmentSlot::Count)> equippedItems{
        {-1, -1, -1, -1}};
    bool alive() const { return hp > 0; }
};

enum class ParticleEffect : std::uint8_t {
    Flame, Vacuum, ShadowInk, ReversalRed, Lightning, RatioMark,
    EmberExplosion, PurpleLightningExplosion, BlackFlash
};

struct Particle {
    float x{};
    float y{};
    float velocityX{};
    float velocityY{};
    float lifetime{};
    float maxLifetime{};
    float size{};
    ParticleEffect effect = ParticleEffect::Flame;
};

struct Message {
    std::string text;
    ImVec4 color{0.88f, 0.90f, 0.94f, 1.0f};
    int remainingHp = -1;
    int maximumHp = 0;
};

struct PachinkoBallState {
    float x = 0.5f;
    float y = 0.08f;
    float velocityX = 0.0f;
    float velocityY = 0.0f;
};

struct LevelState {
    bool generated = false;
    std::vector<Cell> cells;
    std::vector<Actor> enemies;
    std::vector<Point> corpses;
    std::vector<GroundItem> groundItems;
    std::vector<Point> cursedKeys;
    Point upStairs{-1, -1};
    Point downStairs{-1, -1};
    Point pachinkoMachine{-1, -1};
    Point cursedToolShop{-1, -1};
    Point gateOfHeaven{-1, -1};
    int bossKeysRequired = 0;
    int bossKeysCollected = 0;
    std::string bossGateName;
    std::array<int, 13> pachinkoPayouts{
        {0, 0, 1, 0, 2, 0, 5, 0, 2, 0, 1, 0, 0}};
    Point entrance{};
};

class Game {
public:
    Game() : rng_(std::random_device{}()) {}

    bool wantsQuit() const { return quitRequested_; }

    void newGame() {
        zoom_ = 1.5f;
        cells_.assign(MapWidth * MapHeight, {});
        enemies_.clear();
        allies_.clear();
        corpses_.clear();
        groundItems_.clear();
        cursedKeys_.clear();
        particles_.clear();
        messages_.clear();
        levels_ = {};
        currentLevel_ = 0;
        bossPowerLevel_ = 1;
        turn_ = 0;
        points_ = 0;
        earnedPoints_ = 0;
        totalPointsEarned_ = 0;
        spentPoints_ = 0;
        currentPlayerGrade_ = PlayerGrade::Grade4;
        gradeAdvancementPending_ = false;
        advancementHealthCost_ = 5;
        advancementAttackCost_ = 5;
        advancementEnergyCost_ = 5;
        advancementEfficiencyCost_ = 5;
        advancementTechniquePowerCost_ = 5;
        advancementCloseCombatCost_ = 5;
        advancementGenerationCost_ = 5;
        maxCursedEnergy_ = 100 + birthEnergyPoints_ * 10;
        cursedEnergy_ = maxCursedEnergy_;
        cursedEnergyEfficiency_ = 100 + birthEfficiencyPoints_ * 5;
        cursedEnergyGeneration_ = 1 + birthGenerationPoints_;
        cursedTechniquePowerBonus_ = birthTechniquePowerPoints_ * 2;
        reinforcementAttack_ = 0;
        reinforcementDefense_ = 0;
        simpleDomainReady_ = false;
        lastAttackBlockedBySimpleDomain_ = false;
        poisonedTurns_ = 0;
        restrainedTurns_ = 0;
        reversalHealingLearned_ = selectedClan_ == 2;
        simpleDomainLearned_ = simpleDomainTraitSelected_;
        neutralBarrierLearned_ = false;
        neutralBarrierActive_ = false;
        reversalRedLearned_ = false;
        collapseLearned_ = false;
        emberInsectLearned_ = false;
        nueLearned_ = false;
        domainExpansionLearned_ = false;
        domainExpansionActive_ = false;
        domainJustActivated_ = false;
        domainConfirming_ = false;
        domainAmplificationLearned_ = false;
        domainAmplificationActive_ = false;
        hollowWickerBasketLearned_ = false;
        hollowWickerBasketActive_ = false;
        techniqueBurnoutTurns_ = 0;
        burnoutJustApplied_ = false;
        reversalHealingBaseSpend_ = 10;
        bindingVowMenu_ = false;
        advancementMenu_ = false;
        advancementExitConfirm_ = false;
        inventoryMenu_ = false;
        debugMenu_ = false;
        pauseMenu_ = false;
        soundMenu_ = false;
        helpMenu_ = false;
        rulesMenu_ = false;
        helpReturnToPause_ = false;
        rulesReturnToPause_ = false;
        pachinkoMenu_ = false;
        cursedToolMenu_ = false;
        cursedToolShopMenu_ = false;
        techniqueCustomizationMenu_ = false;
        techniqueOutput_ = 1.0f;
        techniqueAreaExpanded_ = false;
        blackFlashAwakened_ = false;
        sorceryHighTurns_ = 0;
        sorceryHighJustApplied_ = false;
        totalityActive_ = false;
        divulgeVowActive_ = false;
        handSignsVowActive_ = false;
        centerCamera_ = true;
        targeting_ = false;
        volcanicTargeting_ = false;
        reversalRedTargeting_ = false;
        collapseTargeting_ = false;
        selectedTargetIndex_ = -1;
        pachinkoBalls_ = 0;
        pachinkoActiveBalls_.clear();
        cursedObjectCounts_ = {{0, 0, 0}};
        cursedBandages_ = 0;
        kamutokeCharges_.clear();
        kamutokeTargeting_ = false;
        hasInspection_ = false;
        inventory_.clear();
        equippedItems_ = {{-1, -1, -1, -1}};
        if (!isJogoClan()) {
            inventory_.push_back(makeEquipmentItem(selectedBirthItem_));
            equippedItems_[static_cast<std::size_t>(inventory_[0].slot)] = 0;
        }
        const int startingHealth = 30 + birthHealthPoints_ * 10;
        const int startingAttack = 5 + birthAttackPoints_;
        player_ = {{}, startingHealth, startingHealth, startingAttack, "Player", '@'};
        player_.classification = selectedClan_ == 2
                                     ? ActorClassification::CursedSpirit
                                     : ActorClassification::Human;
        player_.sorcerer = selectedClan_ != 2;
        player_.closeCombatAbility = 1 + birthCloseCombatPoints_;
        playerClan_ = ClanNames[selectedClan_];
        playerFullName_ = enteredGivenName() + " " + playerClan_;
        player_.name = playerFullName_;
        generateLevel(0);
        loadLevel(0, true);
        addMessage("Welcome. Find your way through the dungeon.");
        addMessage("Move with arrow keys or the numpad. Space waits.");
        addMessage("Equip a Cursed Tool in Inventory, then press Q to activate it.");
        if (isJogoClan())
            addMessage(
                "As a Cursed Spirit, you begin with Cursed Healing. Press H to use it.");
        if (noInnateTechniqueSelected_) {
            addMessage("Negative Trait: No Innate Technique. Clan techniques are unavailable.");
            addMessage("Cursed Energy Reinforcement remains available.");
        } else if (selectedClan_ == 0)
            addMessage("Gojo: press 1 to use Lapse: Limitless.");
        else if (selectedClan_ == 1) {
            addMessage("Zenin: press 1 to use Lapse: Shadow Master.");
            addMessage("Zenin: press 2 to unsummon your Shikigami.");
        }
        else if (selectedClan_ == 2)
            addMessage("Jogo: press 1 to aim Lapse: Disaster Flames.");
        else if (selectedClan_ == 3)
            addMessage("Nanami: press 1 to mark visible enemies with Lapse: Ratio.");
        if (serratedTraitSelected_)
            addMessage("Trait: Serrated Cursed Energy reflects reinforced defense damage.");
        if (simpleDomainTraitSelected_)
            addMessage("Trait: press X to arm Simple Domain.");
        updateFov();
    }

    void handleInput() {
        if (screen_ != Screen::Playing) {
            if (soundMenu_) {
                if (pressedOnce(ImGuiKey_Escape)) soundMenu_ = false;
            } else if (helpMenu_) {
                if (pressedOnce(ImGuiKey_Escape)) helpMenu_ = false;
            } else if (rulesMenu_) {
                if (pressedOnce(ImGuiKey_Escape)) rulesMenu_ = false;
            } else if (screen_ == Screen::NameEntry && pressed(ImGuiKey_Escape))
                screen_ = Screen::MainMenu;
            else if (screen_ == Screen::CharacterSelect &&
                     pressed(ImGuiKey_Escape))
                screen_ = Screen::NameEntry;
            else if (screen_ == Screen::StartingItem && pressed(ImGuiKey_Escape))
                screen_ = Screen::CharacterSelect;
            return;
        }

        if (ImGui::GetIO().WantTextInput) return;

        if (advancementMenu_) {
            if (pressedOnce(ImGuiKey_Escape) || pressedOnce(ImGuiKey_C)) {
                if (advancementExitConfirm_) {
                    // The confirmation popup requires an explicit choice.
                } else if (!advancementAvailable() || points_ == 0) {
                    if (advancementAvailable()) completeGradeAdvancement();
                    advancementMenu_ = false;
                } else {
                    advancementExitConfirm_ = true;
                }
            }
            return;
        }

        const bool helpPressed =
            ImGui::GetIO().KeyShift && pressedOnce(ImGuiKey_Slash);
        if (helpMenu_) {
            if (helpPressed || pressedOnce(ImGuiKey_Escape)) {
                helpMenu_ = false;
                if (helpReturnToPause_) pauseMenu_ = true;
                helpReturnToPause_ = false;
            }
            return;
        }
        if (rulesMenu_) {
            if (pressedOnce(ImGuiKey_Escape)) {
                rulesMenu_ = false;
                if (rulesReturnToPause_) pauseMenu_ = true;
                rulesReturnToPause_ = false;
            }
            return;
        }
        if (helpPressed) {
            bindingVowMenu_ = false;
            advancementMenu_ = false;
            inventoryMenu_ = false;
            debugMenu_ = false;
            pauseMenu_ = false;
            techniqueCustomizationMenu_ = false;
            cursedToolMenu_ = false;
            cursedToolShopMenu_ = false;
            helpReturnToPause_ = false;
            helpMenu_ = true;
            return;
        }
        if (pachinkoMenu_) {
            if (pressedOnce(ImGuiKey_Escape)) pachinkoMenu_ = false;
            return;
        }
        if (cursedToolShopMenu_) {
            if (pressedOnce(ImGuiKey_Escape)) cursedToolShopMenu_ = false;
            return;
        }
        if (cursedToolMenu_) {
            if (pressedOnce(ImGuiKey_Escape) || pressedOnce(ImGuiKey_Q))
                cursedToolMenu_ = false;
            return;
        }

        if (pauseMenu_) {
            if (pressedOnce(ImGuiKey_Escape)) {
                if (soundMenu_) soundMenu_ = false;
                else pauseMenu_ = false;
            }
            return;
        }
        if (techniqueCustomizationMenu_) {
            if (pressedOnce(ImGuiKey_Escape) || pressedOnce(ImGuiKey_T))
                techniqueCustomizationMenu_ = false;
            return;
        }
        if (hollowWickerBasketActive_) {
            if (pressedOnce(ImGuiKey_Escape)) {
                pauseMenu_ = true;
                return;
            }
            bool maintained = false;
            if (pressedOnce(ImGuiKey_V))
                maintained = toggleHollowWickerBasket();
            else if (pressed(ImGuiKey_Space) || pressed(ImGuiKey_Keypad5)) {
                addMessage("You maintain Hollow Wicker Basket.");
                maintained = true;
            }
            if (maintained) finishPlayerTurn();
            return;
        }
        if (pressedOnce(ImGuiKey_I) && !isJogoClan()) {
            inventoryMenu_ = !inventoryMenu_;
            return;
        }
        if (inventoryMenu_) {
            if (pressedOnce(ImGuiKey_Escape)) inventoryMenu_ = false;
            return;
        }
        if (pressedOnce(ImGuiKey_GraveAccent)) {
            debugMenu_ = !debugMenu_;
            return;
        }
        if (debugMenu_) {
            if (pressedOnce(ImGuiKey_Escape)) debugMenu_ = false;
            return;
        }
        if (pressedOnce(ImGuiKey_B)) {
            bindingVowMenu_ = !bindingVowMenu_;
            return;
        }
        if (bindingVowMenu_) {
            if (pressedOnce(ImGuiKey_Escape)) bindingVowMenu_ = false;
            return;
        }
        if (domainConfirming_) {
            if (pressedOnce(ImGuiKey_Escape)) {
                domainConfirming_ = false;
                addMessage("Domain Expansion cancelled.");
            } else if (pressedOnce(ImGuiKey_D) || pressedOnce(ImGuiKey_Space) ||
                       pressedOnce(ImGuiKey_Keypad5)) {
                if (confirmDomainExpansion()) finishPlayerTurn();
            }
            return;
        }
        if (targeting_ || volcanicTargeting_ || reversalRedTargeting_ ||
            collapseTargeting_) {
            handleTargetingInput();
            return;
        }

        if (pressedOnce(ImGuiKey_C)) {
            advancementExitConfirm_ = false;
            advancementMenu_ = true;
            return;
        }

        if (pressedOnce(ImGuiKey_T)) {
            techniqueCustomizationMenu_ = true;
            return;
        }

        if (pressedOnce(ImGuiKey_Escape)) {
            pauseMenu_ = true;
            return;
        }

        // This application has no text-entry widgets, so gameplay input can be
        // read even while the main ImGui window owns keyboard focus.
        if (!player_.alive()) {
            screen_ = Screen::GameOver;
            return;
        }

        Point delta{};
        bool acted = true;
        if (pressedOnce(ImGuiKey_V)) acted = toggleHollowWickerBasket();
        else if (pressedOnce(ImGuiKey_A)) acted = toggleDomainAmplification();
        else if (pressedOnce(ImGuiKey_Q)) acted = activateEquippedCursedTool();
        else if (pressedOnce(ImGuiKey_X)) acted = useSimpleDomain();
        else if (pressedOnce(ImGuiKey_H)) acted = useReversalHealing();
        else if (pressedOnce(ImGuiKey_1)) acted = useClanAbilityOne();
        else if (pressedOnce(ImGuiKey_2)) acted = useClanAbilityTwo();
        else if (pressedOnce(ImGuiKey_3))
            acted = selectedClan_ == 1 ? summonNue() : beginReversalRed();
        else if (domainExpansionLearned_ && pressedOnce(ImGuiKey_D))
            acted = toggleDomainExpansion();
        else if (pressed(ImGuiKey_UpArrow) || pressed(ImGuiKey_Keypad8)) delta = {0, -1};
        else if (pressed(ImGuiKey_DownArrow) || pressed(ImGuiKey_Keypad2)) delta = {0, 1};
        else if (pressed(ImGuiKey_LeftArrow) || pressed(ImGuiKey_Keypad4)) delta = {-1, 0};
        else if (pressed(ImGuiKey_RightArrow) || pressed(ImGuiKey_Keypad6)) delta = {1, 0};
        else if (pressed(ImGuiKey_Keypad7)) delta = {-1, -1};
        else if (pressed(ImGuiKey_Keypad9)) delta = {1, -1};
        else if (pressed(ImGuiKey_Keypad1)) delta = {-1, 1};
        else if (pressed(ImGuiKey_Keypad3)) delta = {1, 1};
        else if (pressed(ImGuiKey_Space) || pressed(ImGuiKey_Keypad5)) addMessage("You wait.");
        else acted = false;

        if (!acted) return;
        if (delta.x != 0 || delta.y != 0) tryPlayerMove(delta);
        finishPlayerTurn();
    }

    void draw() {
#ifdef _WIN32
        if (screen_ == Screen::Playing) {
            stopMenuMusic();
            startGameplayMusic(musicVolume_);
        } else {
            stopGameplayMusic();
            startMenuMusic(musicVolume_);
        }
#endif
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::Begin("Roguelike", nullptr, flags);
        if (screen_ == Screen::MainMenu) {
            drawMainMenu();
        } else if (screen_ == Screen::NameEntry) {
            drawNameEntry();
        } else if (screen_ == Screen::CharacterSelect) {
            drawCharacterSelect();
        } else if (screen_ == Screen::StartingItem) {
            drawStartingItemSelect();
        } else if (screen_ == Screen::GameOver) {
            drawGameOver();
        } else if (screen_ == Screen::Victory) {
            drawVictory();
        } else {
            const float sidebarWidth = 360.0f;
            const ImVec2 available = ImGui::GetContentRegionAvail();
            const float leftWidth = std::max(180.0f, available.x - sidebarWidth);
            ImGui::BeginChild("LeftColumn", ImVec2(leftWidth, 0), false);
            messagePanelHeight_ =
                std::clamp(messagePanelHeight_, 200.0f,
                           std::max(200.0f, available.y * 0.70f));
            const float splitterHeight = 7.0f;
            const float dungeonHeight =
                std::max(120.0f, ImGui::GetContentRegionAvail().y -
                                     messagePanelHeight_ - splitterHeight);
            beginObsidianChild("Dungeon", ImVec2(0, dungeonHeight),
                               ImGuiChildFlags_Borders,
                               ImGuiWindowFlags_NoScrollbar |
                                   ImGuiWindowFlags_NoScrollWithMouse);
            ImGui::BeginChild("MapViewport", ImVec2(0, 0), false,
                              ImGuiWindowFlags_NoScrollWithMouse);
            drawMap();
            ImGui::EndChild();
            ImGui::EndChild();

            ImGui::InvisibleButton("MessagePanelSplitter",
                                   ImVec2(-1, splitterHeight));
            if (ImGui::IsItemHovered() || ImGui::IsItemActive())
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
            if (ImGui::IsItemActive()) {
                messagePanelHeight_ = std::clamp(
                    messagePanelHeight_ - ImGui::GetIO().MouseDelta.y,
                    200.0f, std::max(200.0f, available.y * 0.70f));
            }

            beginObsidianChild("MessagesPanel", ImVec2(0, 0),
                               ImGuiChildFlags_Borders,
                               ImGuiWindowFlags_NoScrollbar |
                                   ImGuiWindowFlags_NoScrollWithMouse);
            drawMessagePanel();
            ImGui::EndChild();
            ImGui::EndChild();
            ImGui::SameLine();
            beginObsidianChild("Sidebar", ImVec2(0, 0),
                               ImGuiChildFlags_Borders,
                               ImGuiWindowFlags_NoScrollbar |
                                   ImGuiWindowFlags_NoScrollWithMouse);
            ImGui::BeginChild("SidebarContent", ImVec2(0, 0), false);
            drawSidebar();
            ImGui::EndChild();
            ImGui::EndChild();
        }
        ImGui::End();
        if (bindingVowMenu_) drawBindingVowMenu();
        if (advancementMenu_) drawAdvancementMenu();
        if (inventoryMenu_) drawInventoryMenu();
        if (debugMenu_) drawDebugMenu();
        if (pauseMenu_) drawPauseMenu();
        if (soundMenu_) drawSoundMenu();
        if (helpMenu_) drawHelpMenu();
        if (rulesMenu_) drawRulesMenu();
        if (pachinkoMenu_) drawPachinkoMenu();
        if (cursedToolMenu_) drawCursedToolMenu();
        if (cursedToolShopMenu_) drawCursedToolShopMenu();
        if (techniqueCustomizationMenu_) drawTechniqueCustomizationMenu();
        if (hasInspection_ && screen_ == Screen::Playing &&
            !bindingVowMenu_ && !advancementMenu_ && !inventoryMenu_ &&
            !debugMenu_ && !pauseMenu_ && !helpMenu_ && !rulesMenu_ && !pachinkoMenu_ &&
            !cursedToolMenu_ && !cursedToolShopMenu_ &&
            !techniqueCustomizationMenu_)
            drawInspectionPanel();
    }

private:
    enum class Screen {
        MainMenu,
        NameEntry,
        CharacterSelect,
        StartingItem,
        Playing,
        GameOver,
        Victory
    };

    std::vector<Cell> cells_;
    std::vector<Actor> enemies_;
    std::vector<Actor> allies_;
    std::vector<Point> corpses_;
    std::vector<GroundItem> groundItems_;
    std::vector<Point> cursedKeys_;
    std::vector<Particle> particles_;
    std::array<LevelState, 15> levels_;
    std::vector<Message> messages_;
    Actor player_;
    std::mt19937 rng_;
    int turn_{};
    int points_ = 0;
    int earnedPoints_ = 0;
    int totalPointsEarned_ = 0;
    int spentPoints_ = 0;
    PlayerGrade currentPlayerGrade_ = PlayerGrade::Grade4;
    bool gradeAdvancementPending_ = false;
    int advancementHealthCost_ = 5;
    int advancementAttackCost_ = 5;
    int advancementEnergyCost_ = 5;
    int advancementEfficiencyCost_ = 5;
    int advancementTechniquePowerCost_ = 5;
    int advancementCloseCombatCost_ = 5;
    int advancementGenerationCost_ = 5;
    int currentLevel_ = 0;
    int bossPowerLevel_ = 1;
    Point upStairs_{-1, -1};
    Point downStairs_{-1, -1};
    Point pachinkoMachine_{-1, -1};
    Point cursedToolShop_{-1, -1};
    Point gateOfHeaven_{-1, -1};
    int cursedEnergy_ = 100;
    int maxCursedEnergy_ = 100;
    int cursedEnergyEfficiency_ = 100;
    int cursedEnergyGeneration_ = 1;
    int cursedTechniquePowerBonus_ = 0;
    int reinforcementAttack_ = 0;
    int reinforcementDefense_ = 0;
    bool simpleDomainReady_ = false;
    bool lastAttackBlockedBySimpleDomain_ = false;
    int poisonedTurns_ = 0;
    int restrainedTurns_ = 0;
    bool reversalHealingLearned_ = false;
    int reversalHealingBaseSpend_ = 10;
    bool bindingVowMenu_ = false;
    bool advancementMenu_ = false;
    bool advancementExitConfirm_ = false;
    bool inventoryMenu_ = false;
    bool debugMenu_ = false;
    bool pauseMenu_ = false;
    bool soundMenu_ = false;
    int musicVolume_ = 25;
    bool helpMenu_ = false;
    bool helpReturnToPause_ = false;
    bool rulesMenu_ = false;
    bool rulesReturnToPause_ = false;
    bool pachinkoMenu_ = false;
    bool cursedToolMenu_ = false;
    bool cursedToolShopMenu_ = false;
    bool techniqueCustomizationMenu_ = false;
    float techniqueOutput_ = 1.0f;
    bool techniqueAreaExpanded_ = false;
    bool blackFlashAwakened_ = false;
    int sorceryHighTurns_ = 0;
    bool sorceryHighJustApplied_ = false;
    bool totalityActive_ = false;
    bool divulgeVowActive_ = false;
    bool handSignsVowActive_ = false;
    int birthHealthPoints_ = 0;
    int birthAttackPoints_ = 0;
    int birthEnergyPoints_ = 0;
    int birthEfficiencyPoints_ = 0;
    int birthGenerationPoints_ = 0;
    int birthTechniquePowerPoints_ = 0;
    int birthCloseCombatPoints_ = 0;
    bool serratedTraitSelected_ = false;
    bool simpleDomainTraitSelected_ = false;
    bool simpleDomainLearned_ = false;
    bool neutralBarrierLearned_ = false;
    bool neutralBarrierActive_ = false;
    bool reversalRedLearned_ = false;
    bool collapseLearned_ = false;
    bool emberInsectLearned_ = false;
    bool nueLearned_ = false;
    bool domainExpansionLearned_ = false;
    bool domainExpansionActive_ = false;
    bool domainJustActivated_ = false;
    bool domainConfirming_ = false;
    bool domainAmplificationLearned_ = false;
    bool domainAmplificationActive_ = false;
    bool hollowWickerBasketLearned_ = false;
    bool hollowWickerBasketActive_ = false;
    int techniqueBurnoutTurns_ = 0;
    bool burnoutJustApplied_ = false;
    bool noInnateTechniqueSelected_ = false;
    int selectedBirthItem_ = 0;
    float zoom_ = 1.5f;
    bool centerCamera_ = true;
    bool targeting_ = false;
    bool kamutokeTargeting_ = false;
    bool volcanicTargeting_ = false;
    bool reversalRedTargeting_ = false;
    Point reversalRedDirection_{1, 0};
    bool collapseTargeting_ = false;
    Point collapseDirection_{1, 0};
    int selectedTargetIndex_ = -1;
    Point volcanicDirection_{1, 0};
    bool hasInspection_ = false;
    Point inspectedTile_{};
    Screen screen_ = Screen::MainMenu;
    int selectedClan_ = 0;
    std::string playerClan_;
    std::string playerFullName_;
    std::array<char, 64> nameInput_{};
    bool quitRequested_ = false;
    float messagePanelHeight_ = 260.0f;
    int pachinkoBalls_ = 0;
    std::vector<PachinkoBallState> pachinkoActiveBalls_;
    std::array<int, 3> cursedObjectCounts_{{0, 0, 0}};
    int cursedBandages_ = 0;
    std::vector<int> kamutokeCharges_;
    std::vector<Item> inventory_;
    std::array<int, static_cast<std::size_t>(EquipmentSlot::Count)> equippedItems_{
        {-1, -1, -1, -1}
    };

    // The repeat flag gives held movement keys the familiar keyboard-repeat
    // delay and rate configured by Dear ImGui.
    static bool pressed(ImGuiKey key) { return ImGui::IsKeyPressed(key, true); }
    static bool pressedOnce(ImGuiKey key) { return ImGui::IsKeyPressed(key, false); }
    static int sign(int n) { return (n > 0) - (n < 0); }
    static int distanceSquared(Point a, Point b) {
        const int dx = a.x - b.x, dy = a.y - b.y;
        return dx * dx + dy * dy;
    }
    static bool inside(int x, int y) { return x >= 0 && y >= 0 && x < MapWidth && y < MapHeight; }
    bool isJogoClan() const { return selectedClan_ == 2; }
    const char* healingAbilityName() const {
        return isJogoClan() ? "Cursed Healing"
                            : "Reversed Cursed Technique: Healing";
    }
    static int enemySpriteIndex(const Actor& enemy) {
        if (enemy.cursedHost) return 4;
        if (enemy.classification == ActorClassification::Human) return 3;
        if (enemy.spiritAttack == SpiritAttackType::Bite) return 0;
        if (enemy.spiritAttack == SpiritAttackType::Claws) return 1;
        return 2;
    }
    static int mapEnemySpriteIndex(const Actor& enemy) {
        if (enemy.centipedeCurse) return 12;
        if (enemy.centipedeHost) return 13;
        if (enemy.cursedHost) {
            if (enemy.embeddedTechnique == SpiritAttackType::Bite) return 8;
            if (enemy.embeddedTechnique == SpiritAttackType::Claws) return 9;
            if (enemy.embeddedTechnique == SpiritAttackType::Stinger) return 10;
            return 15;
        }
        if (enemy.classification == ActorClassification::Human) return 7;
        if (enemy.spiritAttack == SpiritAttackType::Bite) return 4;
        if (enemy.spiritAttack == SpiritAttackType::Claws) return 5;
        return 6;
    }
    Cell& at(int x, int y) { return cells_[y * MapWidth + x]; }
    const Cell& at(int x, int y) const { return cells_[y * MapWidth + x]; }

    std::string enteredGivenName() const {
        std::string name(nameInput_.data());
        const std::size_t first = name.find_first_not_of(" \t");
        if (first == std::string::npos) return {};
        const std::size_t last = name.find_last_not_of(" \t");
        return name.substr(first, last - first + 1);
    }

    static Item makeEquipmentItem(int itemIndex) {
        if (itemIndex == 0) {
            return {"Black Training Uniform", EquipmentSlot::Clothing,
                    "Reinforced fabric. Reduces incoming direct-hit damage by 25%.",
                    0, 0, false, 100, true};
        }
        if (itemIndex == 1) {
            return {"Steel Katana", EquipmentSlot::Weapon,
                    "A balanced conventional weapon. Multiplies your base Attack by 1.5.",
                    0, 0, false, 150};
        }
        return {"Wrapped Cursed Tanto", EquipmentSlot::Weapon,
                "A low-grade cursed tool. Melee attacks can harm cursed spirits.",
                0, 0, true};
    }

    static Item makeCursedToolItem(bool kamutoke) {
        if (kamutoke)
            return {"Imperfect Kamutoke", EquipmentSlot::CursedTool,
                    "A three-use cursed tool. Press Q while equipped to target "
                    "a visible enemy with 20 cursed lightning damage for 10 CE."};
        return {"Cursed Bandage", EquipmentSlot::CursedTool,
                "A stackable cursed tool. Press Q while equipped to restore "
                "10 Health, consuming one bandage."};
    }

    static Item makeAccessoryItem(bool blindfold) {
        if (blindfold)
            return {"Cursed Blindfold", EquipmentSlot::Accessory,
                    "A perception-binding accessory that increases Cursed "
                    "Energy Efficiency by 10% while equipped."};
        return {"Cursed Pin", EquipmentSlot::Accessory,
                "A compact cursed-energy reservoir that increases maximum "
                "Cursed Energy by 20 while equipped."};
    }

    static int equipmentIconIndex(const Item& item) {
        if (item.name == "Black Training Uniform") return 0;
        if (item.name == "Steel Katana") return 1;
        if (item.name == "Wrapped Cursed Tanto") return 2;
        return -1;
    }

    Actor makeInsectCurse(Point position) {
        std::uniform_int_distribution<int> attackRoll(1, 3);
        const SpiritAttackType attackType =
            static_cast<SpiritAttackType>(attackRoll(rng_));
        const char glyph = attackType == SpiritAttackType::Bite ? 'B' :
                           attackType == SpiritAttackType::Claws ? 'K' : 'S';
        return {position, 5, 5, 3, "Insect Curse", glyph, false,
                ActorClassification::CursedSpirit, false, CurseGrade::Grade4,
                attackType, 40, 40};
    }

    static Actor makeCentipedeCurse(Point position) {
        Actor centipede{position, 15, 15, 7, "Centipede Curse", 'C', false,
                        ActorClassification::CursedSpirit, false,
                        CurseGrade::Grade3, SpiritAttackType::Bite, 80, 80};
        centipede.centipedeCurse = true;
        return centipede;
    }

    static Actor makeSpiderGhoul(Point position) {
        Actor spider{position, 35, 35, 15, "Spider Ghoul", 'W', false,
                     ActorClassification::CursedSpirit, false,
                     CurseGrade::Grade3, SpiritAttackType::None, 80, 80, 1};
        spider.spiderGhoul = true;
        return spider;
    }

    static Actor makeRonin(Point position) {
        return {position, 20, 20, 5, "Ronin", 'R', false,
                ActorClassification::Human, false, CurseGrade::None,
                SpiritAttackType::None, 100, 100};
    }

    static Actor makeSamurai(Point position) {
        Actor samurai{position, 50, 50, 15, "Samurai", 'I', false,
                      ActorClassification::Human, false, CurseGrade::None,
                      SpiritAttackType::None, 100, 100, 1};
        samurai.samurai = true;
        return samurai;
    }

    static const char* bossGradeName(int powerLevel) {
        if (powerLevel <= 1) return "Grade 3";
        if (powerLevel == 2) return "Grade 2";
        if (powerLevel == 3) return "Grade 1";
        return "Special Grade";
    }

    static CurseGrade bossCurseGrade(int powerLevel) {
        if (powerLevel <= 1) return CurseGrade::Grade3;
        if (powerLevel == 2) return CurseGrade::Grade2;
        if (powerLevel == 3) return CurseGrade::Grade1;
        return CurseGrade::SpecialGrade;
    }

    static const char* curseGradeName(CurseGrade grade) {
        switch (grade) {
            case CurseGrade::Grade4: return "Grade 4";
            case CurseGrade::Grade3: return "Grade 3";
            case CurseGrade::Grade2: return "Grade 2";
            case CurseGrade::Grade1: return "Grade 1";
            case CurseGrade::SpecialGrade: return "Special Grade";
            default: return "Unclassified";
        }
    }

    static int bossTechniqueDamage(const Actor& boss) {
        return 5 + 5 * std::max(1, boss.bossPowerLevel);
    }

    Actor makeSorcererBoss(Point position, int forcedClan = -1) {
        const int power = std::max(1, bossPowerLevel_);
        const int health = 40 + 20 * power;
        const int energy = 100 + 25 * power;
        const int meleeDamage = 5 + 5 * power;
        Actor boss{position, health, health, meleeDamage, "Enemy Sorcerer", 'S', false,
                   ActorClassification::Human, true, CurseGrade::None,
                   SpiritAttackType::None, energy, energy, 2};
        boss.bossSorcerer = true;
        boss.bossPowerLevel = power;
        boss.bossCanUseDomain = power >= 4;
        boss.dropsCursedKey = true;
        boss.sorcererClan = forcedClan >= 0
            ? forcedClan
            : std::uniform_int_distribution<int>(0, 3)(rng_);
        boss.closeCombatAbility = power;
        boss.neutralBarrierDefense = boss.sorcererClan == 0;
        boss.bossReversalRed = boss.sorcererClan == 0 && power >= 4;
        if (power >= 3) {
            std::bernoulli_distribution hasAbility(0.5);
            boss.bossReversalHealing = hasAbility(rng_);
            boss.bossSimpleDomain = hasAbility(rng_);
            boss.bossDomainAmplification = hasAbility(rng_);
            boss.bossHollowWickerBasket = hasAbility(rng_);
        }
        if (boss.sorcererClan == 2) {
            boss.name = std::string(bossGradeName(power)) + " Curse Jogo";
            boss.glyph = 'J';
            boss.classification = ActorClassification::CursedSpirit;
            boss.curseGrade = bossCurseGrade(power);
        } else {
            boss.name = std::string(bossGradeName(power)) + " " +
                        ClanNames[boss.sorcererClan] + " Sorcerer";
            for (int itemIndex = 0; itemIndex < 3; ++itemIndex) {
                boss.inventory.push_back(makeEquipmentItem(itemIndex));
                boss.equippedItems[static_cast<std::size_t>(
                    boss.inventory.back().slot)] = itemIndex;
            }
        }
        return boss;
    }

    void generateLevel(int levelIndex) {
        LevelState& level = levels_[levelIndex];
        if (level.generated) return;
        level.cells.assign(MapWidth * MapHeight, {});

        struct Room {
            int x, y, width, height;
            Point center() const { return {x + width / 2, y + height / 2}; }
        };
        std::vector<Room> rooms;
        std::uniform_int_distribution<int> roomWidth(6, 11);
        std::uniform_int_distribution<int> roomHeight(5, 9);
        for (int attempt = 0; attempt < 140 && rooms.size() < 9; ++attempt) {
            const int width = roomWidth(rng_);
            const int height = roomHeight(rng_);
            std::uniform_int_distribution<int> roomX(1, MapWidth - width - 2);
            std::uniform_int_distribution<int> roomY(1, MapHeight - height - 2);
            const Room candidate{roomX(rng_), roomY(rng_), width, height};
            const bool overlaps = std::any_of(rooms.begin(), rooms.end(),
                [&candidate](const Room& room) {
                    return candidate.x <= room.x + room.width + 1 &&
                           candidate.x + candidate.width + 1 >= room.x &&
                           candidate.y <= room.y + room.height + 1 &&
                           candidate.y + candidate.height + 1 >= room.y;
                });
            if (!overlaps) rooms.push_back(candidate);
        }
        // Extremely unlucky placement still produces a valid connected floor.
        if (rooms.size() < 2) {
            rooms = {{2, 2, 12, 8}, {MapWidth - 15, MapHeight - 10, 12, 8}};
        }

        auto cellAt = [&level](int x, int y) -> Cell& {
            return level.cells[y * MapWidth + x];
        };
        auto carveRoom = [&cellAt](int x, int y) {
            Cell& cell = cellAt(x, y);
            cell.tile = Tile::Floor;
            cell.region = FloorRegion::Room;
        };
        auto carveHallway = [&cellAt](int x, int y) {
            Cell& cell = cellAt(x, y);
            cell.tile = Tile::Floor;
            if (cell.region == FloorRegion::None)
                cell.region = FloorRegion::Hallway;
        };
        for (const Room& room : rooms)
            for (int y = room.y; y < room.y + room.height; ++y)
                for (int x = room.x; x < room.x + room.width; ++x)
                    carveRoom(x, y);

        for (std::size_t i = 1; i < rooms.size(); ++i) {
            Point cursor = rooms[i - 1].center();
            const Point destination = rooms[i].center();
            const bool horizontalFirst = std::uniform_int_distribution<int>(0, 1)(rng_) == 0;
            auto carveHorizontal = [&] {
                while (cursor.x != destination.x) {
                    carveHallway(cursor.x, cursor.y);
                    cursor.x += destination.x > cursor.x ? 1 : -1;
                }
            };
            auto carveVertical = [&] {
                while (cursor.y != destination.y) {
                    carveHallway(cursor.x, cursor.y);
                    cursor.y += destination.y > cursor.y ? 1 : -1;
                }
            };
            if (horizontalFirst) { carveHorizontal(); carveVertical(); }
            else { carveVertical(); carveHorizontal(); }
            carveHallway(destination.x, destination.y);
        }

        level.entrance = rooms.front().center();
        const bool bossLevel = (levelIndex + 1) % 3 == 0;
        const bool finalLevel =
            levelIndex == static_cast<int>(levels_.size()) - 1;
        if (levelIndex > 0) level.upStairs = rooms.front().center();
        if (!finalLevel)
            level.downStairs = rooms.back().center();
        if (bossLevel) {
            level.pachinkoMachine = rooms[rooms.size() / 2].center();
            if (level.pachinkoMachine == level.entrance ||
                level.pachinkoMachine == level.upStairs ||
                level.pachinkoMachine == level.downStairs)
                level.pachinkoMachine = {rooms.front().x + 1,
                                         rooms.front().y + 1};
        }
        if (finalLevel)
            level.gateOfHeaven = rooms.back().center();
        level.bossKeysRequired = finalLevel ? 4 : (bossLevel ? 1 : 0);
        if (bossLevel && !finalLevel) {
            const int gateIndex = std::uniform_int_distribution<int>(
                0, static_cast<int>(BossGateNames.size()) - 1)(rng_);
            level.bossGateName = std::string("Gate of ") +
                                 BossGateNames[gateIndex];
        }
        level.cursedToolShop = rooms[(rooms.size() * 3) / 4].center();
        if (level.cursedToolShop == level.entrance ||
            level.cursedToolShop == level.upStairs ||
            level.cursedToolShop == level.downStairs ||
            level.cursedToolShop == level.pachinkoMachine ||
            level.cursedToolShop == level.gateOfHeaven)
            level.cursedToolShop = {rooms.back().x + 1, rooms.back().y + 1};

        // A machine's payout layout is rolled once when its level is first
        // generated. LevelState preserves the result when the player leaves
        // and later returns to the floor.
        const int payoutRoll = std::uniform_int_distribution<int>(1, 100)(rng_);
        if (payoutRoll <= 70) {
            level.pachinkoPayouts =
                {{0, 0, 1, 0, 2, 0, 5, 0, 2, 0, 1, 0, 0}};
        } else if (payoutRoll <= 90) {
            level.pachinkoPayouts =
                {{1, 0, 1, 0, 2, 0, 3, 0, 2, 0, 1, 0, 1}};
        } else {
            level.pachinkoPayouts =
                {{5, 0, 0, 5, 0, 0, 10, 0, 0, 5, 0, 0, 5}};
        }
        std::vector<Point> spawnTiles;
        for (int y = 1; y < MapHeight - 1; ++y) {
            for (int x = 1; x < MapWidth - 1; ++x) {
                const Point position{x, y};
                if (cellAt(x, y).tile == Tile::Floor &&
                    !(position == level.entrance) &&
                    !(position == level.upStairs) &&
                    !(position == level.downStairs) &&
                    !(position == level.pachinkoMachine) &&
                    !(position == level.cursedToolShop) &&
                    !(position == level.gateOfHeaven))
                    spawnTiles.push_back(position);
            }
        }
        std::shuffle(spawnTiles.begin(), spawnTiles.end(), rng_);

        const int spiritBase = 4 + levelIndex * 2;
        const int roninBase = 1 + levelIndex;
        const int spiritCount = spiritBase + std::uniform_int_distribution<int>(0, 2)(rng_);
        const int roninCount = roninBase + std::uniform_int_distribution<int>(0, 1)(rng_);
        const int centipedeCount = levelIndex == 0 ? 0 : 1 + levelIndex / 3;
        // Spider Ghouls now follow the same depth progression as Centipedes.
        const int spiderGhoulCount = centipedeCount;
        // Samurai begin on floor 4 and use half of the level's Ronin range.
        // For example, floor 6 rolls 3-4 and floor 15 rolls 7-8 Samurai.
        const int samuraiCount = levelIndex >= 3
            ? (levelIndex + 1) / 2 +
                  std::uniform_int_distribution<int>(0, 1)(rng_)
            : 0;
        std::size_t spawnIndex = 0;
        for (int i = 0;
             i < spiderGhoulCount && spawnIndex < spawnTiles.size(); ++i)
            level.enemies.push_back(makeSpiderGhoul(spawnTiles[spawnIndex++]));
        for (int i = 0;
             i < centipedeCount && spawnIndex < spawnTiles.size(); ++i)
            level.enemies.push_back(
                makeCentipedeCurse(spawnTiles[spawnIndex++]));
        for (int i = 0; i < spiritCount && spawnIndex < spawnTiles.size(); ++i)
            level.enemies.push_back(makeInsectCurse(spawnTiles[spawnIndex++]));
        for (int i = 0; i < roninCount && spawnIndex < spawnTiles.size(); ++i)
            level.enemies.push_back(makeRonin(spawnTiles[spawnIndex++]));
        for (int i = 0; i < samuraiCount && spawnIndex < spawnTiles.size(); ++i)
            level.enemies.push_back(makeSamurai(spawnTiles[spawnIndex++]));
        if (finalLevel) {
            for (int clan = 0; clan < 4 && spawnIndex < spawnTiles.size(); ++clan)
                level.enemies.push_back(
                    makeSorcererBoss(spawnTiles[spawnIndex++], clan));
        } else if (bossLevel && spawnIndex < spawnTiles.size()) {
            level.enemies.push_back(makeSorcererBoss(spawnTiles[spawnIndex++]));
        }
        level.generated = true;
    }

    void saveCurrentLevel() {
        LevelState& level = levels_[currentLevel_];
        if (!level.generated) return;
        level.cells = cells_;
        level.enemies = enemies_;
        level.corpses = corpses_;
        level.groundItems = groundItems_;
        level.cursedKeys = cursedKeys_;
    }

    void placeAlliesNearPlayer() {
        std::vector<Point> occupied;
        for (Actor& ally : allies_) {
            bool placed = false;
            for (int y = -1; y <= 1 && !placed; ++y) {
                for (int x = -1; x <= 1; ++x) {
                    if (x == 0 && y == 0) continue;
                    const Point candidate{player_.pos.x + x, player_.pos.y + y};
                    const bool enemyThere = std::any_of(
                        enemies_.begin(), enemies_.end(),
                        [candidate](const Actor& enemy) {
                            return enemy.alive() && enemy.pos == candidate;
                        });
                    if (inside(candidate.x, candidate.y) &&
                        at(candidate.x, candidate.y).tile == Tile::Floor &&
                        !enemyThere &&
                        std::find(occupied.begin(), occupied.end(), candidate) == occupied.end()) {
                        ally.pos = candidate;
                        occupied.push_back(candidate);
                        placed = true;
                        break;
                    }
                }
            }
        }
    }

    void loadLevel(int levelIndex, bool enteringFromAbove) {
        LevelState& level = levels_[levelIndex];
        currentLevel_ = levelIndex;
        cells_ = level.cells;
        enemies_ = level.enemies;
        corpses_ = level.corpses;
        groundItems_ = level.groundItems;
        cursedKeys_ = level.cursedKeys;
        upStairs_ = level.upStairs;
        downStairs_ = level.downStairs;
        pachinkoMachine_ = level.pachinkoMachine;
        cursedToolShop_ = level.cursedToolShop;
        gateOfHeaven_ = level.gateOfHeaven;
        player_.pos = enteringFromAbove
            ? (upStairs_.x >= 0 ? upStairs_ : level.entrance)
            : (downStairs_.x >= 0 ? downStairs_ : level.entrance);
        particles_.clear();
        placeAlliesNearPlayer();
        centerCamera_ = true;
        hasInspection_ = false;
        advancementMenu_ = false;
        updateFov();
    }

    void changeLevel(int direction) {
        const int destination = currentLevel_ + direction;
        if (destination < 0 || destination >= static_cast<int>(levels_.size())) return;
        saveCurrentLevel();
        generateLevel(destination);
        loadLevel(destination, direction > 0);
        addMessage("You enter dungeon level " + std::to_string(currentLevel_ + 1) + ".");
    }

    bool bossExitLocked() const {
        const LevelState& level = levels_[currentLevel_];
        return level.bossKeysCollected < level.bossKeysRequired;
    }

    Actor* enemyAt(Point p) {
        for (auto& enemy : enemies_) if (enemy.alive() && enemy.pos == p) return &enemy;
        return nullptr;
    }

    Actor* allyAt(Point p) {
        for (auto& ally : allies_) if (ally.alive() && ally.pos == p) return &ally;
        return nullptr;
    }

    bool blocked(Point p, const Actor* ignore = nullptr) {
        if (!inside(p.x, p.y) || at(p.x, p.y).tile == Tile::Wall || player_.pos == p) return true;
        for (const auto& enemy : enemies_)
            if (&enemy != ignore && enemy.alive() && enemy.pos == p) return true;
        for (const auto& ally : allies_)
            if (&ally != ignore && ally.alive() && ally.pos == p) return true;
        return false;
    }

    void addMessage(std::string text, int remainingHp = -1,
                    int maximumHp = 0) {
        ImVec4 color{0.88f, 0.90f, 0.94f, 1.0f};
        if (text.find("Debug:") != std::string::npos) {
            color = {0.90f, 0.45f, 1.0f, 1.0f};
        } else if (text.find("Rank increased") != std::string::npos ||
                   text.find("You gain") != std::string::npos) {
            color = {1.0f, 0.82f, 0.22f, 1.0f};
        } else if (text.find("dies") != std::string::npos ||
                   text.find("died") != std::string::npos ||
                   text.find("hits Player") != std::string::npos) {
            color = {1.0f, 0.38f, 0.34f, 1.0f};
        } else if (text.find("Poison") != std::string::npos ||
                   text.find("Poisoned") != std::string::npos) {
            color = {0.50f, 0.90f, 0.28f, 1.0f};
        } else if (text.find("heal") != std::string::npos ||
                   text.find("restores") != std::string::npos ||
                   text.find("pick up") != std::string::npos) {
            color = {0.40f, 0.92f, 0.58f, 1.0f};
        } else if (text.find("Lapse:") != std::string::npos ||
                   text.find("Simple Domain") != std::string::npos ||
                   text.find("Cursed Technique") != std::string::npos) {
            color = {0.48f, 0.72f, 1.0f, 1.0f};
        } else if (text.find("Not enough") != std::string::npos ||
                   text.find("cannot") != std::string::npos ||
                   text.find("wall") != std::string::npos ||
                   text.find("no space") != std::string::npos) {
            color = {1.0f, 0.62f, 0.26f, 1.0f};
        }
        messages_.push_back(
            {std::move(text), color, remainingHp, maximumHp});
        if (messages_.size() > 80) messages_.erase(messages_.begin());
    }

    void addColoredMessage(std::string text, ImVec4 color) {
        messages_.push_back({std::move(text), color, -1, 0});
        if (messages_.size() > 80) messages_.erase(messages_.begin());
    }

    bool isAlly(const Actor& actor) const {
        return std::any_of(allies_.begin(), allies_.end(),
                           [&actor](const Actor& ally) { return &ally == &actor; });
    }

    static const char* equipmentSlotName(EquipmentSlot slot) {
        switch (slot) {
            case EquipmentSlot::Clothing: return "Clothing";
            case EquipmentSlot::Weapon: return "Weapon";
            case EquipmentSlot::CursedTool: return "Cursed Tool Inventory";
            case EquipmentSlot::Accessory: return "Accessory";
            default: return "Unknown";
        }
    }

    static ImVec4 clanColor(int clanIndex) {
        switch (clanIndex) {
            case 0: return ImVec4(0.28f, 0.65f, 1.00f, 1.00f); // Gojo
            case 1: return ImVec4(0.72f, 0.38f, 0.95f, 1.00f); // Zenin
            case 2: return ImVec4(1.00f, 0.30f, 0.25f, 1.00f); // Jogo
            case 3: return ImVec4(0.30f, 0.85f, 0.38f, 1.00f); // Nanami
            default: return ImGui::GetStyleColorVec4(ImGuiCol_Text);
        }
    }

    static ImVec4 healthColor() { return ImVec4(0.95f, 0.28f, 0.28f, 1.0f); }
    static ImVec4 attackColor() { return ImVec4(0.30f, 0.88f, 0.38f, 1.0f); }
    static ImVec4 energyColor() { return ImVec4(0.72f, 0.38f, 0.95f, 1.0f); }
    static ImVec4 efficiencyColor() { return ImVec4(0.20f, 0.82f, 0.78f, 1.0f); }
    static ImVec4 generationColor() { return ImVec4(1.00f, 0.42f, 0.72f, 1.0f); }

    PlayerGrade playerGrade() const {
        return currentPlayerGrade_;
    }

    bool advancementAvailable() const {
        return gradeAdvancementPending_;
    }

    int nextAdvancementThreshold() const {
        switch (playerGrade()) {
            case PlayerGrade::Grade4: return 15;
            case PlayerGrade::Grade3: return 50;
            case PlayerGrade::Grade2: return 100;
            case PlayerGrade::Grade1: return 200;
            case PlayerGrade::Special: return 200;
            default: return -1;
        }
    }

    void completeGradeAdvancement() {
        earnedPoints_ = 0;
        gradeAdvancementPending_ = false;
        addMessage("Grade-cycle Earned Points reset to 0.");
    }

    bool spendPoints(int amount) {
        if (amount <= 0 || points_ < amount) return false;
        points_ -= amount;
        spentPoints_ += amount;
        return true;
    }

    static const char* playerGradeName(PlayerGrade grade) {
        switch (grade) {
            case PlayerGrade::Grade4: return "Grade 4";
            case PlayerGrade::Grade3: return "Grade 3";
            case PlayerGrade::Grade2: return "Grade 2";
            case PlayerGrade::Grade1: return "Grade 1";
            case PlayerGrade::Special: return "Special Grade";
            default: return "Unranked";
        }
    }

    ImVec4 playerGradeColor(PlayerGrade grade) const {
        switch (grade) {
            case PlayerGrade::Grade4: return ImVec4(0.68f, 0.70f, 0.74f, 1.0f);
            case PlayerGrade::Grade3: return ImVec4(0.30f, 0.68f, 1.0f, 1.0f);
            case PlayerGrade::Grade2: return ImVec4(0.32f, 0.88f, 0.46f, 1.0f);
            case PlayerGrade::Grade1: return ImVec4(1.0f, 0.34f, 0.28f, 1.0f);
            case PlayerGrade::Special:
                return currentLevel_ % 2 == 0
                    ? ImVec4(1.0f, 0.78f, 0.18f, 1.0f)
                    : ImVec4(0.92f, 0.32f, 1.0f, 1.0f);
            default: return ImGui::GetStyleColorVec4(ImGuiCol_Text);
        }
    }

    void grantPlayerPoints(int amount, bool debugGrant = false) {
        points_ += amount;
        earnedPoints_ += amount;
        if (!debugGrant) totalPointsEarned_ += amount;
        addMessage(debugGrant
                       ? "Debug: added " + std::to_string(amount) + " points."
                       : "You gain " + std::to_string(amount) +
                             (amount == 1 ? " point." : " points."));
        if (gradeAdvancementPending_ ||
            earnedPoints_ < nextAdvancementThreshold()) return;

        if (currentPlayerGrade_ != PlayerGrade::Special) {
            currentPlayerGrade_ = static_cast<PlayerGrade>(
                static_cast<int>(currentPlayerGrade_) + 1);
            addMessage("Rank increased: " +
                       std::string(playerGradeName(currentPlayerGrade_)) + "!");
        } else {
            addMessage("Special Grade advancement earned again!");
        }
        gradeAdvancementPending_ = true;
        addMessage("Grade Advancement is available. Press C when ready.");
    }

    const Item* equippedItem(EquipmentSlot slot) const {
        const int index = equippedItems_[static_cast<std::size_t>(slot)];
        return index >= 0 && index < static_cast<int>(inventory_.size())
                   ? &inventory_[index]
                   : nullptr;
    }

    bool hasInventoryItem(const char* name) const {
        return std::any_of(inventory_.begin(), inventory_.end(),
                           [name](const Item& item) { return item.name == name; });
    }

    int effectiveCursedEnergyEfficiency() const {
        const Item* accessory = equippedItem(EquipmentSlot::Accessory);
        return cursedEnergyEfficiency_ +
               (accessory && accessory->name == "Cursed Blindfold" ? 10 : 0) +
               (sorceryHighTurns_ > 0 ? 25 : 0);
    }

    int effectiveMaxCursedEnergy() const {
        const Item* accessory = equippedItem(EquipmentSlot::Accessory);
        return maxCursedEnergy_ +
               (accessory && accessory->name == "Cursed Pin" ? 20 : 0);
    }

    void clampEnergyToEquipmentMaximum() {
        cursedEnergy_ = std::min(cursedEnergy_, effectiveMaxCursedEnergy());
    }

    void removeInventoryItem(const char* name) {
        const auto found = std::find_if(
            inventory_.begin(), inventory_.end(),
            [name](const Item& item) { return item.name == name; });
        if (found == inventory_.end()) return;
        const int removedIndex = static_cast<int>(found - inventory_.begin());
        for (int& equippedIndex : equippedItems_) {
            if (equippedIndex == removedIndex) equippedIndex = -1;
            else if (equippedIndex > removedIndex) --equippedIndex;
        }
        inventory_.erase(found);
        clampEnergyToEquipmentMaximum();
    }

    int equipmentAttackBonus() const {
        int bonus = 0;
        for (int index : equippedItems_)
            if (index >= 0 && index < static_cast<int>(inventory_.size()))
                bonus += inventory_[index].attackBonus;
        return bonus;
    }

    int equipmentDefenseBonus() const {
        int bonus = 0;
        for (int index : equippedItems_)
            if (index >= 0 && index < static_cast<int>(inventory_.size()))
                bonus += inventory_[index].defenseBonus;
        return bonus;
    }

    bool hasQuarterDamageUniform() const {
        const Item* clothing = equippedItem(EquipmentSlot::Clothing);
        return clothing && clothing->reducesDamageByQuarter;
    }

    bool hasBaseAttackMultiplyingWeapon() const {
        const Item* weapon = equippedItem(EquipmentSlot::Weapon);
        return weapon && weapon->baseAttackMultiplierPercent > 100;
    }

    int katanaBaseAttackBonus() const {
        const Item* weapon = equippedItem(EquipmentSlot::Weapon);
        if (!weapon || weapon->baseAttackMultiplierPercent <= 100) return 0;
        return (player_.attack * (weapon->baseAttackMultiplierPercent - 100) + 99) / 100;
    }

    bool hasCursedWeaponEquipped() const {
        const Item* weapon = equippedItem(EquipmentSlot::Weapon);
        return weapon && weapon->cursedDamage;
    }

    static bool actorHasEquippedItem(const Actor& actor, EquipmentSlot slot) {
        const std::size_t slotIndex = static_cast<std::size_t>(slot);
        const int itemIndex = actor.equippedItems[slotIndex];
        return itemIndex >= 0 && itemIndex < static_cast<int>(actor.inventory.size());
    }

    static const Item* actorEquippedItem(const Actor& actor,
                                         EquipmentSlot slot) {
        const std::size_t slotIndex = static_cast<std::size_t>(slot);
        const int itemIndex = actor.equippedItems[slotIndex];
        return itemIndex >= 0 && itemIndex < static_cast<int>(actor.inventory.size())
                   ? &actor.inventory[itemIndex]
                   : nullptr;
    }

    int availableAttackReinforcement() const {
        return std::min(reinforcementAttack_,
                        cursedEnergy_ / cursedEnergyCost(5));
    }

    int displayedPlayerAttack() const {
        return player_.attack + katanaBaseAttackBonus() +
               equipmentAttackBonus() +
               availableAttackReinforcement();
    }

    bool playerMeleeCanAffectCurses() const {
        return hasCursedWeaponEquipped() || availableAttackReinforcement() > 0;
    }

    int applyDamage(Actor& target, int incomingDamage, bool validAgainstCursedSpirit) {
        if (target.curseGrade == CurseGrade::None) {
            target.hp -= incomingDamage;
            return incomingDamage;
        }
        if (!validAgainstCursedSpirit) return 0;

        const int blockedDamage = target.bossSorcerer
            ? 0
            : std::min(incomingDamage, target.cursedEnergy / 5);
        target.cursedEnergy -= blockedDamage * 5;
        const int dealtDamage = incomingDamage - blockedDamage;
        target.hp -= dealtDamage;
        if (blockedDamage > 0) {
            const std::string defenderName =
                "The " + target.name + " (" +
                curseGradeName(target.curseGrade) + ")";
            addColoredMessage(defenderName + " spends " +
                                  std::to_string(blockedDamage * 5) +
                                  " Cursed Energy on reinforcement.",
                              ImVec4(0.38f, 0.36f, 0.86f, 1.0f));
            addColoredMessage(defenderName + " blocks " +
                                  std::to_string(blockedDamage) + " damage.",
                              ImVec4(0.64f, 0.38f, 0.20f, 1.0f));
        }
        return dealtDamage;
    }

    void announceDeath(Actor& actor) {
        if (actor.domainClashAnnounced && domainExpansionActive_) {
            actor.hostileDomainActive = false;
            resolveDomainClash(actor, true);
        }
        addMessage(actor.name + " dies.");
        int awardedPoints = 0;
        if (actor.bossSorcerer) {
            awardedPoints = 25;
            if (!actor.bossPowerGranted) {
                actor.bossPowerGranted = true;
                ++bossPowerLevel_;
            }
        } else if (actor.classification == ActorClassification::CursedSpirit &&
            actor.curseGrade == CurseGrade::Grade4) {
            awardedPoints = 1;
        } else if (actor.classification ==
                       ActorClassification::CursedSpirit &&
                   actor.curseGrade == CurseGrade::Grade3) {
            awardedPoints = 3;
        } else if (actor.classification == ActorClassification::Human &&
                   !actor.sorcerer) {
            awardedPoints = actor.cursedHost ? 5 : 2;
        }
        if (awardedPoints > 0) {
            grantPlayerPoints(awardedPoints);
        }
        if (actor.dropsCursedKey) {
            actor.dropsCursedKey = false;
            cursedKeys_.push_back(actor.pos);
            addColoredMessage(actor.name + " drops a Cursed Key.",
                              ImVec4(0.72f, 0.38f, 0.95f, 1.0f));
        }
        if (actor.classification == ActorClassification::Human && !actor.sorcerer) {
            corpses_.push_back(actor.pos);
            std::bernoulli_distribution dropsItem(0.5);
            if (dropsItem(rng_)) {
                groundItems_.push_back({actor.pos, makeEquipmentItem(1)});
                addMessage("The " + std::string(actor.samurai ? "Samurai" : "Ronin") +
                           " drops a Steel Katana.");
            }
            if (dropsItem(rng_)) {
                groundItems_.push_back({actor.pos, makeEquipmentItem(0)});
                addMessage("The " + std::string(actor.samurai ? "Samurai" : "Ronin") +
                           " drops a Black Training Uniform.");
            }
            if (actor.cursedHost && dropsItem(rng_)) {
                groundItems_.push_back({actor.pos, makeEquipmentItem(2)});
                addMessage("The Cursed Host drops a Wrapped Cursed Tanto.");
            }
        }
        if (actor.bossSorcerer) {
            corpses_.push_back(actor.pos);
            for (const Item& item : actor.inventory) {
                groundItems_.push_back({actor.pos, item});
                addMessage(actor.name + " drops " + item.name + ".");
            }
        }
        if (actor.curseGrade == CurseGrade::None) return;

        const int restored =
            std::min(actor.maxCursedEnergy / 4,
                     effectiveMaxCursedEnergy() - cursedEnergy_);
        cursedEnergy_ += restored;
        addMessage("Exorcising the " + actor.name + " restores " +
                   std::to_string(restored) + " Cursed Energy.");
    }

    void attack(Actor& attacker, Actor& defender, int fixedDamage = -1,
                bool cursedTechniqueDamage = false,
                bool allowBlackFlash = true) {
        lastAttackBlockedBySimpleDomain_ = false;
        if (defender.neutralBarrierDefense) {
            if ((&attacker == &player_ && domainAmplificationActive_) ||
                (attacker.bossSorcerer &&
                 attacker.bossDomainAmplificationActive)) {
                addMessage("Domain Amplification neutralizes " + defender.name +
                           "'s Neutral Limitless.");
            } else {
                addMessage(defender.name +
                           "'s Neutral Limitless makes the attack impossible to reach.");
                return;
            }
        }
        int damage = 0;
        if (fixedDamage >= 0) {
            damage = fixedDamage;
        } else if (attacker.bossSorcerer) {
            damage = attacker.attack;
        } else {
            std::uniform_int_distribution<int> variance(0, 2);
            damage = std::max(1, attacker.attack + variance(rng_) - 1);
        }
        if (isAlly(attacker))
            damage = customizedTechniquePower(damage);
        if (attacker.knowledge) damage += 1;
        int attackBoost = 0;
        int damageReduction = 0;
        int equipmentReduction = 0;
        bool ratioCritical = false;
        bool blackFlash = false;
        if (&attacker == &player_) {
            damage += katanaBaseAttackBonus() + equipmentAttackBonus();
            attackBoost = spendAttackReinforcement();
            damage += attackBoost;
            const bool blackFlashEligible =
                sorceryHighTurns_ > 0 || player_.hp * 2 < player_.maxHp;
            if (allowBlackFlash && fixedDamage < 0 && blackFlashEligible) {
                std::bernoulli_distribution triggerBlackFlash(
                    sorceryHighTurns_ > 0 ? 0.50 : 0.02);
                blackFlash = triggerBlackFlash(rng_);
                if (blackFlash) damage *= 3;
            }
            if (defender.ratioStacks > 0) {
                damage += cursedTechniquePowerBonus_;
                damage = static_cast<int>(
                    std::ceil(damage * ratioCriticalMultiplier()));
                --defender.ratioStacks;
                ratioCritical = true;
                spawnRatioMarkEffect(defender.pos);
            }
        } else if (attacker.bossSorcerer) {
            if (const Item* weapon = actorEquippedItem(
                    attacker, EquipmentSlot::Weapon)) {
                damage += (attacker.attack *
                           (weapon->baseAttackMultiplierPercent - 100) + 99) / 100;
            }
            if (fixedDamage < 0) {
                attackBoost = std::min(attacker.bossPowerLevel,
                                       attacker.cursedEnergy / 5);
                attacker.cursedEnergy -= attackBoost * 5;
                damage += attackBoost;
            }
            if (attacker.sorcererClan == 3 && defender.ratioStacks > 0) {
                damage *= 2;
                --defender.ratioStacks;
                ratioCritical = true;
                spawnRatioMarkEffect(defender.pos);
            }
        } else if (attacker.cursedHost && attacker.cursedEnergy >= 5) {
            attacker.cursedEnergy -= 5;
            attackBoost = 1;
            damage += 1;
        }
        if (&defender == &player_ && neutralBarrierActive_) {
            if (attacker.bossSorcerer &&
                attacker.bossDomainAmplificationActive) {
                addMessage(attacker.name +
                           " uses Domain Amplification to pass through Neutral Limitless.");
            } else {
                addColoredMessage(
                    "Neutral Limitless stops " + attacker.name +
                        "'s attack before it can reach you.",
                    clanColor(selectedClan_));
                return;
            }
        }
        if (&defender == &player_ && simpleDomainReady_) {
            simpleDomainReady_ = false;
            lastAttackBlockedBySimpleDomain_ = true;
            const int counterDamage = std::max(1, player_.attack * 3 / 2);
            const int dealtDamage = applyDamage(attacker, counterDamage, true);
            addColoredMessage("Simple Domain blocks " + attacker.name +
                                  "'s attack and counters for " +
                                  std::to_string(dealtDamage) + ".",
                              clanColor(selectedClan_));
            if (!attacker.alive()) announceDeath(attacker);
            return;
        }
        if (defender.bossSorcerer && defender.bossSimpleDomainReady) {
            defender.bossSimpleDomainReady = false;
            const int counterDamage = std::max(1, defender.attack * 3 / 2);
            addMessage(defender.name +
                       " blocks the attack with Simple Domain and counters for " +
                       std::to_string(counterDamage) + ".");
            if (&attacker == &player_) attacker.hp -= counterDamage;
            else applyDamage(attacker, counterDamage, true);
            if (&attacker == &player_ && !attacker.alive()) {
                addMessage("You died.");
                screen_ = Screen::GameOver;
                advancementMenu_ = false;
                bindingVowMenu_ = false;
            }
            return;
        }
        if (&defender == &player_) {
            equipmentReduction = hasQuarterDamageUniform()
                                     ? damage / 4
                                     : std::min(damage, equipmentDefenseBonus());
            damage -= equipmentReduction;
            damageReduction = spendDefenseReinforcement(damage);
            damage -= damageReduction;
        } else if (defender.bossSorcerer && damage > 0) {
            equipmentReduction = actorHasEquippedItem(
                                     defender, EquipmentSlot::Clothing)
                                     ? damage / 4
                                     : 0;
            damage -= equipmentReduction;
            damageReduction = std::min(
                {damage, defender.bossPowerLevel, defender.cursedEnergy / 5});
            defender.cursedEnergy -= damageReduction * 5;
            damage -= damageReduction;
        } else if (defender.cursedHost && defender.cursedEnergy >= 5 && damage > 0) {
            defender.cursedEnergy -= 5;
            damageReduction = 1;
            damage -= 1;
        }
        if (blackFlash) {
            cursedEnergy_ = effectiveMaxCursedEnergy();
            const bool grantsSorceryHigh = sorceryHighTurns_ <= 0;
            if (grantsSorceryHigh) {
                sorceryHighTurns_ = 7;
                sorceryHighJustApplied_ = true;
            }
            spawnBlackFlashEffect(defender.pos);
            addColoredMessage("BLACK FLASH! Cursed Energy is fully restored.",
                              ImVec4(1.0f, 0.08f, 0.10f, 1.0f));
            if (grantsSorceryHigh)
                addColoredMessage(
                    "Sorcery High: 50% Black Flash chance and +25% Efficiency for 7 turns.",
                    ImVec4(0.86f, 0.18f, 1.0f, 1.0f));
            if (!blackFlashAwakened_) {
                blackFlashAwakened_ = true;
                cursedEnergyEfficiency_ += 5;
                addColoredMessage(
                    "The first Black Flash permanently increases Cursed Energy Efficiency by 5%.",
                    efficiencyColor());
            }
        }
        if (defender.curseGrade != CurseGrade::None) {
            const bool techniqueDamage =
                cursedTechniqueDamage || ratioCritical || blackFlash ||
                isAlly(attacker) ||
                (&attacker == &player_ &&
                 (hasCursedWeaponEquipped() || attackBoost > 0));
            const int eligibleDamage =
                techniqueDamage ? damage : (&attacker == &player_ ? attackBoost : 0);
            damage = applyDamage(defender, eligibleDamage,
                                 techniqueDamage || attackBoost > 0);
        } else {
            defender.hp -= damage;
        }
        std::string message = attacker.name + " hits " + defender.name + " for " +
                              std::to_string(damage) + ".";
        if (attackBoost > 0)
            message += " Reinforcement adds " + std::to_string(attackBoost) + ".";
        if (damageReduction > 0)
            message += " Reinforcement blocks " + std::to_string(damageReduction) + ".";
        if (equipmentReduction > 0)
            message += " Clothing blocks " + std::to_string(equipmentReduction) + ".";
        if (ratioCritical)
            message += " Ratio guarantees a critical hit! " +
                       std::to_string(defender.ratioStacks) +
                       (defender.ratioStacks == 1
                            ? " Ratio stack remains."
                            : " Ratio stacks remain.");
        if (blackFlash)
            message += " BLACK FLASH triples melee and reinforcement damage!";
        if (attacker.knowledge)
            message += " Knowledge adds 1 damage.";
        if (defender.curseGrade != CurseGrade::None && damage == 0)
            message += " Mundane damage cannot exorcise it.";
        if (&defender == &player_ && damage > 0) {
            message += " Remaining health:";
            addMessage(std::move(message), std::max(0, player_.hp),
                       player_.maxHp);
        } else {
            if (&attacker == &player_ || isAlly(attacker))
                addColoredMessage(std::move(message), clanColor(selectedClan_));
            else
                addMessage(std::move(message));
        }
        if (&defender == &player_ && serratedTraitSelected_ && damageReduction > 0) {
            const int reflectedDamage = applyDamage(attacker, damageReduction, true);
            addColoredMessage("Serrated Cursed Energy reflects " +
                                  std::to_string(reflectedDamage) + " damage to " +
                                  attacker.name + ".",
                              clanColor(selectedClan_));
            if (!attacker.alive()) announceDeath(attacker);
        }
        if (!defender.alive()) announceDeath(defender);
    }

    const char* spiritAttackName(SpiritAttackType type) const {
        switch (type) {
            case SpiritAttackType::Bite: return "Bite";
            case SpiritAttackType::Claws: return "Claws";
            case SpiritAttackType::Stinger: return "Stinger";
            default: return "Strike";
        }
    }

    void performEnemyAttack(Actor& enemy, Actor& defender) {
        if (enemy.centipedeCurse || enemy.centipedeHost) {
            const bool hostTechnique = enemy.centipedeHost;
            if (hostTechnique && enemy.cursedEnergy < 20) {
                addMessage(enemy.name + " lacks the energy for both embedded "
                           "techniques and uses " +
                           std::string(enemy.samurai ? "Iaijutsu Strike." :
                                                       "Katana Slash."));
                attack(enemy, defender, enemy.attack);
                return;
            }
            if (hostTechnique) enemy.cursedEnergy -= 20;
            addMessage(enemy.name +
                       " invokes Bite and Stinger in the same turn.");
            attack(enemy, defender, 7, true);
            if (!enemy.alive() || !defender.alive()) return;
            attack(enemy, defender, 3, true);
            if (&defender == &player_ && defender.alive() &&
                !lastAttackBlockedBySimpleDomain_) {
                poisonedTurns_ = std::max(poisonedTurns_, 3);
                addMessage("You are Poisoned for 3 turns.");
            }
            return;
        }

        if (enemy.spiderGhoul) {
            int reinforcement = 0;
            if (enemy.cursedEnergy >= 5) {
                enemy.cursedEnergy -= 5;
                reinforcement = 1;
            }
            addMessage(enemy.name + " rends its restrained prey" +
                       (reinforcement > 0
                            ? " with reinforced claws."
                            : " with its claws."));
            attack(enemy, defender, 15 + reinforcement, true);
            return;
        }

        SpiritAttackType attackType = enemy.spiritAttack;
        bool embeddedTechnique = false;
        if (enemy.classification == ActorClassification::Human && !enemy.sorcerer) {
            if (enemy.cursedHost && enemy.embeddedTechnique != SpiritAttackType::None &&
                enemy.cursedEnergy >= 10) {
                enemy.cursedEnergy -= 10;
                attackType = enemy.embeddedTechnique;
                embeddedTechnique = true;
                addMessage(enemy.name + " invokes its embedded Cursed Technique: " +
                           spiritAttackName(attackType) + ".");
            } else {
                addMessage(enemy.name + " uses " +
                           std::string(enemy.samurai ? "Iaijutsu Strike." :
                                                       "Katana Slash."));
                attack(enemy, defender, enemy.attack);
                return;
            }
        } else {
            addMessage(enemy.name + " uses " + spiritAttackName(attackType) + ".");
        }

        switch (attackType) {
            case SpiritAttackType::Bite:
                attack(enemy, defender, 7, embeddedTechnique);
                break;
            case SpiritAttackType::Claws:
                attack(enemy, defender, 3, embeddedTechnique);
                if (enemy.alive() && defender.alive())
                    attack(enemy, defender, 3, embeddedTechnique);
                break;
            case SpiritAttackType::Stinger:
                attack(enemy, defender, 3, embeddedTechnique);
                if (&defender == &player_ && defender.alive() &&
                    !lastAttackBlockedBySimpleDomain_) {
                    poisonedTurns_ = std::max(poisonedTurns_, 3);
                    addMessage("You are Poisoned for 3 turns.");
                }
                break;
            default:
                const Item* enemyWeapon = actorEquippedItem(
                    enemy, EquipmentSlot::Weapon);
                attack(enemy, defender, -1,
                       enemy.bossSorcerer && enemyWeapon &&
                           enemyWeapon->cursedDamage);
                break;
        }
    }

    bool useSorcererBossTechnique(Actor& enemy) {
        if (!enemy.bossSorcerer ||
            enemy.bossDomainAmplificationActive ||
            enemy.bossHollowWickerBasketActive ||
            std::max(std::abs(enemy.pos.x - player_.pos.x),
                     std::abs(enemy.pos.y - player_.pos.y)) <= 1 ||
            distanceSquared(enemy.pos, player_.pos) > SightRadius * SightRadius ||
            !lineOfSight(enemy.pos, player_.pos))
            return false;
        const int techniqueDamage = bossTechniqueDamage(enemy);
        if (enemy.bossReversalRed && enemy.cursedEnergy >= ReversalRedCost &&
            std::bernoulli_distribution(0.5)(rng_)) {
            enemy.cursedEnergy -= ReversalRedCost;
            const int redDamage = (techniqueDamage * 3 + 1) / 2;
            addMessage(enemy.name + " uses Cursed Technique Reversal: Red.");
            attack(enemy, player_, redDamage, true);
            const Point push{sign(player_.pos.x - enemy.pos.x),
                             sign(player_.pos.y - enemy.pos.y)};
            for (int step = 0; step < 3 && player_.alive(); ++step) {
                const Point destination{player_.pos.x + push.x,
                                        player_.pos.y + push.y};
                if (push == Point{0, 0} || blocked(destination, &player_)) break;
                player_.pos = destination;
            }
            centerCamera_ = true;
            return true;
        }
        if (enemy.sorcererClan == 0 && enemy.cursedEnergy >= 25) {
            enemy.cursedEnergy -= 25;
            addMessage(enemy.name + " uses Lapse: Limitless.");
            attack(enemy, player_, techniqueDamage, true);
            return true;
        }
        if (enemy.sorcererClan == 1 && enemy.cursedEnergy >= 20) {
            enemy.cursedEnergy -= 20;
            addMessage(enemy.name + " commands a Gyokuken assault.");
            spawnShadowInkParticles(player_.pos);
            attack(enemy, player_, techniqueDamage, true);
            return true;
        }
        if (enemy.sorcererClan == 2 && enemy.cursedEnergy >= 10) {
            enemy.cursedEnergy -= 10;
            addMessage(enemy.name + " uses Lapse: Disaster Flames.");
            attack(enemy, player_, techniqueDamage, true);
            return true;
        }
        if (enemy.sorcererClan == 3 && enemy.cursedEnergy >= 20 &&
            player_.ratioStacks == 0) {
            enemy.cursedEnergy -= 20;
            attack(enemy, player_, techniqueDamage, true);
            ++player_.ratioStacks;
            addMessage(enemy.name +
                       " uses Lapse: Ratio. Your next hit from this sorcerer is critical.");
            return true;
        }
        return false;
    }

    static const char* hostileDomainName(int clan) {
        switch (clan) {
            case 0: return "Infinite Void";
            case 1: return "Chimera Shadow Garden";
            case 2: return "Coffin of the Iron Mountain";
            case 3: return "Unpaid Overtime";
            default: return "Hostile Domain";
        }
    }

    bool playerInsideHostileDomain() const {
        return std::any_of(enemies_.begin(), enemies_.end(),
                           [this](const Actor& enemy) {
            return enemy.alive() && enemy.hostileDomainActive &&
                   std::max(std::abs(enemy.pos.x - player_.pos.x),
                            std::abs(enemy.pos.y - player_.pos.y)) <=
                       DomainExpansionRadius;
        });
    }

    bool domainsOverlap(const Actor& enemy) const {
        return domainExpansionActive_ && enemy.alive() &&
               enemy.hostileDomainActive &&
               std::max(std::abs(enemy.pos.x - player_.pos.x),
                        std::abs(enemy.pos.y - player_.pos.y)) <=
                   DomainExpansionRadius * 2;
    }

    void announceDomainClash(Actor& enemy) {
        if (enemy.domainClashAnnounced) return;
        enemy.domainClashAnnounced = true;
        addColoredMessage(std::string("DOMAIN CLASH: ") + domainExpansionName() +
                              " collides with " +
                              hostileDomainName(enemy.sorcererClan) + "!",
                          clanColor(selectedClan_));
        addMessage("Both sure-hit techniques are negated until one domain collapses.");
    }

    void clearDomainClash(Actor& enemy) {
        enemy.domainClashAnnounced = false;
        enemy.playerDomainDamageNegated = 0;
        enemy.hostileDomainDamageNegated = 0;
    }

    void resolveDomainClash(Actor& enemy, bool playerWon) {
        if (!enemy.domainClashAnnounced) {
            clearDomainClash(enemy);
            return;
        }
        const int storedPlayerDamage = enemy.playerDomainDamageNegated;
        const int storedHostileDamage = enemy.hostileDomainDamageNegated;
        clearDomainClash(enemy);
        if (playerWon) {
            const int storedDamage = storedPlayerDamage;
            const int restored = std::min(effectiveMaxCursedEnergy() - cursedEnergy_,
                                          effectiveMaxCursedEnergy() / 2);
            cursedEnergy_ += restored;
            if (storedDamage > 0 && enemy.alive()) {
                const int dealt = applyDamage(enemy, storedDamage, true);
                addColoredMessage(enemy.name + " loses the clash and takes " +
                                      std::to_string(dealt) +
                                      " stored domain damage.",
                                  clanColor(selectedClan_));
                if (!enemy.alive()) announceDeath(enemy);
            }
            addColoredMessage("You win the Domain Clash and restore " +
                                  std::to_string(restored) +
                                  " Cursed Energy.",
                              clanColor(selectedClan_));
        } else {
            const int storedDamage = storedHostileDamage;
            const int restored =
                std::min(enemy.maxCursedEnergy - enemy.cursedEnergy,
                         enemy.maxCursedEnergy / 2);
            enemy.cursedEnergy += restored;
            if (storedDamage > 0 && player_.alive()) {
                player_.hp -= storedDamage;
                addColoredMessage(enemy.name + " wins the Domain Clash. You take " +
                                      std::to_string(storedDamage) +
                                      " stored domain damage and have " +
                                      std::to_string(std::max(0, player_.hp)) +
                                      " / " + std::to_string(player_.maxHp) +
                                      " Health remaining.",
                                  clanColor(enemy.sorcererClan));
            }
            addMessage(enemy.name + " restores " + std::to_string(restored) +
                       " Cursed Energy after winning the clash.");
            if (!player_.alive()) {
                addMessage("You died.");
                screen_ = Screen::GameOver;
                advancementMenu_ = false;
                bindingVowMenu_ = false;
            }
        }
    }

    void resolveAllPlayerDomainClashesAsLosses() {
        for (Actor& enemy : enemies_)
            if (enemy.domainClashAnnounced) resolveDomainClash(enemy, false);
    }

    bool playerInDomainClash() const {
        return std::any_of(enemies_.begin(), enemies_.end(),
                           [](const Actor& enemy) {
                               return enemy.alive() && enemy.domainClashAnnounced;
                           });
    }

    bool processHostileDomain(Actor& enemy) {
        if (!enemy.bossSorcerer || !enemy.bossCanUseDomain ||
            enemy.bossDomainAmplificationActive ||
            enemy.bossHollowWickerBasketActive)
            return false;
        const int distance = std::max(std::abs(enemy.pos.x - player_.pos.x),
                                      std::abs(enemy.pos.y - player_.pos.y));
        if (!enemy.hostileDomainActive) {
            if (distance > DomainExpansionRadius || enemy.cursedEnergy < 40 ||
                !lineOfSight(enemy.pos, player_.pos))
                return false;
            enemy.cursedEnergy -= 40;
            enemy.hostileDomainActive = true;
            addColoredMessage(
                enemy.name + " expands " +
                    std::string(hostileDomainName(enemy.sorcererClan)) + "!",
                clanColor(enemy.sorcererClan));
            return true;
        }
        if (enemy.cursedEnergy < 20) {
            if (domainsOverlap(enemy)) resolveDomainClash(enemy, true);
            enemy.hostileDomainActive = false;
            addMessage(enemy.name + " can no longer maintain " +
                       hostileDomainName(enemy.sorcererClan) + ".");
            return true;
        }
        enemy.cursedEnergy -= 20;
        const int domainDamage = bossTechniqueDamage(enemy);
        if (domainsOverlap(enemy)) {
            announceDomainClash(enemy);
            enemy.hostileDomainDamageNegated += domainDamage;
            addMessage("The clash negates " + std::to_string(domainDamage) +
                       " damage from " +
                       std::string(hostileDomainName(enemy.sorcererClan)) + ".");
            return true;
        }
        if (enemy.domainClashAnnounced) clearDomainClash(enemy);
        if (distance <= DomainExpansionRadius) {
            if (hollowWickerBasketActive_) {
                addMessage("Hollow Wicker Basket blocks " + enemy.name +
                           "'s hostile domain damage.");
            } else if (domainAmplificationActive_) {
                addMessage("Domain Amplification nullifies " + enemy.name +
                           "'s hostile domain effect.");
            } else {
                player_.hp -= domainDamage;
                addColoredMessage(
                    hostileDomainName(enemy.sorcererClan) +
                        std::string(" deals ") + std::to_string(domainDamage) +
                        " sure-hit damage. You have " +
                        std::to_string(std::max(0, player_.hp)) + " / " +
                        std::to_string(player_.maxHp) + " Health remaining.",
                    clanColor(enemy.sorcererClan));
            }
        }
        return true;
    }

    int cursedEnergyCost(int baseCost) const {
        const int efficiency = std::max(1, effectiveCursedEnergyEfficiency());
        return std::max(1, (baseCost * 100 + efficiency - 1) / efficiency);
    }

    int cursedTechniquePower(int basePower) const {
        return handSignsVowActive_ ? (basePower * 5 + 3) / 4 : basePower;
    }

    int customizedTechniquePower(int basePower) const {
        const int outputPower =
            static_cast<int>(std::ceil(
                (basePower + cursedTechniquePowerBonus_) * techniqueOutput_));
        return cursedTechniquePower(outputPower);
    }

    int displayedTotalCursedTechniqueDamage() const {
        if (noInnateTechniqueSelected_) return 0;
        switch (selectedClan_) {
            case 0:
                return customizedTechniquePower(10);
            case 1:
                return customizedTechniquePower(totalityActive_ ? 10 : 5);
            case 2:
                return customizedTechniquePower(VolcanicBlastDamage);
            case 3: {
                // Ratio adds CT Power before applying its customized critical
                // multiplier. This shows the deterministic sidebar estimate;
                // the actual melee roll can still vary slightly on impact.
                const int markedMeleeDamage =
                    displayedPlayerAttack() + cursedTechniquePowerBonus_;
                return static_cast<int>(
                    std::ceil(markedMeleeDamage * ratioCriticalMultiplier()));
            }
            default:
                return 0;
        }
    }

    int techniqueEnergyCost(int baseCost) const {
        const int outputCost =
            static_cast<int>(std::ceil(baseCost * techniqueOutput_));
        return cursedEnergyCost(outputCost);
    }

    int areaTechniqueEnergyCost(int baseCost) const {
        int outputCost =
            static_cast<int>(std::ceil(baseCost * techniqueOutput_));
        if (techniqueAreaExpanded_)
            outputCost = static_cast<int>(std::ceil(outputCost * 1.5f));
        return cursedEnergyCost(outputCost);
    }

    int customizedTechniqueRadius(int baseRadius) const {
        return baseRadius + (techniqueAreaExpanded_ ? 1 : 0);
    }

    float ratioCriticalMultiplier() const {
        const float outputMultiplier = 1.0f + techniqueOutput_;
        return handSignsVowActive_ ? outputMultiplier * 1.25f
                                   : outputMultiplier;
    }

    int spentBirthPoints() const {
        return birthHealthPoints_ + birthEnergyPoints_ + birthEfficiencyPoints_ +
               birthAttackPoints_ + birthTechniquePowerPoints_ +
               birthCloseCombatPoints_ * 3 +
               birthGenerationPoints_ * 2 +
               (serratedTraitSelected_ ? 3 : 0) +
               (simpleDomainTraitSelected_ ? 3 : 0);
    }

    int birthPointBudget() const {
        return 5 + (noInnateTechniqueSelected_ ? 5 : 0);
    }

    int knowledgeableEnemyCount() const {
        int count = static_cast<int>(std::count_if(
            enemies_.begin(), enemies_.end(),
            [](const Actor& enemy) { return enemy.alive() && enemy.knowledge; }));
        for (int i = 0; i < static_cast<int>(levels_.size()); ++i) {
            if (i == currentLevel_ || !levels_[i].generated) continue;
            count += static_cast<int>(std::count_if(
                levels_[i].enemies.begin(), levels_[i].enemies.end(),
                [](const Actor& enemy) { return enemy.alive() && enemy.knowledge; }));
        }
        return count;
    }

    void activateDivulgeVow() {
        if (divulgeVowActive_) {
            addMessage("Divulge Cursed Technique is already binding you.");
            return;
        }
        int affected = 0;
        for (Actor& enemy : enemies_) {
            if (enemy.alive() && at(enemy.pos.x, enemy.pos.y).visible) {
                enemy.knowledge = true;
                ++affected;
            }
        }
        if (affected == 0) {
            addMessage("No enemy can hear you divulge your technique.");
            return;
        }
        divulgeVowActive_ = true;
        cursedEnergyEfficiency_ += 25;
        bindingVowMenu_ = false;
        addMessage("Binding Vow: you divulge your Cursed Technique.");
        addMessage(std::to_string(affected) +
                   (affected == 1 ? " enemy gains Knowledge." : " enemies gain Knowledge."));
        addMessage("Cursed Energy Efficiency rises by 25% until they are defeated.");
    }

    void updateDivulgeVow() {
        if (!divulgeVowActive_ || knowledgeableEnemyCount() > 0) return;
        divulgeVowActive_ = false;
        cursedEnergyEfficiency_ = std::max(1, cursedEnergyEfficiency_ - 25);
        addMessage("All enemies with Knowledge are defeated. The Binding Vow ends.");
    }

    void toggleHandSignsVow() {
        handSignsVowActive_ = !handSignsVowActive_;
        if (handSignsVowActive_) {
            const std::size_t weaponSlot =
                static_cast<std::size_t>(EquipmentSlot::Weapon);
            const bool unequippedWeapon = equippedItems_[weaponSlot] >= 0;
            equippedItems_[weaponSlot] = -1;
            addMessage(
                "Binding Vow: Hand Signs increases Cursed Technique power by 1.25x.");
            addMessage(
                "Hand Signs prevents you from wielding a weapon.");
            if (unequippedWeapon)
                addMessage("Your equipped weapon is returned to your inventory.");
        } else {
            addMessage(
                "Binding Vow: Hand Signs ends. Weapons can be equipped again.");
        }
    }

    int spendAttackReinforcement() {
        const int pointCost = cursedEnergyCost(5);
        const int boost = std::min(reinforcementAttack_, cursedEnergy_ / pointCost);
        cursedEnergy_ -= boost * pointCost;
        return boost;
    }

    int spendDefenseReinforcement(int incomingDamage) {
        const int pointCost = cursedEnergyCost(5);
        const int reduction =
            std::min({reinforcementDefense_, incomingDamage, cursedEnergy_ / pointCost});
        cursedEnergy_ -= reduction * pointCost;
        return reduction;
    }

    bool rejectTechniqueDuringBurnout() {
        if (techniqueBurnoutTurns_ <= 0) return false;
        addMessage("Technique Burnout prevents Cursed Technique use for " +
                   std::to_string(techniqueBurnoutTurns_) +
                   (techniqueBurnoutTurns_ == 1 ? " more turn." :
                                                  " more turns."));
        return true;
    }

    bool rejectTechniqueDuringAmplification() {
        if (!domainAmplificationActive_) return false;
        addMessage(
            "Domain Amplification suppresses your Cursed Technique. Only melee attacks are available.");
        return true;
    }

    bool toggleDomainAmplification() {
        if (!domainAmplificationLearned_) {
            addMessage("You have not learned Domain Amplification.");
            return false;
        }
        if (domainAmplificationActive_) {
            domainAmplificationActive_ = false;
            addMessage("You release Domain Amplification.");
            return true;
        }
        if (domainExpansionActive_) {
            resolveAllPlayerDomainClashesAsLosses();
            domainExpansionActive_ = false;
            domainJustActivated_ = false;
            addMessage(std::string("Domain Amplification collapses ") +
                       domainExpansionName() + ".");
            applyTechniqueBurnout();
            if (!player_.alive()) return true;
        }
        neutralBarrierActive_ = false;
        simpleDomainReady_ = false;
        targeting_ = false;
        volcanicTargeting_ = false;
        reversalRedTargeting_ = false;
        collapseTargeting_ = false;
        domainConfirming_ = false;
        for (Actor& ally : allies_)
            if (ally.classification == ActorClassification::Shikigami)
                ally.hp = 0;
        for (Actor& enemy : enemies_) enemy.ratioStacks = 0;
        domainAmplificationActive_ = true;
        addColoredMessage(
            "Domain Amplification surrounds you. Hostile domains and Cursed Technique defenses are neutralized.",
            ImVec4(0.76f, 0.84f, 0.92f, 1.0f));
        return true;
    }

    bool toggleHollowWickerBasket() {
        if (!hollowWickerBasketLearned_) {
            addMessage("You have not learned Hollow Wicker Basket.");
            return false;
        }
        if (hollowWickerBasketActive_) {
            hollowWickerBasketActive_ = false;
            addMessage("You release Hollow Wicker Basket.");
            return true;
        }
        if (domainExpansionActive_) {
            resolveAllPlayerDomainClashesAsLosses();
            domainExpansionActive_ = false;
            domainJustActivated_ = false;
            addMessage(std::string("Hollow Wicker Basket collapses ") +
                       domainExpansionName() + ".");
            applyTechniqueBurnout();
            if (!player_.alive()) return true;
        }
        domainAmplificationActive_ = false;
        neutralBarrierActive_ = false;
        simpleDomainReady_ = false;
        targeting_ = false;
        volcanicTargeting_ = false;
        reversalRedTargeting_ = false;
        collapseTargeting_ = false;
        domainConfirming_ = false;
        hollowWickerBasketActive_ = true;
        addColoredMessage(
            "Hollow Wicker Basket is active. Domain damage is blocked, but you can only maintain or release it.",
            ImVec4(1.0f, 0.32f, 0.72f, 1.0f));
        return true;
    }

    void applyTechniqueBurnout() {
        techniqueBurnoutTurns_ = 10;
        burnoutJustApplied_ = true;
        neutralBarrierActive_ = false;
        simpleDomainReady_ = false;
        targeting_ = false;
        volcanicTargeting_ = false;
        reversalRedTargeting_ = false;
        collapseTargeting_ = false;
        domainConfirming_ = false;
        for (Actor& ally : allies_) {
            if (ally.classification == ActorClassification::Shikigami)
                ally.hp = 0;
        }
        for (Actor& enemy : enemies_)
            enemy.ratioStacks = 0;
        addMessage(
            "Technique Burnout: Cursed Techniques are unavailable for 10 turns. Reinforcement remains usable.");
    }

    bool useClanAbilityOne() {
        if (rejectTechniqueDuringAmplification()) return false;
        if (rejectTechniqueDuringBurnout()) return false;
        if (noInnateTechniqueSelected_) {
            addMessage("No Innate Technique prevents you from using your clan technique.");
            return false;
        }
        if (selectedClan_ == 0) return beginIndividualAttraction();
        if (selectedClan_ == 1) return summonShadowWolf();
        if (selectedClan_ == 2) return beginVolcanicBlast();
        if (selectedClan_ == 3) return applyRatioStatus();
        addMessage("Your clan has no Cursed Technique yet.");
        return false;
    }

    bool toggleNeutralBarrier() {
        if (rejectTechniqueDuringAmplification()) return false;
        if (rejectTechniqueDuringBurnout()) return false;
        if (selectedClan_ != 0 || noInnateTechniqueSelected_) return false;
        if (!neutralBarrierLearned_) {
            addMessage("You have not learned Cursed Technique Neutral: Barrier Technique.");
            return false;
        }
        if (!neutralBarrierActive_ &&
            cursedEnergy_ < cursedEnergyCost(NeutralBarrierUpkeepCost)) {
            addMessage("Not enough Cursed Energy to sustain Neutral Limitless this turn.");
            return false;
        }
        neutralBarrierActive_ = !neutralBarrierActive_;
        addColoredMessage(
            neutralBarrierActive_
                ? "Cursed Technique Neutral: Limitless surrounds you."
                : "You release Cursed Technique Neutral: Limitless.",
            clanColor(selectedClan_));
        return true;
    }

    bool useClanAbilityTwo() {
        if (rejectTechniqueDuringAmplification()) return false;
        if (rejectTechniqueDuringBurnout()) return false;
        if (selectedClan_ == 0) return toggleNeutralBarrier();
        if (selectedClan_ == 1) return unsummonShikigami();
        if (selectedClan_ == 2) {
            if (!emberInsectLearned_) {
                addMessage("You have not learned Cursed Technique Extension: "
                           "Summon Ember Insect.");
                return false;
            }
            if (noInnateTechniqueSelected_) {
                addMessage("No Innate Technique prevents you from summoning an Ember Insect.");
                return false;
            }
            return summonEmberInsect();
        }
        if (selectedClan_ == 3) {
            if (!collapseLearned_) {
                addMessage("You have not learned Cursed Technique Extension: Collapse.");
                return false;
            }
            if (cursedEnergy_ < areaTechniqueEnergyCost(CollapseCost)) {
                addMessage("Not enough Cursed Energy for Cursed Technique Extension: Collapse.");
                return false;
            }
            collapseDirection_ = {1, 0};
            collapseTargeting_ = true;
            addMessage("Aim Cursed Technique Extension: Collapse, then press 2 or Space.");
            return false;
        }
        return false;
    }

    bool enemyInsideDomain(const Actor& enemy) const {
        return enemy.alive() &&
               std::max(std::abs(enemy.pos.x - player_.pos.x),
                        std::abs(enemy.pos.y - player_.pos.y)) <=
                   DomainExpansionRadius;
    }

    const char* domainExpansionName() const {
        switch (selectedClan_) {
            case 0: return "Infinite Void";
            case 1: return "Chimera Shadow Garden";
            case 2: return "Coffin of the Iron Mountain";
            case 3: return "Unpaid Overtime";
            default: return "Domain Expansion";
        }
    }

    const char* domainExpansionDescription() const {
        switch (selectedClan_) {
            case 0:
                return "Infinite Void freezes every enemy within five tiles and "
                       "hits each one with Limitless Cursed Technique damage every turn.";
            case 1:
                return "Chimera Shadow Garden freezes enemies and manifests one "
                       "upkeep-free Domain Gyokuken beside every enemy caught inside. "
                       "The summons ignore the normal limit and vanish with the domain.";
            case 2:
                return "Coffin of the Iron Mountain freezes every enemy within five "
                       "tiles and burns each one with Disaster Flames Cursed Technique "
                       "damage every turn.";
            case 3:
                return "Unpaid Overtime freezes enemies, applies one Ratio stack to "
                       "each target every turn, and immediately makes one player melee "
                       "attack against each of them.";
            default:
                return "Freezes and damages enemies within five tiles.";
        }
    }

    bool toggleDomainExpansion() {
        if (rejectTechniqueDuringAmplification()) return false;
        if (!domainExpansionLearned_) {
            addMessage("You have not learned Domain Expansion.");
            return false;
        }
        if (noInnateTechniqueSelected_) {
            addMessage("No Innate Technique prevents you from expanding a domain.");
            return false;
        }
        if (domainExpansionActive_) {
            resolveAllPlayerDomainClashesAsLosses();
            domainExpansionActive_ = false;
            domainJustActivated_ = false;
            addColoredMessage(std::string("You release ") +
                                  domainExpansionName() + ".",
                              clanColor(selectedClan_));
            applyTechniqueBurnout();
            return true;
        }
        if (rejectTechniqueDuringBurnout()) return false;
        const int cost = cursedEnergyCost(DomainExpansionActivationCost);
        if (cursedEnergy_ < cost) {
            addMessage("Not enough Cursed Energy to expand your domain.");
            return false;
        }
        domainConfirming_ = true;
        addColoredMessage(std::string("Previewing Domain Expansion: ") +
                              domainExpansionName() +
                              ". Press D or Space to confirm.",
                          clanColor(selectedClan_));
        return false;
    }

    bool confirmDomainExpansion() {
        if (!domainConfirming_) return false;
        const int cost = cursedEnergyCost(DomainExpansionActivationCost);
        if (cursedEnergy_ < cost) {
            domainConfirming_ = false;
            addMessage("Not enough Cursed Energy to expand your domain.");
            return false;
        }
        cursedEnergy_ -= cost;
        domainConfirming_ = false;
        domainExpansionActive_ = true;
        domainJustActivated_ = true;
        addColoredMessage(std::string("DOMAIN EXPANSION: ") +
                              domainExpansionName() + "!",
                          clanColor(selectedClan_));
        return true;
    }

    bool unsummonShikigami() {
        if (selectedClan_ != 1) return false;
        if (noInnateTechniqueSelected_) {
            addMessage("No Innate Technique prevents you from controlling Shikigami.");
            return false;
        }
        const int summoned = static_cast<int>(std::count_if(
            allies_.begin(), allies_.end(), [](const Actor& ally) {
                return ally.alive() &&
                       ally.classification == ActorClassification::Shikigami &&
                       !ally.domainSummon && !ally.emberInsect;
            }));
        if (summoned == 0) {
            addMessage("You have no Shikigami to unsummon.");
            return false;
        }
        allies_.erase(std::remove_if(
                          allies_.begin(), allies_.end(), [](const Actor& ally) {
                              return ally.classification ==
                                         ActorClassification::Shikigami &&
                                     !ally.domainSummon;
                          }),
                      allies_.end());
        addMessage(summoned == 1 ? "You unsummon your Shikigami."
                                 : "You unsummon both Gyokuken.");
        return true;
    }

    bool useSimpleDomain() {
        if (rejectTechniqueDuringAmplification()) return false;
        if (rejectTechniqueDuringBurnout()) return false;
        if (!simpleDomainLearned_) {
            addMessage("You do not know Simple Domain.");
            return false;
        }
        if (simpleDomainReady_) {
            addMessage("Simple Domain is already waiting for an attack.");
            return false;
        }
        const int cost = cursedEnergyCost(SimpleDomainCost);
        if (cursedEnergy_ < cost) {
            addMessage("Not enough Cursed Energy for Simple Domain.");
            return false;
        }
        cursedEnergy_ -= cost;
        simpleDomainReady_ = true;
        addMessage("Simple Domain is armed. The next attack will be blocked and countered.");
        return true;
    }

    bool useReversalHealing() {
        if (rejectTechniqueDuringAmplification()) return false;
        if (rejectTechniqueDuringBurnout()) return false;
        if (!reversalHealingLearned_) {
            addMessage("You have not learned " +
                       std::string(healingAbilityName()) + ".");
            return false;
        }
        if (player_.hp >= player_.maxHp) {
            addMessage("You are already at full health.");
            return false;
        }
        reversalHealingBaseSpend_ =
            std::clamp((reversalHealingBaseSpend_ / 10) * 10, 10, 200);
        const int cost = cursedEnergyCost(reversalHealingBaseSpend_);
        if (cursedEnergy_ < cost) {
            addMessage("Not enough Cursed Energy for " +
                       std::string(healingAbilityName()) + ".");
            return false;
        }
        cursedEnergy_ -= cost;
        const int healing = (reversalHealingBaseSpend_ / 10) * 5;
        const int restored = std::min(healing, player_.maxHp - player_.hp);
        player_.hp += restored;
        addMessage(std::string(healingAbilityName()) + " restores " +
                   std::to_string(restored) + " health for " +
                   std::to_string(cost) + " Cursed Energy.");
        return true;
    }

    bool applyRatioStatus() {
        int affected = 0;
        for (const Actor& enemy : enemies_)
            if (enemy.alive() && at(enemy.pos.x, enemy.pos.y).visible)
                ++affected;
        if (affected == 0) {
            addMessage("Lapse: Ratio has no visible targets.");
            return false;
        }
        const int cost = techniqueEnergyCost(RatioCriticalCost);
        if (cursedEnergy_ < cost) {
            addMessage("Not enough Cursed Energy to activate Ratio.");
            return false;
        }
        cursedEnergy_ -= cost;
        for (Actor& enemy : enemies_)
            if (enemy.alive() && at(enemy.pos.x, enemy.pos.y).visible)
                ++enemy.ratioStacks;
        addMessage("Lapse: Ratio marks " + std::to_string(affected) +
                   (affected == 1 ? " visible enemy." : " visible enemies."));
        addMessage("Each target gains one Ratio stack.");
        return true;
    }

    bool summonShadowWolf() {
        const int livingGyokuken = static_cast<int>(std::count_if(
            allies_.begin(), allies_.end(), [](const Actor& ally) {
                return ally.alive() &&
                       ally.classification == ActorClassification::Shikigami &&
                       !ally.domainSummon && !ally.emberInsect;
            }));
        const int summonLimit = totalityActive_ ? 1 : 2;
        if (livingGyokuken >= summonLimit) {
            addMessage(totalityActive_
                           ? "Gyokuken: Totality is already summoned."
                           : "Both Gyokuken are already summoned.");
            return false;
        }
        const bool blackActive = std::any_of(
            allies_.begin(), allies_.end(), [](const Actor& ally) {
                return ally.alive() &&
                       ally.classification == ActorClassification::Shikigami &&
                       !ally.domainSummon &&
                       !ally.emberInsect &&
                       !ally.phantomWhiteGyokuken;
            });
        const bool whiteActive = std::any_of(
            allies_.begin(), allies_.end(), [](const Actor& ally) {
                return ally.alive() &&
                       ally.classification == ActorClassification::Shikigami &&
                       !ally.domainSummon &&
                       !ally.emberInsect &&
                       ally.phantomWhiteGyokuken;
            });
        Point spawn{};
        bool foundSpawn = false;
        for (int y = -1; y <= 1 && !foundSpawn; ++y) {
            for (int x = -1; x <= 1; ++x) {
                if (x == 0 && y == 0) continue;
                const Point candidate{player_.pos.x + x, player_.pos.y + y};
                if (!blocked(candidate)) {
                    spawn = candidate;
                    foundSpawn = true;
                    break;
                }
            }
        }

        if (!foundSpawn) {
            addMessage("There is no space beside you for Gyokuken.");
            return false;
        }

        const int hp = totalityActive_ ? 30 : 15;
        const int attack = totalityActive_ ? 10 : 5;
        spawnShadowInkParticles(spawn);
        allies_.push_back({spawn, hp, hp, attack,
                           totalityActive_ ? "Gyokuken: Totality" : "Gyokuken",
                           'w'});
        allies_.back().classification = ActorClassification::Shikigami;
        allies_.back().gyokukenTotality = totalityActive_;
        allies_.back().phantomWhiteGyokuken =
            !totalityActive_ && blackActive && !whiteActive;
        if (totalityActive_)
            addMessage("Lapse: Shadow Master summons Gyokuken: Totality.");
        else
            addMessage(std::string("Lapse: Shadow Master summons the ") +
                       (allies_.back().phantomWhiteGyokuken ? "phantom white"
                                                            : "black") +
                       " Gyokuken.");
        return true;
    }

    bool summonEmberInsect() {
        const int cost = areaTechniqueEnergyCost(EmberInsectCost);
        if (cursedEnergy_ < cost) {
            addMessage("Not enough Cursed Energy to summon an Ember Insect.");
            return false;
        }
        constexpr std::array<Point, 8> directions{{
            {-1, -1}, {0, -1}, {1, -1}, {-1, 0},
            {1, 0}, {-1, 1}, {0, 1}, {1, 1}}};
        Point spawn{};
        bool foundSpawn = false;
        for (Point direction : directions) {
            const Point candidate{player_.pos.x + direction.x,
                                  player_.pos.y + direction.y};
            if (!blocked(candidate)) {
                spawn = candidate;
                foundSpawn = true;
                break;
            }
        }
        if (!foundSpawn) {
            addMessage("There is no open space for an Ember Insect.");
            return false;
        }
        cursedEnergy_ -= cost;
        allies_.push_back({spawn, 1, 1, 0, "Ember Insect", 'e'});
        Actor& insect = allies_.back();
        insect.classification = ActorClassification::Shikigami;
        insect.emberInsect = true;
        spawnShadowInkParticles(spawn);
        addColoredMessage(
            "Cursed Technique Extension: Summon Ember Insect manifests.",
            clanColor(selectedClan_));
        return true;
    }

    bool summonNue() {
        if (rejectTechniqueDuringAmplification()) return false;
        if (rejectTechniqueDuringBurnout()) return false;
        if (selectedClan_ != 1 || noInnateTechniqueSelected_) return false;
        if (!nueLearned_) {
            addMessage("You have not learned Cursed Technique Extension: Summon Nue.");
            return false;
        }
        const bool activeNue = std::any_of(
            allies_.begin(), allies_.end(),
            [](const Actor& ally) { return ally.alive() && ally.nue; });
        if (activeNue) {
            addMessage("Only one Nue can be active at a time.");
            return false;
        }
        const int cost = areaTechniqueEnergyCost(NueCost);
        if (cursedEnergy_ < cost) {
            addMessage("Not enough Cursed Energy to summon Nue.");
            return false;
        }
        constexpr std::array<Point, 8> directions{{
            {-1, -1}, {0, -1}, {1, -1}, {-1, 0},
            {1, 0}, {-1, 1}, {0, 1}, {1, 1}}};
        for (Point direction : directions) {
            const Point spawn{player_.pos.x + direction.x,
                              player_.pos.y + direction.y};
            if (blocked(spawn)) continue;
            cursedEnergy_ -= cost;
            allies_.push_back({spawn, 1, 1, 0, "Nue", 'n'});
            Actor& nue = allies_.back();
            nue.classification = ActorClassification::Shikigami;
            nue.nue = true;
            spawnShadowInkParticles(spawn);
            addColoredMessage(
                "Cursed Technique Extension: Summon Nue manifests.",
                clanColor(selectedClan_));
            return true;
        }
        addMessage("There is no open space for Nue.");
        return false;
    }

    bool beginVolcanicBlast() {
        if (cursedEnergy_ < areaTechniqueEnergyCost(VolcanicBlastCost)) {
            addMessage("Not enough Cursed Energy for Lapse: Disaster Flames.");
            return false;
        }
        volcanicDirection_ = {1, 0};
        volcanicTargeting_ = true;
        addMessage("Aim Lapse: Disaster Flames, then press 1 or Space to confirm.");
        return false;
    }

    bool beginReversalRed() {
        if (rejectTechniqueDuringAmplification()) return false;
        if (rejectTechniqueDuringBurnout()) return false;
        if (selectedClan_ != 0 || noInnateTechniqueSelected_) return false;
        if (!reversalRedLearned_) {
            addMessage("You have not learned Cursed Technique Reversal: Red.");
            return false;
        }
        if (cursedEnergy_ < techniqueEnergyCost(ReversalRedCost)) {
            addMessage("Not enough Cursed Energy for Cursed Technique Reversal: Red.");
            return false;
        }
        reversalRedDirection_ = {1, 0};
        reversalRedTargeting_ = true;
        addMessage("Aim Cursed Technique Reversal: Red, then press 3 or Space to confirm.");
        return false;
    }

    std::vector<Point> volcanicBlastLine() const {
        std::vector<Point> centerLine;
        Point position = player_.pos;
        for (int i = 0; i < SightRadius; ++i) {
            position = {position.x + volcanicDirection_.x, position.y + volcanicDirection_.y};
            if (!inside(position.x, position.y) || at(position.x, position.y).tile == Tile::Wall)
                break;
            centerLine.push_back(position);
        }
        if (!techniqueAreaExpanded_) return centerLine;

        std::vector<Point> area;
        const Point perpendicular{-volcanicDirection_.y,
                                  volcanicDirection_.x};
        for (Point center : centerLine) {
            for (int width = -1; width <= 1; ++width) {
                const Point tile{center.x + perpendicular.x * width,
                                 center.y + perpendicular.y * width};
                if (!inside(tile.x, tile.y) ||
                    at(tile.x, tile.y).tile == Tile::Wall ||
                    std::find(area.begin(), area.end(), tile) != area.end())
                    continue;
                area.push_back(tile);
            }
        }
        return area;
    }

    std::vector<Point> reversalRedLine() {
        std::vector<Point> line;
        Point position = player_.pos;
        for (int i = 0; i < SightRadius; ++i) {
            position = {position.x + reversalRedDirection_.x,
                        position.y + reversalRedDirection_.y};
            if (!inside(position.x, position.y) ||
                at(position.x, position.y).tile == Tile::Wall)
                break;
            line.push_back(position);
            if (enemyAt(position) != nullptr) break;
        }
        return line;
    }

    void spawnReversalRedParticles(const std::vector<Point>& line) {
        for (Point tile : line) {
            for (int i = 0; i < 8; ++i) {
                const float life = 0.35f + static_cast<float>(i % 3) * 0.08f;
                particles_.push_back({
                    static_cast<float>(tile.x) + 0.5f,
                    static_cast<float>(tile.y) + 0.5f,
                    static_cast<float>(reversalRedDirection_.x) * 2.0f,
                    static_cast<float>(reversalRedDirection_.y) * 2.0f,
                    life, life, 0.16f + static_cast<float>(i % 2) * 0.08f,
                    ParticleEffect::ReversalRed});
            }
        }
    }

    void spawnFlameParticles(const std::vector<Point>& line) {
        std::uniform_real_distribution<float> offset(-0.32f, 0.32f);
        std::uniform_real_distribution<float> drift(-0.55f, 0.55f);
        std::uniform_real_distribution<float> lifetime(0.35f, 0.85f);
        std::uniform_real_distribution<float> size(0.08f, 0.20f);
        for (Point tile : line) {
            for (int i = 0; i < 9; ++i) {
                const float life = lifetime(rng_);
                particles_.push_back({
                    static_cast<float>(tile.x) + 0.5f + offset(rng_),
                    static_cast<float>(tile.y) + 0.5f + offset(rng_),
                    static_cast<float>(volcanicDirection_.x) * 1.4f + drift(rng_),
                    static_cast<float>(volcanicDirection_.y) * 1.4f + drift(rng_),
                    life, life, size(rng_)
                });
            }
        }
    }

    void spawnVacuumParticles(Point center) {
        constexpr float TwoPi = 6.28318530718f;
        std::uniform_real_distribution<float> angle(0.0f, TwoPi);
        std::uniform_real_distribution<float> radius(0.45f, 1.85f);
        std::uniform_real_distribution<float> speed(2.5f, 5.0f);
        std::uniform_real_distribution<float> lifetime(0.35f, 0.75f);
        std::uniform_real_distribution<float> size(0.06f, 0.16f);
        const float centerX = static_cast<float>(center.x) + 0.5f;
        const float centerY = static_cast<float>(center.y) + 0.5f;

        for (int i = 0; i < 90; ++i) {
            const float particleAngle = angle(rng_);
            const float particleRadius = radius(rng_);
            const float directionX = -std::cos(particleAngle);
            const float directionY = -std::sin(particleAngle);
            const float particleSpeed = speed(rng_);
            const float life = lifetime(rng_);
            particles_.push_back({
                centerX - directionX * particleRadius,
                centerY - directionY * particleRadius,
                directionX * particleSpeed,
                directionY * particleSpeed,
                life, life, size(rng_), ParticleEffect::Vacuum
            });
        }
    }

    void spawnShadowInkParticles(Point center) {
        constexpr float TwoPi = 6.28318530718f;
        std::uniform_real_distribution<float> angle(0.0f, TwoPi);
        std::uniform_real_distribution<float> radius(0.08f, 0.72f);
        std::uniform_real_distribution<float> speed(0.25f, 1.15f);
        std::uniform_real_distribution<float> lifetime(0.65f, 1.25f);
        std::uniform_real_distribution<float> size(0.07f, 0.22f);
        const float centerX = static_cast<float>(center.x) + 0.5f;
        const float centerY = static_cast<float>(center.y) + 0.72f;

        // Broad, nearly stationary drops form an expanding ink pool.
        for (int i = 0; i < 14; ++i) {
            const float particleAngle = angle(rng_);
            const float particleRadius = radius(rng_) * 0.55f;
            const float life = lifetime(rng_);
            particles_.push_back({
                centerX + std::cos(particleAngle) * particleRadius,
                centerY + std::sin(particleAngle) * particleRadius * 0.35f,
                std::cos(particleAngle) * 0.08f,
                std::sin(particleAngle) * 0.03f,
                life, life, size(rng_) * 1.8f, ParticleEffect::ShadowInk});
        }

        // Faster droplets rise and curl inward around the emerging Shikigami.
        for (int i = 0; i < 34; ++i) {
            const float particleAngle = angle(rng_);
            const float particleRadius = radius(rng_);
            const float particleSpeed = speed(rng_);
            const float life = lifetime(rng_);
            particles_.push_back({
                centerX + std::cos(particleAngle) * particleRadius,
                centerY + std::sin(particleAngle) * particleRadius * 0.42f,
                -std::sin(particleAngle) * particleSpeed * 0.38f,
                -particleSpeed,
                life, life, size(rng_), ParticleEffect::ShadowInk});
        }
    }

    void spawnEmberExplosionParticles(Point center) {
        constexpr float TwoPi = 6.28318530718f;
        std::uniform_real_distribution<float> angle(0.0f, TwoPi);
        std::uniform_real_distribution<float> speed(1.8f, 5.2f);
        std::uniform_real_distribution<float> lifetime(0.42f, 0.90f);
        std::uniform_real_distribution<float> size(0.08f, 0.24f);
        for (int i = 0; i < 52; ++i) {
            const float direction = angle(rng_);
            const float particleSpeed = speed(rng_);
            const float life = lifetime(rng_);
            particles_.push_back({
                center.x + 0.5f, center.y + 0.5f,
                std::cos(direction) * particleSpeed,
                std::sin(direction) * particleSpeed,
                life, life, size(rng_), ParticleEffect::EmberExplosion});
        }
    }

    void spawnNueExplosionParticles(Point center) {
        constexpr float TwoPi = 6.28318530718f;
        std::uniform_real_distribution<float> angle(0.0f, TwoPi);
        std::uniform_real_distribution<float> speed(2.2f, 6.0f);
        std::uniform_real_distribution<float> lifetime(0.38f, 0.82f);
        std::uniform_real_distribution<float> size(0.07f, 0.20f);
        for (int i = 0; i < 64; ++i) {
            const float direction = angle(rng_);
            const float particleSpeed = speed(rng_);
            const float life = lifetime(rng_);
            particles_.push_back({
                center.x + 0.5f, center.y + 0.5f,
                std::cos(direction) * particleSpeed,
                std::sin(direction) * particleSpeed,
                life, life, size(rng_), ParticleEffect::PurpleLightningExplosion});
        }
    }

    void spawnLightningParticles(Point target) {
        const int steps = std::max(std::abs(target.x - player_.pos.x),
                                   std::abs(target.y - player_.pos.y));
        if (steps <= 0) return;
        std::uniform_real_distribution<float> jitter(-0.16f, 0.16f);
        std::uniform_real_distribution<float> life(0.28f, 0.55f);
        for (int step = 0; step <= steps; ++step) {
            const float t = static_cast<float>(step) / static_cast<float>(steps);
            const float x = player_.pos.x + 0.5f +
                            (target.x - player_.pos.x) * t + jitter(rng_);
            const float y = player_.pos.y + 0.5f +
                            (target.y - player_.pos.y) * t + jitter(rng_);
            const float particleLife = life(rng_);
            particles_.push_back({x, y, jitter(rng_) * 0.35f,
                                  jitter(rng_) * 0.35f, particleLife,
                                  particleLife, 0.10f,
                                  ParticleEffect::Lightning});
        }
    }

    void spawnRatioMarkEffect(Point target) {
        // A stationary particle renders Nanami's ratio line over the struck
        // unit. A faint edge keeps the thin black mark readable on dark tiles.
        particles_.push_back({target.x + 0.5f, target.y + 0.5f,
                              0.0f, 0.0f, 0.48f, 0.48f, 0.46f,
                              ParticleEffect::RatioMark});
    }

    void spawnBlackFlashEffect(Point target) {
        particles_.push_back({target.x + 0.5f, target.y + 0.5f,
                              0.0f, 0.0f, 0.62f, 0.62f, 0.72f,
                              ParticleEffect::BlackFlash});
    }

    bool confirmVolcanicBlast() {
        const std::vector<Point> line = volcanicBlastLine();
        if (line.empty()) {
            addMessage("A wall smothers Lapse: Disaster Flames.");
            return false;
        }
        cursedEnergy_ -= areaTechniqueEnergyCost(VolcanicBlastCost);
        spawnFlameParticles(line);
        int hits = 0;
        for (Point tile : line) {
            if (Actor* enemy = enemyAt(tile)) {
                const int dealtDamage = applyDamage(
                    *enemy, customizedTechniquePower(VolcanicBlastDamage), true);
                ++hits;
                addColoredMessage(
                    "Lapse: Disaster Flames scorches " + enemy->name + " for " +
                        std::to_string(dealtDamage) + ".",
                    clanColor(selectedClan_));
                if (!enemy->alive()) announceDeath(*enemy);
            }
        }
        if (hits == 0) addMessage("Lapse: Disaster Flames tears through the corridor.");
        volcanicTargeting_ = false;
        return true;
    }

    bool confirmReversalRed() {
        const std::vector<Point> line = reversalRedLine();
        if (line.empty()) {
            addMessage("A wall blocks Cursed Technique Reversal: Red.");
            return false;
        }
        cursedEnergy_ -= techniqueEnergyCost(ReversalRedCost);
        spawnReversalRedParticles(line);
        Actor* target = nullptr;
        for (Point tile : line) {
            if ((target = enemyAt(tile)) != nullptr) break;
        }
        if (target == nullptr) {
            addColoredMessage("Cursed Technique Reversal: Red tears through the corridor.",
                              clanColor(selectedClan_));
            reversalRedTargeting_ = false;
            return true;
        }

        const int redDamage =
            (customizedTechniquePower(ReversalRedBaseDamage) * 3 + 1) / 2;
        const int dealtDamage = applyDamage(*target, redDamage, true);
        int spacesRepelled = 0;
        if (target->alive()) {
            for (int step = 0; step < 3; ++step) {
                const Point destination{
                    target->pos.x + reversalRedDirection_.x,
                    target->pos.y + reversalRedDirection_.y};
                if (!inside(destination.x, destination.y) ||
                    at(destination.x, destination.y).tile == Tile::Wall ||
                    destination == player_.pos || enemyAt(destination) != nullptr ||
                    allyAt(destination) != nullptr)
                    break;
                target->pos = destination;
                ++spacesRepelled;
            }
        }
        addColoredMessage(
            "Cursed Technique Reversal: Red hits " + target->name + " for " +
                std::to_string(dealtDamage) + " and repels it " +
                std::to_string(spacesRepelled) +
                (spacesRepelled == 1 ? " space." : " spaces."),
            clanColor(selectedClan_));
        if (!target->alive()) announceDeath(*target);
        reversalRedTargeting_ = false;
        return true;
    }

    std::vector<Point> collapseArea() const {
        std::vector<Point> area;
        const Point perpendicular{-collapseDirection_.y, collapseDirection_.x};
        for (int depth = 1; depth <= 5; ++depth) {
            const Point center{player_.pos.x + collapseDirection_.x * depth,
                               player_.pos.y + collapseDirection_.y * depth};
            const int halfWidth = techniqueAreaExpanded_ ? 1 : 0;
            for (int width = -halfWidth; width <= halfWidth; ++width) {
                const Point tile{center.x + perpendicular.x * width,
                                 center.y + perpendicular.y * width};
                if (inside(tile.x, tile.y)) area.push_back(tile);
            }
        }
        return area;
    }

    bool confirmCollapse() {
        const int cost = areaTechniqueEnergyCost(CollapseCost);
        if (cursedEnergy_ < cost) {
            addMessage("Not enough Cursed Energy for Cursed Technique Extension: Collapse.");
            collapseTargeting_ = false;
            return false;
        }
        cursedEnergy_ -= cost;
        const std::vector<Point> area = collapseArea();
        int targetsHit = 0;
        for (Actor& enemy : enemies_) {
            if (!enemy.alive() ||
                std::find(area.begin(), area.end(), enemy.pos) == area.end())
                continue;
            ++enemy.ratioStacks;
            attack(player_, enemy, -1, false, false);
            ++targetsHit;
        }
        if (targetsHit == 0) {
            addColoredMessage(
                "Cursed Technique Extension: Collapse fractures the empty area.",
                clanColor(selectedClan_));
        } else {
            addColoredMessage(
                "Cursed Technique Extension: Collapse strikes " +
                    std::to_string(targetsHit) +
                    (targetsHit == 1 ? " enemy." : " enemies."),
                clanColor(selectedClan_));
        }
        collapseTargeting_ = false;
        return true;
    }

    std::vector<int> visibleTargetIndices() const {
        std::vector<int> targets;
        for (int i = 0; i < static_cast<int>(enemies_.size()); ++i) {
            const Actor& enemy = enemies_[i];
            if (enemy.alive() && at(enemy.pos.x, enemy.pos.y).visible)
                targets.push_back(i);
        }
        return targets;
    }

    bool activateEquippedCursedTool() {
        if (domainAmplificationActive_) {
            addMessage("Domain Amplification permits only melee attacks. "
                       "Disable it with A first.");
            return false;
        }
        const Item* tool = equippedItem(EquipmentSlot::CursedTool);
        if (!tool) {
            addMessage("No Cursed Tool is equipped. Equip one from Inventory [I].");
            return false;
        }
        if (tool->name == "Cursed Bandage") {
            if (cursedBandages_ <= 0) {
                removeInventoryItem("Cursed Bandage");
                addMessage("You have no Cursed Bandages remaining.");
                return false;
            }
            if (player_.hp >= player_.maxHp) {
                addMessage("Health is already full.");
                return false;
            }
            const int healed =
                std::min(CursedBandageHealing, player_.maxHp - player_.hp);
            player_.hp += healed;
            --cursedBandages_;
            addMessage("The Cursed Bandage restores " +
                       std::to_string(healed) + " Health.");
            if (cursedBandages_ == 0)
                removeInventoryItem("Cursed Bandage");
            return true;
        }
        if (tool->name == "Imperfect Kamutoke")
            return beginKamutokeTargeting();
        addMessage("The equipped Cursed Tool cannot be activated.");
        return false;
    }

    bool beginKamutokeTargeting() {
        const Item* tool = equippedItem(EquipmentSlot::CursedTool);
        if (!tool || tool->name != "Imperfect Kamutoke" ||
            kamutokeCharges_.empty()) {
            addMessage("You do not have an Imperfect Kamutoke.");
            return false;
        }
        if (cursedEnergy_ < 10) {
            addMessage("Imperfect Kamutoke requires 10 Cursed Energy.");
            return false;
        }
        const std::vector<int> targets = visibleTargetIndices();
        if (targets.empty()) {
            addMessage("Imperfect Kamutoke has no visible target.");
            return false;
        }
        selectedTargetIndex_ = targets.front();
        int nearestDistance = SightRadius * SightRadius + 1;
        for (int index : targets) {
            const int distance = distanceSquared(player_.pos, enemies_[index].pos);
            if (distance < nearestDistance) {
                nearestDistance = distance;
                selectedTargetIndex_ = index;
            }
        }
        cursedToolMenu_ = false;
        targeting_ = true;
        kamutokeTargeting_ = true;
        addMessage("Select an enemy, then press Q or Space to call lightning.");
        return false;
    }

    bool confirmKamutoke() {
        if (selectedTargetIndex_ < 0 ||
            selectedTargetIndex_ >= static_cast<int>(enemies_.size()) ||
            !enemies_[selectedTargetIndex_].alive()) {
            targeting_ = false;
            kamutokeTargeting_ = false;
            selectedTargetIndex_ = -1;
            addMessage("That target is no longer available.");
            return false;
        }
        if (kamutokeCharges_.empty() || cursedEnergy_ < 10) {
            targeting_ = false;
            kamutokeTargeting_ = false;
            selectedTargetIndex_ = -1;
            addMessage("Imperfect Kamutoke cannot be activated.");
            return false;
        }
        Actor& target = enemies_[selectedTargetIndex_];
        cursedEnergy_ -= 10;
        spawnLightningParticles(target.pos);
        const int dealt = applyDamage(target, 20, true);
        addColoredMessage("Imperfect Kamutoke blasts " + target.name +
                              " with lightning for " + std::to_string(dealt) + ".",
                          ImVec4(0.35f, 0.82f, 1.0f, 1.0f));
        if (!target.alive()) announceDeath(target);
        if (--kamutokeCharges_.back() <= 0) {
            kamutokeCharges_.pop_back();
            addMessage("The Imperfect Kamutoke breaks after its third use.");
            if (kamutokeCharges_.empty())
                removeInventoryItem("Imperfect Kamutoke");
        }
        targeting_ = false;
        kamutokeTargeting_ = false;
        selectedTargetIndex_ = -1;
        return true;
    }

    bool beginIndividualAttraction() {
        if (selectedClan_ != 0) {
            addMessage("Your clan has no Cursed Technique yet.");
            return false;
        }
        if (cursedEnergy_ < areaTechniqueEnergyCost(IndividualAttractionCost)) {
            addMessage("Not enough Cursed Energy for Lapse: Limitless.");
            return false;
        }

        const std::vector<int> targets = visibleTargetIndices();
        if (targets.empty()) {
            addMessage("Lapse: Limitless has no visible target.");
            return false;
        }

        selectedTargetIndex_ = targets.front();
        int nearestDistance = SightRadius * SightRadius + 1;
        for (int index : targets) {
            const Actor& enemy = enemies_[index];
            const int distance = distanceSquared(player_.pos, enemy.pos);
            if (distance < nearestDistance) {
                nearestDistance = distance;
                selectedTargetIndex_ = index;
            }
        }
        targeting_ = true;
        addMessage("Select a target, then press 1 or Space to confirm.");
        return false;
    }

    void cycleTarget(int direction) {
        const std::vector<int> targets = visibleTargetIndices();
        if (targets.empty()) {
            targeting_ = false;
            selectedTargetIndex_ = -1;
            return;
        }
        const auto current = std::find(targets.begin(), targets.end(), selectedTargetIndex_);
        int position = current == targets.end() ? 0 : static_cast<int>(current - targets.begin());
        position = (position + direction + static_cast<int>(targets.size())) %
                   static_cast<int>(targets.size());
        selectedTargetIndex_ = targets[position];
    }

    void handleTargetingInput() {
        if (pressedOnce(ImGuiKey_Escape)) {
            targeting_ = false;
            kamutokeTargeting_ = false;
            volcanicTargeting_ = false;
            reversalRedTargeting_ = false;
            collapseTargeting_ = false;
            selectedTargetIndex_ = -1;
            addMessage("Targeting cancelled.");
            return;
        }

        if (volcanicTargeting_ || reversalRedTargeting_ || collapseTargeting_) {
            Point& aimDirection = volcanicTargeting_
                ? volcanicDirection_
                : reversalRedTargeting_ ? reversalRedDirection_
                                        : collapseDirection_;
            if (pressedOnce(ImGuiKey_UpArrow) || pressedOnce(ImGuiKey_Keypad8))
                aimDirection = {0, -1};
            else if (pressedOnce(ImGuiKey_DownArrow) || pressedOnce(ImGuiKey_Keypad2))
                aimDirection = {0, 1};
            else if (pressedOnce(ImGuiKey_LeftArrow) || pressedOnce(ImGuiKey_Keypad4))
                aimDirection = {-1, 0};
            else if (pressedOnce(ImGuiKey_RightArrow) || pressedOnce(ImGuiKey_Keypad6))
                aimDirection = {1, 0};
            else if (pressedOnce(ImGuiKey_Keypad7))
                aimDirection = {-1, -1};
            else if (pressedOnce(ImGuiKey_Keypad9))
                aimDirection = {1, -1};
            else if (pressedOnce(ImGuiKey_Keypad1))
                aimDirection = {-1, 1};
            else if (pressedOnce(ImGuiKey_Keypad3))
                aimDirection = {1, 1};

            const bool confirm =
                (volcanicTargeting_ && pressedOnce(ImGuiKey_1)) ||
                (reversalRedTargeting_ && pressedOnce(ImGuiKey_3)) ||
                (collapseTargeting_ && pressedOnce(ImGuiKey_2)) ||
                pressedOnce(ImGuiKey_Space) ||
                pressedOnce(ImGuiKey_Keypad5);
            if (confirm) {
                const bool completed = volcanicTargeting_
                    ? confirmVolcanicBlast()
                    : reversalRedTargeting_ ? confirmReversalRed()
                                            : confirmCollapse();
                if (completed) finishPlayerTurn();
            }
            return;
        }

        if (pressedOnce(ImGuiKey_Tab) || pressedOnce(ImGuiKey_RightArrow) ||
            pressedOnce(ImGuiKey_DownArrow) || pressedOnce(ImGuiKey_Keypad6) ||
            pressedOnce(ImGuiKey_Keypad2) || pressedOnce(ImGuiKey_Keypad3) ||
            pressedOnce(ImGuiKey_Keypad9))
            cycleTarget(1);
        else if (pressedOnce(ImGuiKey_LeftArrow) || pressedOnce(ImGuiKey_UpArrow) ||
                 pressedOnce(ImGuiKey_Keypad4) || pressedOnce(ImGuiKey_Keypad8) ||
                 pressedOnce(ImGuiKey_Keypad7) || pressedOnce(ImGuiKey_Keypad1))
            cycleTarget(-1);

        const bool confirmSelectedTarget = kamutokeTargeting_
            ? (pressedOnce(ImGuiKey_Q) || pressedOnce(ImGuiKey_Space) ||
               pressedOnce(ImGuiKey_Keypad5))
            : (pressedOnce(ImGuiKey_1) || pressedOnce(ImGuiKey_Space) ||
               pressedOnce(ImGuiKey_Keypad5));
        if (confirmSelectedTarget) {
            const bool completed = kamutokeTargeting_
                ? confirmKamutoke()
                : confirmIndividualAttraction();
            if (completed) finishPlayerTurn();
        }
    }

    bool confirmIndividualAttraction() {
        if (selectedTargetIndex_ < 0 ||
            selectedTargetIndex_ >= static_cast<int>(enemies_.size()) ||
            !enemies_[selectedTargetIndex_].alive()) {
            targeting_ = false;
            selectedTargetIndex_ = -1;
            addMessage("That target is no longer available.");
            return false;
        }
        const Point targetPosition = enemies_[selectedTargetIndex_].pos;
        cursedEnergy_ -= areaTechniqueEnergyCost(IndividualAttractionCost);
        spawnVacuumParticles(targetPosition);
        const int damage = customizedTechniquePower(10);
        int hits = 0;
        addMessage("Lapse: Limitless collapses space around the target.");
        for (Actor& enemy : enemies_) {
            if (!enemy.alive() ||
                std::max(std::abs(enemy.pos.x - targetPosition.x),
                         std::abs(enemy.pos.y - targetPosition.y)) >
                    customizedTechniqueRadius(1))
                continue;
            const int dealtDamage = applyDamage(enemy, damage, true);
            ++hits;
            addColoredMessage("The vacuum hits " + enemy.name + " for " +
                                  std::to_string(dealtDamage) + ".",
                              clanColor(selectedClan_));
            if (!enemy.alive()) announceDeath(enemy);
        }
        if (hits > 1)
            addMessage("Lapse: Limitless strikes " + std::to_string(hits) + " enemies.");
        targeting_ = false;
        selectedTargetIndex_ = -1;
        return true;
    }

    void applyPoisonTick() {
        if (poisonedTurns_ <= 0 || !player_.alive()) return;
        player_.hp -= 3;
        --poisonedTurns_;
        addMessage("Poison deals 3 damage. " + std::to_string(poisonedTurns_) +
                       (poisonedTurns_ == 1 ? " turn remains. " :
                                              " turns remain. ") +
                       "Remaining health:",
                   std::max(0, player_.hp), player_.maxHp);
        if (!player_.alive()) {
            announceDeath(player_);
            addMessage("You died.");
            screen_ = Screen::GameOver;
            advancementMenu_ = false;
            bindingVowMenu_ = false;
        }
    }

    void advanceRestrainedStatus() {
        if (restrainedTurns_ <= 0) return;
        --restrainedTurns_;
        if (restrainedTurns_ == 0)
            addColoredMessage("You tear free from Gossamer Snare.",
                              ImVec4(1.0f, 0.32f, 0.78f, 1.0f));
    }

    void regenerateCursedEnergy() {
        if (player_.alive())
            cursedEnergy_ = std::min(effectiveMaxCursedEnergy(),
                                     cursedEnergy_ + cursedEnergyGeneration_);
        for (Actor& enemy : enemies_) {
            if (enemy.alive() && enemy.maxCursedEnergy > 0)
                enemy.cursedEnergy =
                    std::min(enemy.maxCursedEnergy,
                             enemy.cursedEnergy + enemy.cursedEnergyGeneration);
        }
    }

    void payGyokukenUpkeep() {
        for (Actor& ally : allies_) {
            if (!ally.alive() ||
                ally.classification != ActorClassification::Shikigami)
                continue;
            // Disposable extension summons pay their full cost when created;
            // only persistent Gyokuken consume Cursed Energy each turn.
            if (ally.domainSummon || ally.emberInsect || ally.nue) continue;
            const int baseCost = ally.gyokukenTotality ? TotalityUpkeepCost
                                                       : GyokukenUpkeepCost;
            const int cost = techniqueEnergyCost(baseCost);
            if (cursedEnergy_ >= cost) {
                cursedEnergy_ -= cost;
                continue;
            }
            ally.hp = 0;
            addMessage(ally.name +
                       " dissolves because its Cursed Energy upkeep cannot be paid.");
        }
    }

    void payNeutralBarrierUpkeep() {
        if (!neutralBarrierActive_) return;
        const int cost = cursedEnergyCost(NeutralBarrierUpkeepCost);
        if (cursedEnergy_ >= cost) {
            cursedEnergy_ -= cost;
            return;
        }
        neutralBarrierActive_ = false;
        addMessage(
            "Neutral Limitless collapses because its Cursed Energy upkeep cannot be paid.");
    }

    void payDomainAmplificationUpkeep() {
        if (!domainAmplificationActive_) return;
        const int cost = cursedEnergyCost(DomainAmplificationUpkeepCost);
        if (cursedEnergy_ >= cost) {
            cursedEnergy_ -= cost;
            return;
        }
        domainAmplificationActive_ = false;
        addMessage(
            "Domain Amplification deactivates because its Cursed Energy upkeep cannot be paid.");
    }

    void payHollowWickerBasketUpkeep() {
        if (!hollowWickerBasketActive_) return;
        const int cost = cursedEnergyCost(HollowWickerBasketUpkeepCost);
        if (cursedEnergy_ >= cost) {
            cursedEnergy_ -= cost;
            return;
        }
        hollowWickerBasketActive_ = false;
        addMessage(
            "Hollow Wicker Basket collapses because its Cursed Energy upkeep cannot be paid.");
    }

    void maintainZeninDomainSummons() {
        std::vector<Actor*> caughtEnemies;
        for (Actor& enemy : enemies_)
            if (enemyInsideDomain(enemy) &&
                !enemy.bossDomainAmplificationActive &&
                !enemy.bossHollowWickerBasketActive)
                caughtEnemies.push_back(&enemy);
        int domainSummons = static_cast<int>(std::count_if(
            allies_.begin(), allies_.end(), [](const Actor& ally) {
                return ally.alive() && ally.domainSummon;
            }));

        for (int index = 0;
             index < static_cast<int>(caughtEnemies.size()); ++index) {
            Point spawn{};
            bool foundSpawn = false;
            const Point target = caughtEnemies[index]->pos;
            const bool alreadySummoned = std::any_of(
                allies_.begin(), allies_.end(), [&target](const Actor& ally) {
                    return ally.alive() && ally.domainSummon &&
                           ally.domainSummonAnchor == target;
                });
            if (alreadySummoned) continue;
            for (int y = -1; y <= 1 && !foundSpawn; ++y) {
                for (int x = -1; x <= 1; ++x) {
                    if (x == 0 && y == 0) continue;
                    const Point candidate{target.x + x, target.y + y};
                    if (!blocked(candidate)) {
                        spawn = candidate;
                        foundSpawn = true;
                        break;
                    }
                }
            }
            if (!foundSpawn) continue;

            spawnShadowInkParticles(spawn);
            allies_.push_back({spawn, 15, 15, 5, "Domain Gyokuken", 'w'});
            Actor& summon = allies_.back();
            summon.classification = ActorClassification::Shikigami;
            summon.domainSummon = true;
            summon.domainSummonAnchor = target;
            summon.phantomWhiteGyokuken = domainSummons % 2 == 1;
            addColoredMessage(
                "The domain summons a Gyokuken beside " +
                    caughtEnemies[index]->name + ".",
                clanColor(selectedClan_));
            ++domainSummons;
        }
    }

    void processDomainExpansionTurn() {
        if (!domainExpansionActive_) return;
        if (domainJustActivated_) {
            domainJustActivated_ = false;
        } else {
            const int upkeep = cursedEnergyCost(DomainExpansionUpkeepCost);
            if (cursedEnergy_ < upkeep) {
                resolveAllPlayerDomainClashesAsLosses();
                domainExpansionActive_ = false;
                addMessage(std::string(domainExpansionName()) +
                           " collapses because its upkeep cannot be paid.");
                applyTechniqueBurnout();
                return;
            }
            cursedEnergy_ -= upkeep;
        }

        bool clashing = false;
        const int clashDamage = customizedTechniquePower(10);
        for (Actor& enemy : enemies_) {
            if (!domainsOverlap(enemy)) continue;
            announceDomainClash(enemy);
            enemy.playerDomainDamageNegated += clashDamage;
            clashing = true;
        }
        if (clashing) {
            addMessage("Your domain's " + std::to_string(clashDamage) +
                       " technique damage is held within the clash.");
            return;
        }

        if (selectedClan_ == 1) maintainZeninDomainSummons();
        const int damage = customizedTechniquePower(10);
        int targetsHit = 0;
        for (Actor& enemy : enemies_) {
            if (!enemyInsideDomain(enemy)) continue;
            if (enemy.bossDomainAmplificationActive) {
                addMessage(enemy.name +
                           " nullifies your domain with Domain Amplification.");
                continue;
            }
            if (enemy.bossHollowWickerBasketActive) {
                addMessage(enemy.name +
                           " blocks your domain with Hollow Wicker Basket.");
                continue;
            }
            ++targetsHit;
            if (selectedClan_ == 1) {
                continue;
            } else if (selectedClan_ == 3) {
                ++enemy.ratioStacks;
                addColoredMessage(
                    "Nanami's domain applies Ratio to " + enemy.name + ".",
                    clanColor(selectedClan_));
                attack(player_, enemy, -1, false, false);
            } else {
                const int dealt = applyDamage(enemy, damage, true);
                addColoredMessage("The domain hits " + enemy.name + " for " +
                                      std::to_string(dealt) + ".",
                                  clanColor(selectedClan_));
                if (!enemy.alive()) announceDeath(enemy);
            }
        }
        if (targetsHit == 0)
            addMessage(std::string(domainExpansionName()) +
                       " holds, but contains no enemies.");
    }

    void advanceTechniqueBurnout() {
        if (techniqueBurnoutTurns_ <= 0) return;
        if (burnoutJustApplied_) {
            burnoutJustApplied_ = false;
            return;
        }
        --techniqueBurnoutTurns_;
        if (techniqueBurnoutTurns_ == 0)
            addColoredMessage("Technique Burnout ends. Your Cursed Technique returns.",
                              clanColor(selectedClan_));
    }

    void advanceSorceryHigh() {
        if (sorceryHighTurns_ <= 0) return;
        if (sorceryHighJustApplied_) {
            sorceryHighJustApplied_ = false;
            return;
        }
        --sorceryHighTurns_;
        if (sorceryHighTurns_ == 0)
            addColoredMessage("Sorcery High fades.",
                              ImVec4(0.72f, 0.28f, 0.90f, 1.0f));
    }

    void finishPlayerTurn() {
        applyPoisonTick();
        advanceRestrainedStatus();
        if (!player_.alive()) {
            ++turn_;
            updateFov();
            return;
        }
        payNeutralBarrierUpkeep();
        payDomainAmplificationUpkeep();
        payHollowWickerBasketUpkeep();
        payGyokukenUpkeep();
        processDomainExpansionTurn();
        allyTurns();
        if (player_.alive()) enemyTurns();
        regenerateCursedEnergy();
        enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(),
                       [](const Actor& actor) { return !actor.alive(); }), enemies_.end());
        allies_.erase(std::remove_if(allies_.begin(), allies_.end(),
                      [](const Actor& actor) { return !actor.alive(); }), allies_.end());
        updateDivulgeVow();
        advanceTechniqueBurnout();
        advanceSorceryHigh();
        ++turn_;
        updateFov();
    }

    void tryPlayerMove(Point d) {
        if (domainExpansionActive_) {
            addMessage("You cannot move while maintaining your Domain Expansion.");
            return;
        }
        if (restrainedTurns_ > 0) {
            addMessage("Gossamer Snare Restrains you. Movement is disabled for " +
                       std::to_string(restrainedTurns_) + " more turn" +
                       (restrainedTurns_ == 1 ? "." : "s."));
            return;
        }
        if (playerInsideHostileDomain() && !domainAmplificationActive_) {
            addMessage("The hostile Domain Expansion freezes you in place. "
                       "Activate Domain Amplification [A] to move.");
            return;
        }
        Point target{player_.pos.x + d.x, player_.pos.y + d.y};
        if (!inside(target.x, target.y) || at(target.x, target.y).tile == Tile::Wall) {
            addMessage("A wall blocks your way.");
            return;
        }
        if ((target == upStairs_ || target == downStairs_) &&
            bossExitLocked()) {
            const std::string& gateName = levels_[currentLevel_].bossGateName;
            if (target == downStairs_ && !gateName.empty())
                addMessage("The " + gateName +
                           " is locked. Defeat the boss and recover its Cursed Key.");
            else
                addMessage("A cursed seal locks the stairs. Defeat the boss and recover its key.");
            return;
        }
        if (target == gateOfHeaven_) {
            const LevelState& level = levels_[currentLevel_];
            if (level.bossKeysCollected < level.bossKeysRequired) {
                addMessage("The Gate of Heaven remains sealed. " +
                           std::to_string(level.bossKeysCollected) + " / " +
                           std::to_string(level.bossKeysRequired) +
                           " Cursed Keys recovered.");
            } else {
                addColoredMessage("The Cursed Keys open the Gate of Heaven.",
                                  ImVec4(1.0f, 0.82f, 0.30f, 1.0f));
                screen_ = Screen::Victory;
            }
            return;
        }
        if (Actor* enemy = enemyAt(target)) {
            for (int strike = 0;
                 strike < player_.closeCombatAbility && enemy->alive(); ++strike)
                attack(player_, *enemy);
        }
        else {
            if (Actor* ally = allyAt(target)) {
                ally->pos = player_.pos;
                addMessage("You swap places with " + ally->name + ".");
            } else if (blocked(target)) {
                addMessage("Something blocks your way.");
                return;
            }
            player_.pos = target;
            centerCamera_ = true;
            for (auto it = cursedKeys_.begin(); it != cursedKeys_.end();) {
                if (*it == player_.pos) {
                    it = cursedKeys_.erase(it);
                    LevelState& level = levels_[currentLevel_];
                    ++level.bossKeysCollected;
                    addColoredMessage(
                        "You recover a Cursed Key (" +
                            std::to_string(level.bossKeysCollected) + " / " +
                            std::to_string(level.bossKeysRequired) + ").",
                        ImVec4(0.72f, 0.38f, 0.95f, 1.0f));
                    if (!bossExitLocked())
                        addMessage(gateOfHeaven_.x >= 0
                                       ? "The Gate of Heaven can now be opened."
                                       : !level.bossGateName.empty()
                                           ? "The " + level.bossGateName +
                                                 " unlocks and opens."
                                           : "The cursed seals on the stairs shatter.");
                } else {
                    ++it;
                }
            }
            if (!isJogoClan()) {
                for (auto it = groundItems_.begin(); it != groundItems_.end();) {
                    if (it->pos == player_.pos) {
                        addMessage("You pick up " + it->item.name + ".");
                        inventory_.push_back(std::move(it->item));
                        it = groundItems_.erase(it);
                    } else {
                        ++it;
                    }
                }
            }
            if (target == downStairs_) changeLevel(1);
            else if (target == upStairs_) changeLevel(-1);
            else if (target == pachinkoMachine_) {
                pachinkoMenu_ = true;
                addMessage("The Cursed Pachinko Machine awakens.");
            } else if (target == cursedToolShop_) {
                cursedToolShopMenu_ = true;
                addMessage("You enter the Cursed Tool Shop.");
            }
        }
    }

    void explodeEmberInsect(Actor& insect, Point center) {
        const int damage = customizedTechniquePower(VolcanicBlastDamage);
        spawnEmberExplosionParticles(center);
        int targetsHit = 0;
        addColoredMessage("The Ember Insect detonates!", clanColor(2));
        for (Actor& enemy : enemies_) {
            if (!enemy.alive() ||
                std::max(std::abs(enemy.pos.x - center.x),
                         std::abs(enemy.pos.y - center.y)) >
                    customizedTechniqueRadius(EmberInsectExplosionRadius))
                continue;
            const int dealt = applyDamage(enemy, damage, true);
            ++targetsHit;
            addColoredMessage(
                "The ember explosion hits " + enemy.name + " for " +
                    std::to_string(dealt) + ".",
                clanColor(2));
            if (!enemy.alive()) announceDeath(enemy);
        }
        insect.hp = 0;
        addMessage("The explosion engulfs a " +
                   std::to_string(customizedTechniqueRadius(
                       EmberInsectExplosionRadius)) +
                   "-tile radius and hits " +
                   std::to_string(targetsHit) +
                   (targetsHit == 1 ? " enemy." : " enemies."));
    }

    void explodeNue(Actor& nue, Point center) {
        const int damage =
            (customizedTechniquePower(VolcanicBlastDamage) + 1) / 2;
        const int radius = customizedTechniqueRadius(NueExplosionRadius);
        spawnNueExplosionParticles(center);
        int targetsHit = 0;
        addColoredMessage("Nue erupts in purple lightning!", clanColor(1));
        for (Actor& enemy : enemies_) {
            if (!enemy.alive() ||
                std::max(std::abs(enemy.pos.x - center.x),
                         std::abs(enemy.pos.y - center.y)) > radius)
                continue;
            const int dealt = applyDamage(enemy, damage, true);
            if (enemy.alive()) enemy.stunnedTurns = std::max(enemy.stunnedTurns, 3);
            ++targetsHit;
            addColoredMessage(
                "Nue's lightning hits " + enemy.name + " for " +
                    std::to_string(dealt) +
                    (enemy.alive() ? " and stuns it for 3 turns." : "."),
                clanColor(1));
            if (!enemy.alive()) announceDeath(enemy);
        }
        nue.hp = 0;
        addMessage("The lightning explosion engulfs a " +
                   std::to_string(radius) + "-tile radius and hits " +
                   std::to_string(targetsHit) +
                   (targetsHit == 1 ? " enemy." : " enemies."));
    }

    void allyTurns() {
        for (auto& ally : allies_) {
            if (!ally.alive()) continue;
            Actor* target = nullptr;
            int nearestDistance = (ally.emberInsect || ally.nue)
                ? MapWidth * MapWidth + MapHeight * MapHeight + 1
                : SightRadius * SightRadius + 1;
            for (auto& enemy : enemies_) {
                if (!enemy.alive()) continue;
                const int distance = distanceSquared(ally.pos, enemy.pos);
                const bool detectable = ally.emberInsect || ally.nue ||
                    (distance <= SightRadius * SightRadius &&
                     lineOfSight(ally.pos, enemy.pos));
                if (detectable && distance < nearestDistance) {
                    nearestDistance = distance;
                    target = &enemy;
                }
            }

            Point goal = player_.pos;
            if (target) {
                goal = target->pos;
                const int dx = goal.x - ally.pos.x;
                const int dy = goal.y - ally.pos.y;
                if (std::max(std::abs(dx), std::abs(dy)) <= 1) {
                    if (ally.emberInsect || ally.nue) {
                        if (ally.nue) explodeNue(ally, target->pos);
                        else explodeEmberInsect(ally, target->pos);
                        continue;
                    }
                    // Shadow Wolves are cursed-technique summons, so their
                    // strikes can damage curses after reinforcement is applied.
                    attack(ally, *target, -1, true);
                    continue;
                }
            } else if (std::max(std::abs(goal.x - ally.pos.x),
                                std::abs(goal.y - ally.pos.y)) <= 1) {
                continue;
            }

            const int dx = goal.x - ally.pos.x;
            const int dy = goal.y - ally.pos.y;
            const std::array<Point, 3> steps{{{sign(dx), sign(dy)}, {sign(dx), 0}, {0, sign(dy)}}};
            for (Point step : steps) {
                const Point destination{ally.pos.x + step.x, ally.pos.y + step.y};
                if (!blocked(destination, &ally)) {
                    ally.pos = destination;
                    break;
                }
            }
            if ((ally.emberInsect || ally.nue) && target && ally.alive() &&
                std::max(std::abs(target->pos.x - ally.pos.x),
                         std::abs(target->pos.y - ally.pos.y)) <= 1) {
                if (ally.nue) explodeNue(ally, target->pos);
                else explodeEmberInsect(ally, target->pos);
            }
        }
    }

    Actor* findRoninToCurse(const Actor& spirit) {
        Actor* target = nullptr;
        int nearestDistance = SightRadius * SightRadius + 1;
        for (Actor& candidate : enemies_) {
            if (!candidate.alive() || candidate.cursedHost ||
                candidate.classification != ActorClassification::Human ||
                candidate.sorcerer)
                continue;
            // Samurai resist low-grade possession. Grade 4 Insect Curses can
            // still host ordinary Ronin, but only Grade 3+ curses may take a
            // Samurai as their vessel.
            if (candidate.samurai && spirit.curseGrade == CurseGrade::Grade4)
                continue;
            const int distance = distanceSquared(spirit.pos, candidate.pos);
            if (distance <= SightRadius * SightRadius && distance < nearestDistance &&
                lineOfSight(spirit.pos, candidate.pos)) {
                nearestDistance = distance;
                target = &candidate;
            }
        }
        return target;
    }

    void embedCurse(Actor& spirit, Actor& ronin) {
        const bool wasSamurai = ronin.samurai;
        ronin.cursedHost = true;
        ronin.samuraiHost = wasSamurai;
        ronin.centipedeHost = spirit.centipedeCurse;
        ronin.spiderHost = spirit.spiderGhoul;
        if (wasSamurai) {
            ronin.name = ronin.centipedeHost
                ? "Centipede Samurai Cursed Host"
                : ronin.spiderHost ? "Spider Samurai Cursed Host"
                                   : "Samurai Cursed Host";
        } else {
            ronin.name = ronin.centipedeHost
                ? "Centipede Cursed Host"
                : ronin.spiderHost ? "Spider Cursed Host" : "Cursed Host";
        }
        ronin.embeddedTechnique =
            (spirit.centipedeCurse || spirit.spiderGhoul)
            ? SpiritAttackType::None
            : spirit.spiritAttack;
        addMessage(spirit.name + " embeds its curse into " +
                   std::string(wasSamurai ? "a Samurai." : "a Ronin."));
        if (ronin.centipedeHost) {
            addMessage("The Centipede Cursed Host gains reinforcement and "
                       "both Bite and Stinger Cursed Techniques.");
        } else if (ronin.spiderHost) {
            addMessage("The Spider Cursed Host gains reinforcement and "
                       "Gossamer Snare.");
        } else {
            addMessage("The Cursed Host gains reinforcement and the " +
                       std::string(spiritAttackName(
                           ronin.embeddedTechnique)) +
                       " Cursed Technique.");
        }
    }

    bool processSpiderGhoulAbility(Actor& enemy) {
        if (!enemy.spiderGhoul && !enemy.spiderHost) return false;

        if (enemy.hp * 2 < enemy.maxHp && enemy.cursedEnergy > 0) {
            const int missingHealth = enemy.maxHp - enemy.hp;
            const int energySpent =
                std::min(enemy.cursedEnergy, (missingHealth + 2) / 3);
            const int healing = std::min(missingHealth, energySpent * 3);
            enemy.hp += healing;
            enemy.cursedEnergy -= energySpent;
            addMessage(enemy.name + " consumes " +
                       std::to_string(energySpent) +
                       " Cursed Energy to regenerate " +
                       std::to_string(healing) + " HP.");
            return true;
        }

        const int distance =
            std::max(std::abs(enemy.pos.x - player_.pos.x),
                     std::abs(enemy.pos.y - player_.pos.y));
        if (restrainedTurns_ > 0 || distance > GossamerSnareRange ||
            enemy.cursedEnergy < GossamerSnareCost ||
            !lineOfSight(enemy.pos, player_.pos))
            return false;

        enemy.cursedEnergy -= GossamerSnareCost;
        addColoredMessage(enemy.name + " casts Gossamer Snare!",
                          ImVec4(1.0f, 0.32f, 0.78f, 1.0f));
        const bool stoppedByNeutralLimitless = neutralBarrierActive_;
        attack(enemy, player_, GossamerSnareDamage, true);
        const bool playerBlockedSnare = lastAttackBlockedBySimpleDomain_;
        if (!enemy.alive()) return true;
        for (Actor& ally : allies_) {
            if (!ally.alive() ||
                std::max(std::abs(ally.pos.x - player_.pos.x),
                         std::abs(ally.pos.y - player_.pos.y)) >
                    GossamerSnareRadius)
                continue;
            attack(enemy, ally, GossamerSnareDamage, true);
        }
        if (player_.alive() && !playerBlockedSnare &&
            !stoppedByNeutralLimitless) {
            restrainedTurns_ = RestrainedDuration;
            addMessage("Cursed webbing Restrains you for 3 turns.");
        }
        return true;
    }

    bool processBossDefensiveAbility(Actor& enemy) {
        if (!enemy.bossSorcerer) return false;
        const bool threatenedByDomain =
            domainExpansionActive_ && enemyInsideDomain(enemy);

        if (enemy.bossHollowWickerBasketActive) {
            if (!threatenedByDomain || enemy.cursedEnergy < 5) {
                enemy.bossHollowWickerBasketActive = false;
                addMessage(enemy.name + " releases Hollow Wicker Basket.");
            } else {
                enemy.cursedEnergy -= 5;
                addMessage(enemy.name + " maintains Hollow Wicker Basket.");
                return true;
            }
        }

        if (enemy.bossDomainAmplificationActive) {
            if ((!threatenedByDomain && !neutralBarrierActive_) ||
                enemy.cursedEnergy < 20) {
                enemy.bossDomainAmplificationActive = false;
                addMessage(enemy.name + " releases Domain Amplification.");
            } else {
                enemy.cursedEnergy -= 20;
            }
        }

        if (threatenedByDomain &&
            !enemy.bossDomainAmplificationActive) {
            if (enemy.bossDomainAmplification && enemy.cursedEnergy >= 20) {
                enemy.cursedEnergy -= 20;
                enemy.bossDomainAmplificationActive = true;
                addMessage(enemy.name +
                           " activates Domain Amplification and moves within your domain.");
            } else if (enemy.bossHollowWickerBasket &&
                       enemy.cursedEnergy >= 5) {
                enemy.cursedEnergy -= 5;
                enemy.bossHollowWickerBasketActive = true;
                addMessage(enemy.name +
                           " activates Hollow Wicker Basket and blocks your domain.");
                return true;
            }
        } else if (neutralBarrierActive_ && enemy.bossDomainAmplification &&
                   !enemy.bossDomainAmplificationActive &&
                   enemy.cursedEnergy >= 20) {
            enemy.cursedEnergy -= 20;
            enemy.bossDomainAmplificationActive = true;
            addMessage(enemy.name +
                       " activates Domain Amplification to negate Neutral Limitless.");
        }

        if (enemy.bossDomainAmplificationActive) return false;

        if (enemy.bossReversalHealing && enemy.hp * 2 < enemy.maxHp &&
            enemy.cursedEnergy >= 10) {
            enemy.cursedEnergy -= 10;
            const int healed = std::min(5, enemy.maxHp - enemy.hp);
            enemy.hp += healed;
            addMessage(enemy.name + (enemy.sorcererClan == 2
                           ? " uses Cursed Healing for "
                           : " uses Reversed Cursed Technique: Healing for ") +
                       std::to_string(healed) + " HP.");
            return true;
        }

        if (enemy.bossSimpleDomain && !enemy.bossSimpleDomainReady &&
            enemy.cursedEnergy >= 10 &&
            distanceSquared(enemy.pos, player_.pos) <=
                SightRadius * SightRadius) {
            enemy.cursedEnergy -= 10;
            enemy.bossSimpleDomainReady = true;
            addMessage(enemy.name + " arms Simple Domain.");
            return true;
        }
        return false;
    }

    void enemyTurns() {
        for (auto& enemy : enemies_) {
            if (!enemy.alive()) continue;
            if (enemy.stunnedTurns > 0) {
                --enemy.stunnedTurns;
                addColoredMessage(
                    enemy.name + " is stunned and cannot act" +
                        (enemy.stunnedTurns > 0
                             ? " (" + std::to_string(enemy.stunnedTurns) +
                                   " turns remain)."
                             : "."),
                    ImVec4(0.72f, 0.38f, 1.0f, 1.0f));
                continue;
            }
            if (processBossDefensiveAbility(enemy)) continue;
            if (processHostileDomain(enemy)) {
                if (!player_.alive()) {
                    addMessage("You died.");
                    screen_ = Screen::GameOver;
                    advancementMenu_ = false;
                    bindingVowMenu_ = false;
                    return;
                }
                continue;
            }
            if (domainExpansionActive_ && enemyInsideDomain(enemy) &&
                !enemy.bossDomainAmplificationActive &&
                !enemy.bossHollowWickerBasketActive)
                continue;
            if (processSpiderGhoulAbility(enemy)) {
                if (!player_.alive()) {
                    addMessage("You died.");
                    screen_ = Screen::GameOver;
                    advancementMenu_ = false;
                    bindingVowMenu_ = false;
                    return;
                }
                continue;
            }
            if (useSorcererBossTechnique(enemy)) {
                if (!player_.alive()) {
                    addMessage("You died.");
                    screen_ = Screen::GameOver;
                    advancementMenu_ = false;
                    bindingVowMenu_ = false;
                    return;
                }
                continue;
            }
            if (enemy.cursedHost && enemy.hp * 2 < enemy.maxHp &&
                enemy.cursedEnergy > 0) {
                const int missingHealth = enemy.maxHp - enemy.hp;
                const int energySpent =
                    std::min(enemy.cursedEnergy, (missingHealth + 2) / 3);
                const int healing = std::min(missingHealth, energySpent * 3);
                enemy.hp += healing;
                enemy.cursedEnergy -= energySpent;
                addMessage("The Cursed Host spends " +
                           std::to_string(energySpent) +
                           " Cursed Energy to heal " +
                           std::to_string(healing) + " HP.");
                continue;
            }
            if (enemy.classification == ActorClassification::CursedSpirit &&
                !enemy.bossSorcerer &&
                !(enemy.spiderGhoul &&
                  distanceSquared(enemy.pos, player_.pos) <= 100)) {
                if (Actor* ronin = findRoninToCurse(enemy)) {
                    const int dx = ronin->pos.x - enemy.pos.x;
                    const int dy = ronin->pos.y - enemy.pos.y;
                    if (std::max(std::abs(dx), std::abs(dy)) <= 1) {
                        embedCurse(enemy, *ronin);
                        continue;
                    }
                    const std::array<Point, 3> steps{{
                        {sign(dx), sign(dy)}, {sign(dx), 0}, {0, sign(dy)}
                    }};
                    for (Point step : steps) {
                        const Point destination{enemy.pos.x + step.x, enemy.pos.y + step.y};
                        if (!blocked(destination, &enemy)) {
                            enemy.pos = destination;
                            break;
                        }
                    }
                    continue;
                }
            }
            Actor* adjacentAlly = nullptr;
            for (auto& ally : allies_) {
                if (ally.alive() &&
                    std::max(std::abs(ally.pos.x - enemy.pos.x),
                             std::abs(ally.pos.y - enemy.pos.y)) <= 1) {
                    adjacentAlly = &ally;
                    break;
                }
            }
            if (adjacentAlly) {
                const int strikes = enemy.bossSorcerer
                                        ? enemy.closeCombatAbility
                                        : 1;
                for (int strike = 0;
                     strike < strikes && adjacentAlly->alive(); ++strike)
                    performEnemyAttack(enemy, *adjacentAlly);
                continue;
            }
            const int dx = player_.pos.x - enemy.pos.x;
            const int dy = player_.pos.y - enemy.pos.y;
            if (std::max(std::abs(dx), std::abs(dy)) <= 1) {
                const int strikes = enemy.bossSorcerer
                                        ? enemy.closeCombatAbility
                                        : 1;
                for (int strike = 0;
                     strike < strikes && player_.alive(); ++strike)
                    performEnemyAttack(enemy, player_);
                if (!player_.alive()) {
                    addMessage("You died.");
                    screen_ = Screen::GameOver;
                    advancementMenu_ = false;
                    bindingVowMenu_ = false;
                    return;
                }
                continue;
            }
            const int awareness =
                (enemy.spiderGhoul || enemy.spiderHost) ? 10 : SightRadius;
            if (distanceSquared(enemy.pos, player_.pos) > awareness * awareness)
                continue;

            // Try the direct diagonal step, then each axis. This is intentionally simple AI.
            const std::array<Point, 3> steps{{{sign(dx), sign(dy)}, {sign(dx), 0}, {0, sign(dy)}}};
            for (Point step : steps) {
                Point target{enemy.pos.x + step.x, enemy.pos.y + step.y};
                if (!blocked(target, &enemy)) { enemy.pos = target; break; }
            }
        }
    }

    bool lineOfSight(Point from, Point to) const {
        // Integer Bresenham ray. The target itself may be a wall and remains visible.
        int x = from.x, y = from.y;
        const int dx = std::abs(to.x - from.x), sx = from.x < to.x ? 1 : -1;
        const int dy = -std::abs(to.y - from.y), sy = from.y < to.y ? 1 : -1;
        int error = dx + dy;
        while (!(x == to.x && y == to.y)) {
            const int twice = 2 * error;
            if (twice >= dy) { error += dy; x += sx; }
            if (twice <= dx) { error += dx; y += sy; }
            if (x == to.x && y == to.y) return true;
            if (at(x, y).tile == Tile::Wall) return false;
        }
        return true;
    }

    void updateFov() {
        for (auto& cell : cells_) cell.visible = false;
        for (int y = 0; y < MapHeight; ++y) {
            for (int x = 0; x < MapWidth; ++x) {
                Point p{x, y};
                if (distanceSquared(player_.pos, p) <= SightRadius * SightRadius && lineOfSight(player_.pos, p)) {
                    at(x, y).visible = true;
                    at(x, y).explored = true;
                }
            }
        }
    }

    void drawMainMenu() {
        const ImVec2 available = ImGui::GetContentRegionAvail();
        constexpr float panelWidth = 472.0f;
        constexpr float panelHeight = 513.0f;
        ImGui::SetCursorPos(ImVec2(std::max(8.0f, (available.x - panelWidth) * 0.5f),
                                   std::max(8.0f, (available.y - panelHeight) * 0.5f)));
        beginObsidianChild("MainMenuPanel", ImVec2(panelWidth, panelHeight),
                           ImGuiChildFlags_Borders,
                           ImGuiWindowFlags_NoScrollbar);
        auto centerNextItem = [](float width) {
            ImGui::SetCursorPosX(
                ImGui::GetCursorPosX() +
                std::max(0.0f, (ImGui::GetContentRegionAvail().x - width) * 0.5f));
        };
        ImGui::Dummy(ImVec2(0, 22));
        const char* title = "CURSED RL";
        centerNextItem(ImGui::CalcTextSize(title).x);
        ImGui::TextUnformatted(title);
        const char* subtitle = "TOWER OF MALEVOLENT DIVINITY";
        centerNextItem(ImGui::CalcTextSize(subtitle).x);
        ImGui::TextDisabled("%s", subtitle);
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 24));
        constexpr float menuButtonWidth = 250.0f;
        centerNextItem(menuButtonWidth);
        if (ImGui::Button("New game", ImVec2(menuButtonWidth, 42))) {
            nameInput_.fill('\0');
            screen_ = Screen::NameEntry;
        }
        ImGui::Dummy(ImVec2(0, 8));
        centerNextItem(menuButtonWidth);
        if (ImGui::Button("Controls", ImVec2(menuButtonWidth, 42))) {
            helpReturnToPause_ = false;
            helpMenu_ = true;
        }
        ImGui::Dummy(ImVec2(0, 8));
        centerNextItem(menuButtonWidth);
        if (ImGui::Button("Sound", ImVec2(menuButtonWidth, 42)))
            soundMenu_ = true;
        ImGui::Dummy(ImVec2(0, 8));
        centerNextItem(menuButtonWidth);
        if (ImGui::Button("Tower Rules", ImVec2(menuButtonWidth, 42))) {
            rulesReturnToPause_ = false;
            rulesMenu_ = true;
        }
        ImGui::Dummy(ImVec2(0, 8));
        centerNextItem(menuButtonWidth);
        if (ImGui::Button("Quit", ImVec2(menuButtonWidth, 42)))
            quitRequested_ = true;
        ImGui::EndChild();
    }

    void drawNameEntry() {
        const ImVec2 available = ImGui::GetContentRegionAvail();
        constexpr float panelWidth = 552.0f;
        constexpr float panelHeight = 377.0f;
        ImGui::SetCursorPos(ImVec2(
            std::max(8.0f, (available.x - panelWidth) * 0.5f),
            std::max(8.0f, (available.y - panelHeight) * 0.5f)));
        beginObsidianChild("NameEntryPanel", ImVec2(panelWidth, panelHeight),
                           ImGuiChildFlags_Borders,
                           ImGuiWindowFlags_NoScrollbar);
        ImGui::Dummy(ImVec2(0, 14));
        ImGui::TextUnformatted("NAME YOUR SORCERER");
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextWrapped(
            "Enter a given name. Your chosen clan will become your surname.");
        ImGui::Spacing();
        ImGui::SetNextItemWidth(-1.0f);
        const bool submitted = ImGui::InputText(
            "##PlayerGivenName", nameInput_.data(), nameInput_.size(),
            ImGuiInputTextFlags_EnterReturnsTrue);
        const std::string givenName = enteredGivenName();
        if (!givenName.empty())
            ImGui::TextDisabled("Example: %s %s", givenName.c_str(),
                                ClanNames[selectedClan_]);
        else
            ImGui::TextDisabled("A name is required.");

        ImGui::Dummy(ImVec2(0, 18));
        const float buttonWidth =
            (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) *
            0.5f;
        ImGui::BeginDisabled(givenName.empty());
        const bool continueClicked =
            ImGui::Button("Continue", ImVec2(buttonWidth, 42));
        ImGui::EndDisabled();
        if ((submitted || continueClicked) && !givenName.empty())
            screen_ = Screen::CharacterSelect;
        ImGui::SameLine();
        if (ImGui::Button("Back", ImVec2(buttonWidth, 42)))
            screen_ = Screen::MainMenu;
        ImGui::TextDisabled("Press Enter to continue or Escape to return.");
        ImGui::EndChild();
    }

    void drawCharacterSelect() {
        const ImVec2 available = ImGui::GetContentRegionAvail();
        const float panelWidth = std::min(992.0f, available.x - 16.0f);
        const float panelHeight = std::min(712.0f, available.y - 16.0f);
        ImGui::SetCursorPos(ImVec2(std::max(8.0f, (available.x - panelWidth) * 0.5f),
                                   std::max(8.0f, (available.y - panelHeight) * 0.5f)));
        beginObsidianChild("CharacterSelectPanel",
                           ImVec2(panelWidth, panelHeight));
        ImGui::TextUnformatted("CHOOSE YOUR CLAN");
        ImGui::TextDisabled("%s's clan becomes their surname.",
                            enteredGivenName().c_str());
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::BeginTable("ClanSelectionAndPreview", 2,
                              ImGuiTableFlags_SizingStretchProp |
                                  ImGuiTableFlags_PadOuterX)) {
            ImGui::TableSetupColumn("Clans", ImGuiTableColumnFlags_WidthStretch,
                                    3.0f);
            ImGui::TableSetupColumn("Preview",
                                    ImGuiTableColumnFlags_WidthStretch, 1.0f);
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            const auto drawClanOption = [this](int i) {
                ImGui::PushID(i);
                const bool selected = selectedClan_ == i;
                const float cardWidth = ImGui::GetContentRegionAvail().x;
                const float contentWidth = std::max(80.0f, cardWidth - 18.0f);
                const float lineHeight = ImGui::GetTextLineHeightWithSpacing();
                const float descriptionHeight =
                    ImGui::CalcTextSize(ClanTechniqueDescriptions[i], nullptr,
                                        false, contentWidth).y;
                const float cardHeight =
                    lineHeight * 2.0f + descriptionHeight + 18.0f;
                const ImVec2 cardStart = ImGui::GetCursorScreenPos();
                if (ImGui::Selectable("##ClanChoice", selected, 0,
                                      ImVec2(cardWidth, cardHeight))) {
                    selectedClan_ = i;
                }
                const ImVec2 afterCard = ImGui::GetCursorScreenPos();
                ImGui::SetCursorScreenPos(
                    ImVec2(cardStart.x + 9.0f, cardStart.y + 7.0f));
                ImGui::BeginGroup();
                ImGui::TextColored(clanColor(i), "%s Clan", ClanNames[i]);
                ImGui::TextColored(clanColor(i), "Cursed Technique: %s",
                                   ClanTechniques[i]);
                ImGui::PushTextWrapPos(cardStart.x + cardWidth - 9.0f);
                ImGui::TextDisabled("%s", ClanTechniqueDescriptions[i]);
                ImGui::PopTextWrapPos();
                ImGui::EndGroup();
                ImGui::SetCursorScreenPos(afterCard);
                ImGui::PopID();
            };
            if (ImGui::BeginTabBar("ClanTypeTabs")) {
                ImGui::PushStyleColor(ImGuiCol_Text,
                                      ImVec4(0.42f, 0.72f, 1.0f, 1.0f));
                const bool sorcerersOpen = ImGui::BeginTabItem("Sorcerers");
                ImGui::PopStyleColor();
                if (sorcerersOpen) {
                    drawClanOption(0); // Gojo
                    drawClanOption(1); // Zenin
                    drawClanOption(3); // Nanami
                    ImGui::EndTabItem();
                }

                ImGui::PushStyleColor(ImGuiCol_Text,
                                      ImVec4(0.92f, 0.30f, 0.24f, 1.0f));
                const bool cursesOpen =
                    ImGui::BeginTabItem("Cursed Spirits");
                ImGui::PopStyleColor();
                if (cursesOpen) {
                    drawClanOption(2); // Jogo
                    ImGui::TextDisabled(
                        "Additional cursed-spirit clans will appear here.");
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }

            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(clanColor(selectedClan_), "%s Clan",
                               ClanNames[selectedClan_]);
            ImGui::TextDisabled("In-game sprite");
            ImGui::Spacing();
            if (mapUnitSpriteTexture != 0) {
                constexpr float atlasCell = 0.25f;
                const float u0 = static_cast<float>(selectedClan_) * atlasCell;
                const float previewWidth =
                    std::min(210.0f, ImGui::GetContentRegionAvail().x);
                const float previewHeight = previewWidth * 0.80f;
                ImGui::Image(static_cast<ImTextureID>(mapUnitSpriteTexture),
                             ImVec2(previewWidth, previewHeight),
                             ImVec2(u0, 0.0f),
                             ImVec2(u0 + atlasCell, atlasCell));
            } else {
                ImGui::TextDisabled("Sprite unavailable");
            }
            ImGui::EndTable();
        }

        ImGui::SeparatorText("Birth Points");
        ImGui::Text("Available: %d / %d",
                    birthPointBudget() - spentBirthPoints(),
                    birthPointBudget());
        ImGui::TextDisabled("Spend points on starting stats or traits.");

        if (ImGui::BeginTable("BirthStatPurchases", 3,
                              ImGuiTableFlags_SizingStretchProp |
                                  ImGuiTableFlags_RowBg |
                                  ImGuiTableFlags_PadOuterX)) {
            ImGui::TableSetupColumn("Stat", ImGuiTableColumnFlags_WidthFixed,
                                    270.0f);
            ImGui::TableSetupColumn("Controls",
                                    ImGuiTableColumnFlags_WidthFixed, 82.0f);
            ImGui::TableSetupColumn("Benefit",
                                    ImGuiTableColumnFlags_WidthStretch);
            auto drawStatPurchase = [this](const char* label, int& points,
                                           const char* benefit, int pointCost) {
                ImGui::PushID(label);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s: %d upgrades (%d points)",
                            label, points, points * pointCost);
                ImGui::TableSetColumnIndex(1);
                if (ImGui::Button("-", ImVec2(32, 0)) && points > 0) --points;
                ImGui::SameLine();
                ImGui::BeginDisabled(
                    birthPointBudget() - spentBirthPoints() < pointCost);
                if (ImGui::Button("+", ImVec2(32, 0))) ++points;
                ImGui::EndDisabled();
                ImGui::TableSetColumnIndex(2);
                ImGui::TextDisabled("%s", benefit);
                ImGui::PopID();
            };
            drawStatPurchase("Health", birthHealthPoints_, "+10 HP per point", 1);
            drawStatPurchase("Attack", birthAttackPoints_, "+1 attack per point", 1);
            drawStatPurchase("Close Combat Ability", birthCloseCombatPoints_,
                             "+1 melee attack per action for 3 points", 3);
            drawStatPurchase("CT Power", birthTechniquePowerPoints_,
                             "+2 technique damage per point", 1);
            drawStatPurchase("Cursed Energy", birthEnergyPoints_,
                             "+10 energy per point", 1);
            drawStatPurchase("Efficiency", birthEfficiencyPoints_,
                             "+5% per point", 1);
            drawStatPurchase("CE Generation", birthGenerationPoints_,
                             "+1 per turn for 2 points", 2);
            ImGui::EndTable();
        }

        ImGui::TextDisabled("Starting stats");
        ImGui::TextColored(healthColor(), "Health: %d",
                           30 + birthHealthPoints_ * 10);
        ImGui::SameLine(135.0f);
        ImGui::TextColored(attackColor(), "Attack: %d",
                           5 + birthAttackPoints_);
        ImGui::SameLine(260.0f);
        ImGui::TextColored(energyColor(), "CE: %d",
                           100 + birthEnergyPoints_ * 10);
        ImGui::SameLine(365.0f);
        ImGui::TextColored(efficiencyColor(), "Efficiency: %d%%",
                           100 + birthEfficiencyPoints_ * 5);
        ImGui::SameLine(520.0f);
        ImGui::TextColored(generationColor(), "Gen: %d",
                           1 + birthGenerationPoints_);
        ImGui::SameLine(620.0f);
        ImGui::TextColored(ImVec4(0.52f, 0.70f, 1.0f, 1.0f),
                           "CT Power: +%d damage",
                           birthTechniquePowerPoints_ * 2);
        ImGui::TextColored(ImVec4(0.92f, 0.68f, 0.28f, 1.0f),
                           "Close Combat Ability: %d attack%s per melee action",
                           1 + birthCloseCombatPoints_,
                           1 + birthCloseCombatPoints_ == 1 ? "" : "s");

        ImGui::SeparatorText("Traits");
        if (ImGui::BeginTabBar("BirthTraitTabs")) {
            ImGui::PushStyleColor(ImGuiCol_Text,
                                  ImVec4(0.35f, 0.92f, 0.55f, 1.0f));
            const bool positiveOpen = ImGui::BeginTabItem("Positive Traits");
            ImGui::PopStyleColor();
            if (positiveOpen) {
                ImGui::TextColored(ImVec4(0.45f, 0.92f, 0.65f, 1.0f),
                                   "Serrated Cursed Energy");
                ImGui::TextWrapped(
                    "Reflects 1 damage to an attacker for every point of defense "
                    "reinforcement actually used against its strike.");
                if (serratedTraitSelected_) {
                    if (ImGui::Button("Remove Serrated (-3 points)"))
                        serratedTraitSelected_ = false;
                } else {
                    ImGui::BeginDisabled(
                        birthPointBudget() - spentBirthPoints() < 3);
                    if (ImGui::Button("Select Serrated (3 points)"))
                        serratedTraitSelected_ = true;
                    ImGui::EndDisabled();
                }

                ImGui::Separator();
                if (isJogoClan()) {
                    ImGui::TextColored(ImVec4(1.0f, 0.38f, 0.22f, 1.0f),
                                       "Cursed Healing (Innate)");
                    ImGui::TextWrapped(
                        "Jogo clan members are Cursed Spirits and cannot learn "
                        "Reversed Cursed Technique. They begin with Cursed "
                        "Healing, which has the same healing and energy scaling.");
                    ImGui::TextDisabled("Automatically learned — no Birth Point cost");
                } else {
                    ImGui::TextColored(ImVec4(0.40f, 0.82f, 1.0f, 1.0f),
                                       "Reversed Cursed Technique: Healing");
                    ImGui::TextWrapped(
                        "Requires Grade 2 and must be learned through Grade "
                        "Advancement. It is unavailable at character creation.");
                    ImGui::TextDisabled("Grade 2 minimum");
                }

                ImGui::Separator();
                ImGui::TextColored(ImVec4(0.72f, 0.62f, 1.0f, 1.0f),
                                   "Simple Domain");
                ImGui::TextWrapped(
                    "Press X to spend 10 base Cursed Energy and arm a domain. "
                    "It blocks the next direct attack and counters for 1.5x "
                    "base Attack damage.");
                ImGui::TextDisabled("Birth Point cost: 3");
                if (simpleDomainTraitSelected_) {
                    if (ImGui::Button("Remove Simple Domain (-3 points)"))
                        simpleDomainTraitSelected_ = false;
                } else {
                    ImGui::BeginDisabled(
                        birthPointBudget() - spentBirthPoints() < 3);
                    if (ImGui::Button("Select Simple Domain (3 points)"))
                        simpleDomainTraitSelected_ = true;
                    ImGui::EndDisabled();
                }
                ImGui::EndTabItem();
            }

            ImGui::PushStyleColor(ImGuiCol_Text,
                                  ImVec4(1.0f, 0.40f, 0.35f, 1.0f));
            const bool negativeOpen = ImGui::BeginTabItem("Negative Traits");
            ImGui::PopStyleColor();
            if (negativeOpen) {
                ImGui::TextColored(ImVec4(1.0f, 0.48f, 0.34f, 1.0f),
                                   "No Innate Technique");
                ImGui::TextWrapped(
                    "Gain 5 additional Birth Points, but lose access to your "
                    "clan's Cursed Technique. Cursed Energy Reinforcement "
                    "remains available.");
                if (noInnateTechniqueSelected_) {
                    const bool removalWouldOverspend = spentBirthPoints() > 5;
                    ImGui::BeginDisabled(removalWouldOverspend);
                    if (ImGui::Button(
                            "Remove No Innate Technique (-5 points)"))
                        noInnateTechniqueSelected_ = false;
                    ImGui::EndDisabled();
                    if (removalWouldOverspend)
                        ImGui::TextDisabled(
                            "Spend 5 or fewer points before removing this trait.");
                } else if (ImGui::Button(
                               "Select No Innate Technique (+5 points)")) {
                    noInnateTechniqueSelected_ = true;
                }
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::Dummy(ImVec2(0, 10));
        const float buttonWidth =
            (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) *
            0.5f;
        const char* continueLabel =
            isJogoClan() ? "Begin adventure" : "Choose starting item";
        if (ImGui::Button(continueLabel, ImVec2(buttonWidth, 42))) {
            if (isJogoClan()) {
                newGame();
                screen_ = Screen::Playing;
            } else {
                screen_ = Screen::StartingItem;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Back", ImVec2(buttonWidth, 42)))
            screen_ = Screen::NameEntry;
        ImGui::EndChild();
    }

    void drawStartingItemSelect() {
        const ImVec2 available = ImGui::GetContentRegionAvail();
        const float panelWidth = std::min(712.0f, available.x - 16.0f);
        const float panelHeight = std::min(592.0f, available.y - 16.0f);
        ImGui::SetCursorPos(ImVec2(std::max(8.0f, (available.x - panelWidth) * 0.5f),
                                   std::max(8.0f, (available.y - panelHeight) * 0.5f)));
        beginObsidianChild("StartingItemPanel", ImVec2(panelWidth, panelHeight));
        ImGui::TextUnformatted("CHOOSE YOUR STARTING ITEM");
        ImGui::TextDisabled("Choose one. It will be carried and equipped when the run begins.");
        ImGui::Separator();
        ImGui::Spacing();

        for (int itemIndex = 0; itemIndex < 3; ++itemIndex) {
            const Item item = makeEquipmentItem(itemIndex);
            ImGui::PushID(itemIndex + 500);
            if (ImGui::Selectable(item.name.c_str(),
                                  selectedBirthItem_ == itemIndex,
                                  0, ImVec2(0, 34)))
                selectedBirthItem_ = itemIndex;
            ImGui::TextDisabled("Slot: %s", equipmentSlotName(item.slot));
            if (item.baseAttackMultiplierPercent > 100)
                ImGui::Text("Stats: Base Attack x%.2g",
                            item.baseAttackMultiplierPercent / 100.0f);
            else if (item.attackBonus > 0)
                ImGui::Text("Stats: +%d melee damage", item.attackBonus);
            else if (item.reducesDamageByQuarter)
                ImGui::Text("Stats: Reduces direct-hit damage by 25%%");
            else if (item.defenseBonus > 0)
                ImGui::Text("Stats: Blocks %d direct damage per hit",
                            item.defenseBonus);
            else if (item.cursedDamage)
                ImGui::TextColored(ImVec4(0.85f, 0.45f, 1.0f, 1.0f),
                                   "Stats: Melee attacks deal cursed damage");
            ImGui::TextWrapped("%s", item.description.c_str());
            ImGui::Separator();
            ImGui::PopID();
        }

        const float buttonWidth =
            (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) *
            0.5f;
        if (ImGui::Button("Begin adventure", ImVec2(buttonWidth, 42))) {
            newGame();
            screen_ = Screen::Playing;
        }
        ImGui::SameLine();
        if (ImGui::Button("Back", ImVec2(buttonWidth, 42)))
            screen_ = Screen::CharacterSelect;
        ImGui::TextDisabled("Escape also returns to character generation.");
        ImGui::EndChild();
    }

    void drawGameOver() {
        const ImVec2 available = ImGui::GetContentRegionAvail();
        constexpr float panelWidth = 522.0f;
        constexpr float panelHeight = 457.0f;
        ImGui::SetCursorPos(ImVec2(std::max(8.0f, (available.x - panelWidth) * 0.5f),
                                   std::max(8.0f, (available.y - panelHeight) * 0.5f)));
        beginObsidianChild("GameOverPanel", ImVec2(panelWidth, panelHeight),
                           ImGuiChildFlags_Borders,
                           ImGuiWindowFlags_NoScrollbar);
        ImGui::Dummy(ImVec2(0, 18));
        const char* title = "GAME OVER";
        ImGui::SetCursorPosX((panelWidth - ImGui::CalcTextSize(title).x) * 0.5f);
        ImGui::TextColored(ImVec4(0.92f, 0.20f, 0.20f, 1.0f), "%s", title);
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, 10));
        ImGui::TextColored(clanColor(selectedClan_), "Clan: %s",
                           playerClan_.c_str());
        ImGui::Text("Name: %s", playerFullName_.c_str());
        ImGui::Text("Dungeon level reached: %d / %d", currentLevel_ + 1,
                    static_cast<int>(levels_.size()));
        ImGui::Text("Turns survived: %d", turn_);
        ImGui::Text("Current / Earned / Total / Spent: %d / %d / %d / %d",
                    points_, earnedPoints_, totalPointsEarned_, spentPoints_);
        ImGui::TextColored(playerGradeColor(playerGrade()), "Rank: %s",
                           playerGradeName(playerGrade()));
        ImGui::Text("Cursed Energy remaining: %d / %d", cursedEnergy_,
                    effectiveMaxCursedEnergy());
        if (serratedTraitSelected_)
            ImGui::Text("Trait: Serrated Cursed Energy");
        ImGui::Dummy(ImVec2(0, 18));

        ImGui::SetCursorPosX(65.0f);
        if (ImGui::Button("Create new character", ImVec2(300, 40)))
            screen_ = Screen::NameEntry;
        ImGui::SetCursorPosX(65.0f);
        if (ImGui::Button("Main menu", ImVec2(300, 40)))
            screen_ = Screen::MainMenu;
        ImGui::SetCursorPosX(65.0f);
        if (ImGui::Button("Quit", ImVec2(300, 40)))
            quitRequested_ = true;
        ImGui::EndChild();
    }

    void drawVictory() {
        const ImVec2 available = ImGui::GetContentRegionAvail();
        constexpr float panelWidth = 560.0f;
        constexpr float panelHeight = 430.0f;
        ImGui::SetCursorPos(ImVec2(
            std::max(8.0f, (available.x - panelWidth) * 0.5f),
            std::max(8.0f, (available.y - panelHeight) * 0.5f)));
        beginObsidianChild("VictoryPanel", ImVec2(panelWidth, panelHeight),
                           ImGuiChildFlags_Borders,
                           ImGuiWindowFlags_NoScrollbar);
        ImGui::Dummy(ImVec2(0, 22));
        const char* title = "THE GATE OF HEAVEN OPENS";
        ImGui::SetCursorPosX((panelWidth - ImGui::CalcTextSize(title).x) * 0.5f);
        ImGui::TextColored(ImVec4(1.0f, 0.82f, 0.28f, 1.0f), "%s", title);
        ImGui::Separator();
        ImGui::TextWrapped(
            "All four final guardians have fallen. Their Cursed Keys release the celestial seal, and you escape the fifteen-level dungeon.");
        ImGui::Spacing();
        ImGui::TextColored(clanColor(selectedClan_), "Victor: %s",
                           playerFullName_.c_str());
        ImGui::Text("Turns taken: %d", turn_);
        ImGui::Text("Current / Earned / Total / Spent: %d / %d / %d / %d",
                    points_, earnedPoints_, totalPointsEarned_, spentPoints_);
        ImGui::TextColored(playerGradeColor(playerGrade()), "Rank: %s",
                           playerGradeName(playerGrade()));
        ImGui::Dummy(ImVec2(0, 20));
        ImGui::SetCursorPosX(100.0f);
        if (ImGui::Button("Create new character", ImVec2(350, 42)))
            screen_ = Screen::NameEntry;
        ImGui::SetCursorPosX(100.0f);
        if (ImGui::Button("Main menu", ImVec2(350, 42)))
            screen_ = Screen::MainMenu;
        ImGui::SetCursorPosX(100.0f);
        if (ImGui::Button("Quit", ImVec2(350, 42)))
            quitRequested_ = true;
        ImGui::EndChild();
    }

    void drawBindingVowMenu() {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(672, 612), ImGuiCond_Always);
        constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings;
        if (!beginObsidianWindow("Binding Vows", &bindingVowMenu_, flags)) {
            ImGui::End();
            return;
        }

        ImGui::TextUnformatted("DIVULGE CURSED TECHNIQUE");
        ImGui::Separator();
        ImGui::TextWrapped(
            "Reveal the nature of your Cursed Technique to every enemy currently in view. "
            "Those enemies gain Knowledge and deal +1 damage.");
        ImGui::Spacing();
        ImGui::TextWrapped(
            "In exchange, your Cursed Energy Efficiency increases by 25%% until every "
            "enemy carrying that Knowledge is defeated.");
        ImGui::Spacing();
        if (divulgeVowActive_) {
            ImGui::TextColored(ImVec4(1.0f, 0.78f, 0.20f, 1.0f),
                               "ACTIVE — %d knowledgeable enemies remain",
                               knowledgeableEnemyCount());
            ImGui::BeginDisabled();
            ImGui::Button("Accept vow", ImVec2(250, 42));
            ImGui::EndDisabled();
        } else if (ImGui::Button("Accept vow", ImVec2(250, 42))) {
            activateDivulgeVow();
        }
        ImGui::SeparatorText("HAND SIGNS");
        ImGui::TextWrapped(
            "Uses hand signs to improve the power of Cursed Techniques by "
            "1.25x, but prevents the use of weapons.");
        ImGui::Spacing();
        if (handSignsVowActive_) {
            ImGui::TextColored(ImVec4(0.35f, 0.72f, 1.0f, 1.0f), "ACTIVE");
            if (ImGui::Button("End Hand Signs", ImVec2(-1, 40)))
                toggleHandSignsVow();
        } else if (ImGui::Button("Activate Hand Signs", ImVec2(-1, 40))) {
            toggleHandSignsVow();
        }

        ImGui::Spacing();
        if (ImGui::Button("Close", ImVec2(-1, 40))) bindingVowMenu_ = false;
        ImGui::TextDisabled("Press B or Escape to close.");
        ImGui::End();
    }

    void drawAdvancementMenu() {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(692, 632), ImGuiCond_Always);
        constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings;
        if (!beginObsidianWindow("Grade Advancement",
                                 nullptr, flags)) {
            ImGui::End();
            return;
        }

        const bool canAdvance = advancementAvailable();
        ImGui::TextColored(playerGradeColor(playerGrade()), "Current rank: %s",
                           playerGradeName(playerGrade()));
        ImGui::Text("Current points total: %d", points_);
        ImGui::Text("Earned Points: %d", earnedPoints_);
        ImGui::Text("Total Earned Points: %d", totalPointsEarned_);
        ImGui::Text("Spent Points: %d", spentPoints_);
        if (canAdvance) {
            ImGui::TextColored(ImVec4(0.45f, 0.92f, 0.55f, 1.0f),
                               "ADVANCEMENT AVAILABLE");
        } else {
            const int threshold = nextAdvancementThreshold();
            if (threshold > 0)
                ImGui::TextDisabled("Next advancement: %d / %d earned points",
                                    earnedPoints_, threshold);
            else
                ImGui::TextDisabled("No further grade advancement is available.");
            ImGui::TextWrapped(
                "You may review this screen, but improvements are locked until you earn the next grade.");
        }
        ImGui::BeginDisabled(!canAdvance);
        ImGui::SeparatorText("Stat Improvements");
        ImGui::TextColored(healthColor(), "Health: %d / %d",
                           player_.hp, player_.maxHp);
        ImGui::SameLine(320.0f);
        ImGui::BeginDisabled(points_ < advancementHealthCost_);
        const std::string healthLabel = "+10 maximum and current HP (" +
                                        std::to_string(advancementHealthCost_) +
                                        " points)";
        if (ImGui::Button(healthLabel.c_str(), ImVec2(245, 0))) {
            spendPoints(advancementHealthCost_++);
            player_.maxHp += 10;
            player_.hp += 10;
        }
        ImGui::EndDisabled();

        ImGui::TextColored(ImVec4(0.92f, 0.68f, 0.28f, 1.0f),
                           "Close Combat Ability: %d",
                           player_.closeCombatAbility);
        ImGui::SameLine(320.0f);
        ImGui::BeginDisabled(points_ < advancementCloseCombatCost_);
        const std::string closeCombatLabel = "+1 melee attack (" +
            std::to_string(advancementCloseCombatCost_) + " points)";
        if (ImGui::Button(closeCombatLabel.c_str(), ImVec2(245, 0))) {
            spendPoints(advancementCloseCombatCost_++);
            ++player_.closeCombatAbility;
        }
        ImGui::EndDisabled();

        ImGui::TextColored(attackColor(), "Base Attack: %d", player_.attack);
        ImGui::SameLine(320.0f);
        ImGui::BeginDisabled(points_ < advancementAttackCost_);
        const std::string attackLabel = "+2 attack (" +
                                        std::to_string(advancementAttackCost_) +
                                        " points)";
        if (ImGui::Button(attackLabel.c_str(), ImVec2(245, 0))) {
            spendPoints(advancementAttackCost_++);
            player_.attack += 2;
        }
        ImGui::EndDisabled();

        ImGui::TextColored(ImVec4(0.52f, 0.70f, 1.0f, 1.0f),
                           "Cursed Technique Power: +%d",
                           cursedTechniquePowerBonus_);
        ImGui::SameLine(320.0f);
        ImGui::BeginDisabled(points_ < advancementTechniquePowerCost_);
        const std::string techniquePowerLabel = "+2 technique damage (" +
            std::to_string(advancementTechniquePowerCost_) + " points)";
        if (ImGui::Button(techniquePowerLabel.c_str(), ImVec2(245, 0))) {
            spendPoints(advancementTechniquePowerCost_++);
            cursedTechniquePowerBonus_ += 2;
        }
        ImGui::EndDisabled();

        ImGui::TextColored(energyColor(), "Cursed Energy: %d / %d",
                           cursedEnergy_, effectiveMaxCursedEnergy());
        ImGui::SameLine(320.0f);
        ImGui::BeginDisabled(points_ < advancementEnergyCost_);
        const std::string energyLabel = "+10 maximum energy (" +
                                        std::to_string(advancementEnergyCost_) +
                                        " points)";
        if (ImGui::Button(energyLabel.c_str(), ImVec2(245, 0))) {
            spendPoints(advancementEnergyCost_++);
            maxCursedEnergy_ += 10;
            cursedEnergy_ += 10;
        }
        ImGui::EndDisabled();

        ImGui::TextColored(efficiencyColor(),
                           "Cursed Energy Efficiency: %d%%",
                           effectiveCursedEnergyEfficiency());
        ImGui::SameLine(320.0f);
        ImGui::BeginDisabled(points_ < advancementEfficiencyCost_);
        const std::string efficiencyLabel = "+5% efficiency (" +
            std::to_string(advancementEfficiencyCost_) + " points)";
        if (ImGui::Button(efficiencyLabel.c_str(), ImVec2(245, 0))) {
            spendPoints(advancementEfficiencyCost_++);
            cursedEnergyEfficiency_ += 5;
        }
        ImGui::EndDisabled();

        ImGui::TextColored(generationColor(),
                           "Cursed Energy Generation: %d / turn",
                           cursedEnergyGeneration_);
        ImGui::SameLine(320.0f);
        ImGui::BeginDisabled(points_ < advancementGenerationCost_);
        const std::string generationLabel = "+1 generation (" +
            std::to_string(advancementGenerationCost_) + " points)";
        if (ImGui::Button(generationLabel.c_str(), ImVec2(245, 0))) {
            spendPoints(advancementGenerationCost_++);
            ++cursedEnergyGeneration_;
        }
        ImGui::EndDisabled();

        ImGui::SeparatorText("Learn Abilities");
        ImGui::TextUnformatted(healingAbilityName());
        ImGui::TextWrapped(
            "Heal 5 HP per 10 base Cursed Energy. The base spend is selected in "
            "10-energy increments; efficiency reduces the energy actually paid.");
        if (isJogoClan())
            ImGui::TextDisabled(
                "Cursed Spirits use Cursed Healing instead of Reverse Cursed Technique.");
        if (reversalHealingLearned_) {
            ImGui::TextColored(ImVec4(0.45f, 0.92f, 0.55f, 1.0f), "LEARNED");
        } else {
            const bool gradeTwoReached =
                static_cast<int>(playerGrade()) >=
                static_cast<int>(PlayerGrade::Grade2);
            ImGui::BeginDisabled(points_ < 10 || !gradeTwoReached);
            if (ImGui::Button("Learn Healing (10 points)", ImVec2(320, 40))) {
                spendPoints(10);
                reversalHealingLearned_ = true;
            }
            ImGui::EndDisabled();
            if (!gradeTwoReached)
                ImGui::TextDisabled("Requires player rank Grade 2.");
        }

        ImGui::Separator();
        ImGui::TextUnformatted("Simple Domain");
        ImGui::TextWrapped(
            "Spend 10 base Cursed Energy to block and counter the next "
            "direct attack.");
        if (simpleDomainLearned_) {
            ImGui::TextColored(ImVec4(0.45f, 0.92f, 0.55f, 1.0f), "LEARNED");
        } else {
            ImGui::BeginDisabled(points_ < 10);
            if (ImGui::Button("Learn Simple Domain (10 points)",
                              ImVec2(320, 40))) {
                spendPoints(10);
                simpleDomainLearned_ = true;
            }
            ImGui::EndDisabled();
        }

        ImGui::Separator();
        ImGui::TextUnformatted("Domain Amplification");
        ImGui::TextWrapped(
            "Toggle with [A]. Nullifies hostile Domain effects and cursed-technique "
            "defenses, but suppresses your own Cursed Technique and restricts you "
            "to melee attacks while active. Costs 20 base Cursed Energy per turn.");
        if (domainAmplificationLearned_) {
            ImGui::TextColored(ImVec4(0.45f, 0.92f, 0.55f, 1.0f), "LEARNED");
        } else {
            const bool gradeTwoReached =
                static_cast<int>(playerGrade()) >=
                static_cast<int>(PlayerGrade::Grade2);
            ImGui::BeginDisabled(points_ < 20 || !gradeTwoReached);
            if (ImGui::Button("Learn Domain Amplification (20 points)",
                              ImVec2(320, 40))) {
                spendPoints(20);
                domainAmplificationLearned_ = true;
            }
            ImGui::EndDisabled();
            if (!gradeTwoReached)
                ImGui::TextDisabled("Requires player rank Grade 2.");
        }

        ImGui::Separator();
        ImGui::TextUnformatted("Hollow Wicker Basket");
        ImGui::TextWrapped(
            "Anti-domain toggle [V]. Blocks hostile Domain damage and can be used "
            "during Technique Burnout. While active, you can only wait to maintain "
            "it or press [V] to release it. Costs 5 base Cursed Energy per turn.");
        if (hollowWickerBasketLearned_) {
            ImGui::TextColored(ImVec4(0.45f, 0.92f, 0.55f, 1.0f), "LEARNED");
        } else {
            const bool gradeTwoReached =
                static_cast<int>(playerGrade()) >=
                static_cast<int>(PlayerGrade::Grade2);
            ImGui::BeginDisabled(points_ < 15 || !gradeTwoReached);
            if (ImGui::Button("Learn Hollow Wicker Basket (15 points)",
                              ImVec2(320, 40))) {
                spendPoints(15);
                hollowWickerBasketLearned_ = true;
            }
            ImGui::EndDisabled();
            if (!gradeTwoReached)
                ImGui::TextDisabled("Requires player rank Grade 2.");
        }

        if (selectedClan_ == 0) {
            ImGui::Separator();
            ImGui::TextColored(clanColor(selectedClan_),
                               "Cursed Technique Neutral: Barrier Technique");
            ImGui::TextWrapped(
                "Toggle with [2]. Neutral Limitless prevents attacks from "
                "reaching you and costs 30 base Cursed Energy each turn.");
            if (neutralBarrierLearned_) {
                ImGui::TextColored(ImVec4(0.45f, 0.92f, 0.55f, 1.0f),
                                   "LEARNED");
            } else {
                ImGui::BeginDisabled(points_ < 10 ||
                                     noInnateTechniqueSelected_);
                if (ImGui::Button("Learn Neutral Barrier (10 points)",
                                  ImVec2(320, 40))) {
                    spendPoints(10);
                    neutralBarrierLearned_ = true;
                }
                ImGui::EndDisabled();
                if (noInnateTechniqueSelected_)
                    ImGui::TextDisabled(
                        "No Innate Technique prevents learning Limitless usages.");
            }

            ImGui::Separator();
            ImGui::TextColored(clanColor(selectedClan_),
                               "Cursed Technique Reversal: Red");
            ImGui::TextWrapped(
                "Fire a repulsive orb in a line with [3]. The first enemy hit "
                "takes 1.5x Cursed Technique damage and is repelled up to 3 spaces.");
            if (reversalRedLearned_) {
                ImGui::TextColored(ImVec4(0.45f, 0.92f, 0.55f, 1.0f),
                                   "LEARNED");
            } else {
                const bool gradeTwoReached =
                    static_cast<int>(playerGrade()) >=
                    static_cast<int>(PlayerGrade::Grade2);
                ImGui::BeginDisabled(points_ < 25 || !gradeTwoReached ||
                                     noInnateTechniqueSelected_);
                if (ImGui::Button("Learn Reversal: Red (25 points)",
                                  ImVec2(320, 40))) {
                    spendPoints(25);
                    reversalRedLearned_ = true;
                }
                ImGui::EndDisabled();
                if (!gradeTwoReached)
                    ImGui::TextDisabled("Requires player rank Grade 2.");
            }
        }

        if (selectedClan_ == 1) {
            ImGui::Separator();
            ImGui::TextColored(clanColor(selectedClan_),
                               "Cursed Technique Extension: Summon Nue");
            ImGui::TextWrapped(
                "Summon with [3]. Nue seeks the nearest enemy and self-destructs "
                "in a 3-tile purple-lightning explosion. It deals 0.5x Cursed "
                "Technique damage and stuns surviving enemies for 3 turns. "
                "Costs 30 base Cursed Energy, has no upkeep, and is limited "
                "to 1 active Nue.");
            if (nueLearned_) {
                ImGui::TextColored(ImVec4(0.45f, 0.92f, 0.55f, 1.0f),
                                   "LEARNED");
            } else {
                const bool gradeThreeReached =
                    static_cast<int>(playerGrade()) >=
                    static_cast<int>(PlayerGrade::Grade3);
                ImGui::BeginDisabled(points_ < 10 || !gradeThreeReached ||
                                     noInnateTechniqueSelected_);
                if (ImGui::Button("Learn Summon Nue (10 points)",
                                  ImVec2(360, 40))) {
                    spendPoints(10);
                    nueLearned_ = true;
                }
                ImGui::EndDisabled();
                if (!gradeThreeReached)
                    ImGui::TextDisabled("Requires player rank Grade 3.");
            }
        }

        if (selectedClan_ == 3) {
            ImGui::Separator();
            ImGui::TextColored(clanColor(selectedClan_),
                               "Cursed Technique Extension: Collapse");
            ImGui::TextWrapped(
                "Aim with [2]. Covers a three-wide, four-deep area, gives each "
                "enemy one Ratio stack, then makes one player melee attack against it.");
            if (collapseLearned_) {
                ImGui::TextColored(ImVec4(0.45f, 0.92f, 0.55f, 1.0f),
                                   "LEARNED");
            } else {
                const bool gradeThreeReached =
                    static_cast<int>(playerGrade()) >=
                    static_cast<int>(PlayerGrade::Grade3);
                ImGui::BeginDisabled(points_ < 10 || !gradeThreeReached ||
                                     noInnateTechniqueSelected_);
                if (ImGui::Button("Learn Extension: Collapse (10 points)",
                                  ImVec2(320, 40))) {
                    spendPoints(10);
                    collapseLearned_ = true;
                }
                ImGui::EndDisabled();
                if (!gradeThreeReached)
                    ImGui::TextDisabled("Requires player rank Grade 3.");
            }
        }

        if (selectedClan_ == 2) {
            ImGui::Separator();
            ImGui::TextColored(
                clanColor(selectedClan_),
                "Cursed Technique Extension: Summon Ember Insect");
            ImGui::TextWrapped(
                "Summon with [2]. The Ember Insect seeks the nearest enemy "
                "and detonates in a 3-tile radius for Cursed Technique damage. "
                "Costs 30 base Cursed Energy and has no upkeep.");
            if (emberInsectLearned_) {
                ImGui::TextColored(ImVec4(0.45f, 0.92f, 0.55f, 1.0f),
                                   "LEARNED");
            } else {
                const bool gradeThreeReached =
                    static_cast<int>(playerGrade()) >=
                    static_cast<int>(PlayerGrade::Grade3);
                ImGui::BeginDisabled(points_ < 10 || !gradeThreeReached ||
                                     noInnateTechniqueSelected_);
                if (ImGui::Button("Learn Summon Ember Insect (10 points)",
                                  ImVec2(360, 40))) {
                    spendPoints(10);
                    emberInsectLearned_ = true;
                }
                ImGui::EndDisabled();
                if (!gradeThreeReached)
                    ImGui::TextDisabled("Requires player rank Grade 3.");
                if (noInnateTechniqueSelected_)
                    ImGui::TextDisabled(
                        "No Innate Technique prevents learning this extension.");
            }
        }

        ImGui::Separator();
        ImGui::TextColored(clanColor(selectedClan_), "Domain Expansion: %s",
                           domainExpansionName());
        ImGui::TextWrapped(
            "Toggle with [D]. Creates a five-tile domain that freezes enemies "
            "and deals Cursed Technique damage every active turn. Costs 40 "
            "base CE to activate and 20 base CE per later turn.");
        if (domainExpansionLearned_) {
            ImGui::TextColored(ImVec4(0.45f, 0.92f, 0.55f, 1.0f), "LEARNED");
        } else {
            const bool gradeOneReached =
                static_cast<int>(playerGrade()) >=
                static_cast<int>(PlayerGrade::Grade1);
            ImGui::BeginDisabled(points_ < 50 || !gradeOneReached ||
                                 noInnateTechniqueSelected_);
            if (ImGui::Button("Learn Domain Expansion (50 points)",
                              ImVec2(320, 40))) {
                spendPoints(50);
                domainExpansionLearned_ = true;
            }
            ImGui::EndDisabled();
            if (!gradeOneReached)
                ImGui::TextDisabled("Requires player rank Grade 1.");
        }

        ImGui::EndDisabled();
        ImGui::Dummy(ImVec2(0, 16));
        if (ImGui::Button("Continue", ImVec2(-1, 42))) {
            if (canAdvance && points_ > 0)
                advancementExitConfirm_ = true;
            else {
                if (canAdvance) completeGradeAdvancement();
                advancementMenu_ = false;
            }
        }
        ImGui::TextDisabled(
            canAdvance && points_ > 0
                ? "Leaving with unspent points requires confirmation."
                : canAdvance
                    ? "All points spent. Continue, C, or Escape."
                    : "Close with Continue, C, or Escape.");

        if (advancementExitConfirm_)
            ImGui::OpenPopup("Leave Grade Advancement?");
        ImGui::SetNextWindowSize(ImVec2(500, 240), ImGuiCond_Appearing);
        if (ImGui::BeginPopupModal("Leave Grade Advancement?", nullptr,
                                   ImGuiWindowFlags_NoResize |
                                       ImGuiWindowFlags_NoSavedSettings)) {
            ImGui::TextColored(ImVec4(1.0f, 0.72f, 0.22f, 1.0f),
                               "You still have %d unspent point%s.", points_,
                               points_ == 1 ? "" : "s");
            ImGui::TextWrapped(
                "If you leave now, these points remain available, but you "
                "cannot spend them on Grade Advancement upgrades until you "
                "reach the next grade level.");
            ImGui::Spacing();
            const float buttonWidth =
                (ImGui::GetContentRegionAvail().x -
                 ImGui::GetStyle().ItemSpacing.x) * 0.5f;
            if (ImGui::Button("Keep Spending", ImVec2(buttonWidth, 42))) {
                advancementExitConfirm_ = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Leave Anyway", ImVec2(buttonWidth, 42))) {
                advancementExitConfirm_ = false;
                completeGradeAdvancement();
                advancementMenu_ = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        ImGui::End();
    }

    void drawCursedToolSprite(int index, ImVec2 size) const {
        if (cursedToolSpritesTexture == 0) return;
        const float u0 = static_cast<float>(index) / 3.0f;
        ImGui::Image(static_cast<ImTextureID>(cursedToolSpritesTexture), size,
                     ImVec2(u0, 0.0f), ImVec2(u0 + 1.0f / 3.0f, 1.0f));
    }

    void drawCursedToolMenu() {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always,
                                ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(720, 520), ImGuiCond_Always);
        constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings;
        if (!beginObsidianWindow("Equipped Cursed Tool [Q]", &cursedToolMenu_, flags)) {
            ImGui::End();
            return;
        }
        ImGui::TextWrapped("Cursed tools are usable items rather than equipment. Using one consumes a turn.");
        ImGui::Separator();

        drawCursedToolSprite(0, ImVec2(128, 128));
        ImGui::SameLine();
        ImGui::BeginGroup();
        ImGui::TextUnformatted("Cursed Bandage");
        ImGui::TextColored(ImVec4(0.80f, 0.58f, 1.0f, 1.0f),
                           "Stack: %d", cursedBandages_);
        ImGui::TextWrapped("Restores 10 Health and is consumed on use.");
        ImGui::BeginDisabled(cursedBandages_ <= 0 || player_.hp >= player_.maxHp);
        if (ImGui::Button("Use Bandage", ImVec2(180, 34))) {
            const int healed =
                std::min(CursedBandageHealing, player_.maxHp - player_.hp);
            player_.hp += healed;
            --cursedBandages_;
            cursedToolMenu_ = false;
            addMessage("The Cursed Bandage restores " + std::to_string(healed) + " Health.");
            finishPlayerTurn();
        }
        ImGui::EndDisabled();
        if (player_.hp >= player_.maxHp)
            ImGui::TextDisabled("Health is already full.");
        ImGui::EndGroup();

        ImGui::Separator();
        drawCursedToolSprite(1, ImVec2(128, 128));
        ImGui::SameLine();
        ImGui::BeginGroup();
        ImGui::TextUnformatted("Imperfect Kamutoke");
        int totalCharges = 0;
        for (int charges : kamutokeCharges_) totalCharges += charges;
        ImGui::TextColored(ImVec4(0.35f, 0.82f, 1.0f, 1.0f),
                           "Owned: %d | Remaining uses: %d",
                           static_cast<int>(kamutokeCharges_.size()), totalCharges);
        ImGui::TextWrapped("Target a visible enemy and deal 20 cursed lightning damage. Costs 10 Cursed Energy. Each tool breaks after 3 uses.");
        ImGui::BeginDisabled(kamutokeCharges_.empty() || cursedEnergy_ < 10);
        if (ImGui::Button("Target Enemy", ImVec2(180, 34)))
            beginKamutokeTargeting();
        ImGui::EndDisabled();
        ImGui::EndGroup();

        ImGui::Separator();
        if (ImGui::Button("Close", ImVec2(-1, 38))) cursedToolMenu_ = false;
        ImGui::TextDisabled("Press Q or Escape to close.");
        ImGui::End();
    }

    void drawCursedToolShopMenu() {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always,
                                ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(760, 540), ImGuiCond_Always);
        constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings;
        if (!beginObsidianWindow("Cursed Tool Shop", &cursedToolShopMenu_, flags)) {
            ImGui::End();
            return;
        }
        ImGui::Text("Available points: %d", points_);
        ImGui::TextWrapped("Purchase cursed tools and accessories. Equip them "
                           "from Inventory [I]; press Q to activate the equipped tool.");
        if (isJogoClan())
            ImGui::TextDisabled("Player Cursed Spirits cannot carry inventory items.");
        ImGui::Separator();

        drawCursedToolSprite(0, ImVec2(150, 150));
        ImGui::SameLine();
        ImGui::BeginGroup();
        ImGui::TextUnformatted("Cursed Bandage");
        ImGui::TextColored(ImVec4(0.85f, 0.68f, 1.0f, 1.0f), "1 point");
        ImGui::TextWrapped("A stackable consumable that restores 10 Health.");
        ImGui::TextDisabled("Owned: %d", cursedBandages_);
        ImGui::BeginDisabled(points_ < 1 || isJogoClan());
        if (ImGui::Button("Buy Bandage", ImVec2(180, 34))) {
            spendPoints(1);
            if (!hasInventoryItem("Cursed Bandage"))
                inventory_.push_back(makeCursedToolItem(false));
            ++cursedBandages_;
            addMessage("Purchased a Cursed Bandage.");
        }
        ImGui::EndDisabled();
        ImGui::EndGroup();

        ImGui::Separator();
        drawCursedToolSprite(1, ImVec2(150, 150));
        ImGui::SameLine();
        ImGui::BeginGroup();
        ImGui::TextUnformatted("Imperfect Kamutoke");
        ImGui::TextColored(ImVec4(0.35f, 0.82f, 1.0f, 1.0f), "5 points");
        ImGui::TextWrapped("Three uses. Each use costs 10 Cursed Energy and deals 20 cursed lightning damage to a visible enemy.");
        ImGui::TextDisabled("Owned: %d", static_cast<int>(kamutokeCharges_.size()));
        ImGui::BeginDisabled(points_ < 5 || isJogoClan());
        if (ImGui::Button("Buy Kamutoke", ImVec2(180, 34))) {
            spendPoints(5);
            if (!hasInventoryItem("Imperfect Kamutoke"))
                inventory_.push_back(makeCursedToolItem(true));
            kamutokeCharges_.push_back(3);
            addMessage("Purchased an Imperfect Kamutoke with 3 uses.");
        }
        ImGui::EndDisabled();
        ImGui::EndGroup();

        ImGui::Separator();
        ImGui::TextUnformatted("Cursed Blindfold");
        ImGui::TextColored(efficiencyColor(), "5 points");
        ImGui::TextWrapped(
            "Accessory. Increases Cursed Energy Efficiency by 10%% while equipped.");
        ImGui::BeginDisabled(points_ < 5 || isJogoClan() ||
                             hasInventoryItem("Cursed Blindfold"));
        if (ImGui::Button("Buy Blindfold", ImVec2(180, 34))) {
            spendPoints(5);
            inventory_.push_back(makeAccessoryItem(true));
            addMessage("Purchased a Cursed Blindfold.");
        }
        ImGui::EndDisabled();
        if (hasInventoryItem("Cursed Blindfold")) ImGui::TextDisabled("Owned");

        ImGui::Separator();
        ImGui::TextUnformatted("Cursed Pin");
        ImGui::TextColored(energyColor(), "5 points");
        ImGui::TextWrapped(
            "Accessory. Increases maximum Cursed Energy by 20 while equipped.");
        ImGui::BeginDisabled(points_ < 5 || isJogoClan() ||
                             hasInventoryItem("Cursed Pin"));
        if (ImGui::Button("Buy Pin", ImVec2(180, 34))) {
            spendPoints(5);
            inventory_.push_back(makeAccessoryItem(false));
            addMessage("Purchased a Cursed Pin.");
        }
        ImGui::EndDisabled();
        if (hasInventoryItem("Cursed Pin")) ImGui::TextDisabled("Owned");

        ImGui::Separator();
        if (ImGui::Button("Leave Shop", ImVec2(-1, 38)))
            cursedToolShopMenu_ = false;
        ImGui::TextDisabled("Purchases do not consume a turn. Escape closes the shop.");
        ImGui::End();
    }

    void drawInventoryMenu() {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always,
                                ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(772, 632), ImGuiCond_Always);
        constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings;
        if (!beginObsidianWindow("Inventory", &inventoryMenu_, flags)) {
            ImGui::End();
            return;
        }

        auto drawEquipmentIcon = [this](const Item& item, ImVec2 size) {
            if (item.slot == EquipmentSlot::CursedTool) {
                drawCursedToolSprite(item.name == "Imperfect Kamutoke" ? 1 : 0,
                                     size);
                return;
            }
            const int iconIndex = equipmentIconIndex(item);
            if (equipmentIconsTexture == 0 || iconIndex < 0) {
                ImGui::Dummy(size);
                return;
            }
            const float u0 = static_cast<float>(iconIndex) / 3.0f;
            ImGui::Image(static_cast<ImTextureID>(equipmentIconsTexture), size,
                         ImVec2(u0, 0.0f), ImVec2(u0 + 1.0f / 3.0f, 1.0f));
        };

        ImGui::SeparatorText("Equipment");
        constexpr std::array<EquipmentSlot, 4> displayedEquipmentSlots{{
            EquipmentSlot::Clothing,
            EquipmentSlot::Weapon,
            EquipmentSlot::Accessory,
            EquipmentSlot::CursedTool}};
        for (EquipmentSlot slot : displayedEquipmentSlots) {
            const std::size_t slotIndex = static_cast<std::size_t>(slot);
            const Item* item = equippedItem(slot);
            ImGui::PushID(static_cast<int>(slotIndex));
            const char* displayLabel =
                slot == EquipmentSlot::CursedTool
                    ? "Equipped Cursed Tool"
                    : equipmentSlotName(slot);
            ImGui::TextUnformatted(displayLabel);
            ImGui::SameLine(205.0f);
            if (item) {
                drawEquipmentIcon(*item, ImVec2(30.0f, 42.0f));
                ImGui::SameLine();
                ImGui::TextUnformatted(item->name.c_str());
                ImGui::SameLine(500.0f);
                if (ImGui::SmallButton("Unequip")) {
                    equippedItems_[slotIndex] = -1;
                    clampEnergyToEquipmentMaximum();
                }
            } else {
                ImGui::TextDisabled("Empty");
            }
            ImGui::PopID();
        }

        ImGui::Spacing();
        if (hasBaseAttackMultiplyingWeapon())
            ImGui::Text("Current weapon: Base Attack x1.5 (+%d damage)",
                        katanaBaseAttackBonus());
        else
            ImGui::Text("Current weapon bonus: +%d melee damage",
                        equipmentAttackBonus());
        if (hasQuarterDamageUniform())
            ImGui::Text("Equipment defense: 25%% direct-hit reduction");
        else
            ImGui::Text("Equipment defense: %d damage blocked",
                        equipmentDefenseBonus());
        ImGui::Text("Cursed melee: %s",
                    hasCursedWeaponEquipped() ? "Yes" : "No");

        ImGui::SeparatorText("Inventory");
        for (int index = 0; index < static_cast<int>(inventory_.size()); ++index) {
            Item& item = inventory_[index];
            const std::size_t slotIndex = static_cast<std::size_t>(item.slot);
            const bool equipped = equippedItems_[slotIndex] == index;
            ImGui::PushID(index + 100);
            drawEquipmentIcon(item, ImVec2(82.0f, 112.0f));
            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Text("%s%s", item.name.c_str(), equipped ? "  [EQUIPPED]" : "");
            ImGui::SameLine(500.0f);
            if (equipped) {
                if (ImGui::SmallButton("Unequip")) {
                    equippedItems_[slotIndex] = -1;
                    clampEnergyToEquipmentMaximum();
                }
            } else {
                const bool weaponBlocked =
                    handSignsVowActive_ &&
                    item.slot == EquipmentSlot::Weapon;
                ImGui::BeginDisabled(weaponBlocked);
                if (ImGui::SmallButton("Equip")) {
                    equippedItems_[slotIndex] = index;
                    clampEnergyToEquipmentMaximum();
                }
                ImGui::EndDisabled();
                if (weaponBlocked)
                    ImGui::TextDisabled(
                        "Hand Signs must be turned off to equip this weapon.");
            }
            ImGui::TextDisabled("%s", equipmentSlotName(item.slot));
            if (item.baseAttackMultiplierPercent > 100)
                ImGui::TextColored(ImVec4(0.45f, 0.92f, 0.55f, 1.0f),
                                   "Stats: Base Attack x%.2g",
                                   item.baseAttackMultiplierPercent / 100.0f);
            else if (item.attackBonus > 0)
                ImGui::TextColored(ImVec4(0.45f, 0.92f, 0.55f, 1.0f),
                                   "Stats: +%d melee damage", item.attackBonus);
            else if (item.reducesDamageByQuarter)
                ImGui::TextColored(ImVec4(0.45f, 0.72f, 1.0f, 1.0f),
                                   "Stats: Reduces direct-hit damage by 25%%");
            else if (item.defenseBonus > 0)
                ImGui::TextColored(ImVec4(0.45f, 0.72f, 1.0f, 1.0f),
                                   "Stats: Blocks %d direct damage per hit",
                                   item.defenseBonus);
            else if (item.cursedDamage)
                ImGui::TextColored(ImVec4(0.85f, 0.45f, 1.0f, 1.0f),
                                   "Stats: Melee attacks deal cursed damage");
            else if (item.name == "Cursed Blindfold")
                ImGui::TextColored(efficiencyColor(),
                                   "Stats: +10%% Cursed Energy Efficiency");
            else if (item.name == "Cursed Pin")
                ImGui::TextColored(energyColor(),
                                   "Stats: +20 maximum Cursed Energy");
            else if (item.name == "Cursed Bandage")
                ImGui::TextColored(healthColor(), "Stack: %d | Heals 5 HP",
                                   cursedBandages_);
            else if (item.name == "Imperfect Kamutoke") {
                int uses = 0;
                for (int charges : kamutokeCharges_) uses += charges;
                ImGui::TextColored(energyColor(),
                                   "Tools: %d | Remaining uses: %d",
                                   static_cast<int>(kamutokeCharges_.size()), uses);
            }
            else
                ImGui::TextDisabled("Stats: None");
            ImGui::TextDisabled("Description");
            ImGui::TextWrapped("%s", item.description.c_str());
            ImGui::EndGroup();
            ImGui::Separator();
            ImGui::PopID();
        }

        if (ImGui::Button("Close", ImVec2(-1, 40))) inventoryMenu_ = false;
        ImGui::TextDisabled("Press I or Escape to close. Equipment changes do not use a turn.");
        ImGui::End();
    }

    void drawDebugMenu() {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always,
                                ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(512, 422), ImGuiCond_Always);
        constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings;
        if (!beginObsidianWindow("Debug Menu", &debugMenu_, flags)) {
            ImGui::End();
            return;
        }

        ImGui::Text("Current points total: %d", points_);
        ImGui::Text("Earned Points: %d", earnedPoints_);
        ImGui::Text("Total Earned Points: %d", totalPointsEarned_);
        ImGui::Text("Spent Points: %d", spentPoints_);
        ImGui::TextColored(playerGradeColor(playerGrade()), "Rank: %s",
                           playerGradeName(playerGrade()));
        ImGui::SeparatorText("Add Points");
        if (ImGui::Button("+10", ImVec2(120, 44)))
            grantPlayerPoints(10, true);
        ImGui::SameLine();
        if (ImGui::Button("+100", ImVec2(120, 44)))
            grantPlayerPoints(100, true);
        ImGui::SameLine();
        if (ImGui::Button("+1000", ImVec2(120, 44)))
            grantPlayerPoints(1000, true);

        ImGui::SeparatorText("Map");
        if (ImGui::Button("Reveal Current Map", ImVec2(-1, 42))) {
            for (Cell& cell : cells_) cell.explored = true;
            levels_[currentLevel_].cells = cells_;
            addMessage("Debug: revealed dungeon level " +
                       std::to_string(currentLevel_ + 1) + ".");
        }

        ImGui::Spacing();
        if (ImGui::Button("Close", ImVec2(-1, 38))) debugMenu_ = false;
        ImGui::TextDisabled(
            "Press ` or Escape to close. Debug actions do not use a turn.");
        ImGui::End();
    }

    void drawPachinkoMenu() {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always,
                                ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(960, 700), ImGuiCond_Always);
        constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings;
        if (!beginObsidianWindow("Cursed Pachinko Machine", &pachinkoMenu_,
                                 flags)) {
            ImGui::End();
            return;
        }

        const std::array<int, 13>& payouts =
            levels_[currentLevel_].pachinkoPayouts;
        constexpr std::array<const char*, 3> objectNames{
            {"Cursed Card", "Cursed Doll", "Cursed Finger"}};
        constexpr std::array<int, 3> objectCosts{{20, 50, 100}};
        ImGui::Text("Available points: %d", points_);
        ImGui::SameLine(220.0f);
        ImGui::TextColored(ImVec4(0.85f, 0.52f, 1.0f, 1.0f),
                           "Pachinko balls: %d", pachinkoBalls_);
        ImGui::SameLine(445.0f);
        ImGui::Text("Balls in play: %d",
                    static_cast<int>(pachinkoActiveBalls_.size()));

        ImGui::BeginDisabled(points_ < 1);
        if (ImGui::Button("Buy 1 ball", ImVec2(130, 36))) {
            spendPoints(1);
            ++pachinkoBalls_;
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        const int fiveBallPurchase = std::min(5, points_);
        ImGui::BeginDisabled(fiveBallPurchase <= 0);
        if (ImGui::Button("Buy up to 5", ImVec2(130, 36))) {
            spendPoints(fiveBallPurchase);
            pachinkoBalls_ += fiveBallPurchase;
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::BeginDisabled(points_ <= 0);
        if (ImGui::Button("Spend all points", ImVec2(150, 36))) {
            const int allPoints = points_;
            spendPoints(allPoints);
            pachinkoBalls_ += allPoints;
        }
        ImGui::EndDisabled();
        auto launchBalls = [&](int requested) {
            const int count = std::min(requested, pachinkoBalls_);
            pachinkoBalls_ -= count;
            for (int i = 0; i < count; ++i) {
                PachinkoBallState ball;
                ball.x = 0.5f + std::uniform_real_distribution<float>(-0.025f, 0.025f)(rng_);
                ball.y = 0.075f - static_cast<float>(i % 5) * 0.004f;
                ball.velocityX = std::uniform_real_distribution<float>(-0.11f, 0.11f)(rng_);
                ball.velocityY = 0.08f + static_cast<float>(i % 3) * 0.01f;
                pachinkoActiveBalls_.push_back(ball);
            }
        };
        ImGui::SameLine();
        ImGui::BeginDisabled(pachinkoBalls_ <= 0);
        if (ImGui::Button("Launch 1", ImVec2(100, 36))) launchBalls(1);
        ImGui::SameLine();
        if (ImGui::Button("Launch 5", ImVec2(100, 36))) launchBalls(5);
        ImGui::SameLine();
        if (ImGui::Button("Launch all", ImVec2(110, 36)))
            launchBalls(pachinkoBalls_);
        ImGui::EndDisabled();

        ImGui::Separator();
        ImGui::BeginTable("PachinkoBoardAndShop", 2,
                          ImGuiTableFlags_BordersInnerV |
                          ImGuiTableFlags_SizingFixedFit);
        ImGui::TableSetupColumn("Board", ImGuiTableColumnFlags_WidthFixed, 585.0f);
        ImGui::TableSetupColumn("Cash out", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableNextColumn();
        const float boardSize = 550.0f;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8.0f);
        const ImVec2 boardStart = ImGui::GetCursorScreenPos();
        if (cursedPachinkoTexture != 0) {
            ImGui::Image(static_cast<ImTextureID>(cursedPachinkoTexture),
                         ImVec2(boardSize, boardSize));
        } else {
            ImGui::Dummy(ImVec2(boardSize, boardSize));
            ImGui::GetWindowDrawList()->AddRectFilled(
                boardStart, ImVec2(boardStart.x + boardSize,
                                   boardStart.y + boardSize),
                IM_COL32(10, 6, 16, 255));
        }

        int framePayout = 0;
        int settledBalls = 0;
        const float deltaTime = std::min(ImGui::GetIO().DeltaTime, 0.025f);
        for (PachinkoBallState& ball : pachinkoActiveBalls_) {
            ball.velocityY += 0.62f * deltaTime;
            ball.x += ball.velocityX * deltaTime;
            ball.y += ball.velocityY * deltaTime;
            if (ball.x < 0.075f || ball.x > 0.925f) {
                ball.x = std::clamp(ball.x, 0.075f, 0.925f);
                ball.velocityX *= -0.78f;
            }

            constexpr float pegRadius = 0.018f;
            constexpr float ballRadius = 0.013f;
            for (int row = 0; row < 8; ++row) {
                const int columns = row % 2 == 0 ? 7 : 8;
                for (int column = 0; column < columns; ++column) {
                    const float pegX = row % 2 == 0
                        ? 0.19f + static_cast<float>(column) * 0.103f
                        : 0.14f + static_cast<float>(column) * 0.103f;
                    const float pegY = 0.235f + static_cast<float>(row) * 0.068f;
                    const float dx = ball.x - pegX;
                    const float dy = ball.y - pegY;
                    const float distanceSquared = dx * dx + dy * dy;
                    const float collisionRadius = pegRadius + ballRadius;
                    if (distanceSquared <= 0.000001f ||
                        distanceSquared >= collisionRadius * collisionRadius)
                        continue;
                    const float distance = std::sqrt(distanceSquared);
                    const float normalX = dx / distance;
                    const float normalY = dy / distance;
                    const float velocityAlongNormal =
                        ball.velocityX * normalX + ball.velocityY * normalY;
                    if (velocityAlongNormal < 0.0f) {
                        ball.velocityX -= 1.72f * velocityAlongNormal * normalX;
                        ball.velocityY -= 1.72f * velocityAlongNormal * normalY;
                    }
                    ball.x = pegX + normalX * collisionRadius;
                    ball.y = pegY + normalY * collisionRadius;
                    ball.velocityX +=
                        std::uniform_real_distribution<float>(-0.035f, 0.035f)(rng_);
                }
            }

            if (ball.y >= 0.82f) {
                const float slotPosition = (ball.x - 0.075f) / 0.85f;
                const int slot = std::clamp(
                    static_cast<int>(slotPosition * 13.0f), 0, 12);
                framePayout += payouts[slot];
                ball.y = 2.0f;
                ++settledBalls;
            }
        }
        if (settledBalls > 0) {
            pachinkoActiveBalls_.erase(
                std::remove_if(pachinkoActiveBalls_.begin(),
                               pachinkoActiveBalls_.end(),
                               [](const PachinkoBallState& ball) {
                                   return ball.y > 1.0f;
                               }),
                pachinkoActiveBalls_.end());
            pachinkoBalls_ += framePayout;
            addMessage("Cursed Pachinko returned " +
                       std::to_string(framePayout) +
                       (framePayout == 1 ? " ball." : " balls."));
        }

        ImDrawList* draw = ImGui::GetWindowDrawList();
        for (int slot = 0; slot < static_cast<int>(payouts.size()); ++slot) {
            const float centerX = boardStart.x +
                (0.075f + (static_cast<float>(slot) + 0.5f) / 13.0f * 0.85f) *
                boardSize;
            const float labelY = boardStart.y + boardSize * 0.91f;
            const std::string label = std::to_string(payouts[slot]);
            const ImVec2 textSize = ImGui::CalcTextSize(label.c_str());
            draw->AddRectFilled(
                ImVec2(centerX - textSize.x * 0.75f, labelY - 2.0f),
                ImVec2(centerX + textSize.x * 0.75f,
                       labelY + textSize.y + 2.0f),
                IM_COL32(8, 4, 14, 220), 4.0f);
            draw->AddText(ImVec2(centerX - textSize.x * 0.5f, labelY),
                          IM_COL32(255, 205, 70, 255), label.c_str());
        }
        for (const PachinkoBallState& ball : pachinkoActiveBalls_) {
            const ImVec2 ballCenter(
                boardStart.x + ball.x * boardSize,
                boardStart.y + ball.y * boardSize);
            draw->AddCircleFilled(ballCenter, boardSize * 0.013f,
                                  IM_COL32(245, 235, 205, 255));
            draw->AddCircle(ballCenter, boardSize * 0.013f,
                            IM_COL32(115, 65, 160, 255), 16, 2.0f);
        }

        ImGui::TableNextColumn();
        ImGui::TextColored(ImVec4(0.96f, 0.78f, 0.28f, 1.0f),
                           "CURSED OBJECT EXCHANGE");
        ImGui::TextWrapped(
            "Winnings are balls. Exchange them for objects to cash out an equal number of spendable points.");
        ImGui::Separator();
        for (int object = 0; object < 3; ++object) {
            ImGui::PushID(object);
            if (cursedObjectsTexture != 0) {
                const float u0 = static_cast<float>(object) / 3.0f;
                const float u1 = static_cast<float>(object + 1) / 3.0f;
                ImGui::Image(static_cast<ImTextureID>(cursedObjectsTexture),
                             ImVec2(92, 142), ImVec2(u0, 0.12f),
                             ImVec2(u1, 0.88f));
                ImGui::SameLine();
            }
            ImGui::BeginGroup();
            ImGui::Text("%s", objectNames[object]);
            ImGui::TextColored(ImVec4(0.85f, 0.52f, 1.0f, 1.0f),
                               "%d balls -> %d points", objectCosts[object],
                               objectCosts[object]);
            ImGui::TextDisabled("Owned: %d", cursedObjectCounts_[object]);
            ImGui::BeginDisabled(pachinkoBalls_ < objectCosts[object]);
            if (ImGui::Button("Exchange", ImVec2(130, 30))) {
                pachinkoBalls_ -= objectCosts[object];
                points_ += objectCosts[object];
                ++cursedObjectCounts_[object];
                addMessage(std::string("Exchanged balls for a ") +
                           objectNames[object] + ".");
            }
            ImGui::EndDisabled();
            ImGui::EndGroup();
            ImGui::Separator();
            ImGui::PopID();
        }
        ImGui::EndTable();

        if (ImGui::Button("Close", ImVec2(-1, 36))) pachinkoMenu_ = false;
        ImGui::TextDisabled(
            "Each purchased ball costs 1 point. Pocket payouts award balls, never points or rank progress.");
        ImGui::End();
    }

    void drawHelpMenu() {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always,
                                ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(780, 660), ImGuiCond_Always);
        constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings;
        if (!beginObsidianWindow("Game Controls", &helpMenu_, flags)) {
            if (helpReturnToPause_) pauseMenu_ = true;
            helpReturnToPause_ = false;
            ImGui::End();
            return;
        }

        ImGui::BeginChild("ControlList", ImVec2(0, -52), false);
        ImGui::SeparatorText("Movement & Turns");
        ImGui::BulletText("Arrow keys: cardinal movement");
        ImGui::BulletText("Q, E, Z, C or numpad 1-9: diagonal/eight-way movement");
        ImGui::BulletText("Space or numpad 5: wait one turn");
        ImGui::BulletText("Hold movement or wait: repeat the input");
        ImGui::BulletText("Move into an enemy: melee action");
        ImGui::BulletText("Move into an allied Shikigami: swap places");
        ImGui::BulletText("Walk onto a Pachinko Machine: open the minigame");
        ImGui::BulletText("Walk onto a Cursed Tool Shop: browse its stock");
        ImGui::BulletText("Boss floors: recover the boss's Cursed Key to break the stair seals");
        ImGui::BulletText("Level 15: collect four keys and enter the Gate of Heaven");

        ImGui::SeparatorText("Cursed Techniques & Abilities");
        ImGui::BulletText("1: use the clan's Lapse ability");
        ImGui::BulletText("2 (Zenin): unsummon all active Shikigami");
        ImGui::BulletText("H: Healing / Cursed Healing");
        ImGui::BulletText("X: arm Simple Domain, if learned");
        ImGui::BulletText("A: toggle Domain Amplification, if learned");
        ImGui::BulletText("V: toggle Hollow Wicker Basket, if learned");
        ImGui::BulletText("T: Cursed Technique Customization");

        ImGui::SeparatorText("Ability Targeting");
        ImGui::BulletText("Arrow keys or numpad directions: aim or select");
        ImGui::BulletText("Tab: cycle visible Limitless targets");
        ImGui::BulletText("Left-click: select a Limitless enemy or aim Volcanic Blast");
        ImGui::BulletText("1, Space, or numpad 5: confirm");
        ImGui::BulletText("Escape: cancel targeting");

        ImGui::SeparatorText("Map & Mouse");
        ImGui::BulletText("Mouse wheel over map: zoom");
        ImGui::BulletText("Middle mouse drag: pan the map");
        ImGui::BulletText("Left-click a tile: open Inspection");
        ImGui::BulletText("Right-click map: clear Inspection");
        ImGui::BulletText("Drag the message divider: resize the log vertically");

        ImGui::SeparatorText("Menus");
        ImGui::BulletText("B: Binding Vows");
        ImGui::BulletText("C: view Grade Advancement");
        if (!isJogoClan())
            ImGui::BulletText("I: Inventory & Equipment");
        else
            ImGui::BulletText("Jogo is a Cursed Spirit and has no inventory");
        ImGui::BulletText("Q: activate the equipped Cursed Tool");
        ImGui::BulletText("Q or Space: confirm an Imperfect Kamutoke target");
        ImGui::BulletText("Backtick (`): Debug Menu");
        ImGui::BulletText("Question mark (?): this controls screen");
        ImGui::BulletText("Escape: close the current interface or open Pause");
        ImGui::BulletText("R on Game Over: restart");

        ImGui::SeparatorText("Turn Cost Notes");
        ImGui::TextWrapped(
            "Movement, waiting, melee, confirmed techniques, healing, Simple "
            "Domain, and successful unsummoning consume a turn. Opening menus, "
            "inspection, equipment changes, zooming, panning, and Technique "
            "Output changes do not consume a turn.");
        ImGui::EndChild();

        if (ImGui::Button("Close", ImVec2(-1, 40))) {
            helpMenu_ = false;
            if (helpReturnToPause_) pauseMenu_ = true;
            helpReturnToPause_ = false;
        }
        ImGui::TextDisabled("Press ? or Escape to close.");
        ImGui::End();
    }

    void drawRulesMenu() {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always,
                                ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(820, 680), ImGuiCond_Always);
        constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings;
        if (!beginObsidianWindow("Tower of Malevolent Divinity Rules",
                                 &rulesMenu_, flags)) {
            if (rulesReturnToPause_) pauseMenu_ = true;
            rulesReturnToPause_ = false;
            ImGui::End();
            return;
        }

        ImGui::BeginChild("TowerRuleList", ImVec2(0, -52), false);
        const ImVec4 ruleHeader(0.94f, 0.70f, 0.25f, 1.0f);

        ImGui::TextColored(ruleHeader, "I. ENTRY");
        ImGui::BulletText(
            "Anyone may enter the Tower of Malevolent Divinity and become a challenger.");
        ImGui::BulletText(
            "Challengers gain the opportunity to learn sorcery or gain greater power in sorcery.");

        ImGui::Spacing();
        ImGui::TextColored(ruleHeader, "II. CHALLENGE");
        ImGui::BulletText(
            "Pass through the Gate of Heaven on the 15th floor to leave the tower with your accumulated power.");

        ImGui::Spacing();
        ImGui::TextColored(ruleHeader, "III. POINTS");
        ImGui::BulletText(
            "Points are earned by exorcising Cursed Spirits and killing other challengers within the tower.");
        ImGui::BulletText(
            "Cursed Spirits are worth points in relation to their grade and strength.");
        ImGui::BulletText("Grade 4 Cursed Spirits are worth 1 point.");
        ImGui::BulletText("Grade 3 Cursed Spirits are worth 3 points.");
        ImGui::BulletText("Humans who are not yet sorcerers are worth 2 points.");
        ImGui::BulletText("Cursed Hosts are worth 5 points.");
        ImGui::BulletText(
            "Other Sorcerers and Special Grade Curses are worth 25 points.");

        ImGui::Spacing();
        ImGui::TextColored(ruleHeader, "IV. GRADING");
        ImGui::TextWrapped(
            "The Tower of Malevolent Divinity grades sorcerers who enter it "
            "based on how many points they earn.");
        ImGui::TextWrapped(
            "When a sorcerer obtains a new grade, they may spend their points "
            "to increase their powers and abilities once for that grade.");
        ImGui::BulletText("Grade 4 - Starting Grade");
        ImGui::BulletText("Grade 3 - 15 Points");
        ImGui::BulletText("Grade 2 - 50 Points");
        ImGui::BulletText("Grade 1 - 100 Points");
        ImGui::BulletText("Special Grade - 200 Points");

        ImGui::EndChild();
        if (ImGui::Button("Close", ImVec2(-1, 40))) {
            rulesMenu_ = false;
            if (rulesReturnToPause_) pauseMenu_ = true;
            rulesReturnToPause_ = false;
        }
        ImGui::TextDisabled("Press Escape to close.");
        ImGui::End();
    }

    void drawPauseMenu() {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always,
                                ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(472, 474), ImGuiCond_Always);
        constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings;
        if (!beginObsidianWindow("Paused", nullptr, flags)) {
            ImGui::End();
            return;
        }

        ImGui::TextUnformatted("GAME PAUSED");
        ImGui::Separator();
        ImGui::Spacing();
        if (ImGui::Button("Resume", ImVec2(-1, 44))) {
            pauseMenu_ = false;
            soundMenu_ = false;
        }
        if (ImGui::Button("Sound", ImVec2(-1, 44)))
            soundMenu_ = true;
        if (ImGui::Button("Controls", ImVec2(-1, 44))) {
            pauseMenu_ = false;
            soundMenu_ = false;
            helpReturnToPause_ = true;
            helpMenu_ = true;
        }
        if (ImGui::Button("Tower Rules", ImVec2(-1, 44))) {
            pauseMenu_ = false;
            soundMenu_ = false;
            rulesReturnToPause_ = true;
            rulesMenu_ = true;
        }
        if (ImGui::Button("New Game", ImVec2(-1, 44))) {
            nameInput_.fill('\0');
            pauseMenu_ = false;
            soundMenu_ = false;
            screen_ = Screen::NameEntry;
        }
        if (ImGui::Button("Main Menu", ImVec2(-1, 44))) {
            pauseMenu_ = false;
            soundMenu_ = false;
            screen_ = Screen::MainMenu;
        }
        if (ImGui::Button("Quit", ImVec2(-1, 44)))
            quitRequested_ = true;
        ImGui::TextDisabled("Press Escape to resume.");
        ImGui::End();
    }

    void drawSoundMenu() {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always,
                                ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(430, 220), ImGuiCond_Always);
        constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings;
        if (!beginObsidianWindow("Sound", &soundMenu_, flags)) {
            ImGui::End();
            return;
        }

        ImGui::TextUnformatted("MUSIC VOLUME");
        ImGui::Separator();
        ImGui::SetNextItemWidth(-1.0f);
        if (ImGui::SliderInt("##MusicVolume", &musicVolume_, 0, 100, "%d%%")) {
#ifdef _WIN32
            setGameplayMusicVolume(musicVolume_);
#endif
        }
        ImGui::TextDisabled("Set the volume to 0%% to mute all music.");
        ImGui::Spacing();
        if (ImGui::Button("Close", ImVec2(-1, 40))) soundMenu_ = false;
        ImGui::End();
    }

    void drawTechniqueCustomizationMenu() {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Always,
                                ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(682, 620), ImGuiCond_Always);
        constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoSavedSettings;
        if (!beginObsidianWindow("Cursed Technique Customization",
                                 &techniqueCustomizationMenu_, flags)) {
            ImGui::End();
            return;
        }

        const char* technique =
            noInnateTechniqueSelected_ ? "None" :
            selectedClan_ == 0 ? "Limitless" :
            selectedClan_ == 1 ? "Shadow Master" :
            selectedClan_ == 2 ? "Disaster Flames" :
            selectedClan_ == 3 ? "Ratio" : "Unawakened";
        ImGui::TextColored(clanColor(selectedClan_), "Clan: %s",
                           playerClan_.c_str());
        ImGui::Text("Cursed Technique: %s", technique);
        ImGui::SeparatorText("Output");

        if (noInnateTechniqueSelected_) {
            ImGui::TextDisabled(
                "No Innate Technique prevents Cursed Technique customization.");
        } else {
            ImGui::Text("Current output: %.2fx", techniqueOutput_);
            ImGui::BeginDisabled(techniqueOutput_ <= 1.0f);
            if (ImGui::Button("-0.25", ImVec2(120, 40)))
                techniqueOutput_ = std::max(1.0f, techniqueOutput_ - 0.25f);
            ImGui::EndDisabled();
            ImGui::SameLine();
            ImGui::BeginDisabled(techniqueOutput_ >= 2.0f);
            if (ImGui::Button("+0.25", ImVec2(120, 40)))
                techniqueOutput_ = std::min(2.0f, techniqueOutput_ + 0.25f);
            ImGui::EndDisabled();
            ImGui::TextDisabled(
                "Output increases technique power and base energy cost equally.");

            ImGui::SeparatorText("Area");
            const bool areaApplicable =
                selectedClan_ == 0 || selectedClan_ == 2 ||
                (selectedClan_ == 1 && nueLearned_) ||
                (selectedClan_ == 3 && collapseLearned_);
            ImGui::BeginDisabled(!areaApplicable);
            ImGui::Checkbox("Expanded Area", &techniqueAreaExpanded_);
            ImGui::EndDisabled();
            ImGui::TextWrapped(
                "Adds 1 tile to applicable radii and to each side of line or "
                "rectangular attacks. Applicable base Cursed Energy costs are "
                "multiplied by 1.5 before Efficiency.");
            if (!areaApplicable) {
                ImGui::TextDisabled(
                    selectedClan_ == 1
                        ? "Learn Summon Nue to unlock area customization."
                        : "Learn Collapse to unlock area customization.");
            } else if (selectedClan_ == 3) {
                ImGui::TextDisabled("Expanded Collapse costs 1.5x its base energy cost.");
            }

            if (selectedClan_ == 1) {
                ImGui::SeparatorText("Shadow Master Form");
                const int livingGyokuken = static_cast<int>(std::count_if(
                    allies_.begin(), allies_.end(), [](const Actor& ally) {
                        return ally.alive() &&
                               ally.classification ==
                                   ActorClassification::Shikigami &&
                               !ally.domainSummon && !ally.emberInsect &&
                               !ally.nue;
                    }));
                ImGui::BeginDisabled(livingGyokuken > 0);
                ImGui::Checkbox("Totality", &totalityActive_);
                ImGui::EndDisabled();
                ImGui::TextWrapped(
                    "Fuses the black and phantom-white Gyokuken. Maximum summons "
                    "becomes 1, but it has 30 HP and 10 base damage.");
                if (livingGyokuken > 0)
                    ImGui::TextDisabled(
                        "Totality cannot be toggled while a Gyokuken is active.");
                else
                    ImGui::TextColored(
                        ImVec4(0.72f, 0.52f, 1.0f, 1.0f), "Current form: %s",
                        totalityActive_ ? "Totality" : "Paired Gyokuken");
            }

            ImGui::SeparatorText("Current Effect");
            const int baseCost =
                selectedClan_ == 0 ? IndividualAttractionCost :
                selectedClan_ == 1 ? (totalityActive_ ? TotalityUpkeepCost
                                                      : GyokukenUpkeepCost) :
                selectedClan_ == 2 ? VolcanicBlastCost :
                                     RatioCriticalCost;
            ImGui::TextColored(ImVec4(0.52f, 0.70f, 1.0f, 1.0f),
                               "Cursed Technique Power: +%d damage",
                               cursedTechniquePowerBonus_);
            ImGui::TextColored(
                energyColor(),
                selectedClan_ == 1 ? "Adjusted upkeep per turn: %d"
                                   : "Adjusted energy cost: %d",
                (selectedClan_ == 0 || selectedClan_ == 2)
                    ? areaTechniqueEnergyCost(baseCost)
                    : techniqueEnergyCost(baseCost));
            if (selectedClan_ == 0) {
                ImGui::TextWrapped(
                    "Lapse: Limitless deals %d flat damage in a radius of %d.",
                    customizedTechniquePower(10), customizedTechniqueRadius(1));
            } else if (selectedClan_ == 1) {
                if (totalityActive_)
                    ImGui::TextWrapped(
                        "Gyokuken: Totality has 30 HP, deals %d damage, costs %d "
                        "CE per turn, and has a maximum of 1 active summon.",
                        customizedTechniquePower(10),
                        techniqueEnergyCost(TotalityUpkeepCost));
                else
                    ImGui::TextWrapped(
                        "Each Gyokuken has 15 HP, deals %d damage, costs %d CE "
                        "per turn, and has a maximum of 2 active summons.",
                        customizedTechniquePower(5),
                        techniqueEnergyCost(GyokukenUpkeepCost));
                if (nueLearned_)
                    ImGui::TextWrapped(
                        "Nue costs %d CE and explodes for %d damage in a radius "
                        "of %d, stunning survivors for 3 turns. Maximum 1 active.",
                        areaTechniqueEnergyCost(NueCost),
                        (customizedTechniquePower(VolcanicBlastDamage) + 1) / 2,
                        customizedTechniqueRadius(NueExplosionRadius));
            } else if (selectedClan_ == 2) {
                ImGui::TextWrapped(
                    "Lapse: Disaster Flames deals %d damage along a %d-tile-wide line. "
                    "Ember Insect explosions have a radius of %d.",
                    customizedTechniquePower(VolcanicBlastDamage),
                    techniqueAreaExpanded_ ? 3 : 1,
                    customizedTechniqueRadius(EmberInsectExplosionRadius));
            } else {
                ImGui::TextWrapped(
                    "Each Ratio stack guarantees a %.2fx critical hit. Collapse "
                    "covers a %dx5 area and costs %d Cursed Energy.",
                    ratioCriticalMultiplier(), techniqueAreaExpanded_ ? 3 : 1,
                    areaTechniqueEnergyCost(CollapseCost));
            }
            if (handSignsVowActive_)
                ImGui::TextColored(
                    ImVec4(0.35f, 0.72f, 1.0f, 1.0f),
                    "Hand Signs is applying an additional 1.25x power bonus.");
        }

        ImGui::Spacing();
        if (ImGui::Button("Close", ImVec2(-1, 40)))
            techniqueCustomizationMenu_ = false;
        ImGui::TextDisabled("Press T or Escape to close. Changes do not use a turn.");
        ImGui::End();
    }

    void updateAndDrawParticles(ImDrawList* draw, ImVec2 start, float cellSize) {
        const float deltaTime = std::min(ImGui::GetIO().DeltaTime, 0.05f);
        for (Particle& particle : particles_) {
            particle.lifetime -= deltaTime;
            particle.x += particle.velocityX * deltaTime;
            particle.y += particle.velocityY * deltaTime;
            if (particle.lifetime <= 0.0f) continue;

            const float life = particle.lifetime / particle.maxLifetime;
            const ImVec2 center(start.x + particle.x * cellSize,
                                start.y + particle.y * cellSize);
            const float radius = std::max(1.5f, particle.size * cellSize * (0.5f + life));
            if (particle.effect == ParticleEffect::Vacuum) {
                const ImVec2 tail(center.x - particle.velocityX * cellSize * 0.045f,
                                  center.y - particle.velocityY * cellSize * 0.045f);
                draw->AddLine(tail, center, ImColor(0.15f, 0.55f, 1.0f, life),
                              std::max(1.0f, radius * 0.65f));
                draw->AddCircleFilled(center, radius * 1.9f,
                                      ImColor(0.02f, 0.28f, 1.0f, life * 0.22f));
                draw->AddCircleFilled(center, radius,
                                      ImColor(0.20f, 0.72f, 1.0f, life));
            } else if (particle.effect == ParticleEffect::ShadowInk) {
                const float bloom = 0.55f + (1.0f - life) * 1.8f;
                const float inkRadius = radius * bloom;
                const ImVec2 tail(
                    center.x - particle.velocityX * cellSize * 0.075f,
                    center.y - particle.velocityY * cellSize * 0.075f);
                draw->AddLine(tail, center,
                              ImColor(0.16f, 0.03f, 0.24f, life * 0.75f),
                              std::max(1.0f, inkRadius * 0.72f));
                draw->AddCircleFilled(
                    center, inkRadius * 1.65f,
                    ImColor(0.015f, 0.008f, 0.025f, life * 0.58f));
                draw->AddCircleFilled(
                    center, inkRadius,
                    ImColor(0.12f, 0.025f, 0.19f, life * 0.90f));
                draw->AddCircle(
                    center, inkRadius * 1.9f,
                    ImColor(0.48f, 0.16f, 0.72f, life * 0.42f), 12,
                    std::max(1.0f, inkRadius * 0.18f));
            } else if (particle.effect == ParticleEffect::ReversalRed) {
                draw->AddCircleFilled(center, radius * 2.6f,
                                      ImColor(1.0f, 0.02f, 0.08f, life * 0.24f));
                draw->AddCircleFilled(center, radius * 1.45f,
                                      ImColor(1.0f, 0.05f, 0.10f, life * 0.72f));
                draw->AddCircleFilled(center, radius * 0.62f,
                                      ImColor(1.0f, 0.72f, 0.76f, life));
            } else if (particle.effect == ParticleEffect::Lightning) {
                const float reach = radius * 3.2f;
                draw->AddCircleFilled(center, radius * 2.3f,
                                      ImColor(0.10f, 0.55f, 1.0f, life * 0.24f));
                draw->AddLine(ImVec2(center.x - reach, center.y + reach * 0.45f),
                              ImVec2(center.x + reach, center.y - reach * 0.45f),
                              ImColor(0.25f, 0.82f, 1.0f, life),
                              std::max(1.5f, radius * 0.52f));
                draw->AddLine(ImVec2(center.x - reach * 0.35f, center.y - reach),
                              ImVec2(center.x + reach * 0.25f, center.y + reach),
                              ImColor(0.82f, 0.96f, 1.0f, life),
                              std::max(1.0f, radius * 0.30f));
            } else if (particle.effect == ParticleEffect::RatioMark) {
                const float progress = 1.0f - life;
                const float pulse = std::sin(progress * 3.14159265f);
                const float reach = cellSize * (0.34f + 0.18f * pulse);
                const float blackThickness =
                    std::max(1.5f, cellSize * 0.055f);
                const ImU32 edge = ImColor(0.78f, 0.72f, 0.56f,
                                           life * 0.46f);
                const ImU32 black = ImColor(0.005f, 0.005f, 0.008f,
                                            std::min(1.0f, life * 1.65f));
                const ImVec2 upperLeft(center.x - reach, center.y - reach);
                const ImVec2 lowerRight(center.x + reach, center.y + reach);
                draw->AddLine(upperLeft, lowerRight, edge,
                              blackThickness + std::max(1.5f, cellSize * 0.04f));
                draw->AddLine(upperLeft, lowerRight, black, blackThickness);
                const float tickHalfLength = cellSize * (0.10f + 0.025f * pulse);
                constexpr float diagonalUnit = 0.70710678f;
                for (int tick = 1; tick <= 3; ++tick) {
                    const float t = static_cast<float>(tick) / 4.0f;
                    const ImVec2 tickCenter(
                        upperLeft.x + (lowerRight.x - upperLeft.x) * t,
                        upperLeft.y + (lowerRight.y - upperLeft.y) * t);
                    const ImVec2 tickStart(
                        tickCenter.x - diagonalUnit * tickHalfLength,
                        tickCenter.y + diagonalUnit * tickHalfLength);
                    const ImVec2 tickEnd(
                        tickCenter.x + diagonalUnit * tickHalfLength,
                        tickCenter.y - diagonalUnit * tickHalfLength);
                    draw->AddLine(tickStart, tickEnd, edge,
                                  blackThickness +
                                      std::max(1.5f, cellSize * 0.04f));
                    draw->AddLine(tickStart, tickEnd, black, blackThickness);
                }
            } else if (particle.effect == ParticleEffect::BlackFlash) {
                const float progress = 1.0f - life;
                const float pulse = std::sin(progress * 3.14159265f);
                const float reach = cellSize * (0.62f + pulse * 0.22f);
                const ImVec2 slashStart(center.x - reach,
                                        center.y + reach * 0.72f);
                const ImVec2 slashEnd(center.x + reach,
                                      center.y - reach * 0.72f);
                const float outlineWidth =
                    std::max(5.0f, cellSize * (0.18f + pulse * 0.06f));
                draw->AddCircleFilled(
                    center, cellSize * (0.52f + pulse * 0.28f),
                    ImColor(0.85f, 0.0f, 0.02f, life * 0.20f));
                draw->AddLine(slashStart, slashEnd,
                              ImColor(1.0f, 0.02f, 0.03f, life),
                              outlineWidth);
                draw->AddLine(slashStart, slashEnd,
                              ImColor(0.0f, 0.0f, 0.0f,
                                      std::min(1.0f, life * 1.8f)),
                              outlineWidth * 0.56f);
                const ImVec2 secondStart(center.x - reach * 0.58f,
                                         center.y - reach * 0.16f);
                const ImVec2 secondEnd(center.x + reach * 0.38f,
                                       center.y + reach * 0.55f);
                draw->AddLine(secondStart, secondEnd,
                              ImColor(1.0f, 0.02f, 0.03f, life),
                              outlineWidth * 0.64f);
                draw->AddLine(secondStart, secondEnd,
                              ImColor(0.0f, 0.0f, 0.0f, life),
                              outlineWidth * 0.34f);
            } else if (particle.effect == ParticleEffect::EmberExplosion) {
                const ImVec2 tail(
                    center.x - particle.velocityX * cellSize * 0.025f,
                    center.y - particle.velocityY * cellSize * 0.025f);
                draw->AddLine(tail, center,
                              ImColor(1.0f, 0.12f, 0.01f, life * 0.92f),
                              std::max(1.5f, radius * 0.75f));
                draw->AddCircleFilled(
                    center, radius * 2.8f,
                    ImColor(1.0f, 0.10f, 0.01f, life * 0.20f));
                draw->AddCircleFilled(
                    center, radius * 1.25f,
                    ImColor(1.0f, 0.48f + 0.38f * life, 0.03f, life));
            } else if (particle.effect ==
                       ParticleEffect::PurpleLightningExplosion) {
                const ImVec2 tail(
                    center.x - particle.velocityX * cellSize * 0.035f,
                    center.y - particle.velocityY * cellSize * 0.035f);
                const ImVec2 kink((tail.x + center.x) * 0.5f + radius * 0.8f,
                                  (tail.y + center.y) * 0.5f - radius * 0.6f);
                draw->AddCircleFilled(
                    center, radius * 3.0f,
                    ImColor(0.42f, 0.04f, 0.78f, life * 0.20f));
                draw->AddLine(tail, kink,
                              ImColor(0.62f, 0.18f, 1.0f, life),
                              std::max(1.5f, radius * 0.70f));
                draw->AddLine(kink, center,
                              ImColor(0.92f, 0.72f, 1.0f, life),
                              std::max(1.0f, radius * 0.42f));
                draw->AddCircleFilled(
                    center, radius * 0.9f,
                    ImColor(0.82f, 0.42f, 1.0f, life));
            } else {
                draw->AddCircleFilled(center, radius * 1.8f,
                                      ImColor(1.0f, 0.12f, 0.01f, life * 0.22f));
                draw->AddCircleFilled(center, radius,
                                      ImColor(1.0f, 0.28f + 0.55f * life, 0.03f, life));
            }
        }
        particles_.erase(std::remove_if(particles_.begin(), particles_.end(),
                         [](const Particle& particle) { return particle.lifetime <= 0.0f; }),
                         particles_.end());
    }

    void drawMap() {
        const ImVec2 available = ImGui::GetContentRegionAvail();
        ImGuiIO& io = ImGui::GetIO();
        const bool mapHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
        if (mapHovered) {
            const float wheel = io.MouseWheel;
            if (wheel != 0.0f) {
                zoom_ = std::clamp(zoom_ + wheel * 0.15f, 0.65f, 3.0f);
            }
            if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
                ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
                ImGui::SetScrollX(ImGui::GetScrollX() - io.MouseDelta.x);
                ImGui::SetScrollY(ImGui::GetScrollY() - io.MouseDelta.y);
                centerCamera_ = false;
            }
        }

        // The camera is scrollable, so the map no longer needs to shrink until
        // all 45x29 tiles fit at once. At 100% zoom each tile is 32x32 pixels.
        constexpr float baseCellSize = 32.0f;
        const float cellSize = std::max(5.0f, std::floor(baseCellSize * zoom_));
        const ImVec2 start = ImGui::GetCursorScreenPos();
        ImDrawList* draw = ImGui::GetWindowDrawList();
        Point hoveredTile{};
        bool hasHoveredTile = false;
        if (mapHovered) {
            hoveredTile = {
                static_cast<int>(std::floor((io.MousePos.x - start.x) / cellSize)),
                static_cast<int>(std::floor((io.MousePos.y - start.y) / cellSize))
            };
            hasHoveredTile = inside(hoveredTile.x, hoveredTile.y);
            if (hasHoveredTile && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                if (targeting_) {
                    for (int enemyIndex = 0;
                         enemyIndex < static_cast<int>(enemies_.size());
                         ++enemyIndex) {
                        const Actor& enemy = enemies_[enemyIndex];
                        if (enemy.alive() && enemy.pos == hoveredTile &&
                            at(enemy.pos.x, enemy.pos.y).visible) {
                            selectedTargetIndex_ = enemyIndex;
                            break;
                        }
                    }
                } else if (volcanicTargeting_) {
                    const int dx = sign(hoveredTile.x - player_.pos.x);
                    const int dy = sign(hoveredTile.y - player_.pos.y);
                    if (dx != 0 || dy != 0)
                        volcanicDirection_ = {dx, dy};
                } else if (reversalRedTargeting_) {
                    const int dx = sign(hoveredTile.x - player_.pos.x);
                    const int dy = sign(hoveredTile.y - player_.pos.y);
                    if (dx != 0 || dy != 0)
                        reversalRedDirection_ = {dx, dy};
                } else if (collapseTargeting_) {
                    const int dx = sign(hoveredTile.x - player_.pos.x);
                    const int dy = sign(hoveredTile.y - player_.pos.y);
                    if (dx != 0 || dy != 0)
                        collapseDirection_ = {dx, dy};
                } else {
                    inspectedTile_ = hoveredTile;
                    hasInspection_ = true;
                }
            }
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                hasInspection_ = false;
        }

        draw->AddRectFilled(start, ImVec2(start.x + MapWidth * cellSize, start.y + MapHeight * cellSize),
                            IM_COL32(8, 10, 14, 255));
        auto wallVariantAt = [&](int wallX, int wallY) {
            bool bordersHallway = false;
            bool bordersRoom = false;
            constexpr std::array<Point, 4> neighbors{{
                {-1, 0}, {1, 0}, {0, -1}, {0, 1}
            }};
            for (Point offset : neighbors) {
                const int neighborX = wallX + offset.x;
                const int neighborY = wallY + offset.y;
                if (!inside(neighborX, neighborY)) continue;
                const FloorRegion region = at(neighborX, neighborY).region;
                bordersHallway |= region == FloorRegion::Hallway;
                bordersRoom |= region == FloorRegion::Room;
            }

            // Hallways use one uniform wall face. Room walls use their main
            // face 75% of the time, with restrained atlas variation elsewhere.
            if (bordersHallway) return 0u;
            const unsigned hash =
                static_cast<unsigned>(wallX) * 19349663u ^
                static_cast<unsigned>(wallY) * 83492791u ^
                static_cast<unsigned>(currentLevel_) * 73856093u;
            if (bordersRoom && hash % 100u < 75u) return 5u;
            unsigned variant = (hash >> 8u) % 15u;
            if (bordersRoom && variant >= 5u) ++variant;
            return variant;
        };
        for (int y = 0; y < MapHeight; ++y) {
            for (int x = 0; x < MapWidth; ++x) {
                const Cell& cell = at(x, y);
                if (!cell.explored) continue;
                const ImVec2 a(start.x + x * cellSize, start.y + y * cellSize);
                const ImVec2 b(a.x + cellSize, a.y + cellSize);
                ImU32 color;
                if (cell.tile == Tile::Wall)
                    color = cell.visible ? IM_COL32(75, 82, 98, 255) : IM_COL32(32, 37, 48, 255);
                else
                    color = cell.visible ? IM_COL32(38, 43, 51, 255) : IM_COL32(18, 22, 29, 255);
                draw->AddRectFilled(a, b, color);
                if (cell.tile == Tile::Wall && wallTexture != 0) {
                    const unsigned variant = wallVariantAt(x, y);
                    const float u0 = static_cast<float>(variant % 4u) * 0.25f;
                    const float v0 = static_cast<float>(variant / 4u) * 0.25f;
                    const ImU32 tint = cell.visible
                        ? IM_COL32(230, 230, 230, 255)
                        : IM_COL32(150, 150, 155, 255);
                    draw->AddImage(
                        static_cast<ImTextureID>(wallTexture), a, b,
                        ImVec2(u0, v0), ImVec2(u0 + 0.25f, v0 + 0.25f), tint);
                    draw->AddRectFilled(
                        a, b, cell.visible ? IM_COL32(0, 0, 0, 95)
                                           : IM_COL32(0, 0, 0, 160));
                }
                if (cell.tile == Tile::Floor && floorTexture != 0) {
                    const unsigned variant =
                        (static_cast<unsigned>(x) * 73856093u ^
                         static_cast<unsigned>(y) * 19349663u ^
                         static_cast<unsigned>(currentLevel_) * 83492791u) &
                        15u;
                    const float u0 = static_cast<float>(variant % 4u) * 0.25f;
                    const float v0 = static_cast<float>(variant / 4u) * 0.25f;
                    const ImU32 tint = cell.visible
                        ? IM_COL32(220, 220, 210, 255)
                        : IM_COL32(82, 82, 86, 255);
                    draw->AddImage(
                        static_cast<ImTextureID>(floorTexture), a, b,
                        ImVec2(u0, v0), ImVec2(u0 + 0.25f, v0 + 0.25f), tint);
                }
            }
        }

        if (volcanicTargeting_) {
            for (Point tile : volcanicBlastLine()) {
                const ImVec2 a(start.x + tile.x * cellSize, start.y + tile.y * cellSize);
                const ImVec2 b(a.x + cellSize, a.y + cellSize);
                draw->AddRectFilled(a, b, IM_COL32(255, 80, 12, 70));
                draw->AddRect(a, b, IM_COL32(255, 145, 35, 230),
                              0.0f, 0, std::max(1.0f, cellSize * 0.08f));
            }
        }
        if (reversalRedTargeting_) {
            for (Point tile : reversalRedLine()) {
                const ImVec2 a(start.x + tile.x * cellSize,
                               start.y + tile.y * cellSize);
                const ImVec2 b(a.x + cellSize, a.y + cellSize);
                draw->AddRectFilled(a, b, IM_COL32(255, 30, 45, 78));
                draw->AddRect(a, b, IM_COL32(255, 90, 105, 240),
                              0.0f, 0, std::max(1.0f, cellSize * 0.09f));
            }
        }
        if (collapseTargeting_) {
            for (Point tile : collapseArea()) {
                const ImVec2 a(start.x + tile.x * cellSize,
                               start.y + tile.y * cellSize);
                const ImVec2 b(a.x + cellSize, a.y + cellSize);
                draw->AddRectFilled(a, b, IM_COL32(45, 220, 85, 70));
                draw->AddRect(a, b, IM_COL32(75, 245, 110, 225),
                              0.0f, 0, std::max(1.0f, cellSize * 0.08f));
            }
        }
        if (targeting_ && selectedTargetIndex_ >= 0 &&
            selectedTargetIndex_ < static_cast<int>(enemies_.size())) {
            const Point center = enemies_[selectedTargetIndex_].pos;
            const int radius = kamutokeTargeting_
                                   ? 0
                                   : customizedTechniqueRadius(1);
            for (int y = -radius; y <= radius; ++y) {
                for (int x = -radius; x <= radius; ++x) {
                    const Point tile{center.x + x, center.y + y};
                    if (!inside(tile.x, tile.y)) continue;
                    const ImVec2 a(start.x + tile.x * cellSize,
                                   start.y + tile.y * cellSize);
                    const ImVec2 b(a.x + cellSize, a.y + cellSize);
                    draw->AddRectFilled(a, b, kamutokeTargeting_
                        ? IM_COL32(35, 205, 255, 70)
                        : IM_COL32(35, 125, 255, 48));
                    draw->AddRect(a, b, kamutokeTargeting_
                        ? IM_COL32(130, 240, 255, 220)
                        : IM_COL32(80, 190, 255, 155));
                }
            }
        }

        auto glyph = [&](Point p, char c, ImU32 color) {
            char text[2]{c, '\0'};
            ImFont* font = ImGui::GetFont();
            const float fontSize = std::max(7.0f, cellSize * 0.82f);
            const ImVec2 size = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text);
            ImVec2 pos(start.x + p.x * cellSize + (cellSize - size.x) * 0.5f,
                       start.y + p.y * cellSize + (cellSize - size.y) * 0.5f);
            draw->AddText(font, fontSize, pos, color, text);
        };
        auto mapUnitSprite = [&](Point p, int spriteIndex) {
            constexpr float atlasCell = 0.25f;
            const float u0 = static_cast<float>(spriteIndex % 4) * atlasCell;
            const float v0 = static_cast<float>(spriteIndex / 4) * atlasCell;
            // The generated atlas cells are slightly wider than square. A
            // small vertical inset preserves the silhouettes' proportions.
            const float verticalInset = cellSize * 0.10f;
            const ImVec2 a(start.x + p.x * cellSize,
                           start.y + p.y * cellSize + verticalInset);
            const ImVec2 b(a.x + cellSize,
                           start.y + (p.y + 1) * cellSize - verticalInset);
            draw->AddImage(static_cast<ImTextureID>(mapUnitSpriteTexture), a, b,
                           ImVec2(u0, v0),
                           ImVec2(u0 + atlasCell, v0 + atlasCell));
        };
        auto drawStairTile = [&](Point p, bool descending) {
            if (stairTexture == 0 || !at(p.x, p.y).explored) return;
            const ImVec2 a(start.x + p.x * cellSize,
                           start.y + p.y * cellSize);
            const ImVec2 b(a.x + cellSize, a.y + cellSize);
            const float u0 = descending ? 0.5f : 0.0f;
            const float v0 = currentLevel_ % 2 == 0 ? 0.0f : 0.5f;
            const ImU32 tint = at(p.x, p.y).visible
                ? IM_COL32(235, 235, 230, 255)
                : IM_COL32(90, 90, 96, 255);
            draw->AddImage(static_cast<ImTextureID>(stairTexture), a, b,
                           ImVec2(u0, v0), ImVec2(u0 + 0.5f, v0 + 0.5f),
                           tint);
        };
        auto drawBossGateTile = [&](Point p) {
            if (!at(p.x, p.y).explored) return;
            const ImVec2 cellMin(start.x + p.x * cellSize,
                                 start.y + p.y * cellSize);
            const float horizontalInset = cellSize * 0.125f;
            const ImVec2 a(cellMin.x + horizontalInset, cellMin.y);
            const ImVec2 b(cellMin.x + cellSize - horizontalInset,
                           cellMin.y + cellSize);
            const int gateColumn = std::clamp((currentLevel_ + 1) / 3 - 1, 0, 3);
            const float u0 = static_cast<float>(gateColumn) * 0.25f;
            const float v0 = bossExitLocked() ? 0.0f : 0.5f;
            const ImU32 tint = at(p.x, p.y).visible
                ? IM_COL32_WHITE : IM_COL32(95, 95, 104, 255);
            if (bossGateSpritesTexture != 0)
                draw->AddImage(static_cast<ImTextureID>(bossGateSpritesTexture),
                               a, b, ImVec2(u0, v0),
                               ImVec2(u0 + 0.25f, v0 + 0.5f), tint);
            else
                glyph(p, bossExitLocked() ? 'X' : 'O',
                      bossExitLocked() ? IM_COL32(255, 65, 155, 255)
                                       : IM_COL32(145, 225, 255, 255));
        };
        const bool hasBossGate =
            !levels_[currentLevel_].bossGateName.empty() &&
            downStairs_.x >= 0;
        if (upStairs_.x >= 0) drawStairTile(upStairs_, false);
        if (hasBossGate) drawBossGateTile(downStairs_);
        else if (downStairs_.x >= 0) drawStairTile(downStairs_, true);
        if (upStairs_.x >= 0 && at(upStairs_.x, upStairs_.y).explored)
            glyph(upStairs_, '<', IM_COL32(115, 205, 255, 255));
        if (!hasBossGate && downStairs_.x >= 0 &&
            at(downStairs_.x, downStairs_.y).explored)
            glyph(downStairs_, '>', IM_COL32(255, 190, 75, 255));
        if (pachinkoMachine_.x >= 0 &&
            at(pachinkoMachine_.x, pachinkoMachine_.y).explored) {
            if (cursedPachinkoTexture != 0) {
                const ImVec2 a(start.x + pachinkoMachine_.x * cellSize,
                               start.y + pachinkoMachine_.y * cellSize);
                const ImVec2 b(a.x + cellSize, a.y + cellSize);
                const ImU32 tint = at(pachinkoMachine_.x, pachinkoMachine_.y).visible
                    ? IM_COL32_WHITE
                    : IM_COL32(90, 90, 96, 255);
                draw->AddImage(static_cast<ImTextureID>(cursedPachinkoTexture),
                               a, b, ImVec2(0, 0), ImVec2(1, 1), tint);
            } else {
                glyph(pachinkoMachine_, 'P',
                      IM_COL32(220, 170, 55, 255));
            }
        }
        if (cursedToolShop_.x >= 0 &&
            at(cursedToolShop_.x, cursedToolShop_.y).explored) {
            if (cursedToolSpritesTexture != 0) {
                const ImVec2 a(start.x + cursedToolShop_.x * cellSize,
                               start.y + cursedToolShop_.y * cellSize);
                const ImVec2 b(a.x + cellSize, a.y + cellSize);
                const ImU32 tint = at(cursedToolShop_.x, cursedToolShop_.y).visible
                    ? IM_COL32_WHITE : IM_COL32(90, 90, 96, 255);
                draw->AddImage(static_cast<ImTextureID>(cursedToolSpritesTexture),
                               a, b, ImVec2(2.0f / 3.0f, 0.0f),
                               ImVec2(1.0f, 1.0f), tint);
            } else {
                glyph(cursedToolShop_, '$', IM_COL32(205, 125, 255, 255));
            }
        }
        if (gateOfHeaven_.x >= 0 &&
            at(gateOfHeaven_.x, gateOfHeaven_.y).explored) {
            const ImVec2 a(start.x + gateOfHeaven_.x * cellSize,
                           start.y + gateOfHeaven_.y * cellSize);
            const ImVec2 b(a.x + cellSize, a.y + cellSize);
            if (cursedGateSpritesTexture != 0) {
                const ImU32 tint = at(gateOfHeaven_.x, gateOfHeaven_.y).visible
                    ? IM_COL32_WHITE : IM_COL32(105, 105, 112, 255);
                draw->AddImage(static_cast<ImTextureID>(cursedGateSpritesTexture),
                               a, b, ImVec2(0.5f, 0.0f), ImVec2(1.0f, 1.0f),
                               tint);
            } else {
                glyph(gateOfHeaven_, 'G', IM_COL32(255, 220, 100, 255));
            }
        }
        for (Point key : cursedKeys_) {
            if (!at(key.x, key.y).visible) continue;
            const ImVec2 a(start.x + key.x * cellSize,
                           start.y + key.y * cellSize);
            const ImVec2 b(a.x + cellSize, a.y + cellSize);
            if (cursedGateSpritesTexture != 0)
                draw->AddImage(static_cast<ImTextureID>(cursedGateSpritesTexture),
                               a, b, ImVec2(0.0f, 0.0f), ImVec2(0.5f, 1.0f));
            else
                glyph(key, 'k', IM_COL32(190, 95, 255, 255));
        }
        for (Point corpse : corpses_) {
            if (at(corpse.x, corpse.y).visible)
                glyph(corpse, '%', IM_COL32(145, 105, 105, 255));
        }
        for (const GroundItem& groundItem : groundItems_) {
            if (!at(groundItem.pos.x, groundItem.pos.y).visible) continue;
            const char itemGlyph =
                groundItem.item.slot == EquipmentSlot::Clothing ? '[' :
                groundItem.item.slot == EquipmentSlot::Weapon ? ')' : '/';
            const ImU32 itemColor =
                groundItem.item.cursedDamage
                    ? IM_COL32(210, 90, 245, 255)
                    : IM_COL32(235, 205, 115, 255);
            glyph(groundItem.pos, itemGlyph, itemColor);
        }
        for (int i = 0; i < static_cast<int>(enemies_.size()); ++i) {
            const Actor& enemy = enemies_[i];
            if (enemy.alive() && at(enemy.pos.x, enemy.pos.y).visible) {
                if (enemy.knowledge) {
                    const ImVec2 a(start.x + enemy.pos.x * cellSize + 1.0f,
                                   start.y + enemy.pos.y * cellSize + 1.0f);
                    const ImVec2 b(start.x + (enemy.pos.x + 1) * cellSize - 1.0f,
                                   start.y + (enemy.pos.y + 1) * cellSize - 1.0f);
                    draw->AddRect(a, b, IM_COL32(255, 205, 55, 235),
                                  0.0f, 0, std::max(1.5f, cellSize * 0.07f));
                }
                if (targeting_ && i == selectedTargetIndex_) {
                    const ImVec2 a(start.x + enemy.pos.x * cellSize,
                                   start.y + enemy.pos.y * cellSize);
                    const ImVec2 b(a.x + cellSize, a.y + cellSize);
                    const float pulse = 0.65f + 0.35f *
                        std::sin(static_cast<float>(ImGui::GetTime()) * 7.0f);
                    draw->AddRect(a, b, kamutokeTargeting_
                                      ? ImColor(0.25f, 0.85f, 1.0f, pulse)
                                      : ImColor(1.0f, 0.25f, 0.85f, pulse),
                                  0.0f, 0, std::max(2.0f, cellSize * 0.12f));
                }
                if (enemy.samurai && samuraiSpritesTexture != 0) {
                    const float u0 = enemy.samuraiHost ? 0.5f : 0.0f;
                    const ImVec2 a(start.x + enemy.pos.x * cellSize,
                                   start.y + enemy.pos.y * cellSize);
                    const ImVec2 b(a.x + cellSize, a.y + cellSize);
                    draw->AddImage(static_cast<ImTextureID>(samuraiSpritesTexture),
                                   a, b, ImVec2(u0, 0.0f),
                                   ImVec2(u0 + 0.5f, 0.5f));
                } else if ((enemy.spiderGhoul || enemy.spiderHost) &&
                    spiderGhoulSpritesTexture != 0) {
                    const float u0 = enemy.spiderHost ? 0.5f : 0.0f;
                    const ImVec2 a(start.x + enemy.pos.x * cellSize,
                                   start.y + enemy.pos.y * cellSize);
                    const ImVec2 b(a.x + cellSize, a.y + cellSize);
                    draw->AddImage(
                        static_cast<ImTextureID>(spiderGhoulSpritesTexture),
                        a, b, ImVec2(u0, 0.0f), ImVec2(u0 + 0.5f, 0.5f));
                } else if (mapUnitSpriteTexture != 0) {
                    mapUnitSprite(enemy.pos,
                                  enemy.bossSorcerer
                                      ? enemy.sorcererClan
                                      : mapEnemySpriteIndex(enemy));
                } else {
                    const ImU32 enemyColor =
                        enemy.cursedHost ? IM_COL32(235, 100, 245, 255) :
                        enemy.classification ==
                                ActorClassification::CursedSpirit ?
                                                   IM_COL32(190, 90, 235, 255) :
                        enemy.classification == ActorClassification::Human ?
                                                   IM_COL32(205, 210, 220, 255) :
                        enemy.glyph == 'O' ? IM_COL32(230, 105, 80, 255) :
                                              IM_COL32(106, 210, 115, 255);
                    glyph(enemy.pos, enemy.glyph, enemyColor);
                }
                if (enemy.cursedHost) {
                    const ImVec2 a(start.x + enemy.pos.x * cellSize,
                                   start.y + enemy.pos.y * cellSize);
                    const ImVec2 b(a.x + cellSize, a.y + cellSize);
                    draw->AddRect(a, b, IM_COL32(215, 55, 235, 255), 0.0f, 0,
                                  std::max(1.5f, cellSize * 0.08f));
                }
                if (enemy.ratioStacks > 0) {
                    const ImVec2 a(start.x + enemy.pos.x * cellSize + 2.0f,
                                   start.y + enemy.pos.y * cellSize + 2.0f);
                    const ImVec2 b(start.x + (enemy.pos.x + 1) * cellSize - 2.0f,
                                   start.y + (enemy.pos.y + 1) * cellSize - 2.0f);
                    draw->AddRect(a, b, IM_COL32(90, 245, 115, 245), 0.0f, 0,
                                  std::max(1.5f, cellSize * 0.07f));
                }
            }
        }
        for (std::size_t i = 0; i < allies_.size(); ++i) {
            const Actor& ally = allies_[i];
            if (!ally.alive() || !at(ally.pos.x, ally.pos.y).visible) continue;
            if (ally.emberInsect && emberInsectMapTexture != 0) {
                const ImVec2 a(start.x + ally.pos.x * cellSize,
                               start.y + ally.pos.y * cellSize);
                const ImVec2 b(a.x + cellSize, a.y + cellSize);
                draw->AddImage(
                    static_cast<ImTextureID>(emberInsectMapTexture), a, b,
                    ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
            } else if (ally.nue && nueMapTexture != 0) {
                const ImVec2 a(start.x + ally.pos.x * cellSize,
                               start.y + ally.pos.y * cellSize);
                const ImVec2 b(a.x + cellSize, a.y + cellSize);
                draw->AddImage(static_cast<ImTextureID>(nueMapTexture), a, b);
            } else if (ally.gyokukenTotality && gyokukenTotalityMapTexture != 0) {
                const ImVec2 a(start.x + ally.pos.x * cellSize,
                               start.y + ally.pos.y * cellSize);
                const ImVec2 b(a.x + cellSize, a.y + cellSize);
                draw->AddImage(
                    static_cast<ImTextureID>(gyokukenTotalityMapTexture), a, b,
                    ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));
            } else if (gyokukenMapSpriteTexture != 0) {
                const float u0 = ally.phantomWhiteGyokuken ? 0.5f : 0.0f;
                const ImVec2 a(start.x + ally.pos.x * cellSize,
                               start.y + ally.pos.y * cellSize);
                const ImVec2 b(a.x + cellSize, a.y + cellSize);
                draw->AddImage(
                    static_cast<ImTextureID>(gyokukenMapSpriteTexture), a, b,
                    ImVec2(u0, 0.0f), ImVec2(u0 + 0.5f, 1.0f));
            } else {
                glyph(ally.pos, ally.glyph, IM_COL32(145, 105, 245, 255));
            }
        }
        if (domainExpansionActive_ || domainConfirming_) {
            const ImVec2 center(
                start.x + (player_.pos.x + 0.5f) * cellSize,
                start.y + (player_.pos.y + 0.5f) * cellSize);
            const ImVec4 color = clanColor(selectedClan_);
            const float pulse = 0.5f + 0.5f *
                std::sin(static_cast<float>(ImGui::GetTime()) * 2.2f);
            const float radius = cellSize *
                (static_cast<float>(DomainExpansionRadius) + 0.45f +
                 pulse * 0.12f);
            for (int offsetY = -DomainExpansionRadius;
                 offsetY <= DomainExpansionRadius; ++offsetY) {
                for (int offsetX = -DomainExpansionRadius;
                     offsetX <= DomainExpansionRadius; ++offsetX) {
                    const Point tile{player_.pos.x + offsetX,
                                     player_.pos.y + offsetY};
                    if (!inside(tile.x, tile.y)) continue;
                    const ImVec2 tileMin(start.x + tile.x * cellSize,
                                         start.y + tile.y * cellSize);
                    const ImVec2 tileMax(tileMin.x + cellSize,
                                         tileMin.y + cellSize);
                    draw->AddRectFilled(
                        tileMin, tileMax,
                        ImColor(color.x, color.y, color.z,
                                domainConfirming_ ? 0.30f : 0.16f));
                    draw->AddRect(
                        tileMin, tileMax,
                        ImColor(color.x, color.y, color.z,
                                domainConfirming_ ? 0.90f : 0.42f),
                        0.0f, 0,
                        std::max(1.0f, cellSize *
                            (domainConfirming_ ? 0.065f : 0.025f)));
                }
            }
            draw->AddCircleFilled(center, radius,
                                  ImColor(color.x, color.y, color.z, 0.10f), 64);
            draw->AddCircle(center, radius,
                            ImColor(color.x, color.y, color.z, 0.88f), 64,
                            std::max(2.0f, cellSize * 0.10f));
            draw->AddCircle(center, radius - cellSize * 0.22f,
                            ImColor(color.x, color.y, color.z, 0.35f), 64,
                            std::max(1.0f, cellSize * 0.04f));
        }
        for (const Actor& enemy : enemies_) {
            if (!enemy.alive() || !enemy.hostileDomainActive ||
                !at(enemy.pos.x, enemy.pos.y).visible)
                continue;
            const ImVec4 color = clanColor(enemy.sorcererClan);
            for (int oy = -DomainExpansionRadius; oy <= DomainExpansionRadius; ++oy) {
                for (int ox = -DomainExpansionRadius; ox <= DomainExpansionRadius; ++ox) {
                    const Point tile{enemy.pos.x + ox, enemy.pos.y + oy};
                    if (!inside(tile.x, tile.y) || !at(tile.x, tile.y).explored) continue;
                    const ImVec2 a(start.x + tile.x * cellSize,
                                   start.y + tile.y * cellSize);
                    draw->AddRectFilled(a, ImVec2(a.x + cellSize, a.y + cellSize),
                                        ImColor(color.x, color.y, color.z, 0.12f));
                }
            }
        }
        if (simpleDomainReady_) {
            const ImVec2 center(
                start.x + (player_.pos.x + 0.5f) * cellSize,
                start.y + (player_.pos.y + 0.5f) * cellSize);
            const float pulse = 0.5f + 0.5f *
                std::sin(static_cast<float>(ImGui::GetTime()) * 5.0f);
            draw->AddCircleFilled(
                center, cellSize * (0.54f + pulse * 0.10f),
                IM_COL32(45, 145, 255, 42), 32);
            draw->AddCircle(
                center, cellSize * (0.58f + pulse * 0.12f),
                IM_COL32(80, 185, 255, 220), 32,
                std::max(2.0f, cellSize * 0.10f));
            draw->AddCircle(
                center, cellSize * (0.78f - pulse * 0.08f),
                IM_COL32(80, 120, 255, 115), 32,
                std::max(1.0f, cellSize * 0.05f));
        }
        if (neutralBarrierActive_) {
            const ImVec2 center(
                start.x + (player_.pos.x + 0.5f) * cellSize,
                start.y + (player_.pos.y + 0.5f) * cellSize);
            const float pulse = 0.5f + 0.5f *
                std::sin(static_cast<float>(ImGui::GetTime()) * 3.5f);
            draw->AddCircleFilled(
                center, cellSize * (0.72f + pulse * 0.08f),
                IM_COL32(55, 145, 255, 38), 40);
            draw->AddCircle(
                center, cellSize * (0.76f + pulse * 0.10f),
                IM_COL32(95, 205, 255, 235), 40,
                std::max(2.0f, cellSize * 0.11f));
            draw->AddCircle(
                center, cellSize * (0.96f - pulse * 0.06f),
                IM_COL32(145, 225, 255, 120), 40,
                std::max(1.0f, cellSize * 0.05f));
        }
        if (domainAmplificationActive_) {
            const ImVec2 center(
                start.x + (player_.pos.x + 0.5f) * cellSize,
                start.y + (player_.pos.y + 0.5f) * cellSize);
            const float pulse = 0.5f + 0.5f *
                std::sin(static_cast<float>(ImGui::GetTime()) * 4.2f);
            draw->AddCircleFilled(center, cellSize * (0.66f + pulse * 0.08f),
                                  IM_COL32(115, 200, 235, 34), 40);
            draw->AddCircle(center, cellSize * (0.72f + pulse * 0.10f),
                            IM_COL32(170, 235, 255, 230), 40,
                            std::max(2.0f, cellSize * 0.10f));
        }
        if (hollowWickerBasketActive_) {
            const ImVec2 center(
                start.x + (player_.pos.x + 0.5f) * cellSize,
                start.y + (player_.pos.y + 0.5f) * cellSize);
            const float pulse = 0.5f + 0.5f *
                std::sin(static_cast<float>(ImGui::GetTime()) * 3.2f);
            const float halfExtent = cellSize * (0.92f + pulse * 0.04f);
            const ImVec2 gridMin(center.x - halfExtent, center.y - halfExtent);
            const ImVec2 gridMax(center.x + halfExtent, center.y + halfExtent);
            draw->AddRectFilled(gridMin, gridMax,
                                IM_COL32(255, 45, 170, 28));
            const float thickness = std::max(1.5f, cellSize * 0.055f);
            constexpr int divisions = 4;
            for (int line = 0; line <= divisions; ++line) {
                const float t = static_cast<float>(line) / divisions;
                const float x = gridMin.x + (gridMax.x - gridMin.x) * t;
                const float y = gridMin.y + (gridMax.y - gridMin.y) * t;
                draw->AddLine(ImVec2(x, gridMin.y), ImVec2(x, gridMax.y),
                              IM_COL32(255, 80, 190, 215), thickness);
                draw->AddLine(ImVec2(gridMin.x, y), ImVec2(gridMax.x, y),
                              IM_COL32(255, 80, 190, 215), thickness);
            }
            draw->AddRect(gridMin, gridMax, IM_COL32(255, 150, 220, 245),
                          0.0f, 0, std::max(2.0f, cellSize * 0.09f));
        }
        if (restrainedTurns_ > 0) {
            const ImVec2 a(start.x + player_.pos.x * cellSize,
                           start.y + player_.pos.y * cellSize);
            const ImVec2 b(a.x + cellSize, a.y + cellSize);
            const ImVec2 center((a.x + b.x) * 0.5f, (a.y + b.y) * 0.5f);
            draw->AddRectFilled(a, b, IM_COL32(255, 45, 175, 35));
            for (int spoke = 0; spoke < 8; ++spoke) {
                const float angle = static_cast<float>(spoke) * 3.14159265f / 4.0f;
                const ImVec2 edge(center.x + std::cos(angle) * cellSize * 0.48f,
                                  center.y + std::sin(angle) * cellSize * 0.48f);
                draw->AddLine(center, edge, IM_COL32(255, 90, 205, 230),
                              std::max(1.0f, cellSize * 0.045f));
            }
            draw->AddCircle(center, cellSize * 0.27f,
                            IM_COL32(255, 155, 225, 220), 16,
                            std::max(1.0f, cellSize * 0.04f));
        }
        if (mapUnitSpriteTexture != 0) {
            mapUnitSprite(player_.pos, selectedClan_);
        } else if (playerSpriteTexture != 0) {
            const ImVec2 a(start.x + player_.pos.x * cellSize,
                           start.y + player_.pos.y * cellSize);
            const ImVec2 b(a.x + cellSize, a.y + cellSize);
            const float u0 = static_cast<float>(selectedClan_ % 2) * 0.5f;
            const float v0 = static_cast<float>(selectedClan_ / 2) * 0.5f;
            draw->AddImage(static_cast<ImTextureID>(playerSpriteTexture), a, b,
                           ImVec2(u0, v0), ImVec2(u0 + 0.5f, v0 + 0.5f));
        } else {
            glyph(player_.pos, player_.glyph, IM_COL32(255, 225, 90, 255));
        }
        updateAndDrawParticles(draw, start, cellSize);
        auto drawSelector = [&](Point tile, ImU32 color, float thickness) {
            const ImVec2 a(start.x + tile.x * cellSize, start.y + tile.y * cellSize);
            const ImVec2 b(a.x + cellSize, a.y + cellSize);
            draw->AddRect(a, b, color, 0.0f, 0, thickness);
        };
        if (hasHoveredTile)
            drawSelector(hoveredTile, IM_COL32(115, 205, 255, 150),
                         std::max(1.0f, cellSize * 0.05f));
        if (hasInspection_)
            drawSelector(inspectedTile_, IM_COL32(80, 235, 255, 255),
                         std::max(2.0f, cellSize * 0.10f));
        ImGui::Dummy(ImVec2(MapWidth * cellSize, MapHeight * cellSize));

        if (centerCamera_) {
            const float playerCenterX = (static_cast<float>(player_.pos.x) + 0.5f) * cellSize;
            const float playerCenterY = (static_cast<float>(player_.pos.y) + 0.5f) * cellSize;
            ImGui::SetScrollX(playerCenterX - available.x * 0.5f);
            ImGui::SetScrollY(playerCenterY - available.y * 0.5f);
            centerCamera_ = false;
        }
    }

    void drawInspectionPanel() {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(
            ImVec2(viewport->WorkPos.x + 20.0f, viewport->WorkPos.y + 50.0f),
            ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(442.0f, 432.0f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowBgAlpha(0.50f);
        constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;
        if (!beginObsidianWindow("Inspection", &hasInspection_, flags, 0.50f)) {
            ImGui::End();
            return;
        }

        ImGui::Text("Tile: %d, %d", inspectedTile_.x, inspectedTile_.y);
        const Cell& cell = at(inspectedTile_.x, inspectedTile_.y);
        if (!cell.explored) {
            ImGui::TextDisabled("Unexplored");
            ImGui::TextDisabled("Right-click the map to clear.");
            ImGui::End();
            return;
        }

        ImGui::Text("Terrain: %s", cell.tile == Tile::Wall ? "Wall" : "Floor");
        if (inspectedTile_ == upStairs_) ImGui::Text("Feature: Up stairs <");
        if (inspectedTile_ == downStairs_) {
            const std::string& gateName = levels_[currentLevel_].bossGateName;
            if (!gateName.empty())
                ImGui::TextWrapped("Feature: %s (%s)", gateName.c_str(),
                                   bossExitLocked() ? "Locked" : "Unlocked");
            else
                ImGui::Text("Feature: Down stairs >");
        }
        if (inspectedTile_ == pachinkoMachine_)
            ImGui::Text("Feature: Cursed Pachinko Machine");
        if (inspectedTile_ == cursedToolShop_)
            ImGui::Text("Feature: Cursed Tool Shop");
        if (inspectedTile_ == gateOfHeaven_)
            ImGui::Text("Feature: Gate of Heaven (%d / %d keys)",
                        levels_[currentLevel_].bossKeysCollected,
                        levels_[currentLevel_].bossKeysRequired);
        if (std::find(cursedKeys_.begin(), cursedKeys_.end(), inspectedTile_) !=
            cursedKeys_.end())
            ImGui::TextColored(ImVec4(0.72f, 0.38f, 0.95f, 1.0f),
                               "Item: Cursed Key");

        const Actor* unit = nullptr;
        if (cell.visible) {
            if (player_.pos == inspectedTile_) unit = &player_;
            for (const Actor& enemy : enemies_)
                if (enemy.alive() && enemy.pos == inspectedTile_) unit = &enemy;
            for (const Actor& ally : allies_)
                if (ally.alive() && ally.pos == inspectedTile_) unit = &ally;
        }

        if (unit) {
            ImGui::Text("Unit: %s", unit->name.c_str());
            if (unit != &player_ &&
                (unit->classification == ActorClassification::Human ||
                 unit->classification == ActorClassification::CursedSpirit)) {
                const bool centipede =
                    unit->centipedeCurse || unit->centipedeHost;
                const bool spider = unit->spiderGhoul || unit->spiderHost;
                const bool samurai = unit->samurai;
                const bool clanBoss = unit->bossSorcerer &&
                                      unit->sorcererClan >= 0;
                const GLuint portraitTexture = clanBoss
                    ? playerSpriteTexture
                    : samurai ? samuraiSpritesTexture
                    : spider ? spiderGhoulSpritesTexture
                    : centipede ? centipedeSpriteTexture : enemySpriteTexture;
                const int spriteIndex = clanBoss
                    ? unit->sorcererClan
                    : samurai ? (unit->samuraiHost ? 1 : 0)
                    : spider ? (unit->spiderHost ? 1 : 0)
                    : centipede ? (unit->centipedeHost ? 1 : 0)
                                : enemySpriteIndex(*unit);
                const float atlasCell = clanBoss || centipede || spider || samurai
                    ? 0.5f : 1.0f / 3.0f;
                const float u0 = clanBoss
                    ? static_cast<float>(spriteIndex % 2) * atlasCell
                    : static_cast<float>((centipede || spider || samurai) ? spriteIndex
                                                    : spriteIndex % 3) * atlasCell;
                const float v0 = clanBoss
                    ? static_cast<float>(spriteIndex / 2) * atlasCell
                    : (spider || samurai) ? 0.5f
                    : static_cast<float>(centipede ? 0 : spriteIndex / 3) * atlasCell;
                const float portraitSize =
                    std::min(280.0f, ImGui::GetContentRegionAvail().x);
                if (portraitTexture != 0) {
                    ImGui::SetCursorPosX(
                        ImGui::GetCursorPosX() +
                        std::max(0.0f,
                                 (ImGui::GetContentRegionAvail().x -
                                  portraitSize) * 0.5f));
                    ImGui::Image(
                        static_cast<ImTextureID>(portraitTexture),
                        ImVec2(portraitSize, portraitSize),
                        ImVec2(u0, v0),
                        ImVec2(u0 + atlasCell, v0 + atlasCell));
                }
            }
            if (unit != &player_ && isAlly(*unit) &&
                (gyokukenInspectionSpriteTexture != 0 ||
                 gyokukenTotalityInspectionTexture != 0 ||
                 emberInsectSpritesTexture != 0 || nueInspectionTexture != 0)) {
                const float portraitSize =
                    std::min(280.0f, ImGui::GetContentRegionAvail().x);
                ImGui::SetCursorPosX(
                    ImGui::GetCursorPosX() +
                    std::max(0.0f,
                             (ImGui::GetContentRegionAvail().x - portraitSize) *
                                 0.5f));
                if (unit->emberInsect && emberInsectSpritesTexture != 0) {
                    ImGui::Image(
                        static_cast<ImTextureID>(emberInsectSpritesTexture),
                        ImVec2(portraitSize, portraitSize), ImVec2(0.5f, 0.0f),
                        ImVec2(1.0f, 1.0f));
                } else if (unit->nue && nueInspectionTexture != 0) {
                    const float nueWidth = portraitSize * (2.0f / 3.0f);
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() +
                                         (portraitSize - nueWidth) * 0.5f);
                    ImGui::Image(static_cast<ImTextureID>(nueInspectionTexture),
                                 ImVec2(nueWidth, portraitSize));
                } else if (unit->gyokukenTotality &&
                    gyokukenTotalityInspectionTexture != 0) {
                    ImGui::Image(
                        static_cast<ImTextureID>(
                            gyokukenTotalityInspectionTexture),
                        ImVec2(portraitSize, portraitSize));
                } else if (gyokukenInspectionSpriteTexture != 0) {
                    const float u0 =
                        unit->phantomWhiteGyokuken ? 0.5f : 0.0f;
                    ImGui::Image(
                        static_cast<ImTextureID>(
                            gyokukenInspectionSpriteTexture),
                        ImVec2(portraitSize, portraitSize), ImVec2(u0, 0.0f),
                        ImVec2(u0 + 0.5f, 1.0f));
                }
            }
            if (unit != &player_)
                ImGui::TextColored(healthColor(), "Health: %d / %d",
                                   std::max(0, unit->hp), unit->maxHp);
            if (unit->maxCursedEnergy > 0)
                ImGui::TextColored(energyColor(), "Cursed Energy: %d / %d",
                                   unit->cursedEnergy,
                                   unit->maxCursedEnergy);
            if (unit == &player_) {
                ImGui::Text("Classification: %s",
                            isJogoClan() ? "Cursed Spirit"
                                         : "Human / Sorcerer");
                if (playerSpriteTexture != 0) {
                    const float portraitSize =
                        std::min(280.0f, ImGui::GetContentRegionAvail().x);
                    const float u0 =
                        static_cast<float>(selectedClan_ % 2) * 0.5f;
                    const float v0 =
                        static_cast<float>(selectedClan_ / 2) * 0.5f;
                    ImGui::SetCursorPosX(
                        ImGui::GetCursorPosX() +
                        std::max(0.0f,
                                 (ImGui::GetContentRegionAvail().x -
                                  portraitSize) * 0.5f));
                    ImGui::Image(
                        static_cast<ImTextureID>(playerSpriteTexture),
                        ImVec2(portraitSize, portraitSize),
                        ImVec2(u0, v0), ImVec2(u0 + 0.5f, v0 + 0.5f));
                }
                ImGui::TextColored(playerGradeColor(playerGrade()), "Rank: %s",
                                   playerGradeName(playerGrade()));
                ImGui::TextColored(healthColor(), "Health: %d / %d",
                                   std::max(0, player_.hp), player_.maxHp);
                ImGui::TextColored(energyColor(), "Cursed Energy: %d / %d",
                                   cursedEnergy_, effectiveMaxCursedEnergy());
                ImGui::TextColored(efficiencyColor(), "Efficiency: %d%%",
                                   effectiveCursedEnergyEfficiency());
                ImGui::TextColored(generationColor(), "CE Generation: %d / turn",
                                   cursedEnergyGeneration_);
                ImGui::TextColored(ImVec4(0.52f, 0.70f, 1.0f, 1.0f),
                                   "Cursed Technique Power: +%d damage",
                                   cursedTechniquePowerBonus_);
                const ImVec4 attackColor = playerMeleeCanAffectCurses()
                    ? ImVec4(0.85f, 0.45f, 1.0f, 1.0f)
                    : Game::attackColor();
                ImGui::TextColored(attackColor, "Attack: %d",
                                   displayedPlayerAttack());
                ImGui::TextColored(ImVec4(0.92f, 0.68f, 0.28f, 1.0f),
                                   "Close Combat Ability: %d melee attack%s",
                                   player_.closeCombatAbility,
                                   player_.closeCombatAbility == 1 ? "" : "s");
                ImGui::TextDisabled("Base %d + weapon %d + gear %d + reinforcement %d",
                                    player_.attack, katanaBaseAttackBonus(),
                                    equipmentAttackBonus(),
                                    availableAttackReinforcement());
            } else if (unit->bossSorcerer) {
                ImGui::TextColored(clanColor(unit->sorcererClan),
                                   unit->sorcererClan == 2
                                       ? "Classification: Special Grade Curse"
                                       : "Classification: Human / Boss Sorcerer");
                ImGui::TextColored(clanColor(unit->sorcererClan),
                                   "Boss Clan: %s",
                                   ClanNames[unit->sorcererClan]);
                ImGui::TextColored(energyColor(), "Cursed Technique: %s",
                                   ClanTechniques[unit->sorcererClan]);
                ImGui::TextColored(attackColor(), "Attack: %d base",
                                   unit->attack);
                ImGui::TextColored(
                    ImVec4(0.92f, 0.68f, 0.28f, 1.0f),
                    "Close Combat Ability: %d melee attacks per turn",
                    unit->closeCombatAbility);
                ImGui::TextColored(ImVec4(0.72f, 0.38f, 0.95f, 1.0f),
                                   "Drops: Cursed Key");
                if (!unit->inventory.empty()) {
                    ImGui::SeparatorText("Inventory & Equipment");
                    for (std::size_t itemIndex = 0;
                         itemIndex < unit->inventory.size(); ++itemIndex) {
                        const Item& item = unit->inventory[itemIndex];
                        const bool equipped =
                            unit->equippedItems[static_cast<std::size_t>(item.slot)] ==
                            static_cast<int>(itemIndex);
                        ImGui::Text("%s: %s%s", equipmentSlotName(item.slot),
                                    item.name.c_str(),
                                    equipped ? " (Equipped)" : "");
                    }
                }
            } else if (unit->classification == ActorClassification::Human) {
                ImGui::Text("Classification: Human / %s",
                            unit->sorcerer ? "Sorcerer" : "Non-sorcerer");
                ImGui::TextColored(generationColor(),
                                   "CE Generation: %d / turn",
                                   unit->cursedEnergyGeneration);
                if (unit->bossSorcerer) {
                    ImGui::Text("Boss Grade: %s",
                                bossGradeName(unit->bossPowerLevel));
                    ImGui::TextColored(clanColor(unit->sorcererClan),
                                       "Boss Clan: %s",
                                       ClanNames[unit->sorcererClan]);
                    ImGui::TextColored(
                        energyColor(), "Cursed Technique: %s",
                        ClanTechniques[unit->sorcererClan]);
                    ImGui::TextColored(attackColor(),
                                       "Technique Damage: %d",
                                       bossTechniqueDamage(*unit));
                    ImGui::TextColored(attackColor(),
                                       "Attack: %d base / %d with Katana",
                                       unit->attack,
                                       (unit->attack * 3 + 1) / 2);
                    ImGui::TextColored(
                        ImVec4(0.92f, 0.68f, 0.28f, 1.0f),
                        "Close Combat Ability: %d melee attacks per turn",
                        unit->closeCombatAbility);
                    ImGui::TextWrapped(
                        "Cursed Energy Reinforcement can add or block up to %d "
                        "damage per strike for 5 CE per point.",
                        unit->bossPowerLevel);
                    ImGui::SeparatorText("Inventory & Equipment");
                    for (std::size_t itemIndex = 0;
                         itemIndex < unit->inventory.size(); ++itemIndex) {
                        const Item& item = unit->inventory[itemIndex];
                        const bool equipped =
                            unit->equippedItems[static_cast<std::size_t>(item.slot)] ==
                            static_cast<int>(itemIndex);
                        ImGui::Text("%s: %s%s", equipmentSlotName(item.slot),
                                    item.name.c_str(),
                                    equipped ? " (Equipped)" : "");
                        ImGui::TextDisabled("%s", item.description.c_str());
                    }
                    ImGui::TextWrapped(
                        "The uniform reduces direct damage by 25%%, the Katana "
                        "doubles base Attack, and the Cursed Tanto makes melee "
                        "damage cursed. This sorcerer cannot become a Cursed Host.");
                } else if (unit->cursedHost) {
                    ImGui::TextColored(ImVec4(0.92f, 0.38f, 0.96f, 1.0f),
                                       "Status: Cursed Host");
                    if (unit->spiderHost) {
                        ImGui::TextColored(
                            attackColor(),
                            unit->samurai
                                ? "Attack: Iaijutsu Strike (15) / Gossamer Snare (5)"
                                : "Attack: Katana Slash (5) / Gossamer Snare (5)");
                        ImGui::TextColored(
                            energyColor(),
                            "Embedded Technique: Gossamer Snare");
                        ImGui::TextWrapped(
                            "Gossamer Snare costs 20 CE, has range 5, and "
                            "covers a 2-tile radius around the player. It "
                            "Restrains the player for 3 turns. Uses melee "
                            "while the player is Restrained.");
                    } else if (unit->centipedeHost) {
                        ImGui::TextColored(
                            attackColor(),
                            "Attack: Bite (7) + Stinger (3, Poison)");
                        ImGui::TextColored(
                            energyColor(),
                            "Embedded Techniques: Bite and Stinger");
                    } else {
                        ImGui::TextColored(
                            attackColor(),
                            unit->samurai
                                ? "Attack: Iaijutsu Strike (15) / %s"
                                : "Attack: Katana Slash (5) / %s",
                            spiritAttackName(unit->embeddedTechnique));
                        ImGui::TextColored(energyColor(),
                                           "Embedded Technique: %s",
                                           spiritAttackName(
                                               unit->embeddedTechnique));
                    }
                    if (!unit->spiderHost) ImGui::TextWrapped(
                        unit->centipedeHost
                            ? "Uses both embedded techniques for 20 CE. "
                              "Reinforcement adds 1 attack damage or blocks 1 "
                              "damage for 5 CE."
                            : "Uses the embedded technique for 10 CE. "
                              "Reinforcement adds 1 attack damage or blocks 1 "
                              "damage for 5 CE.");
                    ImGui::TextWrapped(
                        "Below 50%% HP, it heals 3 HP per 1 CE spent.");
                } else {
                    if (unit->samurai) {
                        ImGui::TextColored(attackColor(),
                                           "Attack: Iaijutsu Strike (15)");
                        ImGui::TextWrapped(
                            "An elite non-sorcerer human with 50 HP. Only a "
                            "Grade 3 or stronger Cursed Spirit can turn this "
                            "Samurai into a Cursed Host.");
                    } else {
                        ImGui::TextColored(attackColor(),
                                           "Attack: Katana Slash (5)");
                    }
                }
            } else if (unit->classification == ActorClassification::CursedSpirit) {
                ImGui::Text("Classification: %s Cursed Spirit",
                            curseGradeName(unit->curseGrade));
                ImGui::TextColored(generationColor(),
                                   "CE Generation: %d / turn",
                                   unit->cursedEnergyGeneration);
                if (unit->bossSorcerer) {
                    ImGui::TextColored(clanColor(unit->sorcererClan),
                                       "Boss Clan: %s",
                                       ClanNames[unit->sorcererClan]);
                    ImGui::TextColored(attackColor(),
                                       "Technique Damage: %d",
                                       bossTechniqueDamage(*unit));
                    ImGui::TextColored(attackColor(), "Melee Damage: %d base",
                                       unit->attack);
                    ImGui::TextColored(
                        ImVec4(0.92f, 0.68f, 0.28f, 1.0f),
                        "Close Combat Ability: %d melee attacks per turn",
                        unit->closeCombatAbility);
                    ImGui::TextWrapped(
                        "Cursed Energy Reinforcement can add or block up to %d "
                        "damage per strike for 5 CE per point.",
                        unit->bossPowerLevel);
                } else if (unit->spiderGhoul) {
                    ImGui::TextColored(energyColor(),
                                       "Technique: Gossamer Snare");
                    ImGui::TextColored(attackColor(),
                                       "Melee Damage: 15 base");
                    ImGui::TextWrapped(
                        "Gossamer Snare deals 5 cursed damage in a 2-tile "
                        "radius, costs 20 CE, has range 5, and Restrains the "
                        "player for 3 turns. Awareness range: 10.");
                    ImGui::TextWrapped(
                        "Uses melee while its target is Restrained, heals "
                        "below 50%% HP, uses Cursed Energy Reinforcement, and "
                        "can turn a Ronin into a Spider Cursed Host.");
                } else if (unit->centipedeCurse)
                    ImGui::TextColored(
                        attackColor(),
                        "Techniques: Bite (7) + Stinger (3, Poison) each turn");
                else
                    ImGui::TextColored(
                        attackColor(), "Attack: %s",
                        spiritAttackName(unit->spiritAttack));
            } else {
                ImGui::Text("Classification: Shikigami");
                if (unit->emberInsect) {
                    ImGui::TextColored(clanColor(2), "Form: Ember Insect");
                    ImGui::TextColored(
                        attackColor(), "Explosion: %d damage | radius 3",
                        customizedTechniquePower(VolcanicBlastDamage));
                    ImGui::TextWrapped(
                        "Seeks the nearest enemy and explodes on contact. "
                        "The summon has no Cursed Energy upkeep.");
                } else if (unit->nue) {
                    ImGui::TextColored(clanColor(1), "Form: Nue");
                    ImGui::TextColored(
                        attackColor(), "Explosion: %d damage | radius %d",
                        (customizedTechniquePower(VolcanicBlastDamage) + 1) / 2,
                        customizedTechniqueRadius(NueExplosionRadius));
                    ImGui::TextWrapped(
                        "Seeks the nearest enemy, explodes on contact, and "
                        "stuns survivors for 3 turns. The summon has no "
                        "Cursed Energy upkeep.");
                } else {
                    ImGui::TextColored(
                        ImVec4(0.72f, 0.52f, 1.0f, 1.0f), "Form: %s Gyokuken",
                        unit->gyokukenTotality
                            ? "Totality"
                            : unit->phantomWhiteGyokuken ? "Phantom White" : "Black");
                    ImGui::TextColored(attackColor(), "Attack: %d", unit->attack);
                    ImGui::TextColored(ImVec4(0.78f, 0.48f, 1.0f, 1.0f),
                                       "Damage type: Cursed Technique");
                }
            }
            if (unit->knowledge)
                ImGui::TextColored(ImVec4(1.0f, 0.80f, 0.22f, 1.0f),
                                   "Status: Knowledge (+1 damage)");
            if (unit->ratioStacks > 0)
                ImGui::TextColored(ImVec4(0.42f, 0.95f, 0.48f, 1.0f),
                                   "Status: Ratio (%d stack%s)",
                                   unit->ratioStacks,
                                   unit->ratioStacks == 1 ? "" : "s");
            if (unit->stunnedTurns > 0)
                ImGui::TextColored(ImVec4(0.72f, 0.38f, 1.0f, 1.0f),
                                   "Status: Stunned (%d turn%s)",
                                   unit->stunnedTurns,
                                   unit->stunnedTurns == 1 ? "" : "s");
        } else {
            const bool corpse = std::find(
                corpses_.begin(), corpses_.end(), inspectedTile_) != corpses_.end();
            ImGui::Text("Occupant: %s", corpse ? "Ronin corpse %" : "None");
        }
        for (const GroundItem& groundItem : groundItems_) {
            if (!(groundItem.pos == inspectedTile_)) continue;
            ImGui::SeparatorText("Ground Item");
            ImGui::Text("%s", groundItem.item.name.c_str());
            ImGui::TextDisabled("%s", equipmentSlotName(groundItem.item.slot));
            ImGui::TextWrapped("%s", groundItem.item.description.c_str());
            ImGui::TextDisabled("Step onto this tile to pick it up.");
        }
        ImGui::TextDisabled("Right-click the map to clear.");
        ImGui::End();
    }

    void drawSidebar() {
        // Apply the sidebar's available width to every normal, disabled, and
        // colored text call. Individual TextWrapped calls continue to work,
        // while long dynamic labels no longer extend beyond the panel.
        ImGui::PushTextWrapPos(0.0f);
        ImGui::TextUnformatted("CURSED RL");
        ImGui::TextWrapped("Tower of Malevolent Divinity");
        ImGui::Separator();

        ImGui::TextColored(healthColor(), "HP  %d / %d",
                           std::max(0, player_.hp), player_.maxHp);
        const float fraction = std::clamp(static_cast<float>(player_.hp) / player_.maxHp, 0.0f, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, IM_COL32(190, 55, 55, 255));
        ImGui::ProgressBar(fraction, ImVec2(-1, 0));
        ImGui::PopStyleColor();

        ImGui::TextColored(energyColor(), "Cursed Energy  %d / %d",
                           cursedEnergy_, effectiveMaxCursedEnergy());
        const float energyFraction =
            std::clamp(static_cast<float>(cursedEnergy_) /
                           effectiveMaxCursedEnergy(),
                       0.0f, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, IM_COL32(125, 70, 210, 255));
        ImGui::ProgressBar(energyFraction, ImVec2(-1, 0));
        ImGui::PopStyleColor();

        ImGui::SeparatorText("Status Effects");
        if (poisonedTurns_ > 0)
            ImGui::TextColored(ImVec4(0.45f, 0.92f, 0.30f, 1.0f),
                               "Poisoned: %d turns", poisonedTurns_);
        if (restrainedTurns_ > 0)
            ImGui::TextColored(ImVec4(1.0f, 0.32f, 0.78f, 1.0f),
                               "Restrained: %d turns (movement disabled)",
                               restrainedTurns_);
        if (simpleDomainReady_)
            ImGui::TextColored(ImVec4(0.45f, 0.72f, 1.0f, 1.0f),
                               "Simple Domain armed");
        if (neutralBarrierActive_)
            ImGui::TextColored(clanColor(selectedClan_),
                               "Neutral Limitless active");
        if (domainAmplificationActive_)
            ImGui::TextColored(ImVec4(0.58f, 0.86f, 1.0f, 1.0f),
                               "Domain Amplification active");
        if (hollowWickerBasketActive_)
            ImGui::TextColored(ImVec4(1.0f, 0.32f, 0.72f, 1.0f),
                               "Hollow Wicker Basket active");
        if (techniqueBurnoutTurns_ > 0)
            ImGui::TextColored(ImVec4(0.86f, 0.32f, 0.22f, 1.0f),
                               "Technique Burnout: %d turn%s",
                               techniqueBurnoutTurns_,
                               techniqueBurnoutTurns_ == 1 ? "" : "s");
        if (sorceryHighTurns_ > 0)
            ImGui::TextColored(
                ImVec4(0.86f, 0.18f, 1.0f, 1.0f),
                "Sorcery High: %d turn%s | 50%% Black Flash | +25%% Efficiency",
                sorceryHighTurns_, sorceryHighTurns_ == 1 ? "" : "s");
        else if (blackFlashAwakened_) {
            ImGui::TextColored(efficiencyColor(),
                               "Black Flash awakened: permanent +5%% Efficiency");
            if (player_.hp * 2 < player_.maxHp)
                ImGui::TextColored(ImVec4(1.0f, 0.10f, 0.12f, 1.0f),
                                   "Black Flash chance: 2%% per melee attack");
        }
        if (poisonedTurns_ <= 0 && restrainedTurns_ <= 0 &&
            !simpleDomainReady_ &&
            !neutralBarrierActive_ && !domainAmplificationActive_ &&
            !hollowWickerBasketActive_ &&
            techniqueBurnoutTurns_ <= 0 && !blackFlashAwakened_)
            ImGui::TextDisabled("None");

        ImGui::SeparatorText("Cursed Technique");
        const char* technique =
            noInnateTechniqueSelected_ ? "None" :
            selectedClan_ == 0 ? "Limitless" :
            selectedClan_ == 1 ? "Shadow Master" :
            selectedClan_ == 2 ? "Disaster Flames" :
            selectedClan_ == 3 ? "Ratio" : "Unawakened";
        ImGui::TextColored(clanColor(selectedClan_), "Technique  %s", technique);
        ImGui::TextColored(clanColor(selectedClan_),
                           "Cursed Technique Power  +%d damage",
                           cursedTechniquePowerBonus_);
        ImGui::TextColored(clanColor(selectedClan_),
                           "Total Cursed Technique Damage  %d",
                           displayedTotalCursedTechniqueDamage());
        if (selectedClan_ == 3 && !noInnateTechniqueSelected_)
            ImGui::TextDisabled(
                "Ratio total uses the current marked-melee estimate; the final hit may vary slightly.");

        ImGui::SeparatorText("Cursed Technique Controls");
        ImGui::PushStyleColor(ImGuiCol_Text, clanColor(selectedClan_));
        if (noInnateTechniqueSelected_) {
            ImGui::TextDisabled(
                "No Innate Technique: clan abilities are unavailable.");
            ImGui::TextDisabled(
                "Cursed Energy Reinforcement can still empower attacks and defense.");
        } else if (selectedClan_ == 0) {
            ImGui::TextWrapped("[1] Lapse: Limitless");
            ImGui::TextDisabled("%d energy | radius %d | %d damage",
                                areaTechniqueEnergyCost(IndividualAttractionCost),
                                customizedTechniqueRadius(1),
                                customizedTechniquePower(10));
            if (neutralBarrierLearned_) {
                ImGui::TextWrapped(
                    "[2] Cursed Technique Neutral: Barrier Technique");
                ImGui::TextDisabled("Toggle | %d energy/turn | blocks all attacks",
                                    cursedEnergyCost(
                                        NeutralBarrierUpkeepCost));
                if (neutralBarrierActive_)
                    ImGui::TextColored(clanColor(selectedClan_), "BARRIER ACTIVE");
            }
            if (reversalRedLearned_) {
                ImGui::TextWrapped("[3] Cursed Technique Reversal: Red");
                ImGui::TextDisabled(
                    "%d energy | line | %d damage | repel 3 spaces",
                    techniqueEnergyCost(ReversalRedCost),
                    (customizedTechniquePower(ReversalRedBaseDamage) * 3 + 1) / 2);
            }
            if (targeting_) {
                ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.85f, 1.0f), "TARGETING");
                ImGui::TextWrapped("Arrows / numpad / Tab / mouse: select\n1 / Space / numpad 5: confirm\nEscape: cancel");
            }
            if (reversalRedTargeting_) {
                ImGui::TextColored(ImVec4(1.0f, 0.16f, 0.22f, 1.0f),
                                   "AIMING REVERSAL: RED");
                ImGui::TextWrapped(
                    "Arrows / numpad / mouse: aim\n"
                    "3 / Space / numpad 5: confirm\nEscape: cancel");
            }
        } else if (selectedClan_ == 1) {
            ImGui::TextWrapped("[1] Lapse: Shadow Master");
            if (totalityActive_)
                ImGui::TextDisabled(
                    "%d energy/turn | Totality | 30 HP | %d damage | maximum 1",
                    techniqueEnergyCost(TotalityUpkeepCost),
                    customizedTechniquePower(10));
            else
                ImGui::TextDisabled(
                    "%d energy/turn each | 15 HP | %d damage | maximum 2",
                    techniqueEnergyCost(GyokukenUpkeepCost),
                    customizedTechniquePower(5));
            ImGui::TextWrapped("[2] Unsummon Shikigami");
            ImGui::TextDisabled("Dismiss all active Shikigami | no energy cost");
            if (nueLearned_) {
                ImGui::TextWrapped("[3] Cursed Technique Extension: Summon Nue");
                ImGui::TextDisabled(
                    "%d energy | no upkeep | maximum 1 | radius %d | %d damage | stun 3 turns",
                    areaTechniqueEnergyCost(NueCost),
                    customizedTechniqueRadius(NueExplosionRadius),
                    (customizedTechniquePower(VolcanicBlastDamage) + 1) / 2);
            }
        } else if (selectedClan_ == 2) {
            ImGui::TextWrapped("[1] Lapse: Disaster Flames");
            ImGui::TextDisabled("%d energy | %d-tile-wide line | %d damage",
                                areaTechniqueEnergyCost(VolcanicBlastCost),
                                techniqueAreaExpanded_ ? 3 : 1,
                                customizedTechniquePower(VolcanicBlastDamage));
            if (emberInsectLearned_) {
                ImGui::TextWrapped(
                    "[2] Cursed Technique Extension: Summon Ember Insect");
                ImGui::TextDisabled(
                    "%d energy | no upkeep | radius %d | %d explosion damage",
                    areaTechniqueEnergyCost(EmberInsectCost),
                    customizedTechniqueRadius(EmberInsectExplosionRadius),
                    customizedTechniquePower(VolcanicBlastDamage));
            }
            if (volcanicTargeting_) {
                ImGui::TextColored(ImVec4(1.0f, 0.38f, 0.08f, 1.0f), "AIMING");
                ImGui::TextWrapped("Arrows / numpad / mouse: aim\n1 / Space / numpad 5: confirm\nEscape: cancel");
            }
        } else if (selectedClan_ == 3) {
            ImGui::TextWrapped("[1] Lapse: Ratio");
            ImGui::TextDisabled("%d energy | marks every visible enemy",
                                techniqueEnergyCost(RatioCriticalCost));
            ImGui::TextDisabled(
                "Each stack guarantees one %.2fx critical against its target.",
                ratioCriticalMultiplier());
            if (collapseLearned_) {
                ImGui::TextWrapped("[2] Cursed Technique Extension: Collapse");
                ImGui::TextDisabled(
                    "%d energy | %dx5 area | +1 Ratio stack | one melee attack per enemy",
                    areaTechniqueEnergyCost(CollapseCost),
                    techniqueAreaExpanded_ ? 3 : 1);
            }
            if (collapseTargeting_) {
                ImGui::TextColored(clanColor(selectedClan_), "AIMING COLLAPSE");
                ImGui::TextWrapped(
                    "Arrows / numpad / mouse: aim\n"
                    "2 / Space / numpad 5: confirm\nEscape: cancel");
            }
        } else {
            ImGui::TextDisabled("No abilities unlocked.");
        }
        ImGui::PopStyleColor();
        ImGui::TextColored(clanColor(selectedClan_),
                           "[T] Cursed Technique Customization");

        ImGui::SeparatorText("Learned Abilities");
        if (simpleDomainLearned_) {
            ImGui::TextWrapped("[X] Simple Domain");
            ImGui::TextDisabled(
                "%d energy | blocks next attack | counters for %d",
                cursedEnergyCost(SimpleDomainCost),
                std::max(1, player_.attack * 3 / 2));
        }
        if (reversalHealingLearned_) {
            ImGui::TextWrapped("[H] %s", healingAbilityName());
            ImGui::SetNextItemWidth(92.0f);
            if (ImGui::InputInt("Base CE", &reversalHealingBaseSpend_, 10, 20))
                reversalHealingBaseSpend_ =
                    std::clamp((reversalHealingBaseSpend_ / 10) * 10, 10, 200);
            const int healing = (reversalHealingBaseSpend_ / 10) * 5;
            ImGui::TextDisabled("Heals %d | costs %d energy", healing,
                                cursedEnergyCost(reversalHealingBaseSpend_));
        }
        if (!simpleDomainLearned_ && !reversalHealingLearned_ &&
            !domainAmplificationLearned_ && !hollowWickerBasketLearned_)
            ImGui::TextDisabled("None learned");

        if (domainAmplificationLearned_) {
            ImGui::TextWrapped("[A] Domain Amplification");
            ImGui::TextDisabled(domainAmplificationActive_
                                    ? "ACTIVE | %d CE/turn | domains nullified | melee only"
                                    : "Ready | %d CE/turn | anti-domain toggle | melee only",
                                cursedEnergyCost(DomainAmplificationUpkeepCost));
        }
        if (hollowWickerBasketLearned_) {
            ImGui::TextWrapped("[V] Hollow Wicker Basket");
            ImGui::TextDisabled(hollowWickerBasketActive_
                                    ? "ACTIVE | %d CE/turn | domain damage blocked | wait only"
                                    : "Ready | %d CE/turn | usable during burnout",
                                cursedEnergyCost(HollowWickerBasketUpkeepCost));
        }

        ImGui::SeparatorText("Cursed Energy Reinforcement");
        ImGui::SetNextItemWidth(92.0f);
        if (ImGui::InputInt("Attack boost", &reinforcementAttack_, 1, 5))
            reinforcementAttack_ = std::clamp(reinforcementAttack_, 0, 20);
        ImGui::SetNextItemWidth(92.0f);
        if (ImGui::InputInt("Defense", &reinforcementDefense_, 1, 5))
            reinforcementDefense_ = std::clamp(reinforcementDefense_, 0, 20);
        ImGui::TextDisabled("Each effective point costs %d energy.",
                            cursedEnergyCost(5));

        const ImVec4 gold(1.0f, 0.78f, 0.20f, 1.0f);
        ImGui::SeparatorText("Binding Vows");
        if (divulgeVowActive_)
            ImGui::TextColored(gold, "Divulged (%d)",
                               knowledgeableEnemyCount());
        if (handSignsVowActive_)
            ImGui::TextColored(gold, "Hand Signs (1.25x CT)");
        if (!divulgeVowActive_ && !handSignsVowActive_)
            ImGui::TextColored(gold, "None active");

        if (domainExpansionLearned_) {
            ImGui::SeparatorText("Domain Expansion");
            ImGui::PushStyleColor(ImGuiCol_Text, clanColor(selectedClan_));
            ImGui::TextWrapped("%s [D]", domainExpansionName());
            ImGui::PopStyleColor();
            ImGui::TextWrapped("%s", domainExpansionDescription());
            ImGui::PushStyleColor(
                ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
            ImGui::TextWrapped(
                "Radius 5 | %d activation CE | %d upkeep CE per turn",
                cursedEnergyCost(DomainExpansionActivationCost),
                cursedEnergyCost(DomainExpansionUpkeepCost));
            ImGui::PopStyleColor();

            const char* status = techniqueBurnoutTurns_ > 0
                ? "Technique Burnout"
                : domainExpansionActive_
                    ? playerInDomainClash() ? "DOMAIN CLASH" : "Active"
                    : domainConfirming_ ? "Confirm AoE" : "Ready";
            const ImVec4 statusColor = techniqueBurnoutTurns_ > 0
                ? ImVec4(0.86f, 0.32f, 0.22f, 1.0f)
                : domainExpansionActive_ || domainConfirming_
                    ? clanColor(selectedClan_)
                    : ImVec4(0.45f, 0.92f, 0.55f, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, statusColor);
            if (techniqueBurnoutTurns_ > 0)
                ImGui::TextWrapped("Status: %s (%d turns)", status,
                                   techniqueBurnoutTurns_);
            else
                ImGui::TextWrapped("Status: %s", status);
            ImGui::PopStyleColor();
        }

        ImGui::SeparatorText("Stats");
        const ImVec4 displayedAttackColor = playerMeleeCanAffectCurses()
            ? ImVec4(0.85f, 0.45f, 1.0f, 1.0f)
            : Game::attackColor();
        ImGui::TextColored(displayedAttackColor, "Attack  %d",
                           displayedPlayerAttack());
        ImGui::TextDisabled("Base %d + weapon %d + gear %d + reinforcement %d",
                            player_.attack, katanaBaseAttackBonus(),
                            equipmentAttackBonus(),
                            availableAttackReinforcement());
        ImGui::TextColored(clanColor(selectedClan_), "Clan    %s",
                           playerClan_.c_str());
        ImGui::Text("Name    %s", playerFullName_.c_str());
        ImGui::TextColored(playerGradeColor(playerGrade()), "Rank    %s",
                           playerGradeName(playerGrade()));
        ImGui::TextDisabled("Grade Advancement [C]: %s",
                            advancementAvailable() ? "Available" : "Locked");
        ImGui::TextColored(ImVec4(0.58f, 0.95f, 1.0f, 1.0f),
                           "Current points total  %d", points_);
        ImGui::TextColored(ImVec4(0.45f, 0.88f, 0.72f, 1.0f),
                           "Earned Points  %d", earnedPoints_);
        ImGui::TextColored(ImVec4(0.29f, 0.20f, 0.72f, 1.0f),
                           "Total Earned Points  %d", totalPointsEarned_);
        ImGui::TextColored(ImVec4(0.92f, 0.58f, 0.30f, 1.0f),
                           "Spent Points  %d", spentPoints_);
        if (serratedTraitSelected_)
            ImGui::Text("Trait   Serrated Cursed Energy");
        if (simpleDomainLearned_)
            ImGui::Text("Trait   Simple Domain");
        if (noInnateTechniqueSelected_)
            ImGui::Text("Negative Trait   No Innate Technique");
        ImGui::TextColored(efficiencyColor(),
                           "Cursed Energy Efficiency  %d%%",
                           effectiveCursedEnergyEfficiency());
        ImGui::TextColored(generationColor(),
                           "Cursed Energy Generation  %d / turn",
                           cursedEnergyGeneration_);
        ImGui::TextColored(ImVec4(0.92f, 0.68f, 0.28f, 1.0f),
                           "Close Combat Ability  %d attack%s",
                           player_.closeCombatAbility,
                           player_.closeCombatAbility == 1 ? "" : "s");

        ImGui::SeparatorText("Equipment");
        if (!isJogoClan()) {
            for (std::size_t slotIndex = 0;
                 slotIndex < static_cast<std::size_t>(EquipmentSlot::Count);
                 ++slotIndex) {
                const auto slot = static_cast<EquipmentSlot>(slotIndex);
                const Item* item = equippedItem(slot);
                ImGui::Text("%s: %s", equipmentSlotName(slot),
                            item ? item->name.c_str() : "Empty");
            }
        } else {
            ImGui::TextDisabled("Cursed Spirits cannot equip items.");
        }

        if (cursedBandages_ > 0 || !kamutokeCharges_.empty()) {
            ImGui::SeparatorText("Cursed Tools [Q]");
            if (cursedBandages_ > 0)
                ImGui::Text("Cursed Bandages: %d%s", cursedBandages_,
                            equippedItem(EquipmentSlot::CursedTool) &&
                                    equippedItem(EquipmentSlot::CursedTool)->name ==
                                        "Cursed Bandage"
                                ? " [EQUIPPED]" : "");
            if (!kamutokeCharges_.empty()) {
                int sidebarKamutokeUses = 0;
                for (int charges : kamutokeCharges_)
                    sidebarKamutokeUses += charges;
                ImGui::Text("Imperfect Kamutoke: %d (%d uses)%s",
                            static_cast<int>(kamutokeCharges_.size()),
                            sidebarKamutokeUses,
                            equippedItem(EquipmentSlot::CursedTool) &&
                                    equippedItem(EquipmentSlot::CursedTool)->name ==
                                        "Imperfect Kamutoke"
                                ? " [EQUIPPED]" : "");
            }
        }

        ImGui::SeparatorText("Dungeon");
        ImGui::Text("Dungeon Level  %d / %d", currentLevel_ + 1,
                    static_cast<int>(levels_.size()));
        if (levels_[currentLevel_].bossKeysRequired > 0) {
            ImGui::TextColored(
                bossExitLocked() ? ImVec4(0.92f, 0.32f, 0.38f, 1.0f)
                                 : ImVec4(0.45f, 0.92f, 0.55f, 1.0f),
                "Cursed Keys  %d / %d",
                levels_[currentLevel_].bossKeysCollected,
                levels_[currentLevel_].bossKeysRequired);
            if (!levels_[currentLevel_].bossGateName.empty())
                ImGui::TextWrapped("%s: %s",
                                   levels_[currentLevel_].bossGateName.c_str(),
                                   bossExitLocked() ? "Locked" : "Unlocked");
            else
                ImGui::TextWrapped("Stair seals: %s",
                                   bossExitLocked() ? "Locked" : "Broken");
        }
        ImGui::Text("Turn    %d", turn_);

        ImGui::SeparatorText("Movement Controls");
        ImGui::TextWrapped("Arrow keys: cardinal movement\nNumpad: 8-direction movement\nSpace: wait\nWalk into an enemy to attack.");
        ImGui::TextDisabled("Mouse wheel: zoom | Middle-drag: pan");
        ImGui::TextDisabled("Left-click: inspect | Right-click: clear");
        ImGui::TextDisabled("< up stairs | > down stairs");

        ImGui::PopTextWrapPos();
    }

    void drawMessagePanel() {
        ImGui::TextUnformatted("MESSAGES");
        ImGui::Separator();
        ImGui::BeginChild("MessageScroll", ImVec2(0, 0), false);
        const int begin = std::max(0, static_cast<int>(messages_.size()) - 40);
        for (int i = begin; i < static_cast<int>(messages_.size()); ++i) {
            const float fade =
                0.45f + 0.55f * static_cast<float>(i - begin + 1) /
                                    static_cast<float>(messages_.size() - begin);
            const Message& message = messages_[i];
            ImVec4 messageColor = message.color;
            messageColor.w *= fade;
            ImGui::PushStyleColor(ImGuiCol_Text, messageColor);
            ImGui::TextWrapped("%s", message.text.c_str());
            ImGui::PopStyleColor();
            if (message.remainingHp >= 0 && message.maximumHp > 0) {
                const float healthFraction =
                    static_cast<float>(message.remainingHp) /
                    static_cast<float>(message.maximumHp);
                const ImVec4 hpColor =
                    healthFraction > 0.75f
                        ? ImVec4(0.35f, 0.92f, 0.40f, fade)
                        : healthFraction >= 0.50f
                              ? ImVec4(1.0f, 0.84f, 0.22f, fade)
                              : ImVec4(1.0f, 0.30f, 0.26f, fade);
                ImGui::SameLine();
                ImGui::TextColored(hpColor, "%d / %d HP",
                                   message.remainingHp, message.maximumHp);
            }
        }
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 4.0f)
            ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();
    }
};

void glfwErrorCallback(int, const char* description) {
#ifdef _WIN32
    OutputDebugStringA(description);
    OutputDebugStringA("\n");
#else
    (void)description;
#endif
}

} // namespace

int main() {
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(
        1100, 720, "CursedRL: Tower of Malevolent Divinity", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // Arrow keys belong exclusively to map movement and targeting. Keeping
    // ImGui keyboard navigation disabled prevents those keys from changing
    // selected clans, items, buttons, or numeric menu controls.
    io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.ChildRounding = 4.0f;
    style.FrameRounding = 3.0f;
    style.ScrollbarRounding = 2.0f;
    style.GrabRounding = 2.0f;
    style.ScrollbarSize = 15.0f;
    style.WindowPadding = ImVec2(8, 8);
    style.Colors[ImGuiCol_Border] = ImVec4(0.72f, 0.49f, 0.17f, 1.0f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.78f, 0.55f, 0.20f, 0.90f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.055f, 0.045f, 0.035f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.085f, 0.035f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarBg] =
        ImVec4(0.025f, 0.022f, 0.018f, 0.96f);
    style.Colors[ImGuiCol_ScrollbarGrab] =
        ImVec4(0.48f, 0.31f, 0.10f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] =
        ImVec4(0.72f, 0.50f, 0.17f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] =
        ImVec4(0.92f, 0.70f, 0.28f, 1.0f);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
#ifdef _WIN32
    wchar_t executablePath[MAX_PATH]{};
    GetModuleFileNameW(nullptr, executablePath, MAX_PATH);
    const std::filesystem::path assetDirectory =
        std::filesystem::path(executablePath).parent_path() / L"assets";
    gameplayMusicPath = assetDirectory / L"gameplay_music.mp3";
    menuMusicPath = assetDirectory / L"menu_music.mp3";
    loadTexture(assetDirectory / L"heian_wood_floor_tiles.png", floorTexture);
    loadTexture(assetDirectory / L"japanese_castle_wall_tiles.png", wallTexture);
    loadTexture(assetDirectory / L"japanese_wood_stair_tiles.png", stairTexture);
    loadTexture(assetDirectory / L"player_inspection_sprites.png",
                playerSpriteTexture);
    loadTexture(assetDirectory / L"enemy_sprites.png", enemySpriteTexture);
    loadTexture(assetDirectory / L"centipede_sprites.png",
                centipedeSpriteTexture);
    loadTexture(assetDirectory / L"spider_ghoul_sprites.png",
                spiderGhoulSpritesTexture);
    loadTexture(assetDirectory / L"samurai_sprites.png",
                samuraiSpritesTexture);
    loadTexture(assetDirectory / L"ember_insect_sprites.png",
                emberInsectSpritesTexture);
    loadTexture(assetDirectory / L"ember_insect_map.png",
                emberInsectMapTexture);
    loadTexture(assetDirectory / L"nue_map.png", nueMapTexture);
    loadTexture(assetDirectory / L"nue_inspection.png", nueInspectionTexture);
    loadTexture(assetDirectory / L"gyokuken_map_sprites.png",
                gyokukenMapSpriteTexture);
    loadTexture(assetDirectory / L"gyokuken_inspection_sprites.png",
                gyokukenInspectionSpriteTexture);
    loadTexture(assetDirectory / L"gyokuken_totality_map.png",
                gyokukenTotalityMapTexture);
    loadTexture(assetDirectory / L"gyokuken_totality_inspection.png",
                gyokukenTotalityInspectionTexture);
    loadTexture(assetDirectory / L"obsidian_gold_ui_atlas.png",
                obsidianGoldUiTexture);
    loadTexture(assetDirectory / L"map_unit_sprites.png",
                mapUnitSpriteTexture);
    loadTexture(assetDirectory / L"cursed_pachinko_machine.png",
                cursedPachinkoTexture);
    loadTexture(assetDirectory / L"cursed_objects.png",
                cursedObjectsTexture);
    loadTexture(assetDirectory / L"cursed_tool_sprites.png",
                cursedToolSpritesTexture);
    loadTexture(assetDirectory / L"cursed_key_gate_sprites.png",
                cursedGateSpritesTexture);
    loadTexture(assetDirectory / L"boss_gate_sprites.png",
                bossGateSpritesTexture);
    loadTexture(assetDirectory / L"equipment_icons.png",
                equipmentIconsTexture);
#endif
    Game game;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        game.handleInput();
        game.draw();
        if (game.wantsQuit()) glfwSetWindowShouldClose(window, GLFW_TRUE);
        ImGui::Render();

        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(0.025f, 0.03f, 0.045f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

#ifdef _WIN32
    stopGameplayMusic();
    stopMenuMusic();
#endif
    if (floorTexture != 0) glDeleteTextures(1, &floorTexture);
    if (wallTexture != 0) glDeleteTextures(1, &wallTexture);
    if (stairTexture != 0) glDeleteTextures(1, &stairTexture);
    if (playerSpriteTexture != 0)
        glDeleteTextures(1, &playerSpriteTexture);
    if (enemySpriteTexture != 0)
        glDeleteTextures(1, &enemySpriteTexture);
    if (centipedeSpriteTexture != 0)
        glDeleteTextures(1, &centipedeSpriteTexture);
    if (spiderGhoulSpritesTexture != 0)
        glDeleteTextures(1, &spiderGhoulSpritesTexture);
    if (samuraiSpritesTexture != 0)
        glDeleteTextures(1, &samuraiSpritesTexture);
    if (emberInsectSpritesTexture != 0)
        glDeleteTextures(1, &emberInsectSpritesTexture);
    if (emberInsectMapTexture != 0)
        glDeleteTextures(1, &emberInsectMapTexture);
    if (nueMapTexture != 0) glDeleteTextures(1, &nueMapTexture);
    if (nueInspectionTexture != 0)
        glDeleteTextures(1, &nueInspectionTexture);
    if (gyokukenMapSpriteTexture != 0)
        glDeleteTextures(1, &gyokukenMapSpriteTexture);
    if (gyokukenInspectionSpriteTexture != 0)
        glDeleteTextures(1, &gyokukenInspectionSpriteTexture);
    if (gyokukenTotalityMapTexture != 0)
        glDeleteTextures(1, &gyokukenTotalityMapTexture);
    if (gyokukenTotalityInspectionTexture != 0)
        glDeleteTextures(1, &gyokukenTotalityInspectionTexture);
    if (obsidianGoldUiTexture != 0)
        glDeleteTextures(1, &obsidianGoldUiTexture);
    if (mapUnitSpriteTexture != 0)
        glDeleteTextures(1, &mapUnitSpriteTexture);
    if (cursedPachinkoTexture != 0)
        glDeleteTextures(1, &cursedPachinkoTexture);
    if (cursedObjectsTexture != 0)
        glDeleteTextures(1, &cursedObjectsTexture);
    if (cursedToolSpritesTexture != 0)
        glDeleteTextures(1, &cursedToolSpritesTexture);
    if (cursedGateSpritesTexture != 0)
        glDeleteTextures(1, &cursedGateSpritesTexture);
    if (bossGateSpritesTexture != 0)
        glDeleteTextures(1, &bossGateSpritesTexture);
    if (equipmentIconsTexture != 0)
        glDeleteTextures(1, &equipmentIconsTexture);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
