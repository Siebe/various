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

package nl.pleduc.mc.pleducplugins.playerdetector;

import org.bukkit.Location;
import org.bukkit.Material;
import org.bukkit.World;
import org.bukkit.block.Block;
import org.bukkit.material.Lever;


public class PdUnit {

	private String world;
	private String permission;
	private double north;
	private double west;
	private double south;
	private double east;
	private Location leverLoc;
	private PlayerDetector plugin;
	
	private World bWorld;
	
	
	public PdUnit(PlayerDetector instance) {
		this.plugin = instance;
		this.permission = null;
	}
	
	
	public boolean setField(String location) {
		String[] coords = location.split(",");
		
		if (coords.length < 4) {
			return false;
		}
	
		this.west = strToDouble(coords[0]);
		this.east = strToDouble(coords[1]);
		this.south = strToDouble(coords[2]);
		this.north = strToDouble(coords[3]);
		
		if (north == 0. || west == 0. || south == 0. || east == 0.) {
			return false;
		}
		
		return true;	
	}
	
	
	public boolean setFieldNW(Location location) {
		if (location.getWorld() == null || location.getWorld().getName() != this.world) {
			return false;
		}
		this.north = location.getZ();
		this.west = location.getX();
		return true;
	}
	
	
	public boolean setFieldSE(Location location) {
		if (location.getWorld() == null || location.getWorld().getName() != this.world) {
			return false;
		}
		this.north = location.getZ();
		this.west = location.getX();
		return true;
	}
	
	
	private String name;
	public String getName() {
		return name;
	}

	
	public boolean setName(String name) {
		
		for(PdUnit unit : this.plugin.getUnitList()) {
			if (unit.getName() == name) {
				return false;
			}
		}
		
		this.name = name;
		return true;
	}

	
	public String getPermission() {
		return permission;
	}

	
	public boolean setPermission(String permission) {
		this.permission = permission;
		return true;
	}

	
	public double getNorth() {
		return north;
	}

	
	public double getWest() {
		return west;
	}

	
	public double getSouth() {
		return south;
	}

	
	public double getEast() {
		return east;
	}

	
	public Location getleverLoc() {
		return leverLoc;
	}

	
	public Boolean setleverLoc(String sLoc) {
		String[] coords = sLoc.split(",");
		
		if (coords.length < 3) {
			return false;
		}	
		
		Double x = strToDouble(coords[0]);
		Double y = strToDouble(coords[1]);
		Double z = strToDouble(coords[2]);
		
		if (x == 0. || y == 0. || z == 0. || this.world == null || this.world == "") {
			return false;
		}

		World w = this.plugin.getPp().getServer().getWorld(this.world);
		
		if (w == null) {
			return false;
		}
		
		Location loc = new Location(w, x, y, z);
		
		if (w.getBlockAt(loc).getType() != Material.LEVER) {
			return false;
		}
		
	    this.leverLoc = loc;
	    this.bWorld = w;
		
		return true;
		
	}
	
	
	public void setWorld(String iWorld) {
	    this.world = iWorld;
	}
	
	
	public String getWorld() {
	    return this.world;
	}
	
	
	public void activate(boolean activate) {
		 Lever lever;
		 Block leverBlock;
		 try {
			 leverBlock = this.bWorld.getBlockAt(leverLoc);
			 lever = (Lever) leverBlock.getState().getData();
		 } catch (Exception e) {
			 plugin.log_warning("PlayerDetector unit '"+this.name+"' cannot activate lever on "+this.leverLoc.toString());
			 plugin.log_debug(e.getMessage());
			 return; 
		 }
		 
		 if (lever.isPowered() == activate) {
			 return;
		 }
		 plugin.log_debug("Toggle power of '"+name+"' to "+activate);
		 
		 
		 lever.setPowered(activate);
		 leverBlock.setData((byte) lever.getData());
	}
	
	
	private Double strToDouble(String input) {
		try {
			double db = Double.parseDouble(input);
			return db;
		} catch (Exception e){
			return 0.;
		}
	}
	
}
