MAME72_Release11_2024 Odds N Ends

An update to MAME 72 Release2 for the xbox 360 originally coded by lantus.
new update by Arcadez compiled by (wolf3s)

New games now supported

1943 - The Battle Of Midway Mark II
Guwange Special Version
Thunder Heroes

Games Fixed and Now Playable

Flower
Kiki Kai Kai
Super Slam Tennis

Games now with Graphical Improvements

Flower
Storm Blade
Super Slam Tennis
Ultra X Weapons

Games now with sound

Super Slam Tennis

Games now with improved sound

Flower

Taito M6801 Protection MCU

Added missing mcu for Kiki Kai Kai which fixes multiple
issues in the game broken logic and enemy attacks etc etc


Source Changes

Added support for 1943 - The Battle Of Midway Mark II to the 1943.c driver [arcadez]
Added Guwange Special Version and Thunder Heroes to the cave.c driver [arcadez]
Updated some parts of flower.c to later MAME fixing some major graphical problems also sorted an issue where the game wouldn't always start [arcadez]
Filled out dips and enabled a little hack in the Flower sound core which prevents certain sound samples playing constantly [arcadez]
Fixed the tile layers in Flower which were incorrectly reversed causing numerous gfx niggles throughout the game [dink]
Added shadow support to the SSV video greatly improving the graphical effects in Storm Blade [arcadez]
Fixed graphical corruption on left side of screen stage 3 onwards in Storm Blade and all stages in Ultra X Weapons [arcadez]
Added some alt sprite drawing to the ssv video to prevent performance drops in some SSV games after the above updates [BritneysPAIRS]
Hooked up the M6801 Protection MCU for Kiki Kai Kai fixing many gameplay issues this game can now be considered 100% in the emulation dept [arcadez]
Added sound support for Super Slam Tennis fixed some graphical niggles and prevent the game from crashing after winning a set [arcadez]
Added a new speedup for the Japanese version of IREM's In The Hunt [arcadez]


MAME72_Release10_2023

An update to MAME 72 Release2 for the xbox 360 originally coded by lantus.
new update by Arcadez compiled by (wolf3s)

New games now supported

Chase Bombers
Bubble Bobble (prototype on Tokio hardware)
Bubble Bobble (Ultra Version)(USA)
Bubble Bobble : Lost Cave V1.2
Danger Express
DJ Boy (Japan)
Ghox (Joystick Version)
Hang-Zo
Mortal Race
Mrs Dynamite
Pack N Bang Bang (Final Release Version)
Renegade (Bootleg)
Rumba Lumber
Space Raider

Games Fixed and Now Playable

Alcon
Final Fight 30th Anniversary Edition (would not coin up)
Freekick
Guardian Getstar
Guardians Of The Hood
Kick N Run
Legion - Spinner-87 / Chouji Meikyuu Legion
Road Riot 4WD
Slap Fight (Official Version)
Tokio / Scramble Formation (Official USA Version)
Underfire

Games now with Graphical Improvements

Armed Formation
Buggy Challenge
Cannon Dancer / Osman
Crazy Climber II
Kodore Ookami
Legion - Spinner-87 / Chouji Meikyuu Legion
Magical Crystals
Mega Blast
Mr Do
Return Of The Invaders
Terra Force
The Fairyland Story

Games now with sound or improved sound

Acrobat Mission
B Rap Boys
Bio-ship Paladin
Black Heart
Ghox
GunNail
Hacha Mecha Fighter
Hit The Ice
Koutetsu Yousai Strahl
Pack N Bang Bang
Riding Fight
Ring Rage
Steel Force
Super Spacefortress Macross
Super Spacefortress Macross II
Tecmo Bowl
Teki Paki
Thunder Dragon
US AAF Mustang
Vandyke
Whoopee

CPS1 Changes

Street Fighter II' - The World Warrior now clocks at 10mhz all
the other versions of Street Fighter II and the following games
now run at 12mhz to get rid of slowdowns where there shouldn't be...

Dai Makai-Mura
Capcom World 2
Final Fight 30th Anniversary Edition
Megaman / Rockman - the Power Battle
Pang 3
Pnickies
Quiz and Dragons
Quiz Tonosama no Yabou 2 Zenkoku-ban
Strider Hiryu (Japan set 1)
Varth 

