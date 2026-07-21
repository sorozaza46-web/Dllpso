#include <windows.h>
#include <iostream>
#include <cmath>

// --- UNREAL ENGINE YAPI TANIMLAMALARI ---

struct FVector {
    double X, Y, Z;
};

struct FRotator {
    double Pitch, Yaw, Roll;
};

// Unreal Engine UObject Taban Sınıfı (Bellek boyutu: 0x28)
class UObject {
public:
    void** VTable;             // 0x00
    char pad_UObject[0x20];    // 0x08 - 0x28
};

// Karakter Koordinatlarını Tutan Bileşen
class USceneComponent : public UObject {
public:
    char pad_0128[0x100];      // 0x28 - 0x128
    FVector RelativeLocation;  // 0x0128
};

// Karakter Hareket Özelliklerini Tutan Bileşen
class UCharacterMovementComponent : public UObject {
public:
    char pad_0170[0x148];      // 0x28 - 0x170
    float GravityScale;        // 0x0170
    float MaxStepHeight;       // 0x0174
    float JumpZVelocity;       // 0x0178
    
    char pad_0248[0xCC];       // 0x17C - 0x248
    float MaxWalkSpeed;        // 0x0248
    float MaxWalkSpeedCrouched;// 0x024C
    float MaxSwimSpeed;        // 0x0250
    float MaxFlySpeed;         // 0x0254
    float MaxCustomMovementSpeed; // 0x0258
    float MaxAcceleration;     // 0x025C
};

// Aktif Oyuncu Karakter Sınıfı (Flat Class Yapısı)
class ACharacter : public UObject {
public:
    char pad_01A0[0x178];      // 0x28 - 0x1A0
    USceneComponent* RootComponent; // 0x01A0
    
    char pad_0320[0x178];      // 0x1A8 - 0x320
    UCharacterMovementComponent* CharacterMovement; // 0x0320
    
    char pad_0470[0x148];      // 0x328 - 0x470
    int32_t JumpMaxCount;      // 0x0470
};

// Kamera Görüş Açısını Tutan Sınıf
class APlayerCameraManager : public UObject {
public:
    char pad_02A8[0x280];      // 0x28 - 0x2A8 (AActor boyutu 0x290 olduğundan offset 0x2A8'dir)
    float DefaultFOV;          // 0x02A8
};

// Oyuncu Kontrolcüsü Sınıfı
class APlayerController : public UObject {
public:
    char pad_02E0[0x2B8];      // 0x28 - 0x2E0
    ACharacter* Character;     // 0x02E0
    
    char pad_0348[0x60];       // 0x2E8 - 0x348
    APlayerCameraManager* PlayerCameraManager; // 0x0348
    
    char pad_0544[0x1F4];      // 0x350 - 0x544
    uint8_t bShowMouseCursor;  // 0x0544
};

class UPlayer : public UObject {
public:
    char pad_0030[0x08];       // 0x28 - 0x30
    APlayerController* PlayerController; // 0x0030
};

class ULocalPlayer : public UPlayer {
    // UPlayer sınıfını miras alır
};

class UGameInstance : public UObject {
public:
    char pad_0038[0x10];       // 0x28 - 0x38
    struct {
        ULocalPlayer** Data;
        int32_t Count;
        int32_t Max;
    } LocalPlayers;            // 0x0038
};

class UWorld : public UObject {
public:
    char pad_01D8[0x1B0];      // 0x28 - 0x1D8
    UGameInstance* OwningGameInstance; // 0x01D8
};

// --- HİLE VE CONFIG ALANI ---

namespace Offsets
{
    constexpr int32 GWorld = 0x07E93198; // Basic.hpp dosyasındaki orijinal GWorld adresi
}

namespace Cheats
{
    bool bSpeedHack = false;
    bool bSuperJump = false;
    bool bLowGravity = false;
    bool bHighFOV = false;
    bool bShowCursor = false;

    float OriginalSpeed = 600.0f;
    float OriginalJumpVelocity = 420.0f;
    float OriginalGravity = 1.0f;
    float OriginalFOV = 90.0f;
    bool bDefaultsCaptured = false;
}

