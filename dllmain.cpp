#include <windows.h>
#include <iostream>
#include "Basic.hpp"
#include "Engine_classes.hpp"

using namespace SDK;

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
    std::cout << "[Pro Soccer Online - C++ Mod Menu]\n";
    std::cout << "===========================================\n";
    std::cout << "[NUMPAD 1] - Hiz Hilesi (Speed Hack)     [ACIK/KAPALI]\n";
    std::cout << "[NUMPAD 2] - Super Ziplama (Super Jump)   [ACIK/KAPALI]\n";
    std::cout << "[NUMPAD 3] - Dusuk Yercekimi (Low Gravity)[ACIK/KAPALI]\n";
    std::cout << "[NUMPAD 4] - Gorus Acisi (FOV Changer)    [ACIK/KAPALI]\n";
    std::cout << "[NUMPAD 5] - Mouse Gosterici (Show Cursor)[ACIK/KAPALI]\n";
    std::cout << "[HOLD SHIFT] - Anlik Depar (Stealth Sprint)\n";
    std::cout << "-------------------------------------------\n";
    std::cout << "[END] - Kapat (Eject DLL)\n";
    std::cout << "===========================================\n";

    while (!GetAsyncKeyState(VK_END))
    {
        Sleep(50);

        UWorld* World = UWorld::GetWorld();
        if (!World) continue;

        UGameInstance* GameInstance = World->OwningGameInstance;
        if (!GameInstance || GameInstance->LocalPlayers.Num() == 0) continue;

        ULocalPlayer* LocalPlayer = GameInstance->LocalPlayers[0];
        if (!LocalPlayer) continue;

        APlayerController* PlayerController = LocalPlayer->PlayerController;
        if (!PlayerController) continue;

        ACharacter* Character = PlayerController->Character;
        if (!Character) continue;

        UCharacterMovementComponent* Movement = Character->CharacterMovement;
        APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;

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

    std::cout << "[Mod] Degerler orijinal haline getiriliyor...\n";
    UWorld* World = UWorld::GetWorld();
    if (World && World->OwningGameInstance && World->OwningGameInstance->LocalPlayers.Num() > 0)
    {
        ULocalPlayer* LocalPlayer = World->OwningGameInstance->LocalPlayers[0];
        if (LocalPlayer && LocalPlayer->PlayerController)
        {
            APlayerController* PlayerController = LocalPlayer->PlayerController;
            PlayerController->bShowMouseCursor = false;

            if (PlayerController->Character && PlayerController->Character->CharacterMovement)
            {
                PlayerController->Character->CharacterMovement->MaxWalkSpeed = Cheats::OriginalSpeed;
                PlayerController->Character->CharacterMovement->JumpZVelocity = Cheats::OriginalJumpVelocity;
                PlayerController->Character->CharacterMovement->GravityScale = Cheats::OriginalGravity;
                PlayerController->Character->JumpMaxCount = 1;
            }
            if (PlayerController->PlayerCameraManager)
            {
                PlayerController->PlayerCameraManager->DefaultFOV = Cheats::OriginalFOV;
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