Taito M68705 Protection MCU's

Added missing mcu's for the following games either to make
them playable or to improve the general emulation of them

Alcon
Chack N Pop
Guardian Get Star
Kick N Run
Onna Sansirou - Typhoon Gal
Renegade / Nekketsu Kouha Kunio-kun
Return Of The Invaders
Slap Fight
The Fairyland Story
Tiger Heli
Tokio / Scramble Formation

Source Changes

Tweaked Tecmo Bowl so it will boot using only a single screen [arcadez]
Fixed incorrect palette colours and clock frequencies in Mr Do [cataylox]
Ported Mamesick's fix for the sound in Super Spacefortress Macross II level 2 onwards [arcadez]
Better balanced the sound for the games using the NMK004 [arcadez]
Updated the rohga.c driver to support the rare prototype Hang-Zo [arcadez]
Hooked up missing MSM5205 sample sounds which are for crowd cheers, game sfx and speech in Tecmo Bowl [arcadez]
Fixed missing OKIM6295 sound speech samples in Hit The Ice [arcadez]
added M68705 MCU dumps for Alcon / Slap Fight and Guardian Get Star official versions of these games are now playable [arcadez]
Added proper M68705 dump for Tiger Heli and removed guesswork simuation code [arcadez]
Fixed game timers removed simulation code and hooked up the proper protection mcu for Chack'n Pop [arcadez]
Backported some fixes for a couple of games in the armedf.c driver garbage sprites covering the entire playfield in Legion and a timer crash bug in Kodore Ookami [arcadez]
Added sprite clut cycling colour effects for all games in the armedf.c driver fixes red ninja display in Kodore Ookami and makes Armed Formation etc etc look far prettier [arcadez]
Fixed some graphical problems where the colours were wrong on level 3 and half the screen was missing eg rendered black in Buggy Challenge [arcadez]
Fixed missing adpcm sound effects in Pack 'n Bang Bang [arcadez]
Added support for a special sprite effect to kaneko16 video used by Magical Crystals on the first boss [arcadez]
Added sound banking to the OKIM6295 in silkroad.c fixing missing Sfx and Speech in The Legend Of Silkroad [MAMEDev, arcadez]
Sorted broken dip switches in Mr Do's Castle, Do! Run Run and Mr Do's Wild Ride plus fixed the sound for Indoor Soccer [MAMEDev, arcadez, SapphireDrew]
Updated the romset for Pack N' Bang Bang to use the final released version rather than the unfinished prototype [arcadez]
Fixed some graphical niggles in Cannon Dancer / Osman such as some objects didn't move previously eg trucks on final level [FBN Dev, MAMEDev, arcadez]
Enabled default English World Region for Lode Runner The Dig Fight [arcadez]
Fixed player progress building silhouette graphics in crazy climber 2 [mahoneyt944]
Hooked up HD647180 MCU sound support for Ghox plus added a dedicated joystick version of the game [grant2258, arcadez]
Added support for Mrs. Dynamite and Space Raider to ladybug.c also updated the SN76496 sound core to allow for 5 chips to be used for both of these games [arcadez]
Removed imperfect MCU simulation code and hooked the proper M68705 MCU's for Renegade and Nekketsu Kouha Kunio-kun [dink, arcadez]
Added Danger Express a super rare Atari prototype to the atarig42 driver [arcadez, mahoneyt944]
Added HD647180 MCU sound support for Teki Paki and Whoopee / Pibi & Bibi [dink, arcadez]
Filled out the dip switches and hooked up the protection mcu for Kick N Run which makes the game 100% in the emulation dept and now fully playable [arcadez]
Removed the Knight Boy bootleg mcu from Kiki Kai Kai and replaced it with a simulation of the proper mcu fixing some game logic problems [arcadez]
Updated the retofinv.c driver to MAME94 fixing some gfx niggles "lives left" display etc etc and hooked up the proper mcu for Return Of The Invaders [arcadez]
Improved the sound for Steel Force then updated the driver and video to support Mortal Race [mistydreams, arcadez]
Reclocked some Capcom CPS1 games as per board type to remove slowdowns in game when there shouldn't have been [arcadez]
Fixed American Horseshoes dip switches and slowdowns in Taito's Fighting Hawk [arcadez]
Backported Kale's Taito F3 sound fixes for Riding Fight and Ring Rage from MAME 144 [arcadez]
Added support for Chase Bombers to undrfire.c [arcadez]
Added fake gunsights for Under Fire making this game now playable [mahoneyt944]
Fixed some graphical problems "dragon attacks" in the final level of The Fairyland Story [arcadez]
Removed a bootleg mcu and some fake mcu simulation code and hooked up the proper Taito mcu's for both The Fairyland Story and Onna Sansirou - Typhoon Gal [arcadez]
Added support for Taito's Rumba Lumba and hooked up the M68705 protection mcu [arcadez]
Hooked up the Taito M68705 protection mcu for Get Star / Guardian both versions are now playable [arcadez]
Fixed the official and parent romset for Free Kick [MAMEDev, arcadez]
Added a previously missing graphical effect for Mega Blast in taito_f2.c now when you get the green pickup the shield orb on your ship can be seen [HAZE, arcadez]
Added the Japan version of DJ Boy which has a totally different musical soundtrack [arcadez]
Got rid of sound popping in B Rap Boys [mistydreams]
Hooked up the M68705 protection MCU for the official Taito USA version of Tokio / Scramble Formation game now playable [arcadez, dink]


