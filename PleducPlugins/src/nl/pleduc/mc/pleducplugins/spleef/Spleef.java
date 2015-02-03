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


package nl.pleduc.mc.pleducplugins.spleef;

import nl.pleduc.mc.pleducplugins.PleducPlugins;

import java.awt.Point;

import org.bukkit.World;
import org.bukkit.block.Block;
import org.bukkit.command.Command;
import org.bukkit.command.CommandSender;
import org.bukkit.entity.Player;


public class Spleef {
	
	private Point pointA;
	private Point pointB;
	private int y;
	private String world;
	private PleducPlugins pp;
	
	private Long delay;
	private Long delayStamp;
	
	public void onEnable(PleducPlugins instance) {	
		double x,z;
		
		this.pp = instance;
		this.pp.getConfig();
				
		//load spleef location and set a default
		String defWorld = this.pp.getServer().getWorlds().get(0).getName();
		
		x = this.pp.getConfig().getInt("spleef.pointa.x", 0);
		z = this.pp.getConfig().getInt("spleef.pointa.z", 0);
		this.pointA.setLocation(x,z);
		x = this.pp.getConfig().getInt("spleef.pointb.x", 0);
		z = this.pp.getConfig().getInt("spleef.pointb.z", 0);
		this.pointB.setLocation(x,z);
		this.y = this.pp.getConfig().getInt("spleef.y", 0);
		this.world = this.pp.getConfig().getString("spleef.world", defWorld);
		
		//load delay time, default one minute
		this.delay = this.pp.getConfig().getLong("spleef.delay", 180);
		
	}
	
	
	
    public boolean onCommand(CommandSender sender, Command cmd, String commandLabel, String[] args){
    	
    	if (!cmd.getName().equals("/spleef")) {
    		return false;
    	}
    	//reset spleef from console (signshop) only
        if (!(sender instanceof Player)) {
            if (args[0] != null && args[0] == "_reset_floor") {//reset floor 'code'
            	this.resetFloor();
            }
            return true;
        }
		Player p = (Player) sender;
				
		//admin section		
		if ((p.hasPermission("spleef.admin") || p.isOp())){
			if ( args.length > 1 && args[0].equals("set")) {     	   
				if (args[1] == "a") {
					this.pointA.setLocation(p.getLocation().getX(), p.getLocation().getZ());
				} else if (args[1] == "b") {
					this.pointB.setLocation(p.getLocation().getX(), p.getLocation().getZ());
				} else {
					p.sendMessage("Usage '/spleef set a' and '/spleef set b'");
					return true;
				}
				this.y = (int) p.getLocation().getY() - 1; //the one your standing on.
				this.world = p.getWorld().getName();
				this.saveConfiguration();
				p.sendMessage("Point saved");
				return true;
			}
			p.sendMessage("Usage '/spleef set a' and '/spleef set b'");
			return true;
		}
		
        return false;
    }
    
		
    public Boolean resetFloor() {
    	Long now = System.currentTimeMillis();
    	
    	if (this.delayStamp != 0 && this.delayStamp + delay > now) { 
    		return false;
    	}
    	this.delayStamp = now;
    	
    	World w = this.pp.getServer().getWorld(this.world);
    	
    	if (w == null) {
    		return false;
    	}
    	
    	for (int x = (int) Math.min(pointA.getX(), pointB.getX()); 
			x <= (int) Math.max(pointA.getX(), pointB.getX());
			x++
		){
        	for (int z = (int) Math.min(pointA.getY(), pointB.getY()); 
				z <= (int) Math.max(pointA.getY(), pointB.getY());
				z++
        	) {
        		Block b = w.getBlockAt(x,this.y,z);
        		if (b.getTypeId() == 0) { //if air
        			b.setTypeId(80);	  //set snow
        		}
        	}
    		
    	}
    	
    	return true;
    }
    
    
    private void saveConfiguration() {
		this.pp.getConfig();
		this.pp.getConfig().set("spleef.pointa.x", this.pointA.getX());
		this.pp.getConfig().set("spleef.pointa.z", this.pointA.getY());
		this.pp.getConfig().set("spleef.pointB.x", this.pointA.getX());
		this.pp.getConfig().set("spleef.pointB.z", this.pointA.getY());
		this.pp.saveConfig();
    }
}



