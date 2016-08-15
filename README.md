# BaseI6_updater

The FlyPlus 10 channels firmware comes from Dave Borthwick ( http://www.rcgroups.com/forums/member.php?u=626757 ). Thanks to him!

## Easy upload:

* Put your transmitter in update mode
* Double click on 'run.bat'
* Wait until upload is completed
* ...
* Profit

## Advanced:

* Edit run.bat to suit your needs (see help)

## Run.bat

bin\1.2\baseI6_updater -a -u -p `path to firmware image file here` -c -v


## Help

### Command line options

| Option        | Parameter     | Description  |
|:-------------:|:-------------:| ------------ |
| `-h` |  | print help and quit  |
| `-bt` |  | if image file contains bootloader |
| `-c` |  | check flash image CRC before upload  |
| `-v` |  | verbose mode (pause at the end)  |
| `-n` | `number` | com port number |
| `-b` | `baudrate` | com port baudrate |
| `-p` | `path` | path to flash image |
| `-a` | `___` | action to perform : |
| `-a` | `-d` | detect transmitter |
| `-a` | `-u` | upload flash image |
| `-a` | `-r` | reset transmitter |


## Change Log

### [1.2] - 15-08-2016
#### Added
- Adding auto detection of the right com port

#### Changed
- Resolved crash caused by not found com port

### [1.1] - 3-08-2016
#### Added
- CRC signature check before uploading the image
- Ability to upload images with or without bootloader (`-bt` option)
- Verbose option to pause the execution at the end (`-v` option)

#### Changed
- nothing