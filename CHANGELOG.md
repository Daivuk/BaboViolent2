2.11d2
===

**new commands:**  
`addAdmin <ladderId>`  - adds the owner of ladder account <ladderid> to the list of admins  
`removeAdmin <ladderId>` - removes admin by his ladderId from admins list  
`admins` - shows list of admins on this server  
  
**notes:**  
* admins don't need to know password anymore. They can be added or removed based on their ladderId.
* admins will not need to authorize (aka remember login and pass for `admin x y` command)
* all server commands could be entered in chat instead of console with `!` prefix. Example, type in chat `!move sas red` will execute the `move` command as if was typed in console as `- move sas red`


2.11d1
===

**new commands:**  
* `swap <player1> <player2>`  -  change player's teams (swap crapper etri)  
* `move <player> <team>` - moves player to a team (move crapper spec)  
* `veto` - cancels a vote in progress  
  
**modified commands:**  
* `ban <player> <reason>` - added reason (example: `ban sasha spectating too much`)  
* `banid <playerid> <reason>` - added reason (example `ban 0 no reason`)  
* `banip <ip> <reason>` - added reason (example: `ban 127.0.0.1 test`)  
* `banlist` - shows list of banned people. Will now show also the name of the admin, reason and time of ban  

**notes:**  
* new commands can accept partial nickname (`move cra red` will affect the first player with "cra" in his nickname)  
* new commands will show help if typed inappropriately (ie `ban x` without writing reason will ask to write reason)  
* command quit stopped working ;/ need cnik to cinc to code review it..  
* fixed includes path in solution