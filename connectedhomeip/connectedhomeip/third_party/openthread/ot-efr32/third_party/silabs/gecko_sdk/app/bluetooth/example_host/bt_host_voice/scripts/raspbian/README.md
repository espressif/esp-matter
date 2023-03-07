Enable audio loop steps:

  1. Start VoBLE application using bash script:
    ./run_voble_app.sh

  2. Start SoX player application using bash script:
    ./run_voble_player.sh
  
    
Convert ADPCM audio data to WAV:

1. Run convert.sh file. This file converts all *.ima or *.s16 files stored in default VoBLE folder. Possible convertion options:
  ima - convert from ADPCM to WAV
  s16 - convert from PCM 16 bits to WAV