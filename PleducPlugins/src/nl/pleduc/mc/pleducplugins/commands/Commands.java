package nl.pleduc.mc.pleducplugins.commands;

import org.apache.commons.lang.StringUtils;
import org.bukkit.command.Command;
import org.bukkit.command.CommandSender;

import org.bukkit.entity.Player;

import nl.pleduc.mc.pleducplugins.PleducPlugins;

public class Commands {
	
	private PleducPlugins pp;
	private String lastCommand;
	private Player lastPlayer;
	
	
	public Commands(PleducPlugins instance) {
		this.pp = instance;
	}
	                                       
	
    public boolean onCommand(CommandSender sender, Command cmd, String commandLabel, String[] args){

    	//Prepare command variables
    	String c = this.lastCommand =  cmd.getName().toLowerCase(); 
    	//Stop if this is not a player
	    if (!(sender instanceof Player)) {	    	
	    	return false;
	    }
		//Player variables
    	Player p = this.lastPlayer = (Player) sender;
    	String name = p.getName();
	    
    	//shortcut(inputcommadn, realcommand) 
    	//These are the simple versions, always return true if they're activated!
    	//Multiple 'realcommands' can be entered, seperated by commas
	    if (shortcut("save", "save all")) return true;
    	if (shortcut("spawn", "mv tp spawn")) return true;
    	if (shortcut("mspawn", "spawn")) return true;
    	if (shortcut("sunny", "weather sunny")) return true;
    	if (shortcut("stormy", "weather stormy")) return true;
		if (shortcut("rp", "p reload")) return true;
		if (shortcut("ra", "reload aliases")) return true;
		if (shortcut("rc", "commandbook reload")) return true;
		if (shortcut("day", "time 8")) return true;
		if (shortcut("tick", "debug clock")) return true;
    	
		//info(inputcommand, message[, message, message, ...])
		//These are commands that invoke messages, separate multiple messages by comma
		//Don't forget to return true;
		if (info("ts", "Join our MineCraft Teamspeak server", "Teamspeak: ts.pleduc.nl")) return true;
		if (info("teamspeak", "Join our MineCraft Teamspeak server", "Teamspeak: ts.pleduc.nl")) return true;
		if (info("care", "You probably don't")) return true;
		if (info("pingwing", "The Awesome Owner of this server")) return true;		
		if (info("staff",  "Our &9[ &cADMIN &9] &7are: Florebor, JFK, Diviton & Hedanito","Our mods are: Anne, Yoshua & Eve" )) return true; 

    	//Shortcut and info combination, using a code block defined within { }
    	if (shortcut("acceptrules", String.format("promote %s firstspawn", name))) {
    		info(c, "You have accepted the rules", "Have a nice time !", "Turn around and exit through the iron door");
    		//ALWAYS return true after doing anything
    		return true;
    	}
    	
    	//Resident command, multi-funcional ;)
    	if (c == "r") {
    		//check if there's an argument after '/r'
    		if (args.length < 1) {
    			shortcut(c, "resident");
    			return true;
    		}
    		//add only the first argument after the resident command
    		shortcut(c, "resident "+args[0]);
    		return true;
    	}
    	
    	//Ragequit command, with broadcast and kick + kickmessage
    	//Command will react on '/ragequit' aswell as '/rq'
    	if (info("ragequit", "RageQuit") || info("r", "RageQuit")) {
    		broadcast(name+" just Ragequit the game.");
    		p.kickPlayer("ARrrrrarggh");
    		return true;
    	}
    	
    	//Make sure after this point we have at least 1 argument, else just stop.
    	//Returning false here means no command has been recognized, thats might
    	//not be totally true, there are more commands to come but this is np.
	    if (args.length < 1) return false;
	   	//Functions that need at least one argument come after here
	    
	    //Shortcuts with multiple arguments, combine(args) will combine all arguments 
	    //and seperate them with spaces, a space after the command is still needed
    	if (shortcut("bc", "broadcast "+combine(args))) return true;
    	if (shortcut("pm", "msg "+combine(args))) return true;
    	if (shortcut("pay", "money pay "+combine(args))) return true;
    	
    	//Tempban command
	    if (c == "tban") {
	    	//check for 3 arguments: player time timeperiod
    		if (args.length < 3) return false;   		
    		//combine(args, 1) will combine arguments with offset 1, so all except first
    		shortcut(c, "/tempan "+combine(args), 
				"/note add "+args[0]+" NEGATIVE tempban for "+combine(args, 1));
    		return true;
	    }
	    
	    //Some special commands, below in functions  
	    if(sayas(args)) return true;
	    if(rollback(args)) return true;
	    
	    //None of the aliases matched, now return false
    	return false;
    	
    }//end of onCommand
    