MAME72_Release9_2023

An update to MAME 72 Release2 for the xbox 360 originally coded by lantus.
new update by Arcadez compiled by (wolf3s)


New games now supported

Action Fighter
Aladdin (bootleg of Japanese Megadrive version)
Bare Knuckle II (Chinese bootleg of Megadrive version)
Bare Knuckle III (bootleg of Megadrive version)
Dogyuun (8/25/1992 location test)
Double Dragon 3 - The Rosetta Stone (Japan)
Fantasy Zone II - The Tears Of OPA OPA
Final Fight Anniversay Edition (3 players)
Final Fight bootleg (multi character select)
Head On Channel
Juezhan Tianhuang
Marble Madness II
Megumi Rescue
Ms Pacman Twin
Ninja Kazan
Oo PArts
Opa Opa (Rev A, unprotected)
Power Up Baseball (Midway Prototype) 
Slap Shooter
SegaSonic Bros
Sonic The Hedgehog 2 (bootleg of Megadrive version)
Super Bubble Bobble (Sun Mixing, Megadrive clone hardware)
Tetris (Japan, System E)

Games fixed and now working

Avengers In Galactic Storm
Beast Busters
Best Bout Boxing
Deroon DeroDero
Desert War / Wangan Sensou
Escape Kids
Gratia - Second Earth
Gunbird 2 (random crashes)
Hard Dunk
Hyper Duel (Reset on starting game)
Jungle Hunt / Jungle king / Pirate Pete (crash on final level)
OutRunners (Graphical flickering)
P47-Aces
Popeye (Random Crashes)
Solomon's Key (levels could become unplayable)
Space Cruiser (Reset on Asteroids level)
Space Seeker (would not coinup)
Stadium Cross
Tetris Plus
Tetris Plus 2 (MegaSystem 32 Version)
The Game Paradise - Master of Shooting!
Thunder Hoop (fixed crash on level 4)
Title Fight
Touki Denshou - Angel Eyes
Turbo Outrun

Games with Graphical Improvements

64th Street - A Detective Story
Alpine Ski
Avenging Spirit
Batsugun / Batsugun Special
Beast Busters
Bells & Whistles / Detana!! Twin Bee
Big Striker
Bio Attack
Chequered Flag
Chimera Beast
Contra / Gryzor
Cybattler
Desert Assault
Double Dragon 3
Dynamite Duke
Dynamite Dux
E.D.F Earth Defense Force
Elevator Action
Elevator Action Returns
Escape Kids
Hachoo!
Heavyweight Champ
Highway Race
In Your Face
Legend Of Makai
Mario Bros
Mechanized Attack
P-47 - The Phantom Fighter
Plus Alpha
Rock 'n Rage
Rod-Land
R-Type Leo
Saint Dragon
Shingen Samurai-Fighter 
SDI - Strategic Defense Initiative
Super Space Invaders 91
The Astyanax
The Combat Tribes
The Tin Star
Thunder Hoop
Thunder Zone
Water Ski
Wild Western
Wrestlewar
WAC Le Mans 24
WWF Superstars


