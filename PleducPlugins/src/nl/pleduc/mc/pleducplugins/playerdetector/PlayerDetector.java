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

import nl.pleduc.mc.pleducplugins.PleducPlugins;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Set;
import java.util.List;
import java.util.Map;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;
import java.util.logging.Logger;


import org.bukkit.World;
import org.bukkit.command.Command;
import org.bukkit.command.CommandSender;
import org.bukkit.configuration.file.YamlConfiguration;
import org.bukkit.entity.Player;

public class PlayerDetector {
	
	private PleducPlugins pp;
	
	private Set <World> worldList;
	private Set <PdUnit> unitList;
	public World singleWorld;
	private int tickCheckId;
	
	private Logger mclog;
	private YamlConfiguration unitYaml;
	private PdListener listener;
	
	private static Boolean DEBUG = false;
	private static String TAG = "PD";
	
	public PleducPlugins getPp() {
		return pp;
	}

	public void onEnable(PleducPlugins instance) {	;
		this.pp = instance;
		this.mclog = this.pp.getServer().getLogger();		
		this.loadConfig();
		this.listener = new PdListener();
		//register listeer
		this.pp.getServer().getPluginManager().registerEvents(this.listener, this.pp);
	}

	
	public void onDisable() {
		toggleTickCheck(false);
	}
	
	
	public boolean onCommand(CommandSender sender, Command cmd, String label, String[] args) {
		
		if (sender.isOp() && 
			sender instanceof Player && 
			cmd.getName().equalsIgnoreCase("pdreload")
		) {
			sender.sendMessage("reloading PlayerDetector");
			log_info("PlayerDetector now reloading.");
			toggleTickCheck(false);
			if (!loadConfig()) {
				sender.sendMessage("No PlayerDetector units were loaded, please see the logs");
				return true;
			}
			if (singleWorld == null) {
				sender.sendMessage("Loaded "+unitList.size()+" PlayerDetector units in "+worldList.size()+" worlds.");
			} else {
				sender.sendMessage("Loaded "+unitList.size()+" PlayerDetector units in singleWorld mode.");
			}
			return true;
		}
				
		return false;
	}
 
	
	public boolean loadConfig() {
		
		this.worldList = new HashSet<World>();
		this.unitList = new HashSet<PdUnit>();
		this.singleWorld = null;
		
		File unitFile = new File(this.pp.getDataFolder().toString()+"/units.yml");
		this.pp.getConfig();
			
		if(!this.pp.getDataFolder().exists()){
			this.pp.saveDefaultConfig();
	        unitFile = getFileFromJar("units.yml", unitFile);
		}
		
		unitYaml = new YamlConfiguration();
		try {
			unitYaml.load(unitFile);
		} catch (Exception e) {
			fatal("yaml error: "+e.toString());
		}
		
		List<Map<?,?>> tempList = new ArrayList<Map<?,?>>();
		tempList = unitYaml.getMapList("unit_list");
		
		for (Map<?,?> cUnit : tempList) {
			PdUnit unit = new PdUnit(this);
			
			String cName = typeSaveString(cUnit.get("name"));
			String cWorld = typeSaveString(cUnit.get("world"));
			String cPermission  = typeSaveString(cUnit.get("permission"));
			String cField = typeSaveString(cUnit.get("field"));
			String cleverLoc = typeSaveString(cUnit.get("lever"));
			
			if (cField == null || cleverLoc == null || cName == null) {
				if (cName == null) {
					log_warning("syntax error in config.yml unit_list"); 
				} else {
					log_warning("syntax error in config.yml unit_list, unit: '"+cName+'"');
				}
				continue;
			}
			

			if (!unit.setName(cName)) {
				log_warning("cannot add name: "+cName);
				continue;
			}
			if (this.pp.getServer().getWorld(cWorld) == null) {
				log_warning("cannot add world of: "+cName+", "+cWorld);
				continue;
			}
			unit.setWorld(cWorld);
			if (!unit.setField(cField)) {
				log_warning("cannot add field of: "+cName+", "+cField);
				continue;
			}
			if (!unit.setleverLoc(cleverLoc)) {
				log_warning("cannot add lever location: "+cName+", "+cleverLoc);
				continue;
			}
			if (cPermission != null) {
				unit.setPermission(cPermission);
			}
			unitList.add(unit);
			worldList.add(this.pp.getServer().getWorld(cWorld));
			log_info("Succesfully added PlayerDetector unit '"+cName+"'");
		}
		
		if (unitList.size() < 1) {
			log_warning("No detectors were added at all");
			return false;
		}
		
		if (worldList.size() == 1) {
			this.singleWorld = worldList.iterator().next();
		}
		
		updateWorldPlayerList();
		return true;
	}
	
	
	private String typeSaveString(Object entry) {
		if (entry != null && entry instanceof String && entry != "") {
			return (String) entry;			
		}
		return null;
	}

	
	public void delayedUpdateWorldPlayerList() {
		
		this.pp.getServer().getScheduler().scheduleSyncDelayedTask(this.pp, 
				new Runnable(){
					public void run() {
						updateWorldPlayerList();
					}
				}
				, 10L);
		
	}
	
	
	public void updateWorldPlayerList() {
		
		if (this.singleWorld != null) {
			toggleTickCheck(this.singleWorld.getPlayers().size() > 0);
			return;
		}
		
	    for (World w: this.getWorldList()) {
	        if (w.getPlayers().size() > 0) {
	            toggleTickCheck(true);
	            return;
	        }
	    }
	    toggleTickCheck(false);
	}

	
	private void toggleTickCheck(boolean toggle) {
	    if (toggle) {
	        if (this.tickCheckId != 0) {
	            return;
	        }
	        log_debug("TickCheck On");
	        this.tickCheckId = this.pp.getServer().getScheduler().scheduleSyncRepeatingTask(this.pp, new PdTickCheck(this), 2L, 0L);
	        return;
	    }
	    if (this.tickCheckId == 0) {
		        return;
	    }
	    log_debug("TickCheck Off");
	    this.pp.getServer().getScheduler().cancelTask(this.tickCheckId);
	    this.tickCheckId = 0;
	    for (PdUnit unit: this.getUnitList()) {
	    	unit.activate(false);
	    }
	}
	
	
	public Set<World> getWorldList() {
		return this.worldList;
	}


