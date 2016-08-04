# irc-bot
A simple, not-yet-complete IRC bot written in C.

Compile with:

    gcc -o bot bot.c -lcurl

See Commands.md for commands.

Along with all of the commands, the bot is also equipped to rejoin any channel from which it is kicked (indefinitely, or until banned). It can also look up the IP of any uncloaked user who joins the room (if `iplookup` is on).

