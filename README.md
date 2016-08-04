# BaseI6_updater

The firmware FlyPlus comes from Dave Borthwick ( http://www.rcgroups.com/forums/member.php?u=626757 ). Thanks to him!

##Easy upload:

* Edit the run.bat file: replace the '3' after the '-n' option with your COM port number
* Edit the run.bat by placing the path to the firmware you want to upload after the '-p' option
* Put your transmitter in update mode
* Double click on 'run.bat'
* Wait until upload is completed
* Profit


## Help

### Command line options

| Option        | Parameter     | Description  |
|:-------------:|:-------------:| ------------ |
| `-c` |  | check flash image CRC before upload  |
| `-bt` |  | if image file contains bootloader |
| `-h` |  | print help and quit  |
| `-n` | `number` | com port number |
| `-b` | `baudrate` | com port baudrate |
| `-p` | `path` | path to flash image |
| `-a` | `___` | action to perform : |
| `-a` | `-d` | detect transmitter |
| `-a` | `-u` | upload flash image |
| `-a` | `-r` | reset transmitter |


## Change Log

### [1.1] - 3-08-2016
#### Added
- CRC signature check before uploading the image

#### Changed
- nothing