	public Set<PdUnit> getUnitList() {
		return this.unitList;
	}

	public void log_info(String msg) {
		mclog.info(TAG + " " + msg);
	}
	

	public void log_warning(String msg) {
		mclog.warning(TAG + " " + msg);
	}
	

	public void log_debug(String msg) {
		if (DEBUG) {mclog.info(TAG + " DEBUG: " + msg);}
	}

	public void fatal(String msg) {
    	mclog.severe(TAG + " " + msg);
    	//this.getServer().getPluginManager().disablePlugin(this);
	}
	
	private File getFileFromJar(String name, File dest) {
		//original code from Ne0nx3r0.SignShop, adapted by JFK
		
        try{
            File jarloc = new File(getClass().getProtectionDomain().getCodeSource().getLocation().toURI()).getCanonicalFile();
            if(jarloc.isFile()){
                JarFile jar = new JarFile(jarloc);
                JarEntry entry = jar.getJarEntry(name);
                
                if(entry != null && !entry.isDirectory()){
                    InputStream in = jar.getInputStream(entry);
                    FileOutputStream out = new FileOutputStream(dest);
                    byte[] tempbytes = new byte[512];
                    int readbytes = in.read(tempbytes,0,512);
                    while(readbytes>-1){
                        out.write(tempbytes,0,readbytes);
                        readbytes = in.read(tempbytes,0,512);
                    }
                    out.close();
                    in.close();

                    log_info("Created default " + name);
                }   
            }
        }catch(Exception e){
            log_warning("Unable to create default " + name + ":" + e);
        }
		
		return dest;
	}
	
}