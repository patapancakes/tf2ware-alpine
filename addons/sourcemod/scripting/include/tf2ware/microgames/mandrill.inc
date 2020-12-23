// Mandrill module

public Mandrill_OnMinigame() {
    AddToForward(g_justEntered, INVALID_HANDLE, Mandrill_justEntered);
    AddToForward(g_OnEndMinigame, INVALID_HANDLE, Mandrill_End);	
    AddToForward(g_OnGameFrame_Minigames, INVALID_HANDLE, Mandrill_OnGameFrame);
}

public Mandrill_OnGameFrame() {
    for (new i = 1; i <= MaxClients; i++) {
        if (IsValidClient(i) && IsPlayerAlive(i) && IsClientParticipating(i)) {
            decl Float:pos[3];
            GetClientAbsOrigin(i, pos);
            if (pos[1] > 7900.0) SetStateClient(i, true, true);
        }
    }
}

public Mandrill_justEntered(client) {
    if (IsValidClient(client)) {

        SetClientClass(client, "sniper");
        DisableClientWeapons(client);
        
        decl Float:vel[3];
        vel[0] = 0.0;
        vel[1] = 0.0;
        vel[2] = 0.0;
        
        decl Float:pos[3];
        pos[0] = 1900.0; //- float(column*55);
        pos[1] = -6000.0; //- float(row*55);
        pos[2] = -2420.0;

        decl Float:ang[3];
        ang[0] = 0.0;
        ang[1] = 90.0;
        ang[2] = 0.0;
        TeleportEntity(client, pos, ang, vel);
        NoCollision(true);
    }
}

public Mandrill_End() {
    for (new i = 1; i <= MaxClients; i++) {
    }
}