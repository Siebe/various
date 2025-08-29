//NoobLottery v1.1 for Soldat made by JFK.
//Thanks to: Boblekonvolutt and KeyDon

//This script uses Regular Expressions to filter out the following three words: Noob, Newb and Fag. However the expressions
//are complex enough to filter out a lot of other things such as: nob, n o o b, n*o*o*b, and even a lot of 'leet-speak' as
//|\|()()|3. It uses two files with expressions. One file (RE.txt) contains al kinds of forms of each letter to filter out.
//The second file (RE_exc.txt) contains a lot of exceptions wich will be filtered out BEFORE the script checks for the
//'banned' words. The exceptions file is rather huge (mainly because this script also detects spaces between words) and
//the file may be never finished. It is based on a dictionary (morewords.com) and a little experience of non-official words.
//It will most likely not work on any foreign languages (hear me.. im dutch). If you find that the Lottery is triggered by
//a 'pretty normal' word or common name, please contact JFK on EliteCTF.com or SoldatForums.com.
//Commands for this script: /lottery [on/off] /nlsilent [on/off] /nlprint /nlload

//You can change the 'prices' and chances by changing the constants under this text

//NoobLottery will need XtraCommands.pas (v1.0) to function. If you don't have that script, check out SoldatForums.com
//(search for XtraCommands, topic titles only) or contact JFK on EliteCTF.com or SoldatForums.com.

//In v1.1:
//Fixed the Overload-RegExp-Bug. It seems that one can overload the RegExpMatch function. In this script it usually happens
//when someone says 'noooooooooooooo'. An extra, simpler RegExpReplace function has been installed to remove the overload of
//vowels.
//Fixed PrintReFiles only printing from RE.txt
//Added some extra messages for when TESTING_MODE is true
//Siilent mode is now off by default


//==============================================================================Constants

const


FIRST_MUTE = 10;	//Highest Mute Price time in minutes(4/100 chance)
SECOND_MUTE = 5;	//Second Mute Price time in minutes(15/100 chance)
THIRD_MUTE = 3;		//Third Mute Price time in minutes(30/100 chance)
THE_BAN = 5;		//Grand Ban Price time in minutes(1/100 chance)

CH_NOTHING = 30;    //Chance of getting away with it (CH_NOTHING/total sum of all chances)
CH_THIRD = 35;      //Chance of third Mute Price
CH_SECOND = 20;     //Chance of second Mute Price
CH_FIRST = 14;      //Chance of highest Mute Price
CH_GRAND = 1;       //Chance of Grand Ban Price

TEST_MODE = false;         //Purely for testing the Regular Exceptions else should be false, do make sure that the Lottery is on
SILENT_DEFAULT = false;     //If false, then every player will see when the lottery is used
BAN_WHEN_LEAVE = true;	   //Ban player when leaving muted for (almost) same amount of time
UNMUTE_WHEN_LEAVE = true;  //Player will be unmuted when leaving, is always true if BAN_WHEN_LEAVE is true
LOTTERY_DEFAULT = true;    //NoobLottery on or off by default

NR_OF_SLOTS = 32;               //maximum number of slots in server including bots


refile = './scripts/NoobLottery/RE.txt';       //file containing all letters plus some of their 'leet' subtitutes
excfile = './scripts/NoobLottery/RE_exc.txt';  //file containing all exceptions
TempMuteProcedure = 'XtraCommands.TempMute';  //Location in ./scripts/ of the TempMute Procedure for CrossFunc

//------------------colors
cAdmin = $EE878900;
cWarning = $FFFE5D5D;
cLottery = $E035FF2A;
cTest = $00FFFFFF;


//=======================================================================Global Variables

var
i: integer;
RegExps, Excepts : TstringArray; //arrays for letter-expressions and exceptions

n, o, e, u, a, w, b, f, g, pun : string;  //strings per letter (just to make it easier to combine them)

Lottery, SilentMode : Boolean;     //lottery and SilentMode on or off


