new g_PlayerVisibleWeapon[MAXPLAYERS+1] = -1;

stock bool:IsValidClient(iClient) {
    if (iClient <= 0) return false;
    if (iClient > MaxClients) return false;
    if (!IsClientConnected(iClient)) return false;
    return IsClientInGame(iClient);
}

RegMinigame(String:game_name[], Function:initFunc, Function:onMinigameLoad = INVALID_FUNCTION) {
    for (new i=0; i<sizeof(g_name); i++) {
        if (StrEqual(game_name, g_name[i])) {
            if (GetConVarBool(ww_log)) LogMessage("Registered minigame %s, id %d", game_name, i+1);
            g_initFuncs[i] = initFunc;
            
            if (onMinigameLoad != INVALID_FUNCTION) {
                if (GetConVarBool(ww_log)) LogMessage("--Running onMinigameLoad function for %s", game_name);
                Call_StartFunction(INVALID_HANDLE, onMinigameLoad);
                Call_Finish();
            }
            return;
        }
    }
    PrintToServer("Warning: Attempted to register minigame %s and failed--configuration not found", game_name);
}

GiveId(bool:mustbealive = false) {
    new id = 0;
    for (new client = 1; client <= MaxClients; client++) {
        if (IsValidClient(client) && (mustbealive == false || IsPlayerAlive(client))) {
            id += 1;
            g_Id[client] = id;
        }
    }
}

NoCollision(bool:input) {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidEntity(i) && IsValidClient(i)) {
            if (input) SetEntData(i, g_offsCollisionGroup, 2, 4, true); // NO COLLISION
            if (input == false) SetEntData(i, g_offsCollisionGroup, 5, 4, true); // BRING BACK COLLISION
        }
    }
}

PyroExplode(Float:vec1[3]) {
    new color[4]={188,220,255,200};
    EmitSoundFromOrigin("ambient/explosions/explode_8.wav", vec1);
    TE_SetupExplosion(vec1, g_ExplosionSprite, 10.0, 1, 0, 0, 750); // 600
    TE_SendToAll();
    TE_SetupBeamRingPoint(vec1, 10.0, 500.0, white, g_HaloSprite, 0, 10, 0.6, 10.0, 0.5, color, 10, 0);
    TE_SendToAll();
}

ExplosionDamage(Float:origin[3]) {
    new maxplayers = GetMaxClients();
    
    decl Float:PlayerVec[3];
    decl Float:distance;
    for (new i = 1; i <= maxplayers; i++) {
        if( !IsClientInGame(i) || !IsPlayerAlive(i) ) continue;
        GetClientAbsOrigin(i, PlayerVec);
        
        distance = GetVectorDistance(origin, PlayerVec, true);
        if(distance > 100000.0) continue;
        
        new dmg = RoundFloat(100000.0 - distance) / 50;
        new health = GetClientHealth(i) - dmg;
        
        if (health > 0) SetEntityHealth(i,health);
        if (health <= 0) ForcePlayerSuicide(i);
    }
}

EmitSoundFromOrigin(const String:sound[],const Float:orig[3]) {
    EmitSoundToAll(sound,SOUND_FROM_WORLD,SNDCHAN_AUTO,SNDLEVEL_NORMAL,SND_NOFLAGS,SNDVOL_NORMAL,SNDPITCH_NORMAL,-1,orig,NULL_VECTOR,true,0.0);
}

SetClientClass(i, String:tfclass[128]) {
    new TFClassType:input = TFClass_Scout;
    if (StrEqual(tfclass,"soldier")) input = TFClass_Soldier;
    if (StrEqual(tfclass,"spy")) input = TFClass_Spy;
    if (StrEqual(tfclass,"demoman")) input = TFClass_DemoMan;
    if (StrEqual(tfclass,"sniper")) input = TFClass_Sniper;
    if (StrEqual(tfclass,"medic")) input = TFClass_Medic;
    if (StrEqual(tfclass,"engineer")) input = TFClass_Engineer;
    if (StrEqual(tfclass,"heavy")) input = TFClass_Heavy;
    if (StrEqual(tfclass,"pyro")) input = TFClass_Pyro;

    if (IsValidClient(i)) {
        if (TF2_GetPlayerClass(i) != input)
        {
            TF2_SetPlayerClass(i, input);
            SetVariantString("");
            AcceptEntityInput(i, "SetCustomModel");
        }
        HealClient(i);
    }
}

HealClient(i) {
    new TFClassType:class = TF2_GetPlayerClass(i);
    new health = 125;
    if (class == TFClass_Soldier) health = 200;
    if (class == TFClass_Pyro) health = 175;
    if (class == TFClass_DemoMan) health = 175;
    if (class == TFClass_Heavy) health = 300;
    if (class == TFClass_Medic) health = 150;
    
    SetEntityHealth(i, health);
}

