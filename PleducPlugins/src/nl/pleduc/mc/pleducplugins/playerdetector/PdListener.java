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

import org.bukkit.event.EventHandler;
import org.bukkit.event.Listener;
import org.bukkit.event.player.PlayerChangedWorldEvent;
import org.bukkit.event.player.PlayerJoinEvent;
import org.bukkit.event.player.PlayerQuitEvent;


public class PdListener implements Listener{
	
	PlayerDetector plugin;
	
	
	@EventHandler
	public void onPlayerJoin(PlayerJoinEvent event) {
		plugin.delayedUpdateWorldPlayerList();
	}

	@EventHandler
	public void onPlayerChangeWorld(PlayerChangedWorldEvent event){
		plugin.updateWorldPlayerList();
	}
	   
	@EventHandler
	public void onPlayerLeave(PlayerQuitEvent event) {
		plugin.log_debug("player is quiting");
		plugin.delayedUpdateWorldPlayerList();
	}
	

}
