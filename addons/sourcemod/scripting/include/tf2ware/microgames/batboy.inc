// batboy module

public Batboy_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, Batboy_justEntered);
    AddToForward(g_OnAlmostEnd, INVALID_HANDLE, Batboy_CheckDeadPlayers);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, Batboy_End);

    randommini = GetRandomInt(0, 5);
    SetStateAll(true);
    g_attack = true;
    
    SetConVarInt(FindConVar("mp_friendlyfire"), 1);
}

public Action:Batboy_OnTakeDamageClient(victim, &attacker, &inflictor, &Float:damage, &damagetype) {
    if (IsValidClient(attacker) && IsValidClient(victim)) {
	SetStateClient(attacker, true, true);
	damage = 25.0;
    }
    return Plugin_Changed;
}

public Batboy_justEntered(client) {
    if (IsValidClient(client)) {
        SDKHook(client, SDKHook_OnTakeDamage, Batboy_OnTakeDamageClient);

        RemoveClientWeapons(client);		
        if (randommini == 0) {
        SetClientClass(client, "medic");
        GiveClientWeapon(client, "tf_weapon_bonesaw", 2, true);
		}
        if (randommini == 1) {
            SetClientClass(client, "heavy");
            GiveClientWeapon(client, "tf_weapon_fists", 2, true);
        }
        if (randommini == 2) {
            SetClientClass(client, "scout");
            GiveClientWeapon(client, "tf_weapon_bat", 2, true);
        }		
        if (randommini == 3) {
            SetClientClass(client, "pyro");
            GiveClientWeapon(client, "tf_weapon_fireaxe", 2, true);
        }
        if (randommini == 4) {
            SetClientClass(client, "soldier");
            GiveClientWeapon(client, "tf_weapon_shovel", 2, true);
        }
        if (randommini == 5) {
            SetClientClass(client, "engineer");
            GiveClientWeapon(client, "tf_weapon_wrench", 2, true);
        }
		
        decl Float:vel[3];
        vel[0] = 0.0;
        vel[1] = 0.0;
        vel[2] = 0.0;
        
        new total = GetActivePlayers();
        new posa = 360 / total * (g_Id[client]-1);
        
        decl Float:pos[3];
        pos[0] = 3546.0   + (Cosine(DegToRad(float(posa)))*220.0);
        pos[1] = 94.0  - (Sine(DegToRad(float(posa)))*220.0);
        pos[2] = -520.0;

        decl Float:ang[3];
        ang[0] = 0.0;
        ang[1] = float(180-posa);
        ang[2] = 0.0;
        TeleportEntity(client, pos, ang, vel);
    }
}

public Batboy_CheckDeadPlayers() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && (!(IsPlayerAlive(i))) && IsClientParticipating(i)) SetStateClient(i, false, true);
    }
}

public Batboy_End() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i)) {
            SDKUnhook(i, SDKHook_OnTakeDamage, Batboy_OnTakeDamageClient);
        }
    }
    SetConVarInt(FindConVar("mp_friendlyfire"), 0);
}	