stock ClientParticle(client, String:effect[128], Float:time) {
    new Float:strflVec[3];
    GetClientEyePosition(client, strflVec);
    
    new strIParticle = CreateEntityByName("info_particle_system");
    new String:strName[128];
    if (IsValidEdict(strIParticle)) {
        new Float:strflPos[3];
        GetEntPropVector(client, Prop_Send, "m_vecOrigin", strflPos);
        TeleportEntity(strIParticle, strflPos, NULL_VECTOR, NULL_VECTOR);
        
        Format(strName, sizeof(strName), "target%i", client);
        DispatchKeyValue(client, "targetname", strName);
        
        DispatchKeyValue(strIParticle, "targetname", "tf2particle");
        DispatchKeyValue(strIParticle, "parentname", strName);
        DispatchKeyValue(strIParticle, "effect_name", effect);
        DispatchSpawn(strIParticle);
        SetVariantString(strName);
        AcceptEntityInput(strIParticle, "SetParent", strIParticle, strIParticle, 0);
        //SetVariantString("head");
        //AcceptEntityInput(strIParticle, "SetParentAttachment", strIParticle, strIParticle, 0);
        ActivateEntity(strIParticle);
        AcceptEntityInput(strIParticle, "start");
        
        CreateTimer(time, killprop_timer, strIParticle);
    }
}

public Action:killprop_timer(Handle:hTimer, any:prop) {
    if (IsValidEntity(prop)) {
        AcceptEntityInput(prop, "Kill" );
    }
    return Plugin_Stop;
}

stock GetActivePlayers(team = 0, bool:mustbealive = false) {
    new output = 0;
    for (new client = 1; client <= MaxClients; client++) {
        if (IsValidClient(client) && g_Spawned[client]) {
            if (((team == 0 && GetClientTeam(client) >= 2) || (team > 0 && GetClientTeam(client) == team)) && (mustbealive == false || IsPlayerAlive(client)) && g_Participating[client] == true) output += 1;
        }
    }
    return output;
}

stock bool:IsPlayerTaunting(client)
{
    new pcond = GetEntProp(client, Prop_Send, "m_nPlayerCond");
    if (pcond & TF2_PLAYER_TAUNTING) 
    {
        return true;
    }
    return false;
}  


public RemoveClientWeapons(i) {
    if (GetConVarBool(ww_log)) LogMessage("Removing all client weapons");
    RemoveFakeWeapon(i);
    if (IsValidClient(i) && (IsPlayerAlive(i)) && (g_Winner[i] == 0)) {
        SetClientSlot(i, 0);
        for (new j=0; j<=5; j++) {
            TF2_RemoveWeaponSlot(i, j);
        }
        SetWeaponState(i, false);
    }
}

public SetWeaponAmmo(client, slot1, slot2) {
    new ActiveWeapon = GetEntDataEnt2(client,FindSendPropOffs("CTFPlayer", "m_hActiveWeapon"));
    if(IsValidEntity(ActiveWeapon)) {
        SetEntData(ActiveWeapon,FindSendPropOffs("CBaseCombatWeapon", "m_iClip1"),slot1,4);
        SetEntData(client,FindSendPropOffs("CTFPlayer", "m_iAmmo")+4,slot2,4);
        SetEntData(client,FindSendPropOffs("CTFPlayer", "m_iAmmo")+8,slot2,4);
    }
}

public Action:SetWeaponState(client, bool:input) {
    new ActiveWeapon = GetEntDataEnt2(client,FindSendPropOffs("CTFPlayer", "m_hActiveWeapon"));
    new iEntity = g_PlayerVisibleWeapon[client];
    if (IsValidEntity(ActiveWeapon)) {
        if (input == true) {
            SetEntityRenderColor(ActiveWeapon, 255, 255, 255, 255);
            SetEntityRenderMode(ActiveWeapon, RENDER_NORMAL);
            SetEntProp(client, Prop_Send, "m_bDrawViewmodel", 1);
        }
        else {
            SetEntityRenderColor(ActiveWeapon, 255, 255, 255, 0);
            SetEntityRenderMode(ActiveWeapon, RENDER_TRANSCOLOR);
            SetWeaponAmmo(client, 0, 0);
            SetEntProp(client, Prop_Send, "m_bDrawViewmodel", 0);
        }
    }
    if (iEntity > 0 && IsValidEntity(iEntity)) {
        if (input == true) {
            SetEntityRenderColor(iEntity, 255, 255, 255, 255);
            SetEntityRenderMode(iEntity, RENDER_NORMAL);
        }
        else {
            SetEntityRenderColor(iEntity, 255, 255, 255, 0);
            SetEntityRenderMode(iEntity, RENDER_TRANSCOLOR);
        }
    }
}

