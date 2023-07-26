# R5Reloaded - External Cheat
An external cheat of R5 Reloaded co-developed with friends.
There's more to do, but I can't get the time for my friends and myself, so it's a stream of disclosure.

## Main functions
- AimBot
   * Smooth
   * Aim forecast
   *visibility check
-ESP
   *Box
   *Line
   * Distance
   *Health Bar
- Misc
   * SpeedHack

### specification
* I use ImGui and its DrawList for normal overlays to render menus, ESP, etc.
* It is possible to obtain player information with the minimum required ReadProcessMemory.
* Emphasizing UI/UX, I finished it as concise and easy-to-use software as possible (although it is not finished)

#### Bugs/Defects/Features and Solutions
* If the ESP loops too quickly, the ESP color will flash in Vis/Norma Color.
   -> Dummy ESP: OFF and maybe there are only a few people on the server? It seems that the lastvisibletime is read at high speed and it repeats changing/unchanging with the previous value, so if you want to solve it, let's conditionally reduce the performance.
  
* AimBot FOV shifts when enemies are in very close range
   -> I don't know, maybe you should add some bones for AimBot to scan.
  
* Aim prediction not applied to dummies
   -> Only Vector3 (0.f, 0.f, 0.f) could be obtained from his dummy m_vecAbsVelocity, so R5Reloaded or Apex specification? I came to the conclusion.

* Y-axis forecast is not accurate
   -> Gravity cannot be obtained because lastprimaryweapon cannot be obtained. Please understand.

* DummyESP: Poor performance when ON
   -> If suddenly 16000 for loops and ReadProcessMemory appear during rendering, the performance will drop

## Disclaimer:
This project is for learning and education.
All developers are not responsible for any damage caused by using it for other purposes.

## Media
![image](https://github.com/FlankGir1/R5Reloaded-ExternalCheat/assets/124275926/c99fdde2-db25-42e6-81a0-869db74cc153)
![image](https://github.com/FlankGir1/R5Reloaded-ExternalCheat/assets/124275926/eab0f461-bf44-4398-88d7-712d4c6c509d)
