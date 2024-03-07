[![GitHub license](https://img.shields.io/badge/License-MIT-green.svg)](https://github.com/ClaudiaCoord/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/blob/master/LICENSE)
[![WIKI EN](https://img.shields.io/badge/wiki-EN-success)](https://claudiacoord.github.io/MIDI-MT/docs/EN/)
[![WIKI RU](https://img.shields.io/badge/wiki-RU-success)](https://claudiacoord.github.io/MIDI-MT/docs/RU/)
[![GitHub release](https://img.shields.io/github/release/ClaudiaCoord/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro.svg)](https://github.com/ClaudiaCoord/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/releases/)
[![GitHub code size](https://img.shields.io/github/languages/code-size/ClaudiaCoord/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro)](https://github.com/ClaudiaCoord/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro)
[![Download x64](https://img.shields.io/badge/Download-x64-brightgreen.svg?style=flat-square)](https://github.com/ClaudiaCoord/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/releases/download/1.0.6.4/MIDIMT_x64.msi)
[![Download x86](https://img.shields.io/badge/Download-x86-brightgreen.svg?style=flat-square)](https://github.com/ClaudiaCoord/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/releases/download/1.0.6.4/MIDIMT_x86.msi)
[![Download x64](https://img.shields.io/badge/Download-x64%2Fportable-brightgreen.svg?style=flat-square)](https://github.com/ClaudiaCoord/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/releases/download/1.0.6.4/MIDIMT_x64_Portable.zip)

# MIDI EasyControl 9 - MIDI-MT is High Level Application Driver for USB MIDI Control Surface

MIDI-MT support x64/x86 Windows 10,11 and any Control Surfaces as **WORDLE**, **Pyle Audio PMIDIPD30**, **LAudio**, **Fegoo** manufacture and other.  

![MIDI EasyControl 9 MIDI-MT](https://claudiacoord.github.io/MIDI-MT/images/helper/web-logo.png)  

# MIDI-MT features

* Mackie translator for Adobe production: `Premiere Pro`, `After Effects`, `Audition` etc.
* Using MIDI-MT as a `MIDI proxy` server, or `HUB` server mode.
* Mixer and Audio panel of audio sessions of running applications.
* Smart House controls: MQTT, DMX512, ARTNET, BOBLIGHT.
* Interactive remote Web Control panel.
* User macro/scripting available: Chaiscript (ECMA 6 syntax compatible).
* Supporting any `USB/MIDI` Control Surfaces as **WORDLE**, **Pyle Audio PMIDIPD30**, **LAudio**, **Fegoo** manufacture and other. 
* Supporting `Joystick` or/and `Gamepad` as a control panel, or such as remote control.

#### Mackie translator for Adobe product

EasyControl-MIDI to MIDI-Mackie Command Translator for Using the **MIDI keyboard** in `Adobe Premiere Pro`.  
You can connect your **MIDI keyboard** to `Adobe Premiere Pro`, or `After Effect`, `Audition` and more. The connection process is described in detail in the [wiki](https://claudiacoord.github.io/MIDI-MT/docs/EN/Settings-Premiere-Pro.html).

<details>
    <summary> Working with Adobe Premiere Pro, After Effects, Audition</summary>

Setting up «Premiere Pro», or other Adobe products, comes down to selecting a MIDI controller from the «Control Surface» submenu in the «Setup» menu.
    You will only have to assign actions to the functions attached to the controller buttons.
</details>

#### Use MIDI-MT as a MIDI proxy HUB/server

<details>
<summary> Use as a MIDI PROXY server</summary>

The number of proxy ports can be selected from `1` to `16`. This mode can be used to share one `MIDI input device` between multiple applications. At the same time, the number of input ports is limited only by the number of connected MIDI devices.  
</details>

<details>
<summary> Working in MIDI HUB mode</summary>

You can create a "common port" that will receive commands from **all MIDI devices** you have connected. Now any software that is connected to the MIDI proxy output ports will receive commands sent from any connected MIDI controller.  
</details>

#### Mixer of audio sessions of running applications

Additionally, it is possible to manage **audio sessions** of running applications.  
Especially relevant, using the [built-in mixer](https://claudiacoord.github.io/MIDI-MT/docs/EN/Audio-Mixer.html) to control sound in **Windows 11** , where the usual sound controls have been removed, and access to existing ones is associated with a long journey through the settings menu.  

<details>
<summary> Applications audio sessions </summary>

Control can occur both from a MIDI keyboard and from the built-in sound control panel.
To control audio sessions from a MIDI keyboard, there is no need to call up the panel.  
The panel is only needed to adjust the sound with the mouse, or to visualize the status of ongoing audio sessions.  
</details>

#### Smart House controls

You can integrate a MIDI keyboard into your [smart home system](https://claudiacoord.github.io/MIDI-MT/docs/EN/Settings-Module-Smart-House.html) to control various devices.
Any **Smart Home** control environments that are based on the MQTT exchange protocol and include an MQTT server are supported.  

<details>
    <summary>MQTT Smart House controls </summary>

In the Smart Home system, the MQTT protocol must be at least level `5.0`, it is possible to work with earlier versions, but stability and full performance for all commands are not guaranteed.
</details>

#### Lighting controls

Supports **DMX512**/**ArtNet**/**BOBLIGHT** protocols, both simultaneous and separate operation is possible.

<details>
<summary> DMX512/ArtNet controls</summary>

Support for both standard 8-bit channels and 16-bit channels has been implemented. They can be used simultaneously, but when designing a network, it must be taken into account that a 16-bit channel takes the place of two 8-bit channels.
</details>

<details>
<summary> BOBLIGHT controls</summary>

Support `BOBLIGHT` server protocol has been implemented. For example, it uses **KODI** multimedia system to intelligently control lighting based on the translated source (Ambilight).
</details>

#### Remote interactive control using a Web application  

<details>
    <summary> Remote control using the built-in Web application</summary>

Remote interactive control using the built-in [Web application](https://claudiacoord.github.io/MIDI-MT/docs/EN/Settings-Module-Remote.html) is possible on any operating system. With its help, you can remotely manage all the controls that are defined in your configuration. The number of connected remote control panels is unlimited.  

Any changes made using the `MIDI keyboard`, `control panels` and other input devices are immediately reflected on all control panels. For example, this can help control lighting fixtures with a team of operators. Also, when setting up sound in a studio, it is convenient to be able to regulate sound channels remotely, changing the sound level locally.  
</details>

#### Joystick and Gamepad support's 

Allows you to use standard [“**Joysticks**” and “**Gamepads**”](https://claudiacoord.github.io/MIDI-MT/docs/EN/Settings-Module-Gamepad.html)
 in two modes, as an input keyboard or as a remote control. 

<details>
    <summary> Game devices as input device</summary>

The module translates commands from a gamepad or joystick into the MIDI protocol. In “**input device**” mode, direction controls are used as sliders, and buttons are used for their intended purpose.
</details>

<details>
    <summary> Game devices as remote controls</summary>

Using the Gamepad or Joystick you can control all the '**Slider**', '**Fader**' or '**Jog**' controls configured in the `MIDI-MT` configuration. In control mode, adjustment is possible in the “**Audio Mixer**”, “**Smart Home**” and “**Light Control**” groups; all connected devices duplicate each other’s functions.
</details>

#### Interactive Macro/Scripting/Scenarios  

The [scripting core](https://claudiacoord.github.io/MIDI-MT/docs/EN/Settings-Module-VM-Script.html) is `ChaiScript`, an open source scripting language created by [Jason Turner](https://chaiscript.com/).  

<details>
<summary> How you can use scripts</summary>

You can use your own scripts to automate management processes across different groups.

Batch control via executable scripts can expand your capabilities, for example: 

- application of a pre-planned level scheme on several regulators,  
- solving problems of simultaneous, smooth adjustment of levels on many regulators,  
- simultaneous switching on or off of multiple sources,  
- various lighting effects and patterns applied at the touch of a button,  
- execution of a sequence of commands for `Adobe` products..  

</details>


## EASYCONTROL 9 - usb midi controller features

![MIDI EasyControl 9 Device](https://claudiacoord.github.io/MIDI-MT/images/helper/web-present.png)  

- 9 assignable control knobs.  
- 9 assignable control sliders.  
- ~~11 assignable buttons.~~ **19** assignable buttons.
- ~~6 transport buttons assignable to controllers.~~ **All buttons** is assignable to controllers.  
- +**ALL** buttons have double functions! Quick and Long press.
- 4 banks for different settings.
- +**JOG** rotary working.
- USB interface, adaptable to USB 2.0 - FULL SPEED. Power suppliedby USB.  
- Compatible with Win7/XP/Vista/10/11 and Mac OS X. Drive free and hot-plug supported.  
- Edited by the WORLDE software Editor.  

## About dependencies

The application uses the MIDI driver Tobias Erichsen v1.3.0.43 dated 2019-12-02, to install the drivers visit the site,  
and install [loopmidi](https://www.tobias-erichsen.de/software/loopmidi.html) or [rtpmidi](https://www.tobias-erichsen.de/software/rtpmidi.html)  
These distributions include the drivers necessary for operation.  
The loopMIDI application itself is useful, allowing you to experiment with connecting and switching MIDI equipment.  
More details about MIDI-MT settings are described in the **wiki** [EN](https://claudiacoord.github.io/MIDI-MT/docs/EN/) | [RU](https://claudiacoord.github.io/MIDI-MT/docs/RU/)  

## License MIT

- NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!  
- [License PDF](https://claudiacoord.github.io/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/LicenseRu.pdf)  
