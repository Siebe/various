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


import org.bukkit.ChatColor;
import org.bukkit.command.Command;
import org.bukkit.command.CommandExecutor;
import org.bukkit.command.CommandSender;
import org.bukkit.entity.Player;


public class PleducRegisterCommand implements CommandExecutor{

	PleducRegister plugin;
	
	  private static final String EMAIL_PATTERN = 
          "^[_A-Za-z0-9-]+(\\.[_A-Za-z0-9-]+)*@[A-Za-z0-9]+(\\.[A-Za-z0-9]+)*(\\.[A-Za-z]{2,})$";
	
	
	@Override
	public boolean onCommand(CommandSender sender, Command command, String label, String[] args) {

		if (!(sender instanceof Player)) {
			sender.sendMessage("PleducRegister can only be used by ingame players.");
	        return true;
		}
		
		Player p = (Player) sender;
				
		if (!p.hasPermission("PleducRegister.register")) {
			sendMsg(p, "You do not have permission to register.");
			return true;
		}
		
		
		if (args.length < 1 || args[0].equalsIgnoreCase("help")) {
			sendMsg(p, "You can register an acount on www.pleduc.nl by using this command:");
			sendMsg(p, "/forum (youremail) (yourpassword)");
			sendMsg(p, "EXAMPLE: /forum johndoe@email.com somepassword");
			sendMsg(p, "Then you may login on the website with your minecraft name and your password.");
			sendMsg(p, ChatColor.RED+"DO NOT use a password you also use for minecraft or anywhere else.");
			sendMsg(p, "Forgot your password? Ask someone from staff to reset it");
			/*
			if (p.hasPermission("PleducRegister.admin")) {
				sendMsg(p, "ADMIN: you can reset an account of a player ingame by using:");
				sendMsg(p, "/ra reset (name) (newpassword)");
			}
			*/
			return true;
			
		}
		
		//ADMIN RESET DISABLED for mnow
		/*
		
		if (p.hasPermission("PleducRegister.admin")) {
			if (args.length == 3 && args[0].equalsIgnoreCase("reset")) {
				if (plugin.getServer().getPlayer(args[1]) == null) {
					sendMsg(p, "Player is not ingame.");
					return true;
				}
				
				if (plugin.updatePlayer(args[1], args[2])) {
					sendMsg(p, "Reset of player "+args[1]+" succesful, new pw: "+args[2]);
					return true;
				}
				sendMsg(p, "Unable to reset. Remember names are case-sensitive.");
				return true;
				
			}
		}
		
		*/
		
		if (args.length != 2) {
			sendMsg(p, "Invalid number of arguments.");
			sendMsg(p, "Usage: /register-account (youremail) (yourpassword)");
			sendMsg(p, "EXAMPLE: /register-account johndoe@email.com somepassword");
			return true;
		}
		
		String email = args[0];
		String password = args[1];
		
		
		//email checks 
		
		if (!email.matches(EMAIL_PATTERN)) {
			sendMsg(p, email +" is not a valid email address.");
			return true;
		}
		
		
		//password checks:
		
		if (password.equals("(yourpassword)") ||
				password.equalsIgnoreCase("yourpassword") ||
				password.equalsIgnoreCase("somepassword") 
		) {
			sendMsg(p, "Instead of 'yourpassword' or 'somepassword' , please fill in a real password!");
			return true;
		}
		
		if (password.equalsIgnoreCase(p.getName())) {
			sendMsg(p, "Please do not use your username as a password, think of something else!");
			return true;
		}
		
		if (password.length() < 6) {
			sendMsg(p, "Password should be at least 6 characters long.");
			return true;			
		}
	
		//one argument filled in, lets register
		
		if (plugin.registerPlayer(p.getName(), email, password)) {
			sendMsg(p, "Succesfully registered on www.pleduc.nl");
			sendMsg(p, "You may log in as: "+p.getName());
			sendMsg(p, "Your password is: "+password);
			return true;
		} 

		return true;
		
		
	}

	
	private void sendMsg(Player p, String msg) {
		p.sendMessage(ChatColor.AQUA + plugin.TAG + " " + msg);
	}
	
	
	
}
