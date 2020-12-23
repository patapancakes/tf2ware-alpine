// Hugging module
#define HEAVY_MODEL "models/player/geavy.mdl"

new String:var_heavy_love[][] = {"imgay/tf2ware/heavy_ilu.wav", "vo/heavy_specialcompleted08.wav", "vo/heavy_award04.wav"};

public Hugging_Init() {
    AddToForward(g_OnMapStart, INVALID_HANDLE, Hugging_OnMapStart);
}

public Hugging_OnMapStart() {
    decl String:input[512];
    for (new i = 1; i <= sizeof(var_heavy_love); i++) {
        Format(input, sizeof(input), "sound/%s", var_heavy_love[i-1]);
        AddFileToDownloadsTable(input);		
        AddFileToDownloadsTable("models/player/geavy.dx80.vtx"); 
        AddFileToDownloadsTable("models/player/geavy.dx90.vtx"); 
        AddFileToDownloadsTable("models/player/geavy.mdl"); 
        AddFileToDownloadsTable("models/player/geavy.phy");
        AddFileToDownloadsTable("models/player/geavy.sw.vtx"); 
        AddFileToDownloadsTable("models/player/geavy.vvd"); 
        AddFileToDownloadsTable("materials/models/player/hvyweapon/eyeball_l.vmt"); 
        AddFileToDownloadsTable("materials/models/player/hvyweapon/eyeball_r.vmt"); 
        AddFileToDownloadsTable("materials/models/player/hvyweapon/gayweapon_blue.vmt"); 
        AddFileToDownloadsTable("materials/models/player/hvyweapon/gayweapon_hands.vmt"); 
        AddFileToDownloadsTable("materials/models/player/hvyweapon/gayweapon_hands.vtf"); 
        AddFileToDownloadsTable("materials/models/player/hvyweapon/gayweapon_red.vmt"); 
        AddFileToDownloadsTable("materials/models/player/hvyweapon/gayweapon_red.vtf"); 
        AddFileToDownloadsTable("materials/models/player/hvyweapon/geavy_head.vtf"); 
        AddFileToDownloadsTable("materials/models/player/hvyweapon/geavy_head_blue.vmt");
        AddFileToDownloadsTable("materials/models/player/hvyweapon/geavy_head_red.vmt");
        precacheSound(var_heavy_love[i-1]);
        PrecacheModel(HEAVY_MODEL, true);		
	}
}

public Hugging_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, Hugging_justEntered);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, Hugging_End);
    AddToForward(g_PlayerDeath, INVALID_HANDLE, Hugging_PlayerDeath);

    new AllPlayers = GetClientCount(true);
    new target = 0;
    new attempts = 0;
    new heavies = GetActivePlayers() / 4;
    if (heavies <= 1) heavies = 2;
    if (heavies >= 4) heavies = 3;
    new heavyclients[MAXPLAYERS+1];
    new scoutclients[MAXPLAYERS+1];
    for (new i = 1; i <= heavies; i++) {
        attempts = 0;
        target = GetRandomInt(1, AllPlayers);
        while (((!IsValidClient(target)) || IsClientParticipating(target) == false || g_Spawned[target] == false || IsFakeClient(target) || g_Mission[target] > 0) && attempts <= 50) {
            target = GetRandomInt(1, AllPlayers);
            attempts += 1;
        }
        if (attempts <= 50) {
            g_Mission[target] = 1;
        }
    }
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && IsClientParticipating(i)) {
            if (g_Mission[i] == 0) {
                SetStateClient(i, true);
                scoutclients[i-1] = i;
            }
            else {
                SetStateClient(i, false);
                heavyclients[i-1] = i;
            }
        }
    }
    
    AddRandVocalization("heaviestaunt", heavyclients, 4.0, 8.0);
    AddRandVocalization("scoutdisgust", scoutclients, 1.5, 5.0);
    
    NoCollision(false);
    
}

public Action:Hugging_OnTakeDamageClient(victim, &attacker, &inflictor, &Float:damage, &damagetype) {
    if (status == 2) {
        if (damage > 0) damage = 1.0;
        return Plugin_Changed;
    }
    return Plugin_Continue;
}

public Hugging_End() {
    for (new i = 1; i <= MaxClients; i++) {
        SDKUnhook(i, SDKHook_Touch, Hugging_OnPlayerTouch);
    }
}

public Hugging_justEntered(client) {
    if (IsValidClient(client)) {
        SDKHook(client, SDKHook_Touch, Hugging_OnPlayerTouch);
    
        decl Float:pos[3];
        decl Float:ang[3];
        decl Float:vel[3];
        vel[0] = 0.0;
        vel[1] = 0.0;
        vel[2] = 0.0;
        new column = client;
        new row = 0;
        while (column > 10) {
            column = column - 10;
            row = row + 1;
        }
        if (g_Mission[client] == 0) {
            if (GetClientTeam(client) == 2) ChangeClientTeamAlive(client, 3);
            SetClientClass(client, "scout");
            DisableClientWeapons(client);       
            pos[0] = 142.0 - float(column*80);
            pos[1] = -2040.0 + float(row*80);
            pos[2] = -700.0;
            ang[0] = 0.0;
            ang[1] = -90.0;
            ang[2] = 0.0;
        }//m_iTotalScore
        if (g_Mission[client] == 1) {
            if (GetClientTeam(client) == 3) ChangeClientTeamAlive(client, 2);
            SetClientClass(client, "heavy");
            Createheavyskin(client);
            RemoveClientWeapons(client);
            GiveClientWeapon(client, "tf_weapon_fists", 2, true);
            pos[0] = 142.0 - float(column*80);
            pos[1] = -2965.0 - float(row*80);
            pos[2] = -700.0;
            ang[0] = 0.0;
            ang[1] = 90.0;
            ang[2] = 0.0;
        }
        TeleportEntity(client, pos, ang, vel);
    }
}