public Action:GiveClientWeapon(client, String:weapon[128], slot, bool:input) {
    if (IsValidClient(client) && IsPlayerAlive(client)) {
        TF2_RemoveWeaponSlot(client, slot);
        
        new wpn = SDKCall(hGiveNamedItem, client, weapon, 0, 0);
        EquipPlayerWeapon(client, wpn);
        
        SetClientSlot(client, slot);

        if (input == true) {
            if (StrEqual(weapon, "tf_weapon_revolver")) SetWeaponAmmo(client, 6, 24);
            if (StrEqual(weapon, "tf_weapon_flamethrower")) SetWeaponAmmo(client, 200, 200);
            if (StrEqual(weapon, "tf_weapon_grenadelauncher")) SetWeaponAmmo(client, 4, 16);
            if (StrEqual(weapon, "tf_weapon_syringegun_medic")) SetWeaponAmmo(client, 40, 150);
            if (StrEqual(weapon, "tf_weapon_sniperrifle")) SetWeaponAmmo(client, 25, 25);
            if (StrEqual(weapon, "tf_weapon_rocketlauncher")) SetWeaponAmmo(client, 4, 20);
            if (StrEqual(weapon, "tf_weapon_pipebomblauncher")) SetWeaponAmmo(client, 8, 24);
            if (StrEqual(weapon, "tf_weapon_minigun")) SetWeaponAmmo(client, 200, 200);			
			}
        
        // Create a model everyone else can oodle at
        RemoveFakeWeapon(client);
        
        decl String:strModel[PLATFORM_MAX_PATH];
        Format(strModel, sizeof(strModel), "");
        
        if (StrEqual(weapon, " ")) Format(strModel, sizeof(strModel), "models/weapons/...");	
        if (!StrEqual(strModel, "")) {
            new iEntity = SDKCall(hGiveNamedItem, client, weapon, 0, 0);
            EquipPlayerWeapon(client, wpn);
            if (iEntity > 0 && IsValidEdict(iEntity)) {
                
                SetEntityModel(iEntity, strModel);
                g_PlayerVisibleWeapon[client] = iEntity;
            }
        }
        SetWeaponState(client, input);
    }
}

RemoveFakeWeapon(&iClient) {
    if (GetConVarBool(ww_log)) LogMessage("Removing all client weapons");
    new iEntity = g_PlayerVisibleWeapon[iClient];
    g_PlayerVisibleWeapon[iClient] = -1;
    
    if (iEntity > 0 && IsValidEdict(iEntity)) {
        RemoveEdict(iEntity);
    }
}

public Action:RemoveClientSlot(i, slot) {
    if (IsValidClient(i) && (IsPlayerAlive(i))) {
        TF2_RemoveWeaponSlot(i, slot);
    }
}

public Action:RemoveAllWeapons() {
    if (GetConVarBool(ww_log)) LogMessage("Removing all weapons");
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && (IsPlayerAlive(i))) {
            RemoveClientWeapons(i);
        }
    }
}

DisableClientWeapons(client) {
    if (IsValidClient(client) && (IsPlayerAlive(client))) {
        RemoveClientWeapons(client);
        
        if (SpecialRound == 6 && (!(StrEqual(minigame, "SimonSays") && simon_action == 0))) {
            GiveClientWeapon(client, "tf_weapon_bat", 2, true);
            SetClientSlot(client, 2);
        }
    
        else {
            new TFClassType:class = TF2_GetPlayerClass(client);
            if (class == TFClass_Scout) GiveClientWeapon(client, "tf_weapon_bat", 2, false);
            if (class == TFClass_Soldier) GiveClientWeapon(client, "tf_weapon_shovel", 2, false);
            if (class == TFClass_Pyro) GiveClientWeapon(client, "tf_weapon_fireaxe", 2, false);
            if (class == TFClass_DemoMan) GiveClientWeapon(client, "tf_weapon_bottle", 2, false);
            if (class == TFClass_Heavy) GiveClientWeapon(client, "tf_weapon_minigun", 0, false);
            if (class == TFClass_Engineer) GiveClientWeapon(client, "tf_weapon_wrench", 2, false);
            if (class == TFClass_Medic) GiveClientWeapon(client, "tf_weapon_bonesaw", 2, false);
            if (class == TFClass_Sniper) GiveClientWeapon(client, "tf_weapon_club", 2, false);
            if (class == TFClass_Spy) GiveClientWeapon(client, "tf_weapon_revolver", 0, false);

            SetWeaponState(client, false);
        }
    }
}

// This function is meant to handle player wearables.
// Ideally it would call some kind of way to re-add a player's wearables, but until we find a way to do that, removing them will do.

public Action:HandlePlayerItems(iClient) {
    RemoveHeadgear(iClient);
    RemoveDemoShield(iClient);
    RemovePowerupBottle(iClient);	
    SetEntProp(iClient, Prop_Send, "m_nBody", 0);
}

