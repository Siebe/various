//https://forums.soldat.pl/index.php?topic=26477.0

//XtraCommands v1.0 for Soldat by JFK
//Thanks to: mikebm

//XtraCommands is a script that contains extra (admin-)commands that can also be used by other scripts, this means
//that every command in the script can be called by CrossFunc.

//What's in v1.0
//OnEndRound 'event':
//This is an unofficial event like OnMapChange. It should take place AFTER the end of the round, and BEFORE the next
//map is started. Now this is a pretty long period of time wich make it a rather vague event. It takes one argument
//(boolean), which will be explained later
//TempMute Command:
//The Tempmute command is used to temporary /gmute a player. There are three options about when the player leaves
//muted: 1. Player gets unmuted and banned, 2. Player stays muted (for aslong as /gmute works, i have NO idea),
//3. the player gets away with it. Tempmute uses the EndOfRound event so it will not ban anyone who is kicked by the
//server for having to download the next map.
//TempMute is currenly used by JFK's NoobLottery.pas (MUST BE ACTIVE WHEN USING THIS SCRIPT) and SubAdmin.pas
//AdminCommand usage: /tempmute [time] [ID] [when player leaves(ban, muted, none; optional)]
//LineMap Command:
//This will line up the next map, once a map is lined it can not be canceled or overruled (in case admins fight about
//it :p). It uses the OnEndRound event to determine WHEN the next map should be started. The argument of OnEndRound
//will decide if it is the right time for the lined map to start.
//LineMap is currently used by JFK's SubAdmin.pas
//AdminCommand usage: /linemap [map] (not case-sensitive, without .pms)

//What's in v1.1
//Fixed : GetMaxScore was called BEFORE /loadcon, now every new map GetMaxScore will be called
//Fixed : Due to some error a player can be muted permanently, bigger problem was that a player could be
//banned permanently also. An extra security has been built in to prevent that.

//==============================================================================Constants

const

XCVERSION = 11;                //Version number to be checked by other scripts using CrossFunc
MAXSLOTS = 32;                 //Maximum number of slots in the server
TM = true;                     //TempMute Active/Unactive
LM = true;                     //LineMap Active/Unactive
BWL_DEFAULT = true;            //Default Ban the player when leaving
UWL_DEFAULT = true;            //Default Unmute the player when leaving (is automaticly true if BanWhenLeave is true)
INFORM_PLAYER = true;          //Inform the Player when tempmuted
SOLDAT_INI = './soldat.ini';   //This might have to be changed if the server uses any onther configfile for Soldat

//-----Colors
cPersonal  = $EEFF0AFB;
cApersonal = $EE2EAB77;


//==================================================================================Types

type

MuteInfo = record
  Time : integer;
  BanWhenLeave, 
  UnMuteWhenLeave
  :boolean;
end;


//=======================================================================Global Variables

var

i : integer;
MapInLine : string;
EndOfRound : boolean;
Mute : array [1..MAXSLOTS] of MuteInfo;
MaxScore : integer;


//=========================================================================Own Procedures

//------------------WriteAdmin Procedure|
//This procedure is designed to write a |
//message to an admin whether it is an         |
//ingame-admin or a TCP-connected Admin.|
//--------------------------------------|

Procedure WriteAdmin(ID: Byte; Text: string);

begin
  if (ID<255) then WriteConsole(ID, Text, cApersonal);
  if ((ID>254) or (ID=0)) then WriteLn(Text);
end;


//-------------------getVersion Function|
//To be used by other scripts to check  |
//if this is the right version of       |
//XtraCommands (not used in v1.0)       |
//--------------------------------------|

Function getVersion(): integer;

begin
result := XCVERSION;
end;


//---------------------LineMap Procedure|
//This Procedure will line up the next  |
//map. Can be used by other scripts     |
//--------------------------------------|

Procedure LineMap(ID: byte; Map: string);

begin
  if (LM) then begin
    if (MapInLine <> '*none*') then WriteAdmin(ID, 'There is already a map in line')
    else begin
      try
        if (ReadFile('./maps/'+Map+'.PMS') <> '') then begin
          MapInLine := Map;
          WriteAdmin(ID, Map+ ' will be the next map to be played');
        end else WriteAdmin(ID, 'Map not found or empty file');
      except
        WriteAdmin(ID, 'Map not found');
      end;
    end;
  end;