public Hugging_OnPlayerTouch(entity, other) {
    if (status != 2) return;
    if (!StrEqual(minigame, "Hugging")) return;

    new String:classname[64];
    new String:classname2[64];
    GetEdictClassname(entity, classname, sizeof(classname));
    GetEdictClassname(other, classname2, sizeof(classname2));
    if (StrEqual(classname, "player") && StrEqual(classname2, "player") && IsValidClient(entity) && IsValidClient(other) && status == 2 && IsPlayerAlive(entity) && IsPlayerAlive(other) && (g_Mission[other] > 0 || g_Mission[entity] > 0)) {
        if (g_Mission[entity] > 0 && g_Mission[other] > 0) return;
        new attacker = entity;
        new victim = other;
        if (g_Mission[other] > 0) {
            attacker = other;
            victim = entity;
        }
        ForcePlayerSuicide(victim);
        EmitSoundToAll(SOUND_HEAVY_KISS, attacker);
        EmitSoundToClient(victim, SOUND_HEAVY_KISS);
    }
}

public Hugging_PlayerDeath(client) {
    SetStateClient(client, false, true);
    
    CreateTimer(0.1, Hugging_Check);
}

public Action:Hugging_Check(Handle:timer) {
    if (status != 2) return;
    if (!StrEqual(minigame, "Hugging")) return;

    new bool:complete = true;
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && (g_Mission[i] == 0) && IsClientParticipating(i) && IsPlayerAlive(i)) complete = false;
    }
    if (complete) {
        for (new i = 1; i <= MaxClients; i++) {
            if (IsValidClient(i) && (g_Mission[i] == 1) && IsClientParticipating(i)) SetStateClient(i, true, true);
        }
    }
}

Createheavyskin(client) {   
    //Add a new model
    decl String:strModel[PLATFORM_MAX_PATH];
    Format(strModel, sizeof(strModel), "");

    if (TF2_GetPlayerClass(client) == TFClass_Heavy) Format(strModel, sizeof(strModel), "models/player/geavy.mdl");
    
    if (IsClientInGame(client) && IsPlayerAlive(client)) {
        SetVariantString(strModel);
        AcceptEntityInput(client, "SetCustomModel");
        if (!StrEqual(strModel, "")) {
            SetEntProp(client, Prop_Send, "m_bUseClassAnimations",1);
            SetEntProp(client, Prop_Send, "m_nBody", 0);
        }
    }
}

stock PrecacheModelEx(String:strFileName[], bool:bPreload=false, bool:bAddToDownloadTable=false)
{
    PrecacheModel(strFileName, bPreload);
    if (bAddToDownloadTable)
    {
        decl String:strDepFileName[PLATFORM_MAX_PATH];
        Format(strDepFileName, sizeof(strDepFileName), "%s.res", strFileName);
        
        if (FileExists(strDepFileName))
        {
            // Open stream, if possible
            new Handle:hStream = OpenFile(strDepFileName, "r");
            if (hStream == INVALID_HANDLE) { LogMessage("Error, can't read file containing model dependencies."); return; }
            
            while(!IsEndOfFile(hStream))
            {
                decl String:strBuffer[PLATFORM_MAX_PATH];
                ReadFileLine(hStream, strBuffer, sizeof(strBuffer));
                CleanString(strBuffer);
                
                // If file exists...
                if (FileExists(strBuffer, true))
                {
                    // Precache depending on type, and add to download table
                    if (StrContains(strBuffer, ".vmt", false) != -1)      PrecacheDecal(strBuffer, true);
                    else if (StrContains(strBuffer, ".mdl", false) != -1) PrecacheModel(strBuffer, true);
                    else if (StrContains(strBuffer, ".pcf", false) != -1) PrecacheGeneric(strBuffer, true);
                    AddFileToDownloadsTable(strBuffer);
                }
            }
            
            // Close file
            CloseHandle(hStream);
        }
    }
}

/* CleanString()
**
** Cleans the given string from any illegal character.
** -------------------------------------------------------------------------- */
stock CleanString(String:strBuffer[])
{
    // Cleanup any illegal characters
    new Length = strlen(strBuffer);
    for (new iPos=0; iPos<Length; iPos++)
    {
        switch(strBuffer[iPos])
        {
            case '\r': strBuffer[iPos] = ' ';
            case '\n': strBuffer[iPos] = ' ';
            case '\t': strBuffer[iPos] = ' ';
        }
    }
    
    // Trim string
    TrimString(strBuffer);
}

/* FMax()
**
** Returns the maximum of the two values given.
** -------------------------------------------------------------------------- */
stock Float:FMax(Float:a, Float:b)
{
    return (a > b)? a:b;
}

/* FMin()
**
** Returns the minimum of the two values given.
** -------------------------------------------------------------------------- */
stock Float:FMin(Float:a, Float:b)
{
    return (a < b)? a:b;
}

/* GetURandomIntRange()
**
** 
** -------------------------------------------------------------------------- */
stock GetURandomIntRange(iMin, iMax)
{
    return iMin + (GetURandomInt() % (iMax - iMin + 1));
}

/* GetURandomFloatRange()
**
** 
** -------------------------------------------------------------------------- */
stock Float:GetURandomFloatRange(Float:fMin, Float:fMax)
{
    return fMin + (GetURandomFloat() * (fMax - fMin));
}

// EOF 