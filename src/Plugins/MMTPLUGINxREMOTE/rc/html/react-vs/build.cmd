@echo off
setlocal ENABLEDELAYEDEXPANSION

IF "%1" NEQ "" (
	cd %1\rc\html\react-vs\
)

dir /b public

IF EXIST "public\MMTClient.js" (
	del public\MMTClient.js
)
IF EXIST "public\MMTClient-clean.js" (
	del public\MMTClient-clean.js
)
IF EXIST "public\MMTClient.min.js" (
	del public\MMTClient.min.js
)
IF EXIST "public\main.js" (
	del public\main.js
)
IF EXIST "public\main.min.js" (
	del public\main.min.js
)
IF EXIST "public\MMTClient.css" (
	del public\MMTClient.css
)
IF EXIST "public\MMTClient.min.css" (
	del public\MMTClient.min.css
)
IF EXIST "public\MMTClient.css.map" (
	del public\MMTClient.css.map
)

cmd /c npx babel src --out-dir public --extensions .jsx --presets=@babel/preset-react --plugins @babel/plugin-transform-react-jsx
IF NOT EXIST "public\MMTClient.js" (
	echo "-- NOT FOUND! MMTClient.js"
	GOTO END
)
powershell -Command "Get-Content .\public\MMTClient.js -Encoding utf8 | Where-Object { $_ -notmatch 'export' } | Where-Object { $_ -notmatch 'import' } | Out-File -encoding utf8 .\public\MMTClient-clean.js"
IF NOT EXIST "public\MMTClient-clean.js" (
	echo "-- NOT FOUND! MMTClient-clean.js"
	GOTO END
)
cmd /c npx minify public\MMTClient-clean.js --out-file public\MMTClient.min.js
IF NOT EXIST "public\MMTClient.min.js" (
	echo "-- NOT FOUND! MMTClient.min.js"
	GOTO END
)

IF NOT EXIST "src\MMTClient.scss" (
	echo "-- NOT FOUND! src\MMTClient.scss"
	GOTO END
)
cmd /c npx sass src\MMTClient.scss public\MMTClient.min.css --style compressed
IF NOT EXIST "public\MMTClient.min.css" (
	echo "-- NOT FOUND! MMTClient.min.css"
	GOTO END
)

IF NOT EXIST "..\MMTPLUGINxREMOTE-template.html" (
	echo "-- NOT FOUND! MMTPLUGINxREMOTE-template.html"
	GOTO END
)
powershell -Command "(gc ..\MMTPLUGINxREMOTE-template.html) | Foreach-Object { $_ -replace 'JS_REACT_SCRIPT_PLACE', (gc .\public\MMTClient.min.js) -replace 'SCSS_STYLE_PLACE', (gc .\public\MMTClient.min.css) } | Out-File -encoding utf8 ..\MMTPLUGINxREMOTE.html"

:END
endlocal
exit 0
