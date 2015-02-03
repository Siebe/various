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

import java.util.List;

import org.bukkit.World;
import org.bukkit.entity.Player;



class PdTickCheck implements Runnable {
   
    private PlayerDetector plugin;
   
    
    public PdTickCheck(PlayerDetector instance) {
        this.plugin = instance;
    }

   
    public void run() {
    	 	
    	if (plugin.singleWorld != null) {
			for (PdUnit unit : plugin.getUnitList()) {
			    checkUnit(unit, plugin.singleWorld.getPlayers());
			}// for unitlist   
			return;
    	}//singleWorld
    	  
        for (World w : plugin.getWorldList()) {

			for (PdUnit unit : plugin.getUnitList()) {
			
			    if (unit.getWorld() != w.getName()) {			
			        continue;	//next unit		
			    }		
			    checkUnit(unit, w.getPlayers());
			}// for unitlist   
        }// for worldlist       
    } // run()
    
    
    public void checkUnit (PdUnit unit, List<Player> players) {
    	
	    for (Player p : players) {

	        if (unit.getPermission() != null && !p.hasPermission(unit.getPermission())) {			
	            continue; //next player	
	        }
	
	        if (    p.getLocation().getX() > unit.getWest() &&			
	        		p.getLocation().getX() < unit.getEast() &&			
	        		p.getLocation().getZ() > unit.getSouth() &&			
	        		p.getLocation().getZ() < unit.getNorth()			
        	) {			
	            unit.activate(true);
	            return; //next unit
	        } //if location
	    }// for world.players   
    	unit.activate(false);
 
    } //checkUnit()
    
    
}// class
