
let count = 0;
let printinfo = function (s) {
    let d = document.getElementById("log");
    let b = document.createElement("br");
    count++;
    d.append(b);
    d.append("WS [" + count.toString() + "] " + s);
};

let socket = new WebSocket("ws://192.168.1.1:8888/data");

socket.onopen = function (e) {
    console.log("[open] Connection established, request configuration from server...");
    printinfo("connection open.");
    socket.send(JSON.stringify({ "action": "config" }));
};

socket.onmessage = function (event) {
    console.log("[message]", JSON.parse(event.data));
    printinfo("new message.. see console log.");
};

socket.onclose = function (event) {
    if (event.wasClean) {
        console.log(`[close] Connection closed cleanly, code=${event.code} reason=${event.reason}`);
        printinfo(`connection close: code=${event.code} reason=${event.reason}.`);
    } else {
        console.log("[close] Connection died");
        printinfo("connection died.");
    }
};

socket.onerror = function (error) {
    console.log(`[error]`);
    printinfo(`exception: [error]`);
};
