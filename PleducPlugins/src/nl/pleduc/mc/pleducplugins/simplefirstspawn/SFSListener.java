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

import java.io.File;

import org.bukkit.entity.Player;
import org.bukkit.event.EventHandler;
import org.bukkit.event.Listener;
import org.bukkit.event.player.PlayerLoginEvent;


public class SFSListener implements Listener {
	
	private SimpleFirstSpawn plugin;

	SFSListener(SimpleFirstSpawn instance) {
		this.plugin = instance;
		
	}
	
	@EventHandler
	public void onPlayerLogin(PlayerLoginEvent e) {
		final Player p = e.getPlayer();
		String name = p.getName();

		File file = new File( plugin.getMainWorld() + "/players/" + name + ".dat" );
		if ( !file.exists() ) {
			plugin.getPp().getServer().getScheduler().scheduleSyncDelayedTask(plugin.getPp(), new Runnable() {

			    public void run() {
			    	p.teleport(plugin.getSpawnLoc());
			    }
			}, 2L);
		}
		
	}
	
	
}