//=================================================================Third Party Procedures

//---------------------Xslplit by KeyDon|
//http://www.soldatforums.com/          |
//index.php?topic=12742.0               |
//--------------------------------------|

function xsplit(const source: string; const delimiter: string):TStringArray;
var
i,x,d:integer;
s:string;
begin
d:=length(delimiter);
x:=0;
i:=1;
SetArrayLength(Result,1);
while(i<=length(source)) do begin
s:=Copy(source,i,d);   
    if(s=delimiter) then begin
    inc(i,d);
    inc(x,1);
    SetArrayLength(result,x+1);
    end else begin       
    result[x]:= result[x]+Copy(s,1,1);
    inc(i,1);
  end;
end;
end;


//=========================================================================Own Procedures


//-----------------LoadReFiles Procedure|
//Will read the expression-files and put|
//it into arrays and strings            |
//--------------------------------------|

Procedure LoadReFiles();

begin
  SetArrayLength(RegExps,0);
  SetArrayLength(Excepts,0);
  RegExps := xsplit(ReadFile(refile), chr(13)+chr(10));
  n := GetPiece(RegExps[0], ' ',1);
  o := GetPiece(RegExps[1], ' ',1);
  e := GetPiece(RegExps[2], ' ',1);
  u := GetPiece(RegExps[3], ' ',1);
  a := GetPiece(RegExps[4], ' ',1);
  w := GetPiece(RegExps[5], ' ',1);
  b := GetPiece(RegExps[6], ' ',1);
  f := GetPiece(RegExps[7], ' ',1);
  g := GetPiece(RegExps[8], ' ',1);
  pun := GetPiece(RegExps[9], ' ',1);
  Excepts := xsplit(ReadFile(excfile), chr(13)+chr(10))
end;


//----------------PrintReFiles Procedure|
//Show all strings of the files in the  |
//console. Used for testing, but now    |
//there are too much strings to read    |
//in-game. Still readable trough ARSSE  |
//--------------------------------------|

Procedure PrintReFiles();

begin
  WriteConsole (0, 'REfile:', $00FFFFFF);
  for i:=0 to ArrayHigh(RegExps) do begin
    WriteConsole(0, inttostr(i)+' : '+RegExps[i], $00FFFFFF);
    WriteLn(inttostr(i)+' : '+RegExps[i]);
  end;
  WriteConsole (0, 'Exceptions:', $00FFFFFF);
  for i:=0 to ArrayHigh(Excepts) do begin
    WriteConsole(0, inttostr(i)+' : '+Excepts[i], $00FFFFFF);
    WriteLn(inttostr(i)+' : '+Excepts[i]);
  end;
end;


//-----------------WonTempMute Procedure|
//Gives the player (and others, if      |
//silent-mode is off) a message that    |
//a mute is won. Also calls tempmute    |
//from XtraCommands.pas                 |
//--------------------------------------|

procedure WonTempMute(ID: byte; Time: integer);
begin
  if (SilentMode) then WriteConsole(ID, 'NoobLottery: Congratulations! You won a mute for ' + inttostr(Time) + ' minutes!',cLottery)
  else WriteConsole(0,'NoobLottery: Congratulations, ' + IDToName(ID) + '! You won a mute for ' + inttostr(Time) + ' minutes!',cLottery);
  CrossFunc([ID, 255, Time, BAN_WHEN_LEAVE, UNMUTE_WHEN_LEAVE], TempMuteProcedure);  //Calls a function within XtraCommands.pas
end;


//------------------WonTempBan Procedure|
//Gives the player (and others, if      |
//silent-mode is off) a message that    |
//a ban is won. Will ban the player.    |
//--------------------------------------|

procedure WonTempBan(ID: byte);
begin
  if (SilentMode) then WriteConsole(ID, 'NoobLottery : Congratulations! You won The Grand Price, a ' + inttostr(THE_BAN) + ' minute BAN!',cLottery)
  else WriteConsole(0,'NoobLottery: Congratulations, ' + IDToName(ID) + '! The Grand Price, a ' + inttostr(THE_BAN) + ' minute BAN!',cLottery);
  BanPlayer(ID,THE_BAN);
