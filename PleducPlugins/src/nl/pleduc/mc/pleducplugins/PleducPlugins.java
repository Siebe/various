 /* 

 * Copyright 2012 by JFK - whydontyouspamme@hotmail.com
 *
 * This file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

//test server port 7776 rtoolkit 7777




package nl.pleduc.mc.pleducplugins;

import org.bukkit.command.Command;
import org.bukkit.command.CommandSender;
import org.bukkit.plugin.java.JavaPlugin;

import nl.pleduc.mc.pleducplugins.commands.*;
import nl.pleduc.mc.pleducplugins.flee.*;
import nl.pleduc.mc.pleducplugins.playerdetector.*;
import nl.pleduc.mc.pleducplugins.pleducregister.*;
import nl.pleduc.mc.pleducplugins.simplefirstspawn.*;
import nl.pleduc.mc.pleducplugins.spleef.*;

public class PleducPlugins extends JavaPlugin {
	
	private Flee flee = new Flee();
	private PlayerDetector playerdetector = new PlayerDetector();
	private PleducRegister pleducregister = new PleducRegister();
	private SimpleFirstSpawn simplefirstspawn = new SimpleFirstSpawn();
	private Spleef spleef = new Spleef();
	private Commands commands = new Commands(this);
	
	public void onEnable() {	
		this.getConfig();
		if(!this.getDataFolder().exists()){
			this.saveDefaultConfig();
		}
		
		flee.onEnable(this);
		playerdetector.onEnable(this);
		pleducregister.onEnable(this);
		simplefirstspawn.onEnable(this);
		spleef.onEnable(this);
	}
	

    public boolean onCommand(CommandSender sender, Command cmd, String commandLabel, String[] args){
    	
    	if (flee.onCommand(sender, cmd, commandLabel, args)) return true;	
    	if (playerdetector.onCommand(sender, cmd, commandLabel, args)) return true;
    	if (simplefirstspawn.onCommand(sender, cmd, commandLabel, args)) return true;
    	if (spleef.onCommand(sender, cmd, commandLabel, args)) return true;
    	if (commands.onCommand(sender, cmd, commandLabel, args)) return true;
    	
    	return false;
    
    }
    

}



