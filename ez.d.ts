declare interface PlayerEntry {
  readonly xuid: string;
  readonly uuid: string;
  readonly name: string;
  readonly address: string;
  readonly alive: boolean;
  getOffline(): OfflinePlayerEntry;
  toString(): string;
}

declare interface OfflinePlayerEntry {
  readonly xuid: string;
  readonly uuid: string;
  readonly name: string;
  getOnline(): PlayerEntry | null;
  toString(): string;
}

declare type ItemStack = {
  readonly raw_name: string;
  readonly name: string;
  readonly hover_name: string;
  readonly custom_hover_name: boolean;
  readonly id: number;
  readonly aux: number;
  readonly count: number;
  readonly max_count: number;
  equals(stack: ItemStack): boolean;
  toString(): string;
};

declare module "ez:player" {
  export function getPlayerByXUID(xuid: string): PlayerEntry;
  export function getPlayerByUUID(uuid: string): PlayerEntry;
  export function getPlayerByNAME(name: string): PlayerEntry;

  export function getOfflinePlayerByXUID(xuid: string): OfflinePlayerEntry;
  export function getOfflinePlayerByUUID(uuid: string): OfflinePlayerEntry;
  export function getOfflinePlayerByNAME(name: string): OfflinePlayerEntry;

  export function getPlayerList(): PlayerEntry[];

  export function onPlayerJoined(cb: (entry: PlayerEntry) => void): void;
  export function onPlayerLeft(cb: (entry: PlayerEntry) => void): void;
}

declare module "ez:basic-anti-cheat" {
  export function onDetected(
    cb: (entry: { name: string; target: PlayerEntry }) => void
  ): void;
}

declare module "ez:blacklist" {
  export function addByXUID(xuid: string, reason: string, op: string): void;
  export function addByUUID(uuid: string, reason: string, op: string): void;
  export function addByNAME(name: string, reason: string, op: string): void;
  export function removeByXUID(xuid: stirng): void;
  export function removeByUUID(uuid: stirng): void;
  export function removeByNAME(name: stirng): void;
}

declare module "ez:chat" {
  export function onChat(cb: (sender: string, content: string) => void): void;
  export function sendBroadcast(sender: string, content: string): void;
}

declare module "ez:command" {
  export function executeCommand(
    data: string,
    cb: (obj: { statusMessage: string; [key: string]: any }) => void
  ): void;
}

declare module "ez:inventory" {
  export function getOffHandItem(player: PlayerEntry): ItemStack;
  export function getEquipmentItems(
    player: PlayerEntry
  ): [ItemStack, ItemStack, ItemStack, ItemStack];
  // prettier-ignore
  export function getInventoryItems(
    player: PlayerEntry
  ): [
    ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack,
    ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack,
    ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack,
    ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack,
  ];
}
