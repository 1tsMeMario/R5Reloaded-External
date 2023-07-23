# R5Reloaded-ExternalCheat
友人と共同開発したR5Reloadedの外部チートです。  
やりたいことはまだまだありますが、友人も自分も時間が取れないので公開という流れになりました。  

## 主な機能
- AimBot
  * Smooth  
  * Aim予測  
  * 可視チェック
- ESP
  * Box
  * Line
  * Distance
  * HealthBar
- Misc
  * SpeedHack

### 仕様
* 普通のオーバーレイにImGuiとそのDrawListを使用してメニューやESP等をレンダリングしています。
* 必要最低限のReadProcessMemoryでプレイヤーの情報を取得できるようにしています。
* UI/UXを重視し可能な限り簡潔に、扱いやすいソフトウェアに仕上げました（仕上がってないけど）

#### バグ/不具合/仕様と解決策
* あまりにESPのループが早すぎると、ESPの色がVis/Norma Colorで点滅します。  
  -> Dummy ESP：OFF でサーバーに数人しかいないとなるかも？lastvisibletimeを高速で読み、前の値と変わる/変わらずを繰り返してるっぽいのでパフォーマンスを低下させましょう。
  
* 敵が超至近距離にいるとAimBot FOVがズレる  
  -> 分らんけど、AimBotのスキャンするBoneいくつか追加した方がいいかもしれないです。
  
* ダミーにAim予測が適応されない  
  -> ダミーの m_vecAbsVelocity からはVector3(0.f, 0.f, 0.f)しか取得できなかったのでR5ReloadedかApexの仕様では？という結論になりました。

* Y軸の予測が正確ではない  
  -> lastprimaryweaponが取得できないのでGravityも取得できません。察してください。

## 免責事項：
このプロジェクトは学習や教育用です。  
それ以外の用途に使用して発生したいかなる損害についても、開発者一同は一切の責任を負いません。

## メディア
![image](https://github.com/FlankGir1/R5Reloaded-ExternalCheat/assets/124275926/c99fdde2-db25-42e6-81a0-869db74cc153)
![image](https://github.com/FlankGir1/R5Reloaded-ExternalCheat/assets/124275926/eab0f461-bf44-4398-88d7-712d4c6c509d)
