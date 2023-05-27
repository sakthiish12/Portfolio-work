setlocal
cd /d %~dp0
powershell -command Expand-Archive '%~dp0\Extern\assimp.zip'        '%~dp0\Extern'
powershell -command Expand-Archive '%~dp0\Extern\Fmod.zip'          '%~dp0\Extern'
powershell -command Expand-Archive '%~dp0\Extern\glfw.zip'          '%~dp0\Extern'
powershell -command Expand-Archive '%~dp0\Extern\glm.zip'           '%~dp0\Extern'
powershell -command Expand-Archive '%~dp0\Extern\JoltPhysics.zip'   '%~dp0\Extern'
powershell -command Expand-Archive '%~dp0\Extern\rapidjson.zip'     '%~dp0\Extern'
powershell -command Expand-Archive '%~dp0\Extern\rttr.zip'          '%~dp0\Extern'
powershell -command Expand-Archive '%~dp0\Extern\SOIL2.zip'         '%~dp0\Extern'
powershell -command Expand-Archive '%~dp0\Extern\tinyddsloader.zip' '%~dp0\Extern'
powershell -command Expand-Archive '%~dp0\Extern\Vulkan.zip'        '%~dp0\Extern'
powershell -command Expand-Archive '%~dp0\Extern\Font.zip'          '%~dp0\Extern'
