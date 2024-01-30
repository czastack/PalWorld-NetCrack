#pragma once

enum class PalPipeRequest
{
    GetFName,
    AddItem,
    AddPal,
    RespawnLocalPlayer,
    Teleport,
    UnlockAllEffigies,
    IncrementInventoryItemCountByIndex,
    ExploitFly,
    ReviveLocalPlayer,
    GiveExperiencePoints,
    SetPlayerAttackParam,
    SetPlayerDefenseParam,
};

DWORD WINAPI PipeThread();
