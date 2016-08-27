All commands are prefixed with the bot's nick, a space, and an @ sign.

The bot's nick has to be the first part of `packet->content` for the bot to react.

| Command | Description |
|---------|-------------|
| `help` | PMs caller a list of commands, excluding admin commands |
| `slap` | performs ACTION of slapping certain user. There is a 1/10 chance of kicking that user |
| `wakeup`/`sleep` | tell the bot to ignore any commands until woken-up |
| `google` | builds link for google query |
| `search` | builds link for 0x00sec query |
| `urban` | builds link for Urban Dictionary Query |
| `topic` | returns link for 0x00sec topic of a certain number |
| `iplookup` | tries to find the location of a host and outputs the local weather if possible |
| `quit` | ends IRC session |
| `restart` | disconnects and then reconnects to IRC session |
| `echo` [0,1] | repeats after caller |
| `kick` | kicks a user. Only works if bot has OP. |
| `repeat` | returns caller's arguments in a colorful way. |
| `iplookupset` | toggle iplookup on or off with 0 or 1 as an argument |
| `join` | joins another channel |
| `part` | leaves specified channel |
| `nick` | globally changes bot's nickname |
| `1337` | transforms arguments into 1337sp34k