end;

//----------------NooobLottery Procedure|
//The actual 'lottery' will give a      |
//'price', or not, based on random()    |
//--------------------------------------|

procedure NoobLottery(ID: byte);
var
Rnumber: integer;
Ch_Total: integer;
begin
  Ch_total := CH_NOTHING + CH_THIRD + CH_SECOND + CH_FIRST + CH_GRAND;
  Rnumber := Random(0, (Ch_Total));
  if (Rnumber < CH_NOTHING) then begin
    if (SilentMode) then WriteConsole(ID, 'NoobLottery: You won nothing, too bad! Better luck next time!', cLottery) else
    WriteConsole(0,'NoobLottery: '+ IDToName(ID) + ' won nothing, too bad! Better luck next time!',cLottery);
  end else
  if ((Rnumber >= CH_NOTHING) and (Rnumber < (CH_NOTHING+CH_THIRD))) then WonTempMute(ID, THIRD_MUTE) else
  if ((Rnumber >= (CH_NOTHING+CH_THIRD)) and (Rnumber < (CH_NOTHING+CH_THIRD+CH_SECOND))) then WonTempMute(ID, SECOND_MUTE) else
  if ((Rnumber >= (CH_NOTHING+CH_THIRD+CH_SECOND)) and (Rnumber < Ch_Total-CH_GRAND)) then WonTempMute(ID, FIRST_MUTE) else
  if (Rnumber = Ch_total-CH_GRAND) then WonTempBan(ID) else
  WriteConsole(0, 'Lottery ERROR (lucky bastard)', cWarning);
end;


//=================================================================SoldatEvent Procedures

procedure ActivateServer();
begin
  Lottery := LOTTERY_DEFAULT;
  SilentMode := SILENT_DEFAULT;
  //A waring in case of /recompile
  if Lottery = true then WriteConsole(0, 'This server runs the NoobLottery-script, better watch your words.', cWarning);
  LoadReFiles;
end;


procedure OnJoinGame(ID, Team: byte);
begin
  if Lottery = true then
  //A warning for everybody who joines
  WriteConsole(ID, 'This server runs the NoobLottery-script, better watch your words.',cWarning);
end;


function OnCommand(ID: Byte; Text: string):boolean;
var
Comm : string;

begin
  Text := lowercase(text);
  Comm := Getpiece(Text, ' ',0);
  case Comm of
    '/lottery': begin
      if ((GetPiece(Text, ' ',1) = '1') or (GetPiece(Text, ' ',1) = 'on')) then begin
        Lottery := true;
        if (ID<255) then WriteConsole(ID, 'Nooblottery activated', cAdmin)
        else WriteLn('Nooblottery activated');
        WriteConsole(0,'This server runs the NoobLottery-script, better watch your words.',cWarning);
      end else
      if (GetPiece(Text, ' ',1) = '0') or (GetPiece(Text, ' ',1) = 'off') then begin
        Lottery := false;
        if (ID<255) then WriteConsole(ID, 'NoobLottery de-activated', cAdmin)
        else WriteLn('Nooblottery activated');
      end;
    end;
    '/nlsilent': begin
      if ((GetPiece(Text, ' ',1) = '1') or (GetPiece(Text, ' ',1) = 'on')) then begin
        SilentMode := true
        if (ID<255) then WriteConsole(ID, 'Nooblottery: Silent Mode activated', cAdmin)
        else WriteLn('Nooblottery: Silent Mode activated');
      end else
        if (GetPiece(Text, ' ',1) = '0') or (GetPiece(Text, ' ',1) = 'off') then begin
        SilentMode := false;
        if (ID<255) then WriteConsole(ID, 'NoobLottery: Silent Mode de-activated', cAdmin)
        else WriteLn('Nooblottery: Silent Mode activated');
      end;
    end;
    '/nlprint' : PrintReFiles;
    '/nlload' : LoadReFiles;
  end;
  Result:= false;
