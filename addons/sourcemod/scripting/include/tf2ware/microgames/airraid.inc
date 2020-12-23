// Airraid module

#define Team_Spec 1
#define Team_Red 2
#define Team_Blu 3

#define CTFPROJECTILE_ROCKET "models/weapons/w_models/w_rocket.mdl"
#define TF_PROJECTILE_ROCKET "models/weapons/w_models/w_rocket.mdl"

public Airraid_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, Airraid_justEntered);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, Airraid_End);	
    AddToForward(g_OnAlmostEnd, INVALID_HANDLE, Airraid_CheckDeadPlayers);
    
    SetStateAll(true);
    g_attack = true;
    CreateTimer(0.5, iRocketsFired);     
}

public Airraid_OnMapStart() {
    PrecacheModel(CTFPROJECTILE_ROCKET, true);
    PrecacheModel(TF_PROJECTILE_ROCKET, true);	
}

public Action:iRocketsFired(Handle:hndl, any:data) {
    new Float:pos[3], Float:ang[3];
    ang[0] = 90.0;
    new Float:rand;
    for (new i=-2; i<=2; i++) {
        for (new j=-2; j<=2; j++) {
            rand = GetRandomFloat(200.0, 500.0);
            pos[2] = rand;
            pos[0] = i*244.0;
            pos[1] = j*244.0;
            RocketsFired(pos, ang, 350.0, 300.0, Team_Spec, true);
        }
    }  
}

RocketsFired(Float:vPosition[3], Float:vAngles[3] = NULL_VECTOR, Float:flSpeed = 600.0, Float:flDamage = 450.0, iTeam = Team_Spec, bool:bCritical = false){

    new String:strClassname[32] = "";
    new String:strEntname[32] = "";

    strClassname = "CTFProjectile_Rocket";
    strEntname = "tf_projectile_rocket";
	
    new iRocket = CreateEntityByName(strEntname);
    if(!IsValidEntity(iRocket))
        return -0;
	
    decl Float:vVelocity[3];
    decl Float:vBuffer[3];
    
    GetAngleVectors(vAngles, vBuffer, NULL_VECTOR, NULL_VECTOR);
    
    vVelocity[0] = vBuffer[0]*flSpeed;
    vVelocity[1] = vBuffer[1]*flSpeed;
    vVelocity[2] = vBuffer[2]*flSpeed;
    
    TeleportEntity(iRocket, vPosition, vAngles, vVelocity);
    
    SetEntData(iRocket, FindSendPropOffs(strClassname, "m_nSkin"), (iTeam-2), 1, true);
    SetEntData(iRocket, FindSendPropOffs(strClassname, "m_bCritical"), bCritical, true);
    SetEntDataFloat(iRocket, FindSendPropOffs(strClassname, "m_iDeflected") + 4, flDamage, true); // Credit to voogru
    
    SetVariantInt(iTeam);
    AcceptEntityInput(iRocket, "TeamNum", -1, -1, 0);

    SetVariantInt(iTeam);
    AcceptEntityInput(iRocket, "SetTeam", -1, -1, 0); 
    
    DispatchSpawn(iRocket);
    return iRocket;
}

public Airraid_justEntered(client) {
    if (IsValidClient(client)) {
        SDKHook(client, SDKHook_PreThink, Airblast_RemoveLeftClick);

        SetClientClass(client, "pyro");
        RemoveClientWeapons(client);
        GiveClientWeapon(client, "tf_weapon_flamethrower", 0, true);

        ClientCommand(client, "thirdperson");		
	}
}

public Airraid_CheckDeadPlayers() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && (!(IsPlayerAlive(i))) && IsClientParticipating(i)) SetStateClient(i, false, true);
    }
}

public Airraid_End() {
    for (new i = 1; i <= MaxClients; i++) {
        SDKUnhook(i, SDKHook_PreThink, Airblast_RemoveLeftClick);
	if (IsValidClient(i))	
	ClientCommand(i, "wait; firstperson");
	}
}