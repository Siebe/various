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

import org.bukkit.entity.Player;

public class FleeTask implements Runnable {
    
	private Player player;
	private final Flee plugin;
	
	public FleeTask(Player p, Flee instance) {
		this.player = p;
		this.plugin = instance;
	}

	public void run() {
		//first remove player out of fleeing list
		plugin.removeFleeing(player);
		
		//cancel if player went offline
		if (!player.isOnline()) {
			return;
		}
		
		//cancel if already out of the world
		if (!plugin.canFleeWorld(player)) {
			return;
		}
		
		//cancel and notify if player is dead
		if (player.isDead()) {
			player.sendMessage("Can't flee when you're dead.");
			return;
		}
		
		//actually flee now
		player.sendMessage("Successfuly fled out of "+player.getWorld().getName());
		player.teleport(plugin.getSpawnLoc());
		
	}
}