end;


procedure OnPlayerSpeak(ID: byte; Text: string);
begin
  if Lottery=true then begin
    Text := lowercase(Text);

    if (TEST_MODE) then begin
      WriteConsole(0, 'Checking for letters:', cTest);
      for i:=0 to (GetArrayLength(RegExps)-2) do begin
        if RegExpMatch(GetPiece(RegExps[i], ' ', 1), Text) then WriteConsole(0, 'RE '+inttostr(i)+': '+GetPiece(RegExps[i],' ',0), cTest);
      end;
    end;

    //First remove all the exceptions from the string
    if (TEST_MODE) then WriteConsole(0, 'Checking for exceptions:', cTest);
    for i:=0 to (GetArrayLength(Excepts)-2) do begin
      if RegExpMatch(Excepts[i], Text) then begin
        Text := RegExpReplace(Excepts[i], Text, '',true);
        if (TEST_MODE) then WriteConsole(0, 'Exc '+inttostr(i)+': '+Excepts[i], cTest);
      end;
    end;

    //The following replacements are to filter out the 'RegExp-Overload-bug'
    if (TEST_MODE) then WriteConsole(0, 'Checking for bug-makers', cTest);
    if RegExpMatch('('+o+'|'+a+'|'+u+'|'+e+'){3,84}', text) then begin
      if (TEST_MODE) then WriteConsole(0, 'Removing bug-makers', cTest);
      text := RegExpReplace(o+'{3,84}', text, 'oo', true);//replace more then three o's for oo
      text := RegExpReplace(a+'{3,84}', text, 'aa', true);
      text := RegExpReplace(u+'{3,84}', text, 'uu', true);
      text := RegExpReplace(e+'{3,84}', text, 'ee', true);
    end;


    //Then look for the 'banned' words
    if (TEST_MODE) then begin
      WriteConsole(0, 'Checking for words(right combination of letters:', cTest);
      if RegExpMatch(n+pun+'*'+'('+o+'|'+a+'|'+u+')+'+pun+'*'+'(('+o+'|'+a+'|'+u+')+'+pun+'*)*'+w+'*'+pun+'*'+b, text) then WriteConsole(0, 'Noob found', cTest);
      if RegExpMatch(n+pun+'*'+e+'+'+pun+'*('+e+'+'+pun+'*)*'+w+'+'+pun+'*'+b, text) then WriteConsole(0, 'Newb found', cTest);
      if RegExpMatch(f+pun+'*'+a+'+'+pun+'*('+a+'+'+pun+'*)*'+g, text) then WriteConsole(0, 'Fag found', cTest);
    end else begin

    //This is where all the 'letter-expressions' from RE.txt are combined in to words
    //if you understand all this, you might be able to avoid the lottery :p
    //plz tell me if you found a way ;)
      if RegExpMatch(n+pun+'*'+'('+o+'|'+a+'|'+u+')+'+pun+'*'+'(('+o+'|'+a+'|'+u+')+'+pun+'*)*'+b, text)
      or RegExpMatch(n+pun+'*'+e+'+'+pun+'*('+e+'+'+pun+'*)*'+w+'+'+pun+'*'+b, text)
      or RegExpMatch(f+pun+'*'+a+'+'+pun+'*('+a+'+'+pun+'*)*'+g, text)
      then NoobLottery(ID);

    end;
  if (TEST_MODE) then WriteConsole(0, 'Finished checking', CTest);
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
//                                                                 Also Made By: KeyDon
//
//DISCLAIMER:
//The creator(s) of this script can be in no way responsible to any damage, software-wise or hardware-wise, of
//the system it runs on, nor to any other systems connected to it. Running this script is entirely at your own risk.
//Everybody is allowed to redistribute the entire and/or parts of the script as long as the original creator(s)
//will be credited.
