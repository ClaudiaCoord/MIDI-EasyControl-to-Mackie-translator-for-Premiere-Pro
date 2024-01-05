@echo off

cmd /c "npm install --save-dev @babel/core @babel/cli"
cmd /c "npx install --save @babel/runtime"
cmd /c "npm install --save-dev @babel/plugin-transform-runtime"

cmd /c "npm install --save-dev @babel/preset-env"
cmd /c "npm install --save-dev @babel/preset-react"
cmd /c "npm install --save-dev @babel/preset-typescript"
cmd /c "npm install --save-dev @babel/preset-flow"

cmd /c "npm install babel-minify --save-dev"
cmd /c "npm install sass"