Games now with sound or improved sound

Asterix
Atomic Boy / Wily Tower
Batsugun / Batsugun Special (Partial sound)
Bells & Whistles / Detana!! Twin Bee
Chequered Flag
City Bomber
Congo Bongo
Dogyuun (8/25/1992 location test)
Escape kids
Hachoo!
Haunted Castle / Akuma-Jou Dracula
Hexion
Inferno
Kick Goal
Kitten Kaboodle / Nyan Nyan Panic
Knuckle Bash (Partial sound)
Lighting Fighters
MagMax
Parodius DA!
Punk Shot
Rim Rockin Basketball (commentary voices)
Rollergames
R-Type Leo
Sunset Riders
Teenage Mutant Ninja Turtles 2
The Simpsons
Thunder Cross 2
Trojan / Tatakai No Banka 
Vendetta
Wonderboy III - Monster Lair (Set 1)

Games supporting new and improved samples

Congo Bongo

Sega games with improved sound via UPD7759 core update

Alien Syndrome
Altered Beast
Aurail
Bayroute
ESWAT Cyber Police
Golden Axe
Passing Shot
Riot City
Shinobi
Sonic Boom
Time Scanner

Games with protection fixes

Chequered Flag
Contra / Gryzor
Solomon's Key
Sunset Riders
Teenage Mutant Ninja Turtles 2
Thunder Cross 

Games with improved performance

Demon's World / Horror Story
Fire Shark / Same Same Same
Hellfire
Out Zone
Rally Bike / Dash Yarou
Truxton
Vimana
Zero Wing


Source Changes

