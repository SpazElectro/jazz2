# Point system
<!-- TODO: make getPlayerStats not use a jjPLAYER, but instead have it so i can do something like: `playerStats[clientID]` for easier use, so we can (i think) remove the for loop -->

Let's first off start by removing our old `onLocalChat` code so we have a clean mutator, Let's start off by making a `PlayerStats` class, having 2 elements, one is the player, the second is the player's points

```cpp
class PlayerStats {
    jjPLAYER@ player;
    int points;

    PlayerStats() {}
};
```
Let's make an array with PlayerStats handles (handles are basically pointers :P)
```cpp
array<PlayerStats@> playerStats;
```

Simple, right? our code should look something like this so far:
```cpp
#pragma name "testdocs"
#pragma require "STVutil.asc"

#include "STVutil.asc"

// for intellisense
#include "../../scripts/STVutil.asc"

class PlayerStats {
    jjPLAYER@ player;
    int points;

    PlayerStats() {}
};

array<PlayerStats@> playerStats;

void onLevelBegin() {
    jjConsole("Started!");
}
```
Let's create a function to get a player's `PlayerStats` from their `jjPLAYER`, and if they don't have one, we create one
```cpp
PlayerStats@ getPlayerStats(jjPLAYER@ player) {
    PlayerStats@ stats;

    // loop through each element of the array `playerStats`
    for(uint i = 0; i < playerStats.length; i++) {
        // if the clientID matches with the other player's client id, we found the stats
        if(playerStats[i].player.clientID == player.clientID) {
            // set the stats using a handle
            @stats = playerStats[i];

            // stop the loop
            break;
        }
    }

    // if stats doesn't exist, we make a new one then
    if(@stats == null) {
        @stats = PlayerStats();
        @stats.player = player;
        stats.points = 0;

        // now we insert the newly created player stats into the playerStats array
        playerStats.insertLast(stats);
    }

    // after that, we return the player stats
    return stats;
}
```
This can be really complicated if you don't understand C++ or AngelScript, so we recommend that you go check out guides for AngelScript if you find this complicated, Now that we have our base stuff ready, let's use the `onRoast` event!
```cpp
void onRoast(jjPLAYER@ victim, jjPLAYER@ killer) {
    // using our functions, we find the victim's state, and the killer's stats
    PlayerStats@ victimStats = getPlayerStats(victim);
    PlayerStats@ killerStats = getPlayerStats(killer);
    
    // if the killer is the victim, aka if the player killed themselves
    if(killer.clientID == victim.clientID) {
        // we punish them by giving them -1 point
        victimStats.points--;
    } else {
        // else if the killer is NOT the victim, aka another player killed `victim`, we give `killer` 1 more point
        killerStats.points++;
    }
}
```
That should work fantastic now, but we still dont have a way to show our points, For now, we'll use the `onLocalChat` event and make a new command called `!points` and jjConsole the player points, If you're wondering what `p` is, `p` is the local player, it is a deprecated global variable, but we're using it here for the tutorial's sake.
```cpp
bool onLocalChat(string &in stringReceived, CHAT::Type chatType) {
    if(chatType == CHAT::NORMAL) {
        if(stringReceived == "!points") {
            jjConsole("Points: " + getPlayerStats(p).points);
            return true;
        }
    }

    return false;
}
```
That should be all done, go test out your newly made point system and go nuts!
<br><br>
Full code:

```cpp
#pragma name "testdocs"
#pragma require "STVutil.asc"

#include "STVutil.asc"

// for intellisense
#include "../../scripts/STVutil.asc"

class PlayerStats {
    jjPLAYER@ player;
    int points;

    PlayerStats() {}
};

array<PlayerStats@> playerStats;

PlayerStats@ getPlayerStats(jjPLAYER@ player) {
    PlayerStats@ stats;

    for(uint i = 0; i < playerStats.length; i++) {
        if(playerStats[i].player.clientID == player.clientID) {
            @stats = playerStats[i];
            break;
        }
    }

    // if stats doesn't exist, we make a new one then
    if(@stats == null) {
        @stats = PlayerStats();
        @stats.player = player;
        stats.points = 0;

        playerStats.insertLast(stats);
    }

    return stats;
}

void onLevelBegin() {
    jjConsole("Started!");
}

void onRoast(jjPLAYER@ victim, jjPLAYER@ killer) {
    PlayerStats@ victimStats = getPlayerStats(victim);
    PlayerStats@ killerStats = getPlayerStats(killer);
    
    if(killer.clientID == victim.clientID) {
        victimStats.points--;
    } else {
        killerStats.points++;
    }
}

bool onLocalChat(string &in stringReceived, CHAT::Type chatType) {
    if(chatType == CHAT::NORMAL) {
        if(stringReceived == "!points") {
            jjConsole("Points: " + getPlayerStats(p).points);
            return true;
        }
    }

    return false;
}
```