void ModMenuLoop(HMODULE hModule)
{
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    std::cout << "===========================================\n";
    std::cout << "[Pro Soccer Online - Bagimsiz C++ Mod Menu]\n";
    std::cout << "===========================================\n";
    std::cout << "[NUMPAD 1] - Hiz Hilesi (Speed Hack)     [ACIK/KAPALI]\n";
    std::cout << "[NUMPAD 2] - Super Ziplama (Super Jump)   [ACIK/KAPALI]\n";
    std::cout << "[NUMPAD 3] - Dusuk Yercekimi (Low Gravity)[ACIK/KAPALI]\n";
    std::cout << "[NUMPAD 4] - Gorus Acisi (FOV Changer)    [ACIK/KAPALI]\n";
    std::cout << "[NUMPAD 5] - Mouse Gosterici (Show Cursor)[ACIK/KAPALI]\n";
    std::cout << "[HOLD SHIFT] - Anlik Depar (Stealth Sprint)\n";
    std::cout << "-------------------------------------------\n";
    std::cout << "[F10] - Konum Kaydet (Save Location)\n";
    std::cout << "[F11] - Kayitli Konuma Isinlan (Teleport)\n";
    std::cout << "-------------------------------------------\n";
    std::cout << "[END] - Kapat (Eject DLL)\n";
    std::cout << "===========================================\n";

    FVector SavedLocation = { 0, 0, 0 };

    while (!GetAsyncKeyState(VK_END))
    {
        Sleep(50);

        uintptr_t BaseAddress = (uintptr_t)GetModuleHandle(NULL);
        if (!BaseAddress) continue;

        // GWorld adresini doğrudan bellekten çekiyoruz (SDK gereksizleştirildi)
        UWorld* World = *(UWorld**)(BaseAddress + Offsets::GWorld);
        if (!World) continue;

        UGameInstance* GameInstance = World->OwningGameInstance;
        if (!GameInstance || GameInstance->LocalPlayers.Count == 0 || !GameInstance->LocalPlayers.Data) continue;

        ULocalPlayer* LocalPlayer = GameInstance->LocalPlayers.Data[0];
        if (!LocalPlayer) continue;

        APlayerController* PlayerController = LocalPlayer->PlayerController;
        if (!PlayerController) continue;

        ACharacter* Character = PlayerController->Character;
        if (!Character) continue;

        UCharacterMovementComponent* Movement = Character->CharacterMovement;
        APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;

        // Orijinal değerleri ilk çalıştırmada dinamik olarak belleğe kaydediyoruz
        if (Movement && !Cheats::bDefaultsCaptured)
        {
            Cheats::OriginalSpeed = Movement->MaxWalkSpeed;
            Cheats::OriginalJumpVelocity = Movement->JumpZVelocity;
            Cheats::OriginalGravity = Movement->GravityScale;
            if (CameraManager)
            {
                Cheats::OriginalFOV = CameraManager->DefaultFOV;
            }
            Cheats::bDefaultsCaptured = true;
            std::cout << "[Sistem] Orijinal oyun degerleri kaydedildi.\n";
        }

        // --- TUŞ TETİKLEMELERİ ---

        if (GetAsyncKeyState(VK_NUMPAD1) & 1)
        {
            Cheats::bSpeedHack = !Cheats::bSpeedHack;
            std::cout << "[Hiz Hilesi] " << (Cheats::bSpeedHack ? "AKTIF" : "KAPALI") << "\n";
        }

        if (GetAsyncKeyState(VK_NUMPAD2) & 1)
        {
            Cheats::bSuperJump = !Cheats::bSuperJump;
            std::cout << "[Super Ziplama] " << (Cheats::bSuperJump ? "AKTIF" : "KAPALI") << "\n";
        }

        if (GetAsyncKeyState(VK_NUMPAD3) & 1)
        {
            Cheats::bLowGravity = !Cheats::bLowGravity;
            std::cout << "[Dusuk Yercekimi] " << (Cheats::bLowGravity ? "AKTIF" : "KAPALI") << "\n";
        }

        if (GetAsyncKeyState(VK_NUMPAD4) & 1)
        {
            Cheats::bHighFOV = !Cheats::bHighFOV;
            std::cout << "[Gorus Acisi] " << (Cheats::bHighFOV ? "115 FOV" : "Varsayilan FOV") << "\n";
        }

        if (GetAsyncKeyState(VK_NUMPAD5) & 1)
        {
            Cheats::bShowCursor = !Cheats::bShowCursor;
            PlayerController->bShowMouseCursor = Cheats::bShowCursor;
            std::cout << "[Mouse Imleci] " << (Cheats::bShowCursor ? "GOSTERILIYOR" : "GIZLENDI") << "\n";
        }

        // F10 - Konum Kaydet
        if (GetAsyncKeyState(VK_F10) & 1)
        {
            if (Character->RootComponent)
            {
                SavedLocation = Character->RootComponent->RelativeLocation;
                std::cout << "[Teleport] Konum Kaydedildi: " << SavedLocation.X << ", " << SavedLocation.Y << ", " << SavedLocation.Z << "\n";
            }
        }

        // F11 - Kayıtlı Konuma Işınlan
        if (GetAsyncKeyState(VK_F11) & 1)
        {
            if (Character->RootComponent && SavedLocation.X != 0)
            {
                Character->RootComponent->RelativeLocation = SavedLocation;
                std::cout << "[Teleport] Kayitli konuma isinlandiniz.\n";
            }
        }

        // --- HİLELERİN UYGULANMASI ---

        if (Movement)
        {
            if (Cheats::bSpeedHack)
            {
                Movement->MaxWalkSpeed = Cheats::OriginalSpeed * 1.8f;
                Movement->MaxAcceleration = 4096.0f;
            }
            else if (GetAsyncKeyState(VK_SHIFT))
            {
                Movement->MaxWalkSpeed = Cheats::OriginalSpeed * 1.4f;
            }
            else
            {
                Movement->MaxWalkSpeed = Cheats::OriginalSpeed;
            }

            if (Cheats::bSuperJump)
            {
                Movement->JumpZVelocity = 850.0f;
                Character->JumpMaxCount = 5;
            }
            else
            {
                Movement->JumpZVelocity = Cheats::OriginalJumpVelocity;
                Character->JumpMaxCount = 1;
            }

            if (Cheats::bLowGravity)
            {
                Movement->GravityScale = 0.25f;
            }
            else
            {
                Movement->GravityScale = Cheats::OriginalGravity;
            }
        }

        if (CameraManager)
        {
            if (Cheats::bHighFOV)
            {
                CameraManager->DefaultFOV = 115.0f;
            }
            else
            {
                CameraManager->DefaultFOV = Cheats::OriginalFOV;
            }
        }
    }

    // --- KAPANIŞTA DEĞERLERİ GERİ YÜKLEME ---
    std::cout << "[Mod] Degerler orijinal haline getiriliyor...\n";
    uintptr_t BaseAddress = (uintptr_t)GetModuleHandle(NULL);
    if (BaseAddress)
    {
        UWorld* World = *(UWorld**)(BaseAddress + Offsets::GWorld);
        if (World && World->OwningGameInstance && World->OwningGameInstance->LocalPlayers.Count > 0)
        {
            ULocalPlayer* LocalPlayer = World->OwningGameInstance->LocalPlayers.Data[0];
            if (LocalPlayer && LocalPlayer->PlayerController)
            {
                APlayerController* PlayerController = LocalPlayer->PlayerController;
                PlayerController->bShowMouseCursor = false;

                if (PlayerController->Character)
                {
                    if (PlayerController->Character->CharacterMovement)
                    {
                        PlayerController->Character->CharacterMovement->MaxWalkSpeed = Cheats::OriginalSpeed;
                        PlayerController->Character->CharacterMovement->JumpZVelocity = Cheats::OriginalJumpVelocity;
                        PlayerController->Character->CharacterMovement->GravityScale = Cheats::OriginalGravity;
                    }
                    PlayerController->Character->JumpMaxCount = 1;
                }
                if (PlayerController->PlayerCameraManager)
                {
                    PlayerController->PlayerCameraManager->DefaultFOV = Cheats::OriginalFOV;
                }
            }
        }
    }

    std::cout << "[Mod] Bellek temizlendi. DLL serbest birakiliyor.\n";
    if (f) fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)ModMenuLoop, hModule, 0, nullptr);
    }
    return TRUE;
}