Updated tecmosys.c to MAME126 both Deroon DeroDero and Touki Denshou - Angel Eyes are now playable [Haze, Arcadez, dink]
Sorted a gfx niggle with Mario Bros where the screen would shake incorrectly left/right rather than up/down when you hit the POW box [MAMEDEv, arcadez]
Fixed some missing graphics in Beast Busters and Mechanized Attack then sorted sprite priorities on level 2 in Beast Busters [bmcphail, dink, arcadez]
Improved the C-Chip protection simulatons for Bonze Adventure and Operation Wolf [arcadez]
Added Konami 007452 multiplier/divider fixes which sorts rolling mines and bullet trajectories in contra/gryzor during the 3D Sections of the game [arcadez]
Backported Haze's changes from MAME119.u3 fixing the background colours in Dynamite Duke / The Double Dynamites [arcadez]
Fixed level four crash in Thunder Hoop due to previously unknown refresh rate protection in both games [MAME Dev Team, arcadez]
Fixed lightgun calibration plus some graphical and sound niggles in Beast Busters [arcadez, mahoneyt944]
Fixed graphical priorities in Thunder Hoop by using bigkarnak video update call [dink, arcadez]
Disabled palette banking for R-Type leo - fixes invincibiliy flashing [Haze, arcadez]
Added support for rare, Ms. Pacman Twin (mspactwin) simultaneous 2 player play [arcadez, grant2258, mahoneyt944]
Added Marble Madness II (prototype) to the batman driver [Haze, MistyDreams, dink, iq_132, arcadez, mahoneyt944]
Added Fantasy Zone II - The Tears of Opa-Opa (fantzn2x) to system16 driver [MistyDreams, mahoneyt944, arcadez]
Added Action Fighter Sega System A unprotected set to the system16 driver hooked up the inputs and sound plus some new gfx calls game now playable [arcadez, mahoneyt944]
Updated Sega System 16 code and the UPD7759 sound to MAME72.u1 fixing many graphical and sound issues in the games [arcadez]
Improved the sound for Congo Bongo by adding new samples and reclocking the 2nd SN76496 channel to fix the drums tempo [MAMEDev, arcadez]
Fixed broken sound for the classic Williams game Inferno by backporting a sound cpu hack from later MAME [Aaron Giles, arcadez]
Added a new hack to fix the lights dont go out when shot in Elevator Action Returns and clones which doesn't break the gfx on the game ending [dink, arcadez]
Updated the Irem GRA20 soundcore to MAME 81 which fixes missing voices and improves the sound more generally for R-Type Leo [arcadez]
Fixed totally broken music and sfx in Trojan / Tatakai No Banka by backporting some code from later MAME [arcadez]
Fixed Gunbird 2 from randomly crashing by adding a hack to the psikyosh driver [arcadez]
Tweaked the fake prio prom values for 64th street as per later MAME to fix some graphical niggles [arcadez]
Added Nicola Salmoria's fix for the butterfly freeze time powerup in Super Space Invaders 91 [arcadez]
Added Opa Opa, Megumi Rescue, Slap Shooter and Tetris to the Sega System E driver [MAMEDev, arcadez]
Hooked up missing player 2 inputs for Opa Opa and Tetris in the System E main cpu memory map [arcadez]
Added some mirror hacks so that Avengers In Galactic Storm now works [grant2258]
Fixed Hyper Duel from crashing once you started a game [arcadez, KMFDManic]
Backported some K051649 and K053260 sound improvements for many Konami games from later MAME [arcadez]
Added multiplier/divider fixes which sorts rolling mines and bullet trajectories in contra during the 3D Sections [arcadez]
Added support for Powerup Baseball the Midway prototype to itech32.c [arcadez]
Fixed collision logic in Thunder Cross [arcadez]
Updated the Konami Core to MAME78 for some graphical and protection improvements [arcadez]
Added two bootleg versions of Final Fight the 3 player game and the play as any character hack [arcadez, BritneysPAIRS]
Improved the sound in MagMax [arcadez]
Fixed Popeye crashing and the Water colour by correctly masking the tile attribute [arcadez]
Updated some parts of the Taito SJ code to MAME 103 vastly improving the emulation for Elevator Action, Jungle Hunt, Sea Fighter Poseidon and more [MAME Dev, arcadez]
Fixed an issue where Space Seeker would not coin up and start due to a previous commit which added an invalid input game now playable again [arcadez]
Stopped Space Cruiser from crashing on the asteroids level and added the dedicated button for the continue mode [MAMEDev, arcadez]
Fixed Jungle Hunt / Jungle King / Pirate Pete from crashing on the last level [MEMEDev, arcadez]
Fixed broken sound in Hachoo and Kick Goal plus sorted some graphical priority issues for 64th. Street - A Detective Story [MAMEDev, arcadez]
Added sprite buffering to the megasys1 driver improving the gfx for 64th. Street - A Detective Story, Avenging Spirit, Chimera Beast, Cybattler
E.D.F. : Earth Defense Force, Hachoo, In Your Face, Legend of Makai, P-47 - The Phantom Fighter, Ninja Kazan, Plus Alpha, Rod-Land, Saint Dragon
Shingen Samurai-Fighter, The Astyanax + more also added missing prio prom dumps for the games that did not have em already [MAMEDev, arcadez]
Ported across the mame2003+ ddragon3 driver for some graphical improvements for Double Dragon 3 and The Combat Tribes [arcadez]
Added support for the Japanese version of Double Dragon 3 to ddragon3.c [BritneysPAIRS]
Updated the V60 cpu to MAME75 which fixes some Sega and Jaleco games including OutRunners [arcadez]
Added support for Dogyuun (8/25/1992 location test) to the toaplan2 driver this version has full sound and music [arcadez, grant2258]
Switched Contra to use the HD6309 for the main CPU to get rid of slowdowns and fix a crash after 2nd enemy base [arcadez]
Backported a fix from later MAME which gets rid of sprites sticking on the screen in Contra [Kale, mahoneyt944]
Fixed missing commentary voices and correctly hooked up player 3 and 4 inputs for Rim Rockin Basketball [arcadez]
Fixed some graphical niggles and hooked up full sound and music for IREM's Atomic Boy / Wily Tower [MAMEDev, arcadez]
Fixed WWF Superstars bad sprites with some of Randy Macho Man Savage moves eg back suplex etc etc [MAMEDev, arcadez]
Hacked around some game breaking protection calls in Solomon's Key which could make levels unplayable [MAMEDEv, arcadez]
Added Aladdin, Bare Knuckle II, Bare Knuckle III, Juezhan Tianhuang and Super Bubble Bobble (Sun Mixing version) to the segac2.c driver [arcadez, mahoneyt944]
Fixed SN76496 sound in Super Bubble Bobble (Sun Mixing) and ported across from MESS extra genesis input code so that Aladdin, Bare Knuckle II + III are controlable [arcadez, dink]
Added three Sega games Head On Channel, Oo Parts and Sega Sonic Bros to the segac2 driver [arcadez, grant2258]
Added partial sound for Batsugun and Knuckle Bash plus fixed some graphical problem in Batsugun [arcadez]
Ported some video improvements from shmupmame which brings all the Toaplan1 games up to full speed with no frame rate drops [ShmupMAME, arcadez]


