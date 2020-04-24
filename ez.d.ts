declare interface PlayerEntry {
  readonly xuid: string;
  readonly uuid: string;
  readonly name: string;
  /** IP and port info */
  readonly address: string;
  /** Detect if player is still exists in server */
  readonly alive: boolean;
  /** A object shared between all instances */
  readonly aux: object;
  /** Get offline player entry */
  getOffline(): OfflinePlayerEntry;
  toString(): string;
}

declare interface OfflinePlayerEntry {
  readonly xuid: string;
  readonly uuid: string;
  readonly name: string;
  /** Get online player entry if the player is online, or you will get null */
  getOnline(): PlayerEntry | null;
  toString(): string;
}

declare module "ez:player" {
  export function getPlayerByXUID(xuid: string): PlayerEntry;
  export function getPlayerByUUID(uuid: string): PlayerEntry;
  export function getPlayerByNAME(name: string): PlayerEntry;

  export function getOfflinePlayerByXUID(xuid: string): OfflinePlayerEntry;
  export function getOfflinePlayerByUUID(uuid: string): OfflinePlayerEntry;
  export function getOfflinePlayerByNAME(name: string): OfflinePlayerEntry;

  export function getPlayerList(): PlayerEntry[];

  /**
   * Called when player joined
   * @param cb Callback
   */
  export function onPlayerJoined(cb: (entry: PlayerEntry) => void): void;
  /**
   * Called before player left
   * @param cb Callback
   */
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
  export function removeByXUID(xuid: string): void;
  export function removeByUUID(uuid: string): void;
  export function removeByNAME(name: string): void;
}

declare module "ez:chat" {
  export function onChat(cb: (sender: string, content: string) => void): void;
  export function sendBroadcast(sender: string, content: string): void;
}

declare module "ez:command" {
  /**
   * Execute server command
   * @param data command
   * @param cb Callback
   */
  export function executeCommand(
    data: string,
    cb: (obj: { statusMessage: string; [key: string]: any }) => void
  ): void;

  /**
   * Execute server command (async version)
   * @param data command
   */
  export function executeCommand(
    data: string
  ): Promise<{ statusMessage: string; [key: string]: any }>;
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
  readonly enchanted: boolean;
  readonly enchants: EnchantmentInstance[];
  dump(): ArrayBuffer;
  equals(stack: ItemStack): boolean;
  toString(): string;
};

declare type EnchantmentInstance = {
  type: number;
  level: number;
  name(): string;
  toString(): string;
};

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
  // prettier-ignore
  export function getEnderChestItems(
    player: PlayerEntry
  ): [
    ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack,
    ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack,
    ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack,
    ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack, ItemStack,
  ];
}

declare module "ez:utils" {
  export function delay(time: number): Promise<void>;
}

declare type BossBar = {
  /**
   * Detect if the bossbar still valid
   * It will be invalid after player left or having been destoryed
   */
  valid(): boolean;
  updateText(text: string): void;
  updatePercent(percent: number): void;
  show(): void;
  hide(): void;
  destory(): void;
};

declare module "ez:bossbar" {
  export function create(
    player: PlayerEntry,
    text: string,
    percent: number
  ): BossBar;
}

declare type Sqlite3BindType =
  | number
  | null
  | undefined
  | string
  | ArrayBuffer
  | ArrayBufferView;
declare type Sqlite3ColumnType = number | null | string | ArrayBuffer;

declare type Sqlite3Statement = {
  reset(): void;
  clearBindings(): void;
  bind(index: number | string, value: Sqlite3BindType): void;
  bindAll(obj: Array<Sqlite3BindType>): void;
  bindAll(obj: Record<string, Sqlite3BindType>): void;
  exec(): void;
  execWith(obj: Array<Sqlite3BindType>): void;
  execWith(obj: Record<string, Sqlite3BindType>): void;
  forEach(fn: (...columns: Array<Sqlite3ColumnType>) => boolean | void): void;
  forEachWith(
    obj: Array<Sqlite3BindType>,
    fn: (...columns: Array<Sqlite3ColumnType>) => boolean | void
  ): void;
  forEachWith(
    obj: Record<string, Sqlite3BindType>,
    fn: (...columns: Array<Sqlite3ColumnType>) => boolean | void
  ): void;
};

declare type Sqlite3Database = {
  exec(sql: string): void;

  prepare(sql: string): Sqlite3Statement;
};

declare module "ez:sqlite3" {
  export function open(filename: string): Sqlite3Database;
}