RemoveHeadgear(iClient) {
    if (!IsValidClient(iClient)) return;
    if (!IsPlayerAlive(iClient)) return;

    new iEntity = -1;
    while ((iEntity = FindEntityByClassname2(iEntity, "tf_wearable")) != -1) {
        if (IsItemEntity(iEntity)) {
            RemoveItemEntity(iClient, iEntity);
        }
    }
}

RemoveDemoShield(iClient) {
    if (!IsValidClient(iClient)) return;
    if (!IsPlayerAlive(iClient)) return;

    new iEntity = -1;
    while ((iEntity = FindEntityByClassname2(iEntity, "tf_wearable_demoshield")) != -1) {
        if (IsItemEntity(iEntity)) {
            RemoveItemEntity(iClient, iEntity);
        }
    }
}

RemovePowerupBottle(iClient) {
    if (!IsValidClient(iClient)) return;
    if (!IsPlayerAlive(iClient)) return;

    new iEntity = -1;
    while ((iEntity = FindEntityByClassname2(iEntity, "tf_powerup_bottle")) != -1) {
        if (IsItemEntity(iEntity)) {
            RemoveItemEntity(iClient, iEntity);
        }
    }
}

stock RemoveItemEntity(iClient, iEntity) {
    if (IsValidClient(iClient) && IsItemEntity(iEntity)) {
        if (GetEntPropEnt(iEntity, Prop_Send, "m_hOwnerEntity") == iClient) {
            SDKCall(g_hSdkRemoveWearable, iClient, iEntity);			
        }
        RemoveEdict(iEntity);
    }
}

stock bool:IsItemEntity(iEntity) {
    if (iEntity > 0) {
        if (IsValidEdict(iEntity)) {
            decl String:strClassname[32];
            GetEdictClassname(iEntity, strClassname, sizeof(strClassname));
            if (StrEqual(strClassname, "tf_wearable", false)) return true;
            if (StrEqual(strClassname, "tf_wearable_demoshield", false)) return true;
            if (StrEqual(strClassname, "tf_usableitem", false)) return true;
            if (StrEqual(strClassname, "tf_powerup_bottle", false)) return true;			
        }
    }
    return false;
}

new Handle:GameTextHandle = INVALID_HANDLE;

ShowGameText(const String:strMessage[], const String:strIcon[] = "leaderboard_dominated", Float:life = 35.0) {
    if (GameTextHandle != INVALID_HANDLE) TriggerTimer(GameTextHandle);

    new iEntity = CreateEntityByName("game_text_tf");
    DispatchKeyValue(iEntity,"message", strMessage);
    DispatchKeyValue(iEntity,"display_to_team", "0");
    DispatchKeyValue(iEntity,"icon", strIcon);
    DispatchKeyValue(iEntity,"targetname", "game_text1");
    DispatchKeyValue(iEntity,"background", "0");
    DispatchSpawn(iEntity);
    AcceptEntityInput(iEntity, "Display", iEntity, iEntity);
    GameTextHandle = CreateTimer(life, KillGameText, EntIndexToEntRef(iEntity));
}

public Action:KillGameText(Handle:hTimer, any:iEntityRef)
{
    new iEntity = _:EntRefToEntIndex(iEntityRef);
    if ((iEntity > 0) && IsValidEntity(iEntity)) AcceptEntityInput(iEntity, "kill");
    
    GameTextHandle = INVALID_HANDLE;
    
    return Plugin_Stop;
}

stock ChangeClientTeamAlive(client, team){
    SetEntProp(client, Prop_Send, "m_lifeState", 2);
    ChangeClientTeam(client, team);
    SetEntProp(client, Prop_Send, "m_lifeState", 0);
}


public GetHealtarget(client) {
    if(!IsValidClient(client) || TF2_GetPlayerClass(client) != TFClass_Medic)
        return 0;
    
    new weapon = GetPlayerWeaponSlot(client, 1);
    if (weapon == -1)
        return 0;
        
    new String:classname[64];
    GetEdictClassname(weapon, classname, 64);
    if(!StrEqual(classname, "tf_weapon_medigun"))
        return 0;
    
    return GetEntDataEnt2(weapon, FindSendPropInfo("CWeaponMedigun", "m_hHealingTarget"));
}

GetClientMaxHealth(iClient) {
    return TF2_GetPlayerResourceData(iClient, TFResource_MaxHealth);
}

stock FindEntityByClassname2(startEnt, const String:classname[]) {
    /* If startEnt isn't valid shifting it back to the nearest valid one */
    while (startEnt > -1 && !IsValidEntity(startEnt)) startEnt--;
    return FindEntityByClassname(startEnt, classname);
}