MAME72_Release8_2018

An update to MAME 72 Release2 for the xbox 360 originally coded by lantus.
new update by Gamezfan.
compiled by Traace

Sourecode included in download

New games now supported

DoDonPachi Dai-Ou-Jou
DoDonPachi Dai-Ou-Jou Black Label
EspGaluda
Ketsui
Oriental Legend Special / Xi Yo Gi Shi Re Zuang Special (tencent version unprotected)
Puzzle Star
The Killing Blade

Games fixed and now working

Gardia
Fighters History
Metal Slug 5 (see notes)
Noboranka
Photo Y2k
Power Instinct Legends


Games now with sound or improved sound

Crazy Balloon
Knights Of Valour / Sangoku Senki
Knights of Valour Plus / Sangoku Senki Plus
Oriental Legend / Xi Yo Gi Shi Re Zuang
Photo Y2k


Source Changes

Backported some changes from later MAME for the ARM CPU core required for all the 32-bit Data East games 
to function correctly in this core affected drivers are avengrgs.c, deco32.c, deco156.c, and simpl1156.c
Fixed up Power Instinct Legends which was broken previously
Tagged Avengers In Galatic Storm as unplayable (needs SH2 CPU update)
Hooked up custom sound for Crazy Balloon game now has music and extra SFX which were missing before
Hooked up the CPU protection and decryption for Gardia game now playable
Fixed missing enemies level 2 onwards in Noboranka
Added support for the Cave PGM games plus an updated and improved custom ICS2115 soundcore
many thanks to shmupmame and amadvance for the original code.

Notes

In order to get Metal Slug 5 to work in mame2003-plus we had to update the rom and how it loads
in the driver's romtable, however someone had already updated the neogeo bios slightly so maybe
that would be required for this core also..?? obviously i dont have the time to update the bios
plus it would not be user friendly as new Neogeo bios would then be required for all the roms.

Anyhow it might work it might not a new mslug5 rom will be required romnation or doperoms would be a 
good place to track one down.

Information about sourcecode

- Replaced AtgFramework 2008 with AtgFramework 2010
- workaround to fix Xbox audio driver symbole error
- Uncomment wcvol195, wcvol195x, hvysmsh, hvysmsha, hvysmshj in drives.c
- Fixed asurabld, asurabus, denjinmk (vidhrdw fuukifg3.c | denjinmk.c named fuukifg3vid.c & denjinmkvid.c now)
- Fixed deployment order
- added cage.c (sndhrw) | ics2115.c (sound) | pgmy2ks.c (machines) | deco156.c (drivers) to vs2010 solutions



MAME72_Release7_2018
An update to MAME 72 Release2 for the xbox 360 originally coded by lantus.
new update by Gamezfan.
compiled by Traace

Sourecode included in download

New games now supported

Dunk Dream 95
Grand Striker 2
Gunmaster
Heavy Smash ( see below )
Hoops '96
Mouse Shooter GoGo
Skull Fang
World Cup Volley 95 ( see below )


Games fixed and now working

Avengers In Galactic Storm
Hang-On


Games now with sound or improved sound

