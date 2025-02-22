# HitmanAbsolutionSDK
 SDK for Hitman Absolution

<a href="https://discord.gg/6UDtuYhZP6" title="Join the absolution-modding channel">
	<img src="https://img.shields.io/badge/discord-join-7289DA.svg?logo=discord&longCache=true&style=flat" />
</a>

![HMA_POST](https://github.com/user-attachments/assets/0bfcc67d-f588-4b8a-98c8-f5108a2e4bcb)

## Install
Extract the content of the `AbsolutionSDK-Release.zip` archive to `<drive>:\Program Files (x86)\Steam\steamapps\common\Hitman Absolution`

## Usage
Open SDK menu by pressing `~` key

## Contracts Mode
To use SDK with [Hitman-5-Server](https://github.com/LennardF1989/Hitman-5-Server) add `ConsoleCmd WebServiceUrl http://localhost:40147/hm5` below `[application]` in HMA.ini \
Port should match port specified in appsettings.json for HM5.Server

## Mods
![image](https://github.com/user-attachments/assets/9ad1b04b-90da-4f3b-9a0b-d198c1c16fd0)

The SDK includes 9 mods which can be activated or deactivated at any time depending on the purpose. Some of them like Freecamera and HUD have a specific key for activation or deactivation.
To enable or disable one or many mods, click on `MODS` in the SDK bar. Check and/or uncheck any mods and press `OK`. <p></p>
![image](https://github.com/user-attachments/assets/01b7624f-2fcd-4357-89e0-e40d301c5ae4)

### Actors
Displays all actors and has options to equip model of actors, teleport actor to hitman, teleport hitman to actor, add weapon to inventory of actor and teleport all actors to hitman. <p></p>
![image](https://github.com/user-attachments/assets/a47b3fd8-7815-4547-bdd5-908c62170ab8)

### Camera
Change FOV and toggle LUT and Vignette effects. <p></p>
![image](https://github.com/user-attachments/assets/c486b511-0474-4ac9-8a03-943829a4c2ed)

Before disabling LUT and FOV change
![image](https://github.com/user-attachments/assets/1ff8f9ac-bc1b-418d-9f6f-85d948a0182a)
<p></p>
After disabling LUT 

![image](https://github.com/user-attachments/assets/70b34a57-b5fd-4ee0-8c96-802af886d24c)
<p></p>
FOV Change

![image](https://github.com/user-attachments/assets/1cf859ee-8843-4749-a50f-d01088311e78)


### Editor
Displays tree with entities of scene and it can be used to modify properties and trigger input/output pins. <p></p>
![image](https://github.com/user-attachments/assets/efd5a54f-3060-47eb-be82-5fb6a8e6e76b)

### HUD
Toggle HUD and all HUD items. <p></p>
![image](https://github.com/user-attachments/assets/40d6ec87-cab2-449b-acea-e4df94af016f)

### Items
Displays all items and has option to teleport all items to hitman. <p></p>
![image](https://github.com/user-attachments/assets/68f62784-3151-4494-b464-2aa0fa646c2d)

### Player
It has tab for cheats(god mode, invisibility, infinite ammo, refill focus), tab for equping disguise/model of specified actors, tabs for spawning specified weapon/item/prop to world/inventory and tab for spawning actor.
![image](https://github.com/user-attachments/assets/9ad3d39a-97d6-4036-85ca-bf4759850534)

### StartingOutfitAndWeaponModifier
Sets starting outfit and weapon. <p></p>
![image](https://github.com/user-attachments/assets/77ae42a8-d34f-4d79-aa8a-a2632c2788a5)

### NoClip
Make player fly.

### FreeCamera
Adds support for free camera.

## Configuration

### General Settings
`Read HMA.ini` is used to re-activate the contract feature in the game. <p></p>
`Patch Resources` is used for installing modified files. <p></p>
`Pause On Focus Loss` will pause the game at ALT-TAB. Uncheck this is you want to switch windows without pause. Usefull option for screenshooting. <p></p>
The last option will minimize the game to the task bar. <p></p>
![image](https://github.com/user-attachments/assets/b9c25952-6a79-484a-815f-1d2d9e92f267)


### Mods Configuration
Mods like `FreeCamera`, `HUD`, `NoClip` and `Player` have a configurable `.ini` file in which you can change the key bindings at wish.
Files are located in `.\steamapps\common\Hitman Absolution\mod`
Accepted bindigs can be found in [THIS FILE](https://github.com/user-attachments/files/18822180/keys.txt).

## Controls

|Key | Description|
| -------- | ----------- |
|**Free Camera**||
|K |	Enable/Disable Free Camera|
|B+R |	Increase/Decrease Camera Speed|
|LS |	Forward/Backward/Left/Right|
|RS |	Pan/Tilt|
|A+LS |	Roll Camera|
|Y+LS |	Change FOV|
|LB |	Lock Camera and Enable 47 Input|
|RT (HOLD) |	Faster Camera Movements|
|LT+RS |	Camera Lift Up/Down|
|A+LS (Press)|	Reset Roll|
|F9 |	Kill NPC|
|CTRL+F9|	Teleport 47 To Camera|
|F10 |	Pause/Unpause Game and Activate/Deactivate Free Camera|
|F11 |	Pause/Unpause Game|
| | |
|**HUD**| |
|F8 |Enable/Disable HUD|
| | |
|**No Clip**| |
|N |Enable/Disable No Clip|
| | |
|**Player** | |
|I |Get Outfit|
|O |Get Model|
|P | Change Outfit|
|J |Teleport Hitman|



<!---
| Name | Description |
| -------- | ----------- |
| [Actors](/Mods/Actors) | Displays all actors and has options to equip model of actors, teleport actor to hitman, teleport hitman to actor, add weapon to inventory of actor and teleport all actors to hitman. |
| [Player](/Mods/Player) | It has tab for cheats(god mode, invisibility, infinite ammo, refill focus), tab for equping disguise/model of specified actors, tabs for spawning specified weapon/item/prop to world/inventory and tab for spawning actor. |
| [Editor](/Mods/Editor) | Displays tree with entities of scene and it can be used to modify properties and trigger input/output pins. |
| [Items](/Mods/Items) | Displays all items and has option to teleport all items to hitman. |
| [StartingOutfitAndWeaponModifier](/Mods/StartingOutfitAndWeaponModifier) | Sets starting outfit and weapon. |
| [FreeCamera](/Mods/FreeCamera) | Adds support for free camera. |
| [Noclip](/Mods/Noclip) | Make player fly. |
| [HUD](/Mods/HUD) | Toggle HUD and all HUD items. |
| [Camera](/Mods/Camera) | Change FOV and toggle LUT and Vignette effects. |
--->