end;


//--------------------TempMute Procedure|
//Will temproray mute a player. Can also|
//be used by other scripts              |
//--------------------------------------|

Procedure TempMute(ID, AdminID: byte; Time: integer; BanWhenLeave, UnMuteWhenLeave: Boolean);

begin
  if (TM) then begin
    if (GetPlayerStat(ID, 'Active') = false) then WriteAdmin(AdminID, 'Player is not ingame')
    else begin
      Command('/gmute ' +inttostr(ID));
      Mute[ID].Time := Time*60;
      Mute[ID].BanWhenLeave := BanWhenLeave;
      if (BanWhenLeave) then UnMuteWhenLeave := true;
      Mute[ID].unMuteWhenLeave := unMuteWhenLeave;
      if (INFORM_PLAYER) then WriteConsole(ID, 'You have been muted for ' +inttostr(time)+ ' minutes',cPersonal);
      if (BanWhenLeave) then begin
        if (INFORM_PLAYER) then WriteConsole(ID, 'If you leave you will be banned for the same amount of time', cPersonal)
        WriteAdmin(AdminID, 'Player has been muted for '+inttostr(time)+' minutes and will be banned on leave');
      end else begin
        if (UnMuteWhenLeave=false) then begin
          if (INFORM_PLAYER) then Writeconsole(ID, 'If you leave you wont be unmuted', cPersonal);
          WriteAdmin(AdminID, 'Player has been muted for '+inttostr(time)+' minutes and will not be unmuted on leave');
        end else WriteAdmin(AdminID, 'Player has been muted for '+inttostr(time)+' minutes, but will be unmuted on leave');
      end;
    end;
  end;
end;


//------------------UnTempMute Procedure|
//Will be called by /ungmute and will   |
//'untempmute' a player                 |
//--------------------------------------|

Procedure UnTempMute(ID, AdminID: byte);

begin
  if (TM) then begin
    if (GetPlayerStat(ID, 'Active')= false) then WriteAdmin(AdminID, 'Player is not ingame')
    else begin
      if (Mute[ID].time > 0) then begin
        Mute[ID].Time := 0;
        if (INFORM_PLAYER) then WriteConsole(ID, 'You have been unmuted by an admin.', cPersonal);
        WriteAdmin(AdminID, 'Player has been unmuted');
      end;
    end;
  end;
end;


//-------------TempMuteCommand Procedure|
//Will be called by /tempmute and will  |
//call TempMute on its turn             |
//--------------------------------------|

Procedure TempMuteCommand(ID: byte; Text: string);

begin
  if (TM = false) then WriteAdmin(ID, 'Sorry, this command has been de-activated') else begin
    if (GetPiece(Text, ' ', 3) = 'ban') then TempMute(strtoint(GetPiece(text, ' ', 2)), ID, strtoint(GetPiece(text, ' ', 1)), true, false) else
    if (GetPiece(Text, ' ', 3) = 'muted') then TempMute(strtoint(GetPiece(text, ' ', 2)), ID, strtoint(GetPiece(text, ' ', 1)), false, false) else
    if (GetPiece(Text, ' ', 3) = 'none') then TempMute(strtoint(GetPiece(text, ' ', 2)), ID, strtoint(GetPiece(text, ' ', 1)), false, true) else
    TempMute(strtoint(GetPiece(text, ' ', 2)), ID, strtoint(GetPiece(text, ' ', 1)), BWL_DEFAULT, UWL_DEFAULT);
  end;
end;


//-----------------GetMaxScore Procedure|
//Is to determine the Maximum Score of  |
//any gamemode. Is needed to see when   |
//OnEndRound should be called in some   |
//gamemodes. Will also be called every  |
//time the gamemode will change         |
//--------------------------------------|

Procedure GetMaxScore();

