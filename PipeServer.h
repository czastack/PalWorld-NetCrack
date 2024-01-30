#pragma once

enum class PalPipeRequest
{
    None,
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
