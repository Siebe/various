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


package nl.pleduc.mc.pleducplugins.pleducregister;


import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.URL;
import java.net.URLConnection;
import java.net.URLEncoder;
import java.util.logging.Logger;

import org.bukkit.ChatColor;

//import com.palmergames.bukkit.towny.Towny;
//import com.palmergames.bukkit.towny.object.TownyUniverse;

import nl.pleduc.mc.pleducplugins.PleducPlugins;

public class PleducRegister {
	
	private PleducPlugins pp;
	
	private String siteURL;
	
	private Logger mclog;
	public String TAG = "[PleducReg]";
	public Boolean DEBUG = false;
	
	//private TownyUniverse townyUniverse;
	
	//private PleducRegisterListener listener;	

	private PleducRegisterCommand command;
	
	public void onEnable(PleducPlugins instance) {
		this.pp = instance;
		this.pp.getConfig();
		
		if(!this.pp.getDataFolder().exists()){
			this.pp.saveDefaultConfig();
		}
		
		mclog = this.pp.getServer().getLogger();
		
		if (!setupConfig()) {
			return;
		}
		
		//if (!setupTowny()) {
		//	return;
		//}
		
		
		this.command = new PleducRegisterCommand();
		this.pp.getCommand("pleducregister").setExecutor(this.command);
		//this.listener = new PleducRegisterListener(this);
		//this.getServer().getPluginManager().registerEvents(this.listener, this);
		
		log_info("PleducRegister is ready.");
		
	}
	
	
	public void onDisable() {
	}
	
	
	private boolean setupConfig() {
		this.pp.getConfig();
		this.siteURL = pp.getConfig().getString("pleducregister.siteurl");
		if (siteURL == null && siteURL.isEmpty()) {
			fatal("Error in config: siteurl not found.");
			return false;
		}
		                                           
		return true;
	}
	
	/*
	private boolean setupTowny(){
		Plugin towny = pp.getServer().getPluginManager().getPlugin("Towny");
		if (towny == null) {
			fatal("Towny plugin not found.");
			return false;
		}

		townyUniverse = ((Towny) towny).getTownyUniverse();
		if (townyUniverse == null) {
			fatal("Towny found but unable to hook, check your versions");
			return false;
		}

		log_info("Succesfully hooked into Towny.");
	  	return true;
							
	}
	*/
	
	public boolean playerCanBeRegistered(String name) {
	
		return true;
	}
	
	
	public boolean registerPlayer(String name, String email, String password) {		
		/*
		if(!playerCanBeRegistered(name)) {
			return false;
		}
		
		String town;
		boolean mayor = false;
		
		//String town = townyUniverse.getResident(name).getTown().toString();
		try {
			 TownyUniverse.getDataSource().getResident(name);
		}
		catch (Exception e) {
			log_warning("Towny resident not found! "+e.toString());
			return false;
		}
		
		try {
			town = TownyUniverse.getDataSource().getResident(name).getTown().toString();
			if (TownyUniverse.getDataSource().getResident(name).isMayor()) {
				mayor = true;
			}
		}
		catch (Exception e) {
			town = "none";
			mayor = false;
		}
		
		*/
		if (name == null || name == "" || name.length() > 50 ||
				password == null || password.length() < 6 ||
				//town.length() > 50 ||  //town and mayor disabled!!!
				email == null || email.length() < 7
				)
		{
			log_warning("Input error while formatting HttpPostNew");
			return false;
		}
		
		boolean postReturn = false;
		
		try {
		   postReturn = HttpPost(name, email, password, null, false); //Town and mayor disable!!!
		}
		catch (Exception e) {
			log_warning(e.toString());
		}
		
		if (postReturn == false) {
		  log_warning("Error while trying to register player "+name+", HttpPost failed.");
		  return false;
		}	
		
		log_info("Player "+name+" registered on website.");		
		return true;
	}
	
	
	public boolean updatePlayer(String name, String password) {
		/*
		
		if (playerCanBeRegistered(name)) {
			return false;
		}
		
		String town;
		int mayor = 0;
		
		//String town = townyUniverse.getResident(name).getTown().toString();
		try {
			 TownyUniverse.getDataSource().getResident(name);
		}
		catch (Exception e) {
			log_warning("Towny resident not found! "+e.toString());
			return false;
		}
		
		try {
			town = TownyUniverse.getDataSource().getResident(name).getTown().toString();
			if (TownyUniverse.getDataSource().getResident(name).isMayor()) {
				mayor = 1;
			}
		}
		catch (Exception e) {
			town = "none";
			mayor = 0;
		}
		
		*/		
		return true;
	}
	
	private boolean HttpPost(String name, String email, String password, String town, boolean mayor) throws IOException {
		
		//http://robbamforth.wordpress.com/2009/04/27/java-how-to-post-to-a-htmlphp-post-form/
		
		
		//Create Post String
		String data = URLEncoder.encode("username", "UTF-8") + "=" + URLEncoder.encode(name, "UTF-8");
		data += "&" + URLEncoder.encode("email", "UTF-8") + "=" + URLEncoder.encode(email, "UTF-8");
		data += "&" + URLEncoder.encode("password", "UTF-8") + "=" + URLEncoder.encode(password, "UTF-8");
		                
		         
		// Send Data To Page
		URL url = new URL(this.siteURL);
		URLConnection conn = url.openConnection();
		conn.setDoOutput(true);		
		OutputStreamWriter wr = new OutputStreamWriter(conn.getOutputStream());
		wr.write(data);
		wr.flush();
		   
		// Get The Response
		// (true/false):(returnmsg)
		
		BufferedReader rd = new BufferedReader(new InputStreamReader(conn.getInputStream()));
		String line;
		while ((line = rd.readLine()) != null) {
				log_debug("Return from site: "+line);
				
		        if (line.matches("^true:.*")) {
		        	return true;
		        }
		        if (line.matches("^false:.*")) {
		        	String lines[] = line.split(":");
		        	if (lines.length < 2) {
		        	    log_warning("Unable to register player "+name+", site returned 'false'.");
		        	    return false;
		        	}
		        	String msg = lines[1];
		        	log_warning("Unable to register player "+name+", site returned 'false'.");
		        	log_warning("Message from site: "+msg);
		        	this.pp.getServer().getPlayer(name).sendMessage(ChatColor.AQUA + TAG + " " + 
		        			"Message from site: "+msg);
		        	return false;
		        }
		}
		
		log_warning("Unable to register player "+name+", site did not return expected string.");
		return false;
	}
	
	
	public void log_info(String msg) {
		mclog.info(TAG + " " + msg);
	}
	

	public void log_warning(String msg) {
		mclog.warning(TAG + " " + msg);
	}
	

	public void log_debug(String msg) {
		if (DEBUG) {
			mclog.info(TAG + " DEBUG: " + msg);
		}
	}
	

	public void fatal(String msg) {
    	mclog.severe(TAG + " " + msg);
    	mclog.severe(TAG + "Now disabling PleducRegister");
    	//this.pp.getServer().getPluginManager().disablePlugin(this);
	}
	
	
}