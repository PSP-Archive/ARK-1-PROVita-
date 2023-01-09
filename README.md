# ARK-1-PROVita-

This is the original source code of PROVita (ARK-1) by Coldbird, Hrimfaxi and Neur0n, creators of the PRO and ME CFW for PSP.

`FOR ARCHIVAL PURPOSES ONLY`


## Original ReadMe by Team PRO

PSP Emulator Custom Firmware ARK for PSVita 2.00-2.02
by Team PRO

Instructions :

    - Go into the ARK folder
    - Choose your region and copy the savedata to your '\PS Vita\PSAVEDATA\XXXXXXXXXXXXXXXX\' folder
    - Transfer it to your PSVita using CMA

Features:

    - PSX game support (soundless so far, our sound plugin is too glitchy for public use at the moment)
    - ISO/CSO game support
    - NoDRM Engine for decrypted dlc playback
    - MS-SpeedBoost (most noticable on games like soul calibur)
    - Stargate Game Patches (fixes several anti-cfw games)
    - Customizable Main Menu
    - Plugin support
    - Recovery support
    - ISO-cache support (speed up ISO gaming)

... and much more!

Donations :
    We are always happy to receive donations, but do not enforce them.
    Should you like the software that we provide, please consider buying our team a beer or two at...

    > https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=RJB74KKZAXS3E

About yMenu:
Default menu, created by Yosh - for more details see http://wololo.net/talk/viewtopic.php?f=53&t=12131
Special thanks to ind3x-gosu for his custom theme!

About PROShell Recovery:
While triggerring the exploit press R to enter the PROShell minimalistic launcher menu!
This menu can be used to sort out or launch corrupted icon EBOOTs.

About Plugins:
To load plugins, place a plugins.txt file in your savedata folder containing lines as:
mode,path,1

    - Mode can be either umdemu (for isos), game (for homebrews), or pops (for psx)
    - Path is the absolute plugin's path
    - And then 1 for enabled, or 0 for disabled

Warning! Never put any additionnal spaces or anything between commas

For instance :

     umdemu,ms0:/PSP/SAVEDATA/NPEH00020DATA00/plugin.prx,1
     pops,ms0:/PSP/SAVEDATA/NPEH00020DATA00/plugin2.prx,0
     game,ms0:/PSP/SAVEDATA/NPEH00020DATA00/plugin3.prx,1

Credits :

- Dark_Alex for the initial M33 CFW who guided us mentally through several CFW generations by giving good examples on how to patch Sony OFW.
- VirtuousFlame: For the ISO Loading Code, Custom PSX EBOOT Support, PSPCipher and most of the CFW related code.
- Coldbird: For Proshell, and all his CFW related code & Vita redesign of PROCFW.
- Yosh: For the initial sceWlanGetEtherAddr leaked kxploit, yMenu, other kxploit related things and various pieces of code.
- thecobra & frostegater: For precisions about this kxploit.
- neur0n for his syscall execution in kernel mode, several bug fixes and other CFW related code.
- ardi for his umd4homebrew to enable UMD in homebrews.
- Zer01ne for the initial Uno exploit discovery.
- All our testers.

And last but not least - Sony for making wonderful, however very locked down, portable gaming handhelds.
