// Kamikaze module

#define WW_BOMB "pl_hoodoo/alarm_clock_ticking_3.wav"
#define WW_BOMB_SCOUT_MODEL "models/tf2ware/bobomb.mdl"
#define PARTICLE_BOMB "cinefx_goldrush_embers"
#define PARTICLE_EXPLODE "cinefx_goldrush_initial_smoke"

public Kamikaze_Init() {
    AddToForward(g_OnMapStart, INVALID_HANDLE, Kamikaze_OnMapStart);
}   

public Kamikaze_OnMapStart() {
    AddFileToDownloadsTable("models/tf2ware/bobomb.dx80.vtx"); 
    AddFileToDownloadsTable("models/tf2ware/bobomb.dx90.vtx"); 
    AddFileToDownloadsTable("models/tf2ware/bobomb.mdl");
    AddFileToDownloadsTable("models/tf2ware/bobomb.phy");
    AddFileToDownloadsTable("models/tf2ware/bobomb.sw.vtx"); 
    AddFileToDownloadsTable("models/tf2ware/bobomb.vvd"); 
    AddFileToDownloadsTable("materials/tf2ware/bobomb.vmt"); 
    AddFileToDownloadsTable("materials/tf2ware/bobomb.vtf");
    AddFileToDownloadsTable("materials/tf2ware/outline.vmt");	
    AddFileToDownloadsTable("materials/tf2ware/outline.vtf");

    PrecacheModel(WW_BOMB_SCOUT_MODEL, true);
    PrecacheSound(WW_BOMB, true);
}

public Kamikaze_OnMinigame() {   
    AddToForward(g_justEntered, INVALID_HANDLE, Kamikaze_justEntered);
    AddToForward(g_OnAlmostEnd, INVALID_HANDLE, Kamikaze_OnAlmostEnd);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, Kamikaze_End);
    AddToForward(g_OnTimerMinigame, INVALID_HANDLE, Kamikaze_TimerMinigame);
    AddToForward(g_PlayerDeath, INVALID_HANDLE, Kamikaze_PlayerDeath);

    randommini = GetRandomInt(0, 2);
    
    new AllPlayers = GetClientCount(true);
    new target = GetRandomInt(1, AllPlayers);
    while ((!IsValidClient(target)) || GetClientTeam(target) < 2 || g_Spawned[target] == false || !IsPlayerAlive(target)) {
        target = GetRandomInt(1, AllPlayers);
    }
    
    EmitSoundToClient(target, WW_BOMB, SOUND_FROM_PLAYER, SNDCHAN_AUTO, SNDLEVEL_NORMAL, SND_NOFLAGS, SNDVOL_NORMAL,GetSoundMultiplier());
    ClientParticle(target, PARTICLE_BOMB, 2.0);
    
    g_Mission[target] = 1;
    g_result = 0;
    SetStateAll(true);
    SetStateClient(target, false);
}

public Kamikaze_End() {
    if ((g_bomb > 0) && IsValidEntity(g_bomb)) {
        AcceptEntityInput(g_bomb, "Kill" );
        g_bomb = 0;
    }

    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            SetVariantString("");
            AcceptEntityInput(i, "SetCustomModel");
            SetVariantInt(0);
            AcceptEntityInput(i, "SetCustomModelRotates");
        }
    }
}

public Kamikaze_OnAlmostEnd() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && (g_Mission[i] == 1) && (g_result >= 2)) SetStateClient(i, true);
    }
}

public Kamikaze_PlayerDeath(client) {
	if (g_Mission[client] == 0) {
        SetStateClient(client, false);
        g_result += 1;
    }
    
	if(IsValidClient(client)) {
        SetVariantString("");
        AcceptEntityInput(client, "SetCustomModel");
        SetVariantInt(0);
        AcceptEntityInput(client, "SetCustomModelRotates");
        CreateTimer(0.0, Kamikaze_Scout_Ragdoll, client);	
    }
}

public Action:Kamikaze_Scout_Ragdoll(Handle:timer, any:client) {
    if(IsValidClient(client)) {
        new rag = GetEntPropEnt(client, Prop_Send, "m_hRagdoll");
        if (rag > MaxClients && IsValidEntity(rag))
            AcceptEntityInput(rag, "Kill");
    }
    return Plugin_Handled;	
}

public Kamikaze_TimerMinigame(timer) {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && IsPlayerAlive(i) && (g_Mission[i] == 1)) {
            if (timer > 2) {
                ClientParticle(i, PARTICLE_BOMB, 2.0);
            }
            if (timer == 2) {
                ClientParticle(i, PARTICLE_EXPLODE, 8.0);
                decl Float:pos[3];
                GetClientAbsOrigin(i, pos);
                        
                PyroExplode(pos);
                ExplosionDamage(pos);
                if ((g_bomb > 0) && IsValidEntity(g_bomb)) {
                    AcceptEntityInput(g_bomb, "Kill" );
                    g_bomb = 0;
                }
            }
        }
    }
}

public Kamikaze_justEntered(client) {
    if (IsValidClient(client)) {
        if (g_Mission[client] == 0) {
            SetClientClass(client, "heavy");
            DisableClientWeapons(client);
        }
        if (g_Mission[client] == 1) {
            SetClientClass(client, "scout");
            RemoveClientWeapons(client);

            SetVariantString(WW_BOMB_SCOUT_MODEL);
            AcceptEntityInput(client, "SetCustomModel");
            SetVariantInt(1);
            AcceptEntityInput(client, "SetCustomModelRotates");            

            CreateTimer(0.1, Kamikaze_Scout_Ragdoll, client);
        }    
        if ((g_bomb > 0) && IsValidEntity(g_bomb)) {
            AcceptEntityInput(g_bomb, "Kill" );
            g_bomb = 0;    
        }
    }
}