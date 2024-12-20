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
    img.src = `https://jj2multiplayer.online/fur/?a=${furA}&b=${furB}&c=${furC}&d=${furD}&char=${char}&frame=3`;
    
    let span = document.createElement("span")
    span.innerText = name

    li.appendChild(img);
    li.appendChild(span);
    document.getElementById("player-list").appendChild(li);
}

function removeAllPlayers() {
    document.getElementById("player-list").innerHTML = "";
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
    content = `${includeFormattedTime ? (getCurrentFormattedTime() + " ") : ""}${content}`;
    item.textContent = content;
    // let imgurl = "https://www.jazz2online.com/media/generate_small.php?s=" + content;

    // item.appendChild(img)
    messages.appendChild(item);
    window.scrollTo(0, document.body.scrollHeight);
}

let players = []

socket.onopen = () => {
    console.log("WebSocket connection established!");
    socket.send(assemblePacket("request", "players"));
};

socket.onmessage = (event) => {
    const message = event.data;

    var [type, content] = disassemblePacket(message);

    if (type == "message") {
        addMessage(content, false);
    } else if (type == "players") {
        removeAllPlayers();
        console.log(content);
        players = JSON.parse(content);
        console.log(players);
        players.forEach(player => {
            let fur = atob(player.fur); // 4 bytes
            addNewPlayer(fur.charCodeAt(0), fur.charCodeAt(1), fur.charCodeAt(2), fur.charCodeAt(3), player.character, player.name);
        });
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

// for (let index = 0; index < 5; index++) {
//     addNewPlayer(Math.round(Math.random() * 127), Math.round(Math.random() * 127), Math.round(Math.random() * 127), Math.round(Math.random() * 127), "jazz", "test")
// }
// for (let index = 0; index < 5; index++) {
//     addNewPlayer(Math.round(Math.random() * 127), Math.round(Math.random() * 127), Math.round(Math.random() * 127), Math.round(Math.random() * 127), "spaz", "test2")
// }
// for (let index = 0; index < 5; index++) {
//     addNewPlayer(Math.round(Math.random() * 127), Math.round(Math.random() * 127), Math.round(Math.random() * 127), Math.round(Math.random() * 127), "lori", "test3")
// }

