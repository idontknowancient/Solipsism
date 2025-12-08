# Solipsism 打包指南

## 快速打包步驟

### 第一步：準備檔案

執行以下命令將必要檔案複製到 Release 資料夾：

```powershell
# 從專案根目錄執行

# 複製遊戲執行檔
copy game.exe Solipsism_Release\

# 複製關卡資料
copy stages.txt Solipsism_Release\

# 複製所有資源（圖片、字型、音樂）
xcopy /E /I /Y assets Solipsism_Release\assets

# 複製 SFML DLL（根據你的 SFML 安裝位置調整）
copy C:\SFML-3.0.2\bin\sfml-graphics-3.dll Solipsism_Release\
copy C:\SFML-3.0.2\bin\sfml-window-3.dll Solipsism_Release\
copy C:\SFML-3.0.2\bin\sfml-system-3.dll Solipsism_Release\
copy C:\SFML-3.0.2\bin\sfml-audio-3.dll Solipsism_Release\
```

或者手動方式：
1. 在 Windows 檔案總管中打開 `Solipsism_Release` 資料夾
2. 複製 `game.exe` 到此資料夾
3. 複製 `stages.txt` 到此資料夾
4. 複製 `assets` 資料夾（整個目錄）到此資料夾
5. 從 `C:\SFML-3.0.2\bin\` 複製以下 DLL 檔案到此資料夾：
   - `sfml-graphics-3.dll`
   - `sfml-window-3.dll`
   - `sfml-system-3.dll`
   - `sfml-audio-3.dll`

### 第二步：驗證打包

檢查 `Solipsism_Release` 資料夾結構：

```
Solipsism_Release/
├── game.exe
├── run_game.bat
├── README.md
├── stages.txt
├── sfml-graphics-3.dll
├── sfml-window-3.dll
├── sfml-system-3.dll
├── sfml-audio-3.dll
└── assets/
    ├── *.png
    ├── *.jpg
    ├── *.ttf
    ├── *.otf
    └── sølips.mp3
```

### 第三步：測試執行

在 `Solipsism_Release` 資料夾內：
1. 雙擊 `run_game.bat`，或
2. 在命令列執行 `game.exe`

確認遊戲能正常啟動、載入資源、完成一個關卡。

### 第四步：壓縮發行

使用 Windows 內建的壓縮工具或 7-Zip：

**方法 A：Windows 內建（簡單）**
```
1. 在 Solipsism_Release 資料夾上按右鍵
2. 選擇「傳送到」 > 「壓縮的資料夾」
3. 重新命名為 Solipsism_v1.0.zip
```

**方法 B：PowerShell（指令碼）**
```powershell
Compress-Archive -Path .\Solipsism_Release -DestinationPath .\Solipsism_v1.0.zip -Force
```

**方法 C：7-Zip（推薦，較小壓縮檔）**
```powershell
# 使用 7-Zip 命令列（需事先安裝）
& "C:\Program Files\7-Zip\7z.exe" a Solipsism_v1.0.7z .\Solipsism_Release\
```

## 部署到他人電腦

### 發行格式

發行完整 ZIP 檔案給使用者，包含：
- 遊戲執行檔
- 所有必要的 DLL（SFML 庫）
- 所有遊戲資源
- README 和 run_game.bat 腳本

### 使用者安裝步驟

1. 下載 `Solipsism_v1.0.zip`
2. 解壓到任意資料夾（例如 `C:\Games\Solipsism\`）
3. 進入資料夾，雙擊 `run_game.bat`
4. 開始遊戲！

## 進階打包選項

### 可選：建立 Windows 安裝程式

使用 NSIS（Nullsoft Scriptable Install System）建立 .exe 安裝程式：

1. 下載並安裝 [NSIS](https://nsis.sourceforge.io/)
2. 建立 `.nsi` 腳本檔案
3. 使用 NSIS 編譯器生成 `Solipsism_Setup.exe`

**範例 NSIS 腳本 (installer.nsi)：**
```nsis
; Solipsism Game Installer

!include "MUI2.nsh"

Name "Solipsism Game"
OutFile "Solipsism_Setup.exe"
InstallDir "$PROGRAMFILES\Solipsism"

!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_LANGUAGE "TradChinese"

Section "Install"
  SetOutPath "$INSTDIR"
  File /r "Solipsism_Release\*.*"
  CreateDirectory "$SMPROGRAMS\Solipsism"
  CreateShortCut "$SMPROGRAMS\Solipsism\Play Solipsism.lnk" "$INSTDIR\game.exe"
  CreateShortCut "$SMPROGRAMS\Solipsism\Uninstall.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

Section "Uninstall"
  RMDir /r "$INSTDIR"
  RMDir /r "$SMPROGRAMS\Solipsism"
SectionEnd
```

### 可選：建立便攜版本

便攜版本可在任何 USB 隨身碟上執行，無需安裝：

1. 建立資料夾 `Solipsism_Portable`
2. 複製 `Solipsism_Release` 中的所有檔案
3. 新增 `.bat` 或 `.exe` 啟動器（已包含 `run_game.bat`）
4. 使用者可直接在隨身碟上執行

## 版本管理建議

檔案命名慣例：
- `Solipsism_v1.0.zip` - 初版（當前）
- `Solipsism_v1.1.zip` - 修復版本
- `Solipsism_v2.0.zip` - 新增功能版本

## 常見打包問題

**Q: 如何減小 ZIP 檔案大小？**
- 刪除 debug 資源（`debug_log.txt`）
- 使用 7-Zip 或 WinRAR（更高壓縮率）
- 考慮移除不使用的舊素材（`*_old.*` 檔案）

**Q: 如何確保使用者有正確的 DLL？**
- 總是將 SFML DLL 包含在 ZIP 中
- 提供清晰的檔案結構文件
- 在 README 中強調 DLL 的重要性

**Q: 能否製作不需 DLL 的版本？**
- 靜態編譯 SFML（較複雜，編譯時間長）
- 將 SFML 設定為 `SFML_STATIC = TRUE` 重新編譯

---

**建議**：先進行第一至第三步的打包與測試，確認一切正常後再進行壓縮與發行。
