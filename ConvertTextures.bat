@echo off

cd /d"%~dp0\Resources\Textures"

for %%f in  (*.png) do (
	..\..\Tools\texconv.exe -m 0 -f BC1_UNORM -y -o "..\Textures" "%%f"
)