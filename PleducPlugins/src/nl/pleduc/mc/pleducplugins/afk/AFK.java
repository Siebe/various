package nl.pleduc.mc.pleducplugins.afk;

import org.bukkit.command.Command;
import org.bukkit.command.CommandSender;

import org.bukkit.entity.Player;

import nl.pleduc.mc.pleducplugins.PleducPlugins;

public class AFK {
	
	private PleducPlugins pp;
	private String[] afkList;
	
	
	
	public AFK(PleducPlugins instance) {
		this.pp = instance;
		//listener
	}
	                                       
	
    public boolean onCommand(CommandSender sender, Command cmd, String commandLabel, String[] args){

    	if ((!(sender instanceof Player)) || cmd.getName().toLowerCase() == "afk") {
    		
    		
    	}
    	
    	
    	return false;
    	
    }//end of onCommand
    
    
    
    
}
