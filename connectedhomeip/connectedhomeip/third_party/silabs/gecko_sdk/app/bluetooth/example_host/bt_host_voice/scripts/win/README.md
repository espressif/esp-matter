Software requirements:
  Scripts require some external application and internal command.
  * External applications
    - SoX version 14.4.1 ( https://sourceforge.net/projects/sox/files/sox/14.4.1/ ),
    - Cygwin ( https://www.cygwin.com/ ),
  * Internal commands (Windows 7, 8, 8.1, 10)
    - taskkill,
	- tasklist
	- find
	- wmic
	- timeout


Configuration:
1. Make sure audio device is first on audio devices (Control Panel->Sound->Playback)
   If not it should be set correctly by modifying AUDIODEV variable in config.bat file.

2. Make sure that VOBLE_APP_PARAM variable contains correct COM port. 
   If not VOBLE_APP_PARAM should be modified.

3. Make sure the SoX path is correctly set in config.bat file. SOX_APP variable should be set.



Enable real time audio player steps:
1. Start VoBLE application using batch script (double click):
   run_voble_app.sh

2. Start SoX player application using batch script (double click):
   run_voble_player.sh
  
  
  
Convert ADPCM audio data to WAV:
1. Run convert.bat batch file. This file converts all *.ima or *.s16 files stored in default VoBLE folder. Possible convertion options:
  ima - convert from ADPCM to WAV
  s16 - convert from PCM 16 bits to WAV
  
