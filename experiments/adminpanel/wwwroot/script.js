
const socket = new WebSocket("ws://127.0.0.1:1337/"); // WebSocket server URL

socket.onopen = () => {
    console.log("WebSocket connection established!");
};

socket.onmessage = (event) => {
  const message = event.data;

  const type = message.slice(0, message.indexOf(':')).trim();
  const content = message.slice(message.indexOf(':') + 1).trim();

  console.log(`Type: ${type}`);
  console.log(`Content: ${content}`);
  
  if(type == "message") {
      var item = document.createElement("li");
      item.textContent = content;
      messages.appendChild(item);
      window.scrollTo(0, document.body.scrollHeight);
  }
};

socket.onclose = () => {
    console.log("WebSocket connection closed!");
};

var form = document.getElementById("form");
var input = document.getElementById("input");

function getCurrentFormattedTime() {
    const d = new Date();
    const formattedTime = `[${d.getHours().toString().padStart(2, '0')}:${d.getMinutes().toString().padStart(2, '0')}:${d.getSeconds().toString().padStart(2, '0')}]`;
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

addNewPlayer(32, 48, 64, 0, "spaz", "PI");

form.addEventListener("submit", function(e) {
    e.preventDefault();

    if (input.value) {
        // prediction
        var item = document.createElement("li");
        item.textContent = `${getCurrentFormattedTime()} Admin: ${input.value}`;
        messages.appendChild(item);
        window.scrollTo(0, document.body.scrollHeight);

        socket.send("message:" + input.value);
        input.value = "";
    }
});