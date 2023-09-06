var socket = new WebSocket("ws://127.0.0.1:1337/"); // WebSocket server URL
var form = document.getElementById("form");
var input = document.getElementById("input");

function getCurrentFormattedTime() {
    const d = new Date();
    const formattedTime = `[${d.getHours().toString().padStart(2, "0")}:${d
        .getMinutes()
        .toString()
        .padStart(2, "0")}:${d.getSeconds().toString().padStart(2, "0")}]`;
    return formattedTime;
}

function addNewPlayer(furA, furB, furC, furD, char, name) {
    let li = document.createElement("li");
    let img = document.createElement("img");
    img.src = `https://jazzjackrabbit.net/fur/fur.php?a=${furA}&b=${furB}&c=${furC}&d=${furD}&char=${char}&frame=3`;
    li.appendChild(img);
    li.appendChild(document.createTextNode(name));
    document.getElementById("player-list").appendChild(li);
}

function assemblePacket(type, content) {
    return `${type}:${content}`;
}

function disassemblePacket(message) {
    const type = message.slice(0, message.indexOf(":")).trim();
    const content = message.slice(message.indexOf(":") + 1).trim();
    
    return [type, content];
}

function addMessage(content, includeFormattedTime) {
    var item = document.createElement("li");
    item.textContent = `${includeFormattedTime ? (getCurrentFormattedTime() + " ") : ""}${content}`;
    messages.appendChild(item);
    window.scrollTo(0, document.body.scrollHeight);
}

socket.onopen = () => {
    console.log("WebSocket connection established!");
};

socket.onmessage = (event) => {
    const message = event.data;

    var [type, content] = disassemblePacket(message);

    if (type == "message") {
        addMessage(content, false);
    } else {
        console.warn(`Unknown packet with type \`${type}\` and with content \`${content}\`, full packet: \`${message}\``);
    }
};

socket.onclose = () => {
    console.log("WebSocket connection closed!");
};

form.addEventListener("submit", function (e) {
    e.preventDefault();

    if (input.value) {
        // prediction :P
        addMessage(`Admin: ${input.value}`, true);

        socket.send(assemblePacket("message", input.value));
        input.value = "";
    }
});

addNewPlayer(32, 48, 64, 0, "spaz", "PI");
