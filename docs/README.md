# MIDI EasyControl 9 - MIDI-MT is High Level Application Driver for USB MIDI Control Surface

[![WIKI EN](https://img.shields.io/badge/wiki-EN-success)](https://claudiacoord.github.io/MIDI-MT/docs/EN/)
[![WIKI RU](https://img.shields.io/badge/wiki-RU-success)](https://claudiacoord.github.io/MIDI-MT/docs/RU/)
[![Download x64](https://img.shields.io/badge/Download-x64-brightgreen.svg?style=flat-square)](https://github.com/ClaudiaCoord/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/releases/download/1.0.6.3/MIDIMT_x64.msi)
[![Download x86](https://img.shields.io/badge/Download-x86-brightgreen.svg?style=flat-square)](https://github.com/ClaudiaCoord/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/releases/download/1.0.6.3/MIDIMT_x86.msi)  
[![Download x64](https://img.shields.io/badge/Download-x64%2Fportable-brightgreen.svg?style=flat-square)](https://github.com/ClaudiaCoord/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/releases/download/1.0.6.3/MIDIMT_x64_Portable.zip)

MIDI-MT support x64/x86 Windows 10,11 and any Control Surfaces as **WORDLE**, **Pyle Audio PMIDIPD30**, **LAudio**, **Fegoo** manufacture and other.  

![MIDI EasyControl 9 MIDI-MT](https://claudiacoord.github.io/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/Images/web-logo.png)  

# MIDI-MT features

* Mackie translator for Adobe Premiere Pro, After Effect, Audition
* Mixer of audio sessions of running applications
* Smart House controls (MQTT)
* Lighting controls (DMX512/ArtNet)
* Interactive remote controls

#### Mackie translator for Adobe product

EasyControl-MIDI to MIDI-Mackie Command Translator for Using the **MIDI keyboard** in `Adobe Premiere Pro`.  
You can connect your **MIDI keyboard** to `Adobe Premiere Pro`, or `After Effect` and `Audition`. The connection process is described in detail in the [wiki](https://github.com/ClaudiaCoord/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/wiki/EN-Settings-Premiere-Pro).

<details>
    <summary> Working with Adobe Premiere Pro </summary>
    Setting up «Premiere Pro» comes down to selecting a MIDI controller from the «Control Surface» submenu in the «Setup» menu.
    You will only have to assign actions to the functions attached to the controller buttons.
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

You can integrate a **MIDI keyboard** into your [smart home system](https://claudiacoord.github.io/MIDI-MT/docs/EN/Settings-Module-Smart-House.html) to control various devices.
Any **Smart Home** control environments that are based on the MQTT exchange protocol and include an MQTT server are supported.  

<details>
    <summary> Smart House controls </summary>
    In the Smart Home system, the MQTT protocol must be at least level `5.0`,
    it is possible to work with earlier versions, but stability and full performance for all commands are not guaranteed.
</details>

#### Remote interactive control using a Web application  
<details>

Remote interactive control using the built-in [Web application](https://claudiacoord.github.io/MIDI-MT/docs/EN/Settings-Module-Remote.html) is possible on any operating system. With its help, you can remotely manage all the controls that are defined in your configuration. The number of connected remote control panels is unlimited.  

Any changes made using the `MIDI keyboard`, `control panels` and other input devices are immediately reflected on all control panels. For example, this can help control lighting fixtures with a team of operators. Also, when setting up sound in a studio, it is convenient to be able to regulate sound channels remotely, changing the sound level locally.  
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

# EASYCONTROL Documentation

- [License](https://claudiacoord.github.io/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/LicenseRu.pdf)  
- [Readme](https://claudiacoord.github.io/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/ReadmeRu.pdf)  
- Manual [EN](https://claudiacoord.github.io/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/Manual-En.pdf) | [RU](https://claudiacoord.github.io/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/Manual-Ru.pdf)  
- [WORLDE USB EASYCONTROL USERS MANUAL](https://claudiacoord.github.io/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/WORLDE_USB_EASYCONTROL_USERS_MANUAL.pdf)  
- [MidiController config](Dist/MidiController.cnf)  
- [MidiController default registry settings](/MIDIMT.reg)  

# EASYCONTROL Utilites

- [EasyControl used setup settings](https://claudiacoord.github.io/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/Dist/MIDIMT.Ctrl_data)  
- [EasyControl configurator binary](https://claudiacoord.github.io/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/Dist/EasyControl.zip)  
- [loopMIDI Setup (1.0.16.27)](https://claudiacoord.github.io/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/Dist/loopMIDISetup_1_0_16_27.zip)  

# EASYCONTROL Mackie protocol

- [Mackie Control MIDI Map](https://claudiacoord.github.io/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/Mackie/MackieControlMIDIMap.pdf)  
- [Mackie HUI ZONES](Mackie/HUIZONES.txt)  
- [Mackie HUI TX](Mackie/HUIREFTX.txt)  
- [Mackie HUI RX](Mackie/HUIREFRX.txt)  

## License MIT

- NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!  
- [License PDF](https://claudiacoord.github.io/MIDI-EasyControl-to-Mackie-translator-for-Premiere-Pro/LicenseRu.pdf)  