Alien Syndrome
Altered Beast
Asterix
Aurail
Bayroute
Combat School
Daitoride
Dharma Doujou
ESWAT Cyber Police
Golden Axe
Lady Killer
Last Fortress - Toride
Pang Poms
Passing Shot
Poitto!
Pururun
Puzzli
Riot City
Sankokushi
Shinobi
Sky Alert
Sonic Boom
Space Firebird (Via Samples)
The Karate Tournament
Time Scanner
Toride II Adauchi Gaiden
Wonderboy III Monsterlair (Set 1)


Improvements

Added sound sample support for Nintendo's Space Firebird
Fixed a sprite bug in Ghost's n Goblins
Improved the MCU simulation for Renegade
Hooked up the Metro driver with the UPD7810 sound CPU core
Updated the UPD7759 soundcore to MAME78 fixing sound issues with lots of Sega System 16 games
Fixed slowdown issues in Double Dragon (code via BritneysPAIRS)
improved sound tempo and pitch for Combat School
Updated the ARM CPU core making some changes so the new Data East games will work
Updated the UPD7810 CPU core as some changes were needed to get the sound to work in the Metro games
Added support for iq_132's Deco_mlc video code into vidhrdw/avengrgs.c
added support for the Deco_mlc driver via drivers/avengrgs.c
Fixed sound pitch and levels for Asterix

Information about sourcecode

Changelog (Diffs. between this and gamez fan source) :
- Replaced AtgFramework 2008 with AtgFramework 2010
- Fixed Xbox audio driver symbole error
- Fixed deployment order
- Temp.exclude asurabld, asurabus, denjinmk
- Added cage.c to solutions | Added deco156.c to solutions
- Uncomment wcvol195, wcvol195x, hvysmsh, hvysmsha, hvysmshj in drives.c



MAME72_Release6_2018
An update to MAME 72 Release2 for the xbox 360 originally coded by lantus.
new update by Gamezfan.
compiled by Traace

Sourcecode included in download.

Primal Rage now working and playable with sound(only the version 2.0 rom is working,so us that one)
Tmek now working

added an alternative cheat file as some games do not have cheats with the cheat file usually included with this release.
games like battle garegga,night slashers.....do not have cheats.
i have put the new cheat.dat file in a folder called "alternative cheat dat file" in the general folder.
just change around the cheat.dat files if you want to use the new cheat file.


MAME72_Release5_2018
An update to MAME 72 Release2 for the xbox 360 originally coded by lantus.
new update by Gamezfan.

Sourcecode included in download.

Namco System 1 Driver Games Now Playable
Bakutotsu Kijuutei / Baraduke 2
Berabow Man
Blast Off
Blazer
Dangerous Seed
Dragon Spirit
Face Off
Galaga '88
Marchen Maze / Alice In Wonderland
Pac-Mania
Pistol Daimyo no Bouken / Quest of Pistol Daimyo
Puzzle Club
Quester
Quester (Special Edition)
Rompers
Souko Ban Deluxe / Boxy Boy
Splatterhouse
Tank Force
World Court
World Stadium
World Stadium '89
World Stadium '90
Yokai Douchuuki / Shadowland


MAME72_Release4_2018
An update to MAME 72 Release2 for the xbox 360 originally coded by lantus.
new update by Gamezfan.

Sourcecode included in download.


fixed a graphical issue with the midway mortal kombat/nba jam and rampage world tour games.


MAME72_Release3_2018
An update to MAME 72 Release2 for the xbox 360 originally coded by lantus.
new update by Gamezfan.

Sourcecode included in download.

New games now supported in no particular order