    //Functions:

    private boolean shortcut(String alias, String... commands) {
    	if (this.lastCommand == alias) {
    		for (String c : commands)
    		this.lastPlayer.performCommand(c);
    		return true;
    	}
    	return false;
    }
    
    private boolean info(String alias, String... messages) {
    	if (this.lastCommand == alias) {
    		for (String m : messages)
    		this.lastPlayer.sendMessage(m);
    		return true;
    	}
    	return false;
    }
    
    private String combine(String[] words, int offset) {
    	String result = "";
    	if (words.length < 1) {
    		return "";
    	}
    	for (int i = offset; i < words.length-1; i++) {
    		result += words[i]+" ";
    	}
    	result += words[words.length-1];
    	return result;
    }
    
    private String combine(String[] words) {
    	return combine(words, 0);
    }
    
    private void broadcast(String message) {
    	this.pp.getServer().broadcastMessage(message);
    }
    
    //Special Commands:
    
    private boolean sayas(String[] args) {
	    //sayas command, needs permission since it doesn't use performcommmand, but
	    //fetches and forces target to speak through bukkit API
	    if (this.lastCommand == "sayas" && (this.lastPlayer.hasPermission("pleduc.sayas") || 
	    		this.lastPlayer.hasPermission("commandhelper.chatas"))
		) {
	    	if (args.length < 2) {
	    		//p.sendMessage is sort for info(c, "message");
	    		this.lastPlayer.sendMessage("Not enough arguments");
	    		return true;
	    	}
	    	Player player = this.pp.getServer().getPlayer(args[0]);
	    	if (player == null) {
	    		this.lastPlayer.sendMessage("Player not found");
	    		return true;
	    	}
	    	player.chat(combine(args, 1));
	    	return true;
	    }
	    return false;    
    }
    
    private boolean rollback(String[] args) {
    	//permission as well
	    if (this.lastCommand == "rollback" && this.lastPlayer.hasPermission("pleduc.rollback")) {
	    	String target = args[0];
	    	//default values;
	    	int area = 10;
	    	int since = 24;
	    	//if there are arguments check if they're integers
	    	if ((args[1] != null && StringUtils.isNumeric(args[1])) ||
				(args[2] != null && StringUtils.isNumeric(args[2])) ||
    			(args[0] == "help")	
	    	) {
	    		this.lastPlayer.sendMessage("usage: /rollback (player) (area) (since)");
	    		this.lastPlayer.sendMessage("area and since are optional, since in hours");
	    		return true;
	    	}
	    	//maximum values
	    	if (args[1] != null) {
	    		area = Math.min(20, Integer.valueOf(args[1]));	    	
		    	if (args[2] != null) {
		    		since = Math.min(168, Integer.valueOf(args[2]));
		    	}
	    	}	    	
	    	//here we go give player temporary op, get new instance of player to be sure.. java
	    	this.pp.getServer().getPlayer(this.lastPlayer.getName()).setOp(true);
	    	this.pp.getServer().getPlayer(this.lastPlayer.getName()).performCommand(
	    			"/lb rollback player "+target+" area "+area+" since "+since+"h");
	    	this.pp.getServer().getPlayer(this.lastPlayer.getName()).setOp(false);
	    	return true;
	    }
	    return false;
    }
    
    
    
}
