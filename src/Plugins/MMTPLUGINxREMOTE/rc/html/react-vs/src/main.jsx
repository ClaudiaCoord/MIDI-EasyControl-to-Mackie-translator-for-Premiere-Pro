import React from 'react'
import ReactDOM from 'react-dom/client'
import MMTClient from './MMTClient.jsx'

ReactDOM.createRoot(document.getElementById('root')).render(
  <React.StrictMode>
        <MMTClient isembed={false} />
  </React.StrictMode>
)
