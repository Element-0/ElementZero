# Built-in mods

Because reading through source files is hard work, here's a collection of all relevant information:

(Please note `usage` refers to in-game commands, while `settings` refers to the custom.yaml file.)

### Audit

Record player action, inventory transaction packet into database. Provide api to handle those events. API usage: See [SpawnProtection mod](BuiltinMods/SpawnProtection/)

Usage: `/audit [on|off]`

Settings: `database: audit.db`

### BasicAntiCheat

It's an anti cheat. As the name suggests, it's fairly basic. But it does get the job done.

Usage: `no commands`

Settings: `none`

### Blacklist

Finally! A /ban command

Usage: `/ban <name: string> [reason: string]`

Settings: `enable-commands: true `

### BossBarSupport

Usage: `no commands`

Settings: `none`

### ChatAPI

Usage: `no commands`

Settings: `database: chat.db`

### CommandSupport

Usage: `no commands`

Settings: `none`

### CrashHandler

Usage: `no commands`

Settings: `none`

### CustomScoreboard

Usage: `no commands`

Settings: `none`

### DebugTool

Usage: `no commands`

Settings:

```
command:
  log-register: false
  enable-all: false
```

### DynamicMotd

Have a changing Moto of the Day!

Usage: `no commands`

Settings:

```
list:
  - example
  - example 2
interval: 5 # refresh interval
```

### Economy

Money system.

Usage:

```
/balance
/update-balance <target> <amount> <reason>
/get-balance <target>
```

Settings:

```
database: economy.db
command: true
```

### Essentials

A simple port of Bukkit's [essentials plugin](https://dev.bukkit.org/projects/essentials).

Usage:

```
/custom-name set <key> <target> <str>
/custom-name clear <target>
/home
/spawn
/tpa <selector>
/tpaccept
/tpdeny
/transferserver <target> <hostname> [port]
/warp to <name>
/warp list
/warp set <name>
/warp del <name>
/global-warp set|del <name>
```

Settings: `todo`

### Experimental

Use [experimental gameplay features](https://minecraft.gamepedia.com/Experimental_Gameplay).

Usage: `no commands`

Settings: `todo`

### FlatWorld

It's a flat world.

Usage: `no commands`

Settings: `todo`

### NetworkStatus

Usage: `no commands`

Settings: `todo`

### NoEncryption

Usage: `no commands`

Settings: `todo`

### NullPlayerData

Usage: `no commands`

Settings: `todo`

### PacketDumper

Usage: `no commands`

Settings: `todo`

### Remote

Usage: `no commands`

Settings: `todo`

### ScriptingSupport

Usage: `no commands`

Settings: `todo`

### SignCommand

Usage: `no commands`

Settings: `todo`

### SpawnProtection

Protect a certain world area.

Usage: `/spawn-protection <mode>`

Settings: `todo`
