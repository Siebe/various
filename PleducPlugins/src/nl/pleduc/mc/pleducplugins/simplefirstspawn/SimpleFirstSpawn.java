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


package nl.pleduc.mc.pleducplugins.simplefirstspawn;


import java.util.logging.Level;

import org.bukkit.Location;
import org.bukkit.command.Command;
import org.bukkit.command.CommandSender;
import org.bukkit.entity.Player;

import nl.pleduc.mc.pleducplugins.*;

public class SimpleFirstSpawn {
	
	private PleducPlugins pp;
	
	private Location spawnLoc;
	private String mainWorld = null;
	private SFSListener listener;
	
	public PleducPlugins getPp() {
		return pp;
	}
	
	public void onEnable(PleducPlugins instance) {	
		this.pp = instance;
		this.pp.getConfig();
		
		if(!this.pp.getDataFolder().exists()){
			this.pp.saveDefaultConfig();
		}
		this.spawnLoc = new Location(this.pp.getServer().getWorlds().get(0), 0, 0, 0);
		
		this.spawnLoc.setWorld(this.pp.getServer().getWorld(this.pp.getConfig().getString("simplefirstspawn.spawn.world")));
		this.spawnLoc.setX(this.pp.getConfig().getDouble("simplefirstspawn.spawn.x"));
		this.spawnLoc.setY(this.pp.getConfig().getDouble("simplefirstspawn.spawn.y"));
		this.spawnLoc.setZ(this.pp.getConfig().getDouble("simplefirstspawn.spawn.z"));
		this.spawnLoc.setPitch((float)this.pp.getConfig().getDouble("simplefirstspawn.spawn.pitch"));
		this.spawnLoc.setYaw((float)this.pp.getConfig().getDouble("simplefirstspawn.spawn.yaw"));
		
		this.mainWorld = this.pp.getConfig().getString("simplefirstspawn.main_world", null);
		if (mainWorld != null && this.pp.getServer().getWorld(mainWorld) == null) {
			mainWorld = null;
			this.pp.getServer().getLogger().log(Level.WARNING, "main_world as stated in config.yml does not exist!");
		}
		
		this.listener = new SFSListener(this);
		this.pp.getServer().getPluginManager().registerEvents(this.listener, this.pp);
		
	}
	
    public boolean onCommand(CommandSender sender, Command cmd, String commandLabel, String[] args){
    	if (cmd.getName().equals("fs") && (sender instanceof Player)) {
    		Player p = (Player) sender;
    		if (p.isOp()) {
    			if (args.length == 0) {
    				p.teleport(this.spawnLoc);
    				p.sendMessage("Teleported to FirstSpawn.");
    				return true;
    			}
    			if (args.length > 0 && args[0].equals("set")) {     	   
    				this.spawnLoc = p.getLocation().clone();
    				this.pp.getConfig().set("spawn.world", this.spawnLoc.getWorld().getName());
    				this.pp.getConfig().set("spawn.x", this.spawnLoc.getX());
    				this.pp.getConfig().set("spawn.y", this.spawnLoc.getY());
    				this.pp.getConfig().set("spawn.z", this.spawnLoc.getZ());
    				this.pp.getConfig().set("spawn.pitch", this.spawnLoc.getPitch());
    				this.pp.getConfig().set("spawn.yaw", this.spawnLoc.getYaw());
    				this.pp.saveConfig();
    				p.sendMessage("FirstSpawn location set.");
    				return true;
    			}
    		} else {
    			p.sendMessage("No permission.");
    			return true;
    		}
    	} else {
            sender.sendMessage("You must be a ingame op to use this.");
            return true;
        }

        return false;
    }

    public Location getSpawnLoc() {
    	return this.spawnLoc;
    }

	public String getMainWorld() {
		if (mainWorld != null) {
			return mainWorld;
		}
		return this.pp.getServer().getWorlds().get(0).getName();
	}

}