Hyper Street Fighter II
Puzz Loop 2
Title Fight
Air Rescue
Baryon
Cute Fighters
Dream World
Gaia : The Last Choice Of Earth
Rolling Crush
FixEight (bootleg)
DJ Boy
Hyper Crash
Gunbarich
Power Instinct Legends
Counter Run
Dyna Gears
Vasara 2
Sonic Boom
Bang Bang Busters
Ghost Loop
Choutetsu Brikin'ger - iron clad
Sengoku 3
Zupapa
King Of Fighters 2001
King Of Fighters 2002
King Of Fighters 2003
Metal Slug 4
Metal Slug 5
Rage Of The Dragons
Power Instinct Matrimelee
Pochi & Nyaa
Snk Vs Capcom : Svc Chaos
Samurai Shodown V
Samurai Shodown V Special
Zintrick
Crossed Swords 2
The Last Hope
Cabal (Joystick with 3 buttons version)
Knuckle Bash 2
Denjin Makai
Asura Blade
Asura Buster
In Your Face
Snk Vs Capcom : Svc Chaos (Bootleg)
Bonk's Adventure
Thunder Hoop
Explosive Breaker
Pack'n Bang Bang
Wing Force
Night Slashers
Joe & Mac Returns
Osman
Charlie Ninja
Boogie Wings
Diet Go Go
Chain Reaction
Pocket Gal Deluxe
Hacha Mecha Figher (Unprotected)
Air Attack
Wonderboy In Monsterland (English Virtual Console Version)
Riot
Teeter Torture
Donkey Kong II Jumpman Returns



Games fixed and or now working

1000 Miglia : Great 1000 Miles Rally
Avenging Spirit
Bakuretsu Breaker
Blood Warrior
Driftout 94
Fire Barrel
Funky Jet
Hacha Mecha Figher
Monster Slider
Rogha Armor Force
Snowboard Championship
Street Fighter (Via Added Speedups)
Task Force Harrier
Thunder Dragon
Twin Eagle 2
Progear


Games now with sound or improved sound

Cosmic Alien
Fire Shark
Vimana
Donkey Kong
Donkey Kong JR
Sasuke Vs Commander
Fantasy
Nibbler
Pioneer Balloon
Rohga Armor Force
Vanguard
SD Gundam Psycho Salamander no Kyoui
US AAF Mustang
Bio-ship Paladin
Vandyke
Black Heart
Acrobat Mission
Koutetsu Yousai Strahl
Hacha Mecha Fighter
Super Spacefortress Macross
GunNail
Thunder Dragon


Improvements

Small performance boost for the Midway T-Unit and W-Unit games
Added sound support to Fire Shark and Vimana
Added sound support for Cosmic Alien
Added BritneysPAIRS improved sample support to Donkey Kong and Donkey kong JR
New and improved sample support for Sasuke, Fantasy, Pioneer Balloon and Vanguard
Added sound support for SD Gundam Psycho Salamander no Kyoui
Enabled single screen hack for Lode Runner The Dig Fight
Fixed end of game crash in Double Dragon
Fixed random crashes in Rampage World Tour
Added protection simulation for Snowboard Championship
Added dsp handling for Driftout 94 and Twin Eagle 2 both games now playable
Fixed incorrect game logic in Monster Slider game now playable
Small graphical improvements for Biomechanical Toy and Maniac Square
Backported iq_132's Kaneko16 driver B.Rap Boys and Shogun Warriors wont work though
Updated the Deco Protection Rohga Armor Force now playable
Fixed possible crash in Funky Jet by updating Deco Protection
Fixed sound and music in Rohga Armor Force
Fixed 1000 Miglia : Great 1000 Miles Rally gtmr.zip
Added M6801 MCU dump to Bubble Bobble game now 100% Emulated
Updated the NMK16 driver for many sound and graphical improvements
Fixed a reset problem after level 3 in Night Slashers
Enabled breakable backgrounds in 64th street code via BritneysPAIRS
Improved sprite drawing for Gun Force 2

you'll need new samples for these games listed below which are available via
twistys MAME samples site........

Cosmic Alien
Donkey Kong
Donkey Kong JR
Sasuke Vs Commander
Fantasy
Pioneer Balloon
Vanguard

gamezfan with code additions via iq_132 and BritneysPAIRS.

MAME72_Release2 2011
An update to MAME 72 Release2 for the xbox 360 originally coded by lantus.

What's New
=========
- Fixed sound issues
- Added a basic option menu.
- Removed software filters and added Pixel shaders. More filters will be added later.
- Source Code is included in this release.

MAME72_Release1 2011
MAME 72 for the xbox 360 originally coded by lantus.

- First Public Release