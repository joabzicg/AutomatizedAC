# AutomatizedAC
## Automatizing an air conditioner utilizing ESP32 equipped with an IR emitter. 

![](https://i.imgur.com/mMtwr7b.jpg)

### °Firstly, the IR code of the AC remote controller is decodified, with an IR sensor connected to the ESP32 utilizing IRRemote library.
### °Then, the IR code is put in the .ino file, the IRRemote library is a modified one, to work with ESP32.
### °The ESP32 instantiates a webpage on local WIFI, an HTML with the controls to turn on or off the AC.
### °The ESP32 with the IR emitter is put closely to the AC, and now it's possible to control the AC via internet from anywhere the globe with ngrok with you want :)
