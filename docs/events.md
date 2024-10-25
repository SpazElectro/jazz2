# Events
Now that you've dealt with a simple `onLocalChat` event, let's go by each one, one by one.

## Chat events: `void onChat(int clientID, string &in stringReceived, CHAT::Type chatType)` or `bool onLocalChat(string &in stringReceived, CHAT::Type chatType)`
`onChat` is different from `onLocalChat`, `onChat` is an event ONLY for the server, as in no other player can receive this event, `onChat` doesn't allow you to prevent messages from being sent, but the parameters tell you about the `clientID`, which the `onLocalChat` event doesn't have, but in `onLocalChat` you can get it by simply using `jjPLAYER@ p`, but it is not recommended to do so as it is deprecated, The way I do this is either: accepting my fate and using the deprecated `jjPLAYER@ p` or using the `onPlayer` event to get the player and store it
<br><br>
Examples of `onLocalChat` can be found at [this topic](/chatcommands.md)

## `bool onCheat(string &in cheat)`
`onCheat` allows you to see if a cheat code is being typed out, so for example if you're typing out the cheat `jjgod`, the function will be called three times, the results would look something like this: [ `jjg` `jjgo` `jjgod` ], BUT you shouldn't manually check for every cheat, instead you should use the function `bool jjIsValidCheat(const string &in text)` which tells you if `cheat` from `onCheat` is valid or not, Notice that you have to return something to `onCheat`, that allows you to disable the cheat for that `onCheat` call, so if you return `true` the cheat will not trigger, but if you return `false` the cheat will trigger successfully.

## `void onLevelLoad()` or `void onLevelBegin()`
`onLevelLoad` is only called once, when the level starts, but this is before a lot of initalization, while `onLevelBegin` is also called once when the level starts, but called before any jjOBJs have been created and before some jjPLAYER properties have been set

## `void onGameStart()` or `void onGameStart(bool firstTime)` or `void onGameStop()` or `void onGameStop(bool firstTime)`
`onGameStart` is called when a multiplayer game (CTF and other gamemodes) starts, opposite for `onGameStop`, `firstTime` should be true for the game being started or stopped at the beginning of the level, and false after that. However, it will also be true for clients who join the server mid-game.

## `void onLevelReload()`
`onLevelReload` is only ever called in single player, it is called when you run out of lives, so if you have anything you want to reset to it's original state, you should most definetly do that here

## `void onMain()`
`onMain` is one of the most important functions, it is called every tick (a second is 70 ticks), and you should update everything you want here

## `void onPlayer(jjPLAYER@ player)`
`onPlayer` gets called more than `onMain` sometimes, it is called once a tick per local player, so: If you're playing splitscreen with two players, it will be called twice a tick, one huge advantage `onPlayer` has, is that it returns the player, so it's basically `onMain` but with the player provided

## `void onPlayerInput(jjPLAYER@ player)`
`onPlayerInput` is called very slightly earlier than a normal tick, but it is only called when `player` produces input, same rule of `it is called once a tick per local player, so: If you're playing splitscreen with two players, it will be called twice a tick` applies here too

## `void onPlayerTimerEnd(jjPLAYER@ player)`
`onPlayerTimerEnd` is self-explanatory, it only gets called when a player timer ends

## `void onRoast(jjPLAYER@ victim, jjPLAYER@ killer)`
`onRoast` is called when a player kills another player, `victim` is the player who got killed, `killer` is the player who killed `victim`, also: not every death counts as a roast, for example: deaths from the /frustration command or from unspectating wouldn't call onRoast

Wow, that was a lot, Now that you have an understanding of each event, how about we make a simple player point system when you kill another player using the `onRoast` function?

[Next topic](pointsystem.md)
