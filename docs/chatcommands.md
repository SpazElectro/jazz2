# Chat Commands
Now that we have our first mutator up and running, let's make it actually do something instead of print hello world, The JJ2+ AngelScript API has a LOT to offer, including `bool onLocalChat(string &in stringReceived, CHAT::Type chatType)`, which is an event that allows you to allow and read a message that's being sent, and how it's being sent (global or teamchat or whisper or /me)

Let's make use of that event, shall we?

```cpp
#pragma name "testdocs"
#pragma require "STVutil.asc"

#include "STVutil.asc"

// for intellisense
#include "../../scripts/STVutil.asc"

void onLevelBegin() {
    jjConsole("Started!");
}

bool onLocalChat(string &in stringReceived, CHAT::Type chatType) {
    return true;
}
```

In onLocalChat we return `true` indicating that the message shall be sent, if we return `false` the message will not be sent, now let's use `stringReceived` which is the message

```cpp
bool onLocalChat(string &in stringReceived, CHAT::Type chatType) {
    if(stringReceived == "!hello") {
        jjConsole("Hello, World!");
    }

    return true;
}
```

Awesome, now try running the mutator, If we chat and say `!hello`, we will get "Hello, World!" in the console, but our message still gets sent, We can change that by adding a `return false;` after the jjConsole line as such:

```cpp
bool onLocalChat(string &in stringReceived, CHAT::Type chatType) {
    if(stringReceived == "!hello") {
        jjConsole("Hello, World!");
        return false;
    }

    return true;
}
```

![](images/chatmessages_1.png)

Great, We have a command now, how cool is that?<br>
One issue is that, if we type this command in team chat or whisper or /me it will get triggered, so we can add a simple if statement to check if `chatType` is `CHAT::NORMAL`

```cpp
bool onLocalChat(string &in stringReceived, CHAT::Type chatType) {
    if(chatType == CHAT::NORMAL) {
        if(stringReceived == "!hello") {
            jjConsole("Hello, World!");
            return false;
        }
    }

    return true;
}
```

Awesome, so now our final code should look something like this:

```cpp
#pragma name "testdocs"
#pragma require "STVutil.asc"

#include "STVutil.asc"

// for intellisense
#include "../../scripts/STVutil.asc"

void onLevelBegin() {
    jjConsole("Started!");
}

bool onLocalChat(string &in stringReceived, CHAT::Type chatType) {
    if(chatType == CHAT::NORMAL) {
        if(stringReceived == "!hello") {
            jjConsole("Hello, World!");
            return false;
        }
    }

    return true;
}
```

<!-- finish me later lol -->
[Next topic](events.md)
