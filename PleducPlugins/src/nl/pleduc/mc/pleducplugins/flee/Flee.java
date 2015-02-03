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


package nl.pleduc.mc.pleducplugins.flee;

import nl.pleduc.mc.pleducplugins.PleducPlugins;
import nl.pleduc.mc.pleducplugins.flee.FleeTask;

import java.util.ArrayList;
import org.bukkit.Location;
import org.bukkit.command.Command;
import org.bukkit.command.CommandSender;
import org.bukkit.entity.Player;


public class Flee {
	
	private Location spawnLoc;
	private ArrayList<String> worldList = null;
	private ArrayList<String> fleeing = null;
	private PleducPlugins pp;
	private int delay;

	
	public void onEnable(PleducPlugins instance) {	
		this.pp.getConfig();
				
		//load flee location and set a default
		Location defLoc = this.pp.getServer().getWorlds().get(0).getSpawnLocation();
		this.spawnLoc = new Location(this.pp.getServer().getWorlds().get(0), 0, 0, 0);
		
		this.spawnLoc.setWorld(this.pp.getServer().getWorld(this.pp.getConfig().getString("flee.spawn.world", defLoc.getWorld().getName())));
		this.spawnLoc.setX(this.pp.getConfig().getDouble("flee.spawn.x", defLoc.getX()));
		this.spawnLoc.setY(this.pp.getConfig().getDouble("flee.spawn.y", defLoc.getY()));
		this.spawnLoc.setZ(this.pp.getConfig().getDouble("flee.spawn.z", defLoc.getZ()));
		this.spawnLoc.setPitch((float)this.pp.getConfig().getDouble("flee.spawn.pitch", defLoc.getPitch()));
		this.spawnLoc.setYaw((float)this.pp.getConfig().getDouble("flee.spawn.yaw", defLoc.getYaw()));
		
		//load worldList to flee from, at least the flee location world
		this.worldList = (ArrayList<String>) this.pp.getConfig().getStringList("flee.world_list");
		
		if (worldList.isEmpty()) {
				worldList.add(spawnLoc.getWorld().getName());			
		}	
		
		//load delay time, default one minute
		this.delay = this.pp.getConfig().getInt("flee.delay", 60);
		
		this.fleeing = new ArrayList<String>();
		
	}
	
	
	
    public boolean onCommand(CommandSender sender, Command cmd, String commandLabel, String[] args){
    	
    	if (!cmd.getName().equals("flee")) {
    		return false;
    	}
    	//ingame use only
        if (!(sender instanceof Player)) {
            sender.sendMessage("You must be a ingame op to use this.");
            return true;
        }
		Player p = (Player) sender;
				
		//admin section		
		if ((p.hasPermission("flee.admin") || p.isOp())
				&& args.length > 0 
				&& args[0].equals("set")
			) {     	   
			this.spawnLoc = p.getLocation().clone();
			saveConfiguration();
			p.sendMessage("Flee location set.");
			return true;
		}
		
		//check worldList
		if (!canFleeWorld(p)){
			p.sendMessage("You can't flee out of this world.");
			return true;
		}
		
		//check permission	
		if (!p.hasPermission("flee."+p.getWorld().getName()) 
				&& !p.hasPermission("flee")
				&& !p.isOp()
			) {
			p.sendMessage("You have no permission to flee out of this world.");
			return true;
		}
		
		//check if player is not already fleeing
		if (fleeing.contains(p.getName())) {
			p.sendMessage("You're already in the process of fleeing, just wait a while.");
			return true;
		}
		
		//start the delayed task
		this.pp.getServer().getScheduler().scheduleSyncDelayedTask(pp, new FleeTask(p, this), 20 * delay);
		p.sendMessage("You will be teleported in "+delay+" seconds.");
		
		//add player to fleeing list
		fleeing.add(p.getName());
		
        return true;
    }
    
    public Location getSpawnLoc() {
    	return this.spawnLoc;
    }
    
    public boolean canFleeWorld(Player p) {
    	return this.worldList.contains(p.getWorld().getName());
    }
    

    public void removeFleeing(Player p) {
    	if (fleeing.contains(p.getName())) {
    		fleeing.remove(p.getName());
    	}
    }
    
    private void saveConfiguration() {
		this.pp.getConfig();
		this.pp.getConfig().set("flee.spawn.world", this.spawnLoc.getWorld().getName());
		this.pp.getConfig().set("flee.spawn.x", this.spawnLoc.getX());
		this.pp.getConfig().set("flee.spawn.y", this.spawnLoc.getY());
		this.pp.getConfig().set("flee.spawn.z", this.spawnLoc.getZ());
		this.pp.getConfig().set("flee.spawn.pitch", this.spawnLoc.getPitch());
		this.pp.getConfig().set("flee.spawn.yaw", this.spawnLoc.getYaw());
		this.pp.getConfig().set("flee.delay", this.delay);
		this.pp.getConfig().set("flee.world_list", this.worldList);
		this.pp.saveConfig();
    }
}