begin
  MaxScore := 0;
  case GameStyle of
    2 : MaxScore := strtoint(ReadINI(SOLDAT_INI, 'GAME', 'Pointmatch_Limit', '-1'));
    3 : MaxScore := strtoint(ReadINI(SOLDAT_INI, 'GAME', 'Capture_Limit', '-1'));
    5 : MaxScore := strtoint(ReadINI(SOLDAT_INI, 'GAME', 'Infiltration_Limit', '-1'));
    6 : MaxScore := strtoint(ReadINI(SOLDAT_INI, 'GAME', 'Hold_Limit', '-1'));
  end;
  if (Maxscore = -1) then WriteAdmin (0, 'Error: couldn`t find the maximum score');
end;


//------------------OnEndRound Procedure|
//'Unofficial' Event wich will be called|
//at the end of every round. The        |
//argument will tell the procedure to   |
//line up the next map. In the case of  |
//a round out of time this should happen|
//slightly later, as example.           |
//--------------------------------------|

Procedure OnEndRound(LineTheMap: boolean);

begin
  //WriteConsole(0, 'END OF ROUND', cApersonal);
  if (TM) then EndOfRound := true;
  if ((LM) and (MapInLine <> '*none*') and (LineTheMap)) then begin
    Command('/map '+MapInLine);
    MapInLine := '*none*';
  end;
end;


//==============================================================SoldatEvent Procedures

procedure ActivateServer();

begin
  for i:=1 to MAXSLOTS do Mute[i].time := 0;
  EndOfRound := false;
  GetMaxScore();
  MapInLine := '*none*'
end;


procedure AppOnIdle(Ticks: integer);

begin
  if (TM) then begin
    for i:=1 to MAXSLOTS do begin
      if (Mute[i].time > 0)  then begin
        dec(Mute[i].time, 1);
        if (Mute[i].time < 1) then begin
          command('/ungmute '+inttostr(i));
          if (INFORM_PLAYER) then WriteConsole(i, 'Your mute-time is over', cPersonal);
        end;
      end;
    end;
  end;
  if (TimeLeft = 0) then begin
    OnEndRound(false);  // it IS the end of the round, but the next map should not start YET
    Sleep(120);
    OnEndRound(true);   //NOW the next (lined) map can be started, right after the original next map is called
  end;
  if ((GameStyle >= 5) and ((AlphaScore >= MaxScore) or (BravoScore >= MaxScore))) then begin
    OnEndRound(false);
    Sleep(120);
    OnEndRound(true);
  end;
end;


procedure OnMapChange(NewMap: string);

begin
  if ((TM) and (MapInLine = '*none*')) then EndOfRound := false;
  if ((LM) and (MapInLine <> '*none*')) then begin
    Command('/map '+MapInLine);
    MapInLine := '*none*';
    GetMaxScore();
  end;
end;


procedure OnJoinGame(ID, Team: byte);

begin
  if (TM) then Mute[ID].time := 0;
end;


procedure OnLeaveGame(ID, Team: byte; Kicked: boolean);
var
MuteMin : integer;

begin
  if (TM) then begin
    if (Mute[ID].Time > 0) then begin
      MuteMin := round(Mute[ID].Time/60);
      Mute[ID].Time := 0;
      if (Mute[ID].UnMuteWhenLeave) then Command('/ungmute '+inttostr(ID));
      if ((Mute[ID].BanWhenLeave) and (EndofRound = false)) then begin
        if (MuteMin < 1) then MuteMin := 30; //extra security to ensure player wont get banned permanently (half hour instead)
        BanPlayerReason(ID, MuteMin, 'Left while tempmuted');
      end;
    end;
  end;
end;


function OnCommand(ID: Byte; Text: string): boolean;
var
comm : string;

begin
  Result := false;
  comm := GetPiece(text, ' ', 0);
  case comm of
    '/tempmute' : if (TM) then TempMuteCommand(ID, Text) else WriteAdmin(ID, 'This command has been de-activated');
    '/linemap'  : if (LM) then LineMap(ID, getPiece(text, ' ', 1)) else WriteAdmin(ID, 'This command has been de-activated');
    '/gmute'    : UnTempMute (strtoint(getPiece(text, ' ', 1)), ID);    //first untempmute the player before 'permanent' /gmute
    '/ungmute'  : UnTempMute (strtoint(getPiece(text, ' ', 1)), ID);
    '/nextmap'  : begin
                    if (MapInLine <> '*none*') then Result := true;
                    OnEndRound(true);
                  end;
    '/map'      : begin
                    if (MapInLine <> '*none*') then Result := true;
                    OnEndRound(true);
                  end;
    '/restart'  : OnEndRound(false);
    '/loadcon'  : begin
                    OnEndRound(false);
                  end;
    '/gamemode' : begin
                    GameStyle := strtoint(GetPiece(Text, ' ', 1));
                    GetMaxScore();
                  end;
  end;
end;


procedure OnPlayerKill(Killer, Victim: byte; Weapon: string);

begin
  if ((GameStyle < 2) or (Gamestyle =4)) then begin
    if (GetPlayerStat(Killer, 'Kills') >= ScoreLimit) then OnEndRound(true);
  end;
  if (GameStyle = 2) then begin
    if (MaxScore > -1) then begin
      case GetPlayerStat(Killer, 'Team') of
        1 : if (AlphaScore >= MaxScore) then OnEndRound(true);
        2 : if (BravoScore >= MaxScore) then OnEndRound(true);
        3 : if (CharlieScore >= MaxScore) then OnEndRound(true);
        4 : if (DeltaScore >= MaxScore) then OnEndRound(true);
      end;
    end;
  end;
end;


procedure OnFlagScore(ID, TeamFlag: byte);
begin
  if ((Gamestyle = 3) or (Gamestyle = 5)) then begin
    if ((TeamFlag = 2) and (AlphaScore >= MaxScore)) then OnEndRound(true);
    if ((TeamFlag = 1) and (BravoScore >= MaxScore)) then OnEndRound(true);
  end;
end;


//================================================================================End
//
//
//Made By:
//
//                                             ``..---.`                                                                       
//               `/osyssssssyyyyhhyo:   .yNMNNMMMMMMMMMMMNds+.     /ydh-          -oyy:                                        
//             /MMMMMMMMMMMMMMMMMMMM- .NMMMMMMMMMMMMMMMMMMMMN`  `mMMMMo       `+mMMMMh                                        
//              yMMMMMMMMMMMMMMMMMMm/  dMMMMNdhyysssssyydNMms.   yMMMMd      :yMMMMMN+                                         
//               ..---:hMMMMN-..--.   oMMMMM.                   /MMMMN.   `+mMMMMMd/                                           
//                    -MMMMM/        -MMMMMo                   .NMMMM:  :yMMMMMNy-                                             
//                   `mMMMMy        `mMMMMd  ``...```          hMMMMs`omMMMMMd+`                                               
//                   yMMMMm`        sMMMMMmNMMMMMMMMMMNh      oMMMMMhMMMMMNs-                                                  
//                  /MMMMN-        -MMMMMMMMMMMMMMMMMMMh     :MMMMMMMMMMh/                                                     
//                 .NMMMM/        `mMMMMMNdhyyyyyhhddy/     `mMMMMMMMm+`                                                       
//                `mMMMMs         sMMMMN.                   hMMMMMMMMN+                                                        
//    -//-        yMMMMh         :MMMMM/                   +MMMMMMMMMMMh.                                                      
//  .mMMMM.      +MMMMd`        `mMMMMh                   -MMMMMyhMMMMMMN+                                                     
//  mMMMMN      -MMMMd`         yMMMMN`                  `mMMMMm  +NMMMMMMd:                                                   
// `MMMMMMd/`  :NMMMh`         /MMMMM/                   sMMMMM-   .dMMMMMMMms:                                                
//  sMMMMMMMMmmMMMMs          `NMMMMy                   :MMMMMo      +mMMMMMMMM`                                               
//   :dMMMMMMMMMMd:           yMMMMm`                   mMMMMd         /yNMMMd-                                                
//     -+ydmNmds-             /hhs/                     /dmh+             `.`     TM
//
//
//DISCLAIMER:
//The creator(s) of this script can be in no way responsible to any damage, software-wise or hardware-wise, of
//the system it runs on, nor to any other systems connected to it. Running this script is entirely at your own risk.
//Everybody is allowed to redistribute the entire and/or parts of the script as long as the original creator(s)
//will be